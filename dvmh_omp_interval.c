#include <assert.h>
#include <float.h>
#include <stdlib.h>
// #include <stdio.h>
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
    int calls_count;
    double io_time;
    double execution_time;
    double sync_barrier;
    double used_time;
    int used_threads_number;
    double idle_critical;
    double sync_flush;
    double idle_parallel;
    double load_imbalance;
    double thread_prod_max;
    double thread_prod_min;
    double thread_prod_avg;
    double total_time;
    double lost_time;
    double productive_time;
    double insufficient_parallelism;
    double efficiency;
    int is_in_parallel;
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
static void interval_sync_barrier(dvmh_omp_interval *i);
static void interval_used_time(dvmh_omp_interval *i);
static void interval_used_threads_number(dvmh_omp_interval *i);
static void interval_idle_critical(dvmh_omp_interval *i);
static void interval_sync_flush(dvmh_omp_interval *i);
static void interval_idle_parallel(dvmh_omp_interval *i);
static void interval_load_imbalance(dvmh_omp_interval *i);
static void interval_total_time(dvmh_omp_interval *i);
static void interval_productive_time(dvmh_omp_interval *i);
static void interval_lost_time(dvmh_omp_interval *i);
static void interval_insufficient_parallelism(dvmh_omp_interval *i);
static void interval_efficiency(dvmh_omp_interval *i);
static void interval_is_in_parallel(dvmh_omp_interval *i);

dvmh_omp_interval *dvmh_omp_interval_build(dvmh_omp_event *e)
{
    building_context *bc = building_context_create();
    dvmh_omp_interval *i = build_intervals(bc, e);
    building_context_destroy(bc);
    // characteristics
    interval_calls_count(i);
    interval_io_time(i);
    interval_execution_time(i);
    interval_sync_barrier(i);
    interval_used_time(i);
    interval_used_threads_number(i);
    interval_idle_critical(i);
    interval_sync_flush(i);
    interval_idle_parallel(i);
    interval_load_imbalance(i);
    interval_total_time(i);
    interval_insufficient_parallelism(i);
    interval_lost_time(i);
    interval_productive_time(i);
    interval_efficiency(i);
    interval_is_in_parallel(i);
    return i;
}

static dvmh_omp_interval *dvmh_omp_interval_create(context_descriptor *d)
{
    dvmh_omp_interval *i = (dvmh_omp_interval *) malloc(sizeof(dvmh_omp_interval));
    assert(i);
    i->descriptor = d;
    i->subintervals = list_create();
    i->occurrences = NULL;
    i->calls_count = 0;
    i->io_time = 0.0;
    i->execution_time = 0.0;
    i->sync_barrier = 0.0;
    i->used_time = 0.0;
    i->used_threads_number = 0;
    i->idle_critical = 0.0;
    i->sync_flush = 0.0;
    i->idle_parallel = 0.0;
    i->load_imbalance = 0.0;
    i->thread_prod_max = 0.0;
    i->thread_prod_min = DBL_MAX;
    i->thread_prod_avg = 0.0;
    i->total_time = 0.0;
    i->lost_time = 0.0;
    i->productive_time = 0.0;
    i->insufficient_parallelism = 0.0;
    i->efficiency = 0.0;
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
    /* destroy bc->registered_intervals */
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

/* calls count, с учетом параллелизма */
static void interval_calls_count(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_calls_count(subinterval);
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        i->calls_count += list_size(o->events);
    }
    // fprintf(stderr, "interval %ld, calls_count %d\n", (long) i->descriptor, i->calls_count);
}

/* IO time */
static double event_io_time(dvmh_omp_event *e)
{
    double io_time = 0.0;
    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        if (dvmh_omp_event_get_type(s) == DVMH_OMP_EVENT_INTERVAL){
            continue;
        }
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
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_io_time(subinterval);
        i->io_time += subinterval->io_time;
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->io_time += event_io_time(e);
        }
        list_iterator_destroy(it);
    }
    // fprintf(stderr, "interval %ld, io_time %lf\n", (long) i->descriptor, i->io_time);
}

/* Execution time */
//TODO optimization
static int events_compare(const void *a, const void *b)
{
    double t1 = dvmh_omp_event_get_begin_time((dvmh_omp_event *) a);
    double t2 = dvmh_omp_event_get_begin_time((dvmh_omp_event *) b);
    return t1 == t2 ? 0 : (t1 > t2 ? 1 : -1);
}


static void interval_execution_time(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_execution_time(subinterval);
    }
    list_iterator_destroy(it);

    if (i->calls_count == 0){
        return;
    }

    dvmh_omp_event **ordered_events =
            (dvmh_omp_event **) malloc(i->calls_count * sizeof(dvmh_omp_event *));
    assert(ordered_events);

    int index = 0;
    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            ordered_events[index++] = e;
        }
        list_iterator_destroy(it);
    }

    /* sorting events by occurence time */
    qsort(ordered_events, i->calls_count, sizeof(dvmh_omp_event *), events_compare);

    /* calculate execution time */
    double begin_time = dvmh_omp_event_get_begin_time(ordered_events[0]);
    double end_time = dvmh_omp_event_get_end_time(ordered_events[0]);
    for (index = 1; index < i->calls_count; ++index){
        double current_begin_time = dvmh_omp_event_get_begin_time(ordered_events[index]);
        double current_end_time = dvmh_omp_event_get_end_time(ordered_events[index]);

        if (end_time < current_begin_time){
            i->execution_time += end_time - begin_time;
            begin_time = current_begin_time;
            end_time = current_end_time;
            continue;
        }
        if (current_end_time > end_time){
            end_time = current_end_time;
        }
    }
    i->execution_time += end_time - begin_time;

    free(ordered_events);
    // fprintf(stderr, "interval %ld, execution_time %lf\n", (long) i->descriptor, i->execution_time);
}

/* Sync barrier time */
static double event_sync_barrier(dvmh_omp_event *e, int include_subintervals)
{
    double sync_time = 0.0;
    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        if (!include_subintervals && dvmh_omp_event_get_type(s) == DVMH_OMP_EVENT_INTERVAL){
            continue;
        }
        sync_time += event_sync_barrier(s, include_subintervals);
    }
    dvmh_omp_subevent_iterator_destroy(it);

    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_BARRIER){
        sync_time += dvmh_omp_event_duration(e);
    }

    return sync_time;
}

static void interval_sync_barrier(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_sync_barrier(subinterval);
        i->sync_barrier += subinterval->sync_barrier;
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->sync_barrier += event_sync_barrier(e, 0);
        }
        list_iterator_destroy(it);
    }
    // fprintf(stderr, "interval %ld, sync_barrier %lf\n", (long) i->descriptor, i->sync_barrier);
}

/* User time - считаем, что вложенного параллелизма нет */
//TODO optimization
static double event_used_time(dvmh_omp_event *e, int level)
{
    double used_time = 0.0;
    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_PARALLEL_REGION){
        dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
        while (dvmh_omp_subevent_iterator_has_next(it)){
            dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
            if (dvmh_omp_event_get_thread_id(s) == dvmh_omp_event_get_thread_id(e)){
                continue;
            }
            used_time += dvmh_omp_event_duration(s);
        }
        dvmh_omp_subevent_iterator_destroy(it);
    } else {
        dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
        while (dvmh_omp_subevent_iterator_has_next(it)){
            dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
            used_time += event_used_time(s, level + 1);
        }
        dvmh_omp_subevent_iterator_destroy(it);
    }

    if (level == 0){
        used_time += dvmh_omp_event_duration(e);
    }

    return used_time;
}

static void interval_used_time(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_used_time(subinterval);
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->used_time += event_used_time(e, 0);
        }
        list_iterator_destroy(it);
    }
    // fprintf(stderr, "interval %ld, used_time %lf\n", (long) i->descriptor, i->used_time);
}

/* Threads count - вложенного параллелизма нет */
//TODO optimization
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

/* Idle time in critical */
static double event_idle_critical(dvmh_omp_event *e, int include_subintervals)
{
    double idle_time = 0.0;

    dvmh_omp_event_type event_type = dvmh_omp_event_get_type(e);

    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        if (!include_subintervals && dvmh_omp_event_get_type(s) == DVMH_OMP_EVENT_INTERVAL){
            continue;
        }
        idle_time += event_idle_critical(s, include_subintervals);
        if (event_type != DVMH_OMP_EVENT_CRITICAL_OUTER){
            continue;
        }
        idle_time += dvmh_omp_event_get_begin_time(s) - dvmh_omp_event_get_begin_time(e);
    }
    dvmh_omp_subevent_iterator_destroy(it);

    return idle_time;
}

static void interval_idle_critical(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_idle_critical(subinterval);
        i->idle_critical += subinterval->idle_critical;
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->idle_critical += event_idle_critical(e, 0);
        }
        list_iterator_destroy(it);
    }
    // fprintf(stderr, "interval %ld, idle_critical %lf\n", (long) i->descriptor, i->idle_critical);
}

/* Sync flush time */
static double event_sync_flush(dvmh_omp_event *e, int include_subintervals)
{
    double sync_time = 0.0;
    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        if (!include_subintervals && dvmh_omp_event_get_type(s) == DVMH_OMP_EVENT_INTERVAL){
            continue;
        }
        sync_time += event_sync_flush(s, include_subintervals);
    }
    dvmh_omp_subevent_iterator_destroy(it);

    if (dvmh_omp_event_get_type(e) == DVMH_OMP_EVENT_FLUSH){
        sync_time += dvmh_omp_event_duration(e);
    }

    return sync_time;
}

static void interval_sync_flush(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_sync_flush(subinterval);
        i->sync_flush += subinterval->sync_flush;
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->sync_flush += event_sync_flush(e, 0);
        }
        list_iterator_destroy(it);
    }
    // fprintf(stderr, "interval %ld, sync_flush %lf\n", (long) i->descriptor, i->sync_flush);
}

/* Idle time in parallel end */
static double event_idle_parallel(dvmh_omp_event *e)
{
    double idle_time = 0.0;

    dvmh_omp_event_type event_type = dvmh_omp_event_get_type(e);

    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *s = dvmh_omp_subevent_iterator_next(it);
        if (dvmh_omp_event_get_type(s) == DVMH_OMP_EVENT_INTERVAL){
            continue;
        }
        idle_time += event_idle_parallel(s);
        if (event_type != DVMH_OMP_EVENT_PARALLEL_REGION){
            continue;
        }
        idle_time += dvmh_omp_event_get_end_time(e) - dvmh_omp_event_get_end_time(s);
    }
    dvmh_omp_subevent_iterator_destroy(it);

    return idle_time;
}

static void interval_idle_parallel(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_idle_parallel(subinterval);
        i->idle_parallel += subinterval->idle_parallel;
    }
    list_iterator_destroy(it);

    events_occurrences *o, *tmp;
    HASH_ITER(hh, i->occurrences, o, tmp){
        list_iterator *it = list_iterator_new(o->events);
        while(list_iterator_has_next(it)){
            dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
            i->idle_parallel += event_idle_parallel(e);
        }
        list_iterator_destroy(it);
    }
    // fprintf(stderr, "interval %ld, idle_parallel %lf\n", (long) i->descriptor, i->idle_parallel);
}

/* Load imbalance, без вложенного параллелизма */
typedef struct _thread_load {
    long thread_id; /* this is key */
    double load_time; /* this is value */
    double sync_time; /* this is a value too */
    double prod_time; /* the third value */
    UT_hash_handle hh; /* makes this structure hashable */
} thread_load;

static void event_thread_load(dvmh_omp_event *e, thread_load **tl)
{
    if (dvmh_omp_event_get_type(e) != DVMH_OMP_EVENT_PARALLEL_REGION){
        dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
        while (dvmh_omp_subevent_iterator_has_next(it)){
            dvmh_omp_event *se = dvmh_omp_subevent_iterator_next(it);
            event_thread_load(se, tl);
        }
        dvmh_omp_subevent_iterator_destroy(it);
        return;
    }

    dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
    while (dvmh_omp_subevent_iterator_has_next(it)){
        dvmh_omp_event *se = dvmh_omp_subevent_iterator_next(it);
        thread_load *current_tl;
        long thread_id = dvmh_omp_event_get_thread_id(se);
        HASH_FIND_LONG(*tl, &thread_id, current_tl);
        if (current_tl == NULL){
            current_tl = (thread_load *) malloc(sizeof(thread_load));
            assert(current_tl);
            current_tl->thread_id = thread_id;
            current_tl->load_time = 0.0;
            current_tl->sync_time = 0.0;
            HASH_ADD_LONG(*tl, thread_id, current_tl);
        }
        current_tl->load_time += dvmh_omp_event_duration(se);
        current_tl->sync_time += event_sync_flush(se, 1);
        current_tl->sync_time += event_idle_critical(se, 1);
        current_tl->sync_time += event_sync_barrier(se, 1);
    }
    dvmh_omp_subevent_iterator_destroy(it);
    return;
}

static void interval_load_imbalance(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_load_imbalance(subinterval);
    }
    list_iterator_destroy(it);

    if (i->used_threads_number < 2){
        i->thread_prod_max = i->used_time;
        i->thread_prod_min = i->used_time;
        i->thread_prod_avg = i->used_time;
        return;
    }

    thread_load *tl = NULL;

    /* prepare thread_load *tl */
    if (HASH_COUNT(i->occurrences) > 1){ /* if called from parallel region */
        events_occurrences *o, *tmp;
        HASH_ITER(hh, i->occurrences, o, tmp){
            thread_load *new_tl = (thread_load *) malloc(sizeof(thread_load));
            assert(new_tl);
            new_tl->thread_id = o->thread_id;
            new_tl->load_time = 0.0;
            new_tl->sync_time = 0.0;
            list_iterator *it = list_iterator_new(o->events);
            while(list_iterator_has_next(it)){
                dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
                new_tl->load_time += dvmh_omp_event_duration(e);
                new_tl->sync_time += event_sync_flush(e, 1);
                new_tl->sync_time += event_idle_critical(e, 1);
                new_tl->sync_time += event_sync_barrier(e, 1);
            }
            list_iterator_destroy(it);
            HASH_ADD_LONG(tl, thread_id, new_tl);
        }
    } else { /* if called from non-parallel region */
        events_occurrences *o, *tmp;
        HASH_ITER(hh, i->occurrences, o, tmp){
            list_iterator *it = list_iterator_new(o->events);
            while(list_iterator_has_next(it)){
                dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
                event_thread_load(e, &tl);
            }
            list_iterator_destroy(it);
            
            /* for current thread rewrite thread load time */
            thread_load *this_tl;
            HASH_FIND_LONG(tl, &o->thread_id, this_tl);
            assert(this_tl);
            this_tl->load_time = 0.0;
            this_tl->sync_time = 0.0;

            it = list_iterator_new(o->events);
            while(list_iterator_has_next(it)){
                dvmh_omp_event *e = (dvmh_omp_event *) list_iterator_next(it);
                this_tl->load_time += dvmh_omp_event_duration(e);
                this_tl->sync_time += event_sync_flush(e, 1);
                this_tl->sync_time += event_idle_critical(e, 1);
                this_tl->sync_time += event_sync_barrier(e, 1);
            }
            list_iterator_destroy(it);

            thread_load *current_tl, *tmp_tl;
            HASH_ITER(hh, tl, current_tl, tmp_tl){
                if (current_tl->thread_id != o->thread_id){
                    this_tl->sync_time -= current_tl->sync_time;
                }
            }
        }
    }

    /* analyze thread_load *tl */
    thread_load *current_tl, *tmp;
    HASH_ITER(hh, tl, current_tl, tmp){
        current_tl->prod_time = current_tl->load_time - current_tl->sync_time;
        if (current_tl->prod_time > i->thread_prod_max){
            i->thread_prod_max = current_tl->prod_time;
        }
        if (current_tl->prod_time < i->thread_prod_min){
            i->thread_prod_min = current_tl->prod_time;
        }
        i->thread_prod_avg += current_tl->prod_time;
    }
    i->thread_prod_avg /= HASH_COUNT(tl);

    HASH_ITER(hh, tl, current_tl, tmp){
        i->load_imbalance += i->thread_prod_max - current_tl->prod_time;
    }

    /* clear memory */
    HASH_ITER(hh, tl, current_tl, tmp){
        HASH_DEL(tl, current_tl);
        free(current_tl);
    }

    // fprintf(stderr, "interval %ld, load_imbalance %lf\n", (long) i->descriptor, i->load_imbalance);
    // fprintf(stderr, "interval %ld, thread_prod_max %lf\n", (long) i->descriptor, i->thread_prod_max);
    // fprintf(stderr, "interval %ld, thread_prod_min %lf\n", (long) i->descriptor, i->thread_prod_min);
    // fprintf(stderr, "interval %ld, thread_prod_avg %lf\n", (long) i->descriptor, i->thread_prod_avg);
}

/* Total time */
static void interval_total_time(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_total_time(subinterval);
    }
    list_iterator_destroy(it);

    i->total_time = i->execution_time * i->used_threads_number;
    // fprintf(stderr, "interval %ld, total_time %lf\n", (long) i->descriptor, i->total_time);
}

/* Lost time */
static void interval_lost_time(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_lost_time(subinterval);
    }
    list_iterator_destroy(it);

    i->lost_time += i->sync_barrier;
    i->lost_time += i->idle_critical;
    i->lost_time += i->sync_flush;
    i->lost_time += i->insufficient_parallelism;
    // fprintf(stderr, "interval %ld, lost_time %lf\n", (long) i->descriptor, i->lost_time);
}

/* Productive time */
static void interval_productive_time(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_productive_time(subinterval);
    }
    list_iterator_destroy(it);

    i->productive_time = i->used_time;
    i->productive_time -= i->sync_barrier;
    i->productive_time -= i->idle_critical;
    i->productive_time -= i->sync_flush;
    // fprintf(stderr, "interval %ld, productive_time %lf\n", (long) i->descriptor, i->productive_time);
}

/* Efficiency */
static void interval_insufficient_parallelism(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_insufficient_parallelism(subinterval);
    }
    list_iterator_destroy(it);

    i->insufficient_parallelism = i->total_time - i->used_time;
    // fprintf(stderr, "interval %ld, efficiency %lf\n", (long) i->descriptor, i->efficiency);
}

/* Efficiency */
static void interval_efficiency(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_efficiency(subinterval);
    }
    list_iterator_destroy(it);

    i->efficiency = i->productive_time / i->total_time;
    // fprintf(stderr, "interval %ld, efficiency %lf\n", (long) i->descriptor, i->efficiency);
}

/* Is in parallel */
static void interval_is_in_parallel(dvmh_omp_interval *i)
{
    list_iterator *it = list_iterator_new(i->subintervals);
    while (list_iterator_has_next(it)){
        dvmh_omp_interval *subinterval = (dvmh_omp_interval *) list_iterator_next(it);
        interval_is_in_parallel(subinterval);
    }
    list_iterator_destroy(it);

    i->is_in_parallel = HASH_COUNT(i->occurrences) > 1 ? 1 : 0;
    // fprintf(stderr, "interval %ld, is_in_parallel %d\n", (long) i->descriptor, i->is_in_parallel);
}

/* getters */
int dvmh_omp_interval_get_calls_count(dvmh_omp_interval *i)
{
    return i->calls_count;
}

double dvmh_omp_interval_get_io_time(dvmh_omp_interval *i)
{
    return i->io_time;
}

double dvmh_omp_interval_get_execution_time(dvmh_omp_interval *i)
{
    return i->execution_time;
}

double dvmh_omp_interval_get_sync_barrier_time(dvmh_omp_interval *i)
{
    return i->sync_barrier;
}

double dvmh_omp_interval_get_used_time(dvmh_omp_interval *i)
{
    return i->used_time;
}

int dvmh_omp_interval_get_used_threads_number(dvmh_omp_interval *i)
{
    return i->used_threads_number;
}

double dvmh_omp_interval_get_idle_critical_time(dvmh_omp_interval *i)
{
    return i->idle_critical;
}

double dvmh_omp_interval_get_sync_flush_time(dvmh_omp_interval *i)
{
    return i->sync_flush;
}

double dvmh_omp_interval_get_idle_parallel_time(dvmh_omp_interval *i)
{
    return i->idle_parallel;
}

double dvmh_omp_interval_get_load_imbalance_time(dvmh_omp_interval *i)
{
    return i->load_imbalance;
}

double dvmh_omp_interval_get_thread_prod_max(dvmh_omp_interval *i)
{
    return i->thread_prod_max;
}

double dvmh_omp_interval_get_thread_prod_min(dvmh_omp_interval *i)
{
    return i->thread_prod_min;
}

double dvmh_omp_interval_get_thread_prod_avg(dvmh_omp_interval *i)
{
    return i->thread_prod_avg;
}

double dvmh_omp_interval_get_total_time(dvmh_omp_interval *i)
{
    return i->total_time;
}

double dvmh_omp_interval_get_lost_time(dvmh_omp_interval *i)
{
    return i->lost_time;
}

double dvmh_omp_interval_get_productive_time(dvmh_omp_interval *i)
{
    return i->productive_time;
}

double dvmh_omp_interval_get_insufficient_parallelism(dvmh_omp_interval *i)
{
    return i->insufficient_parallelism;
}

double dvmh_omp_interval_get_efficiency(dvmh_omp_interval *i)
{
    return i->efficiency;
}

int dvmh_omp_interval_is_in_parallel(dvmh_omp_interval *i)
{
    return i->is_in_parallel;
}

long dvmh_omp_interval_get_id(dvmh_omp_interval *i)
{
    return (long) i->descriptor;
}

int dvmh_omp_interval_begin_line(dvmh_omp_interval *i)
{
    if (i->descriptor == NULL){
        return 1;
    }
    return i->descriptor->info.begin_line;
}

char *dvmh_omp_interval_get_file_name(dvmh_omp_interval *i)
{
    if (i->descriptor == NULL){
        return "MAIN";
    }
    return i->descriptor->info.file_name;
}

/* subinterval iterator */
struct _dvmh_omp_subinterval_iterator {
	list_iterator *li;
};

dvmh_omp_subinterval_iterator * dvmh_omp_subinterval_iterator_new(dvmh_omp_interval *i)
{
	if (i == NULL || i->subintervals == NULL){
		return NULL;
	}
	dvmh_omp_subinterval_iterator *it =
			(dvmh_omp_subinterval_iterator *) malloc(sizeof(dvmh_omp_subinterval_iterator));
	assert(it);
	it->li = list_iterator_new(i->subintervals);
	return it;
}

int dvmh_omp_subinterval_iterator_has_next(dvmh_omp_subinterval_iterator *it)
{
	return it != NULL ? list_iterator_has_next(it->li) : 0;
}

dvmh_omp_interval *dvmh_omp_subinterval_iterator_next(dvmh_omp_subinterval_iterator *it)
{
	return (dvmh_omp_interval *) list_iterator_next(it->li);
}

void dvmh_omp_subinterval_iterator_destroy(dvmh_omp_subinterval_iterator *it)
{
	if (it == NULL){
		return;
	}
	list_iterator_destroy(it->li);
	free(it);
}