#include <stdlib.h>
#include "stack.h"
#include "list.h"
#include "context_types.h"
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

typedef struct _dvmh_omp_interval {
    context_descriptor *descriptor;
    list *subintervals;
    events_occurrences *occurrences; //hashtable := thread_id to list of events
} dvmh_omp_interval;

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


/* list of functions for interval */
dvmh_omp_interval *dvmh_omp_interval_create();
void dvmh_omp_interval_add_occurrence(dvmh_omp_interval *i, dvmh_omp_event *e);
void dvmh_omp_interval_add_subinterval(dvmh_omp_interval *i, dvmh_omp_interval *s);

building_context *building_context_create()
{
    building_context *bc = (building_context *) malloc(sizeof(building_context));
    bc->active_intervals = stack_create();
    bc->registered_intervals = NULL;
    return bc;
}

void building_context_destroy(building_context *bc)
{
    //TODO destroy bc->registered_intervals
    stack_destroy(bc->active_intervals, NULL);
    free(bc);
}

void building_context_set_active_interval(building_context *bc, dvmh_omp_interval *i)
{
    stack_push(bc->active_intervals, i);
}

void building_context_finish_active_interval(building_context *bc)
{
    stack_pop(bc->active_intervals);
}

dvmh_omp_interval *building_context_get_active_interval(building_context *bc)
{
    return (dvmh_omp_interval *) stack_peek(bc->active_intervals);
}

dvmh_omp_interval *building_context_register_interval(building_context *bc, context_descriptor *d)
{
    registered_interval *r;
    HASH_FIND_PTR(bc->registered_intervals, &d, r);
    if (r == NULL){
        r = (registered_interval *) malloc(sizeof(registered_interval));
        r->interval = dvmh_omp_interval_create(d);
        HASH_ADD_PTR(bc->registered_intervals, descriptor, r);
    }
    return r->interval;
}

dvmh_omp_interval *build_intervals(building_context *bc, dvmh_omp_event *e)
{
    dvmh_omp_interval *i = NULL;
    dvmh_omp_event_type t = dvmh_omp_event_get_type(e);
    if (t == DVMH_OMP_EVENT_INIT){
        i = building_context_register_interval(bc, NULL);
        dvmh_omp_interval_add_occurrence(i, e);
        building_context_set_active_interval(bc, i);
    }
    if (t == DVMH_OMP_EVENT_BEFORE_INTERVAL){
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

    if (t == DVMH_OMP_EVENT_INIT || t == DVMH_OMP_EVENT_BEFORE_INTERVAL){
        building_context_finish_active_interval(bc);
    }

    return i;
}

dvmh_omp_interval *dvmh_omp_interval_create(context_descriptor *d)
{
    dvmh_omp_interval *i = (dvmh_omp_interval *) malloc(sizeof(dvmh_omp_interval));
    i->descriptor = d;
    i->subintervals = list_create();
    i->occurrences = NULL;
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
    // TODO destroy occurences hahtable
    free(i);
}

void dvmh_omp_interval_add_subinterval(dvmh_omp_interval *i, dvmh_omp_interval *s)
{
    if (!list_has_element(i->subintervals, s)){
        list_append_tail(i->subintervals, s);
    }
}

void dvmh_omp_interval_add_occurrence(dvmh_omp_interval *i, dvmh_omp_event *e)
{
    events_occurrences *o;
    long thread_id = dvmh_omp_event_get_thread_id(e);
    HASH_FIND_LONG(i->occurrences, &thread_id, o);
    if (o == NULL){
        o = (events_occurrences *) malloc(sizeof(events_occurrences));
        o->thread_id = thread_id;
        o->events = list_create();
        HASH_ADD_LONG(i->occurrences, thread_id, o);
    }
    list_append_tail(o->events, e);
}

dvmh_omp_interval *dvmh_omp_interval_build(dvmh_omp_event *e)
{
    building_context *bc = building_context_create();
    dvmh_omp_interval *i = build_intervals(bc, e);
    building_context_destroy(bc);
    return i;
}