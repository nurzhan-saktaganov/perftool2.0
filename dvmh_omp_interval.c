#include <stdlib.h>
#include "list.h"
#include "context_types.h"
#include "dvmh_omp_event.h"
#include "uthash.h"
#include "dvmh_omp_interval.h"

#define HASH_ADD_LONG(head,longfield,add)                                          \
    HASH_ADD(hh,head,longfield,sizeof(long),add)
#define HASH_FIND_LONG(head,findlong,out)                                          \
    HASH_FIND(hh,head,findlong,sizeof(long),out)

dvmh_omp_interval *dvmh_omp_interval_create();
void dvmh_omp_interval_set_descriptor(dvmh_omp_interval *i, context_descriptor *d);

dvmh_omp_interval *current_interval = NULL;

typedef struct _registered_interval {
    context_descriptor *descriptor;  /* this is key*/
    dvmh_omp_interval *interval; /* this is value */
    UT_hash_handle hh;
} registered_interval;

registered_interval *registered_intervals = NULL; /* this is hash */

dvmh_omp_interval *dvmh_omp_interval_register(context_descriptor *d)
{
    registered_interval *r;
    HASH_FIND_PTR(registered_intervals, &d, r);
    if (r == NULL){
        r = (registered_interval *) malloc(sizeof(registered_interval));
        r->interval = dvmh_omp_interval_create();
        dvmh_omp_interval_set_descriptor(r->interval, d);
        HASH_ADD_PTR(registered_intervals, descriptor, r);
    }
    return r->interval;
}

void dvmh_omp_interval_finalize()
{
    //TODO delete registered_intervals
}

dvmh_omp_interval *dvmh_omp_interval_create()
{
    dvmh_omp_interval *i = (dvmh_omp_interval *) malloc(sizeof(dvmh_omp_interval));
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
    // TODO destroy occurences;
    free(i);
}

void dvmh_omp_interval_add_subinterval(dvmh_omp_interval *i, dvmh_omp_interval *s)
{
    if (!list_has_element(i->subintervals, s)){
        list_append_tail(i->subintervals, s);
    }
}

void dvmh_omp_interval_set_descriptor(dvmh_omp_interval *i, context_descriptor *d)
{
    i->descriptor = d;
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
