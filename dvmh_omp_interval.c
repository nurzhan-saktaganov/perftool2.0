#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "list.h"
#include "context_descriptor.h"
#include "dvmh_omp_event.h"
#include "uthash.h"
#include "dvmh_omp_interval.h"

#define HASH_ADD_LONG(head,longfield,add)                                          \
    HASH_ADD(hh,head,longfield,sizeof(long),add)
#define HASH_FIND_LONG(head,findlong,out)                                          \
    HASH_FIND(hh,head,findlong,sizeof(long),out)

typedef struct _events_occurrences {
    long thread_id;     /* this is key */
    list *events; /* this is value */
    UT_hash_handle hh;  /* makes this structure hashable */
} events_occurrences;

struct _dvmh_omp_interval {
    context_descriptor *descriptor;
    list *subintervals;
    events_occurrences *occurrences; //hashtable := thread_id to list of events
    int calls; // calls
    double io_time;
    double execution_time;
    double barrier_time;
    double user_time;
    int used_threads_number;
};

typedef struct _registered_interval {
    context_descriptor *descriptor;  /* this is key*/
    dvmh_omp_interval *interval; /* this is value */
    UT_hash_handle hh;
} registered_interval;

typedef struct _building_context {
    stack *active_intervals;
    registered_interval *registered_intervals; /* this is hash, must be NULL initialized */
} building_context;


/* list of functions for building context */
static building_context *building_context_create();
static dvmh_omp_interval *build_intervals(building_context *bc, dvmh_omp_event *e);
static void building_context_destroy(building_context *bc);

/* list of functions for interval */
static dvmh_omp_interval *dvmh_omp_interval_create();
static void dvmh_omp_interval_add_occurrence(dvmh_omp_interval *i, dvmh_omp_event *e);
static void dvmh_omp_interval_add_subinterval(dvmh_omp_interval *i, dvmh_omp_interval *s);
static void interval_calls_count(dvmh_omp_interval *i);
static void interval_io_time(dvmh_omp_interval *i);
static void interval_execution_time(dvmh_omp_interval *i);
static void interval_barrier_time(dvmh_omp_interval *i);
static void interval_user_time(dvmh_omp_interval *i);
static void interval_used_threads_number(dvmh_omp_interval *i);

dvmh_omp_interval *dvmh_omp_interval_build(dvmh_omp_event *e)
{
    building_context *bc = building_context_create();
    dvmh_omp_interval *i = build_intervals(bc, e);
    building_context_destroy(bc);
    // characteristics
    interval_calls_count(i);
    interval_io_time(i);
    interval_execution_time(i);
    interval_barrier_time(i);
    interval_user_time(i);
    interval_used_threads_number(i);
    return i;
}

static dvmh_omp_interval *dvmh_omp_interval_create(context_descriptor *d)
{
    dvmh_omp_interval *i = (dvmh_omp_interval *) malloc(sizeof(dvmh_omp_interval));
    assert(i);
    i->descriptor = d;
    i->subintervals = list_create();
    i->occurrences = NULL;
    i->calls = 0;
    i->io_time = 0.0;
    i->execution_time = 0.0;
    i->barrier_time = 0.0;
    i->user_time = 0.0;
    i->used_threads_number = 0;
    return i;
}

void dvmh_omp_interval_destroy(dvmh_omp_interval *i)
{
    dvmh_omp_interval *s;
    while(list_size(i->subintervals) > 0){
        s = (dvmh_omp_interval *) list_remove_tail(i->subintervals);
        dvmh_omp_interval_destroy(s);
    }
    list_destroy(i->subintervals, NULL);
    // destroy occurences hahtable
    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        HASH_DEL(i->occurrences, o);
        list_destroy(o->events, NULL);
        free(o);
    }
    free(i);
}


static building_context *building_context_create()
{
    building_context *bc = (building_context *) malloc(sizeof(building_context));
    assert(bc);
    bc->active_intervals = stack_create();
    bc->registered_intervals = NULL;
    return bc;
}

static void building_context_destroy(building_context *bc)
{
    // destroy bc->registered_intervals
    registered_interval *current, *tmp;
    HASH_ITER(hh, bc->registered_intervals, current, tmp){
        HASH_DEL(bc->registered_intervals, current);
        free(current);
    }

    stack_destroy(bc->active_intervals, NULL);
    free(bc);
}

static void building_context_set_active_interval(building_context *bc, dvmh_omp_interval *i)
{
    stack_push(bc->active_intervals, i);
}

static void building_context_finish_active_interval(building_context *bc)
{
    stack_pop(bc->active_intervals);
}

static dvmh_omp_interval *building_context_get_active_interval(building_context *bc)
{
    return (dvmh_omp_interval *) stack_peek(bc->active_intervals);
}

static dvmh_omp_interval *building_context_register_interval(building_context *bc, context_descriptor *d)
{
    registered_interval *r;
    HASH_FIND_PTR(bc->registered_intervals, &d, r);
    if (r == NULL){
        r = (registered_interval *) malloc(sizeof(registered_interval));
        assert(r);
        r->interval = dvmh_omp_interval_create(d);
        r->descriptor = d;
        HASH_ADD_PTR(bc->registered_intervals, descriptor, r);
    }
    return r->interval;
}

static dvmh_omp_interval *build_intervals(building_context *bc, dvmh_omp_event *e)
{
    dvmh_omp_interval *i = NULL;
    dvmh_omp_event_type t = dvmh_omp_event_get_type(e);
    if (t == DVMH_OMP_EVENT_INIT){
        i = building_context_register_interval(bc, NULL);
        dvmh_omp_interval_add_occurrence(i, e);
        building_context_set_active_interval(bc, i);
    }
    if (t == DVMH_OMP_EVENT_INTERVAL){
        context_descriptor *d = dvmh_omp_event_get_context_descriptor(e);
        i = building_context_register_interval(bc, d);
        dvmh_omp_interval *parent = building_context_get_active_interval(bc);
        dvmh_omp_interval_add_subinterval(parent, i);
        dvmh_omp_interval_add_occurrence(i, e);
        building_context_set_active_interval(bc, i);
    }

    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
	while (dvmh_omp_subevent_iterator_has_next(it)){
		dvmh_omp_event *subevent = dvmh_omp_subevent_iterator_next(it);
		build_intervals(bc, subevent);
	}
	dvmh_omp_subevent_iterator_destroy(it);

    if (t == DVMH_OMP_EVENT_INIT || t == DVMH_OMP_EVENT_INTERVAL){
        building_context_finish_active_interval(bc);
    }

    return i;
}

static void dvmh_omp_interval_add_subinterval(dvmh_omp_interval *i, dvmh_omp_interval *s)
{
    if (!list_has_element(i->subintervals, s)){
        list_append_tail(i->subintervals, s);
    }
}

static void dvmh_omp_interval_add_occurrence(dvmh_omp_interval *i, dvmh_omp_event *e)
{
    events_occurrences *o;
    long thread_id = dvmh_omp_event_get_thread_id(e);
    HASH_FIND_LONG(i->occurrences, &thread_id, o);
    if (o == NULL){
        o = (events_occurrences *) malloc(sizeof(events_occurrences));
        assert(o);
        o->thread_id = thread_id;
        o->events = list_create();
        HASH_ADD_LONG(i->occurrences, thread_id, o);
    }
    list_append_tail(o->events, e);
}

static void interval_calls_count(dvmh_omp_interval *i)
{
    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        i->calls += list_size(o->events);
    }
    fprintf(stderr, "interval %ld, calls %d\n", (long) i->descriptor, i->calls);
    
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_calls_count(subinterval);
    }
    list_iterator_destroy(it);
}

/* IO time */
static double event_io_time(dvmh_omp_event *e)
{
    double io_time = 0.0;
    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        io_time += event_io_time(s);
    }
    dvmh_omp_subevent_iterator_destroy(it);

    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_IO){
        io_time += dvmh_omp_event_duration(e);
    }

    return io_time;
}

static void interval_io_time(dvmh_omp_interval *i)
{
    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->io_time += event_io_time(e);
        }
        list_iterator_destroy(it);
    }
    fprintf(stderr, "interval %ld, io_time %lf\n", (long) i->descriptor, i->io_time);
    
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_io_time(subinterval);
    }
    list_iterator_destroy(it);
}

/* Execution time */
static void interval_execution_time(dvmh_omp_interval *i)
{
    if (HASH_COUNT(i->occurrences) < 2){
        events_occurrences *o, *tmp;
        HASH_ITER(hh, i->occurrences, o, tmp){
            list_iterator *it = list_iterator_new(o->events);
            while(list_iterator_has_next(it)){
                dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
                i->execution_time += dvmh_omp_event_duration(e);
            }
            list_iterator_destroy(it);
        }
    } else {
        // TODO
    }
    fprintf(stderr, "interval %ld, execution_time %lf\n", (long) i->descriptor, i->execution_time);
    
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_execution_time(subinterval);
    }
    list_iterator_destroy(it);
}

/* Barrier time */
static double event_barrier_time(dvmh_omp_event *e)
{
    double barrier_time = 0.0;
    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        barrier_time += event_barrier_time(s);
    }
    dvmh_omp_subevent_iterator_destroy(it);

    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_BARRIER){
        barrier_time += dvmh_omp_event_duration(e);
    }

    return barrier_time;
}

static void interval_barrier_time(dvmh_omp_interval *i)
{
    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->barrier_time += event_barrier_time(e);
        }
        list_iterator_destroy(it);
    }
    fprintf(stderr, "interval %ld, barrier_time %lf\n", (long) i->descriptor, i->barrier_time);
    
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_barrier_time(subinterval);
    }
    list_iterator_destroy(it);
}

/* User time - считаем, что вложенного параллелизма нет */
static double event_user_time(dvmh_omp_event *e, int level)
{
    double user_time = 0.0;
    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_PARALLEL_REGION){
        dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
        while (dvmh_omp_subevent_iterator_has_next(it)){
            dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
            if (dvmh_omp_event_get_thread_id(s) == dvmh_omp_event_get_thread_id(e)){
                continue;
            }
            user_time += dvmh_omp_event_duration(s);
        }
        dvmh_omp_subevent_iterator_destroy(it);
    } else {
        dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
        while (dvmh_omp_subevent_iterator_has_next(it)){
            dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
            user_time += event_user_time(s, level + 1);
        }
        dvmh_omp_subevent_iterator_destroy(it);
    }

    if (level == 0){
        user_time += dvmh_omp_event_duration(e);
    }

    return user_time;
}

static void interval_user_time(dvmh_omp_interval *i)
{
    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->user_time += event_user_time(e, 0);
        }
        list_iterator_destroy(it);
    }
    fprintf(stderr, "interval %ld, user_time %lf\n", (long) i->descriptor, i->user_time);
    
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_user_time(subinterval);
    }
    list_iterator_destroy(it);
}

/* Threads count - вложенного параллелизма нет */
static int event_used_threads_number(dvmh_omp_event *e)
{
    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_PARALLEL_REGION){
        return dvmh_omp_event_subevents_count(e);
    }

    int threads_number = 1;

    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        int tmp = event_used_threads_number(s);
        if (tmp > threads_number){
            threads_number = tmp;
        }
    }
    dvmh_omp_subevent_iterator_destroy(it);

    return threads_number;
}

static void interval_used_threads_number(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_used_threads_number(subinterval);
    }
    list_iterator_destroy(it);

    i->used_threads_number = HASH_COUNT(i->occurrences);
    if (i->used_threads_number > 1){
        return;
    }

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            int tmp = event_used_threads_number(e);
            if (tmp > i->used_threads_number){
                i->used_threads_number = tmp;
            }
        }
        list_iterator_destroy(it);
    }
}
