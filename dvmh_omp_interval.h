#ifndef INTERVAL_H
#define INTERVAL_H

#include "list.h"
#include "context_types.h"
#include "uthash.h"
#include "dvmh_omp_event.h"

/*
struct _events_occurrences;
struct _dvmh_omp_interval;

typedef struct _events_occurrences events_occurrences;
typedef struct _dvmh_omp_interval dvmh_omp_interval;
*/

typedef struct _events_occurrences {
    long thread_id;     /* key */
    list *events;
    UT_hash_handle hh;  /* makes this structure hashable */
} events_occurrences;

typedef struct _dvmh_omp_interval {
    context_descriptor *descriptor;
    list *subintervals;
    events_occurrences *occurrences; //thread_id to list of events
} dvmh_omp_interval;

dvmh_omp_interval *dvmh_omp_interval_register(context_descriptor *d);
void dvmh_omp_interval_finalize();
void dvmh_omp_interval_destroy(dvmh_omp_interval *i);
void dvmh_omp_interval_add_subinterval(dvmh_omp_interval *i, dvmh_omp_interval *s);
void dvmh_omp_interval_add_occurrence(dvmh_omp_interval *i, dvmh_omp_event *e);

#endif
