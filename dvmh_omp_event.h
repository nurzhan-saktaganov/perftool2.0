#ifndef DVMH_OMP_EVENT_H
#define DVMH_OMP_EVENT_H

#include <omp.h>
#include "list.h"
#include "context_types.h"

typedef enum {
	DVMH_OMP_EVENT_PARALLEL_REGION,
	DVMH_OMP_EVENT_THREAD,
	DVMH_OMP_EVENT_THREAD_UNPAIRED, //такие события нужно закрывать руками
} dvmh_omp_event_type;

typedef struct _dvmh_omp_event {
	dvmh_omp_event_type type;
	omp_lock_t *mutex;
	double begin_time;
	double end_time;
	long thread_id;
	list *subevents;
	context_descriptor *descriptor;
} dvmh_omp_event;

typedef struct _dvmh_omp_subevent_iterator {
	list_iterator *li;
} dvmh_omp_subevent_iterator;

dvmh_omp_event *dvmh_omp_event_create(dvmh_omp_event_type event_type);
void dvmh_omp_event_destroy(dvmh_omp_event *event_p);
void dvmh_omp_event_add_subevent(dvmh_omp_event *event_p, dvmh_omp_event *subevent_p);
void dvmh_omp_event_set_begin_time(dvmh_omp_event *event_p, double begin_time);
double dvmh_omp_event_get_begin_time(dvmh_omp_event *event_p);
void dvmh_omp_event_set_end_time(dvmh_omp_event *event_p, double end_time);
double dvmh_omp_event_get_end_time(dvmh_omp_event *event_p);
double dvmh_omp_event_duration(dvmh_omp_event *e);
void dvmh_omp_event_set_thread_id(dvmh_omp_event *event_p, long thread_id);
long dvmh_omp_event_get_thread_id(dvmh_omp_event *event_p);
dvmh_omp_event_type dvmh_omp_event_get_type(dvmh_omp_event *e);
int dvmh_omp_event_has_subevent(dvmh_omp_event *e);
void dvmh_omp_event_set_context_descriptor(dvmh_omp_event *e, context_descriptor *d);
context_descriptor * dvmh_omp_event_get_context_descriptor(dvmh_omp_event *e);

dvmh_omp_subevent_iterator * dvmh_omp_subevent_iterator_new(dvmh_omp_event *e);
int dvmh_omp_subevent_iterator_has_next(dvmh_omp_subevent_iterator *it);
dvmh_omp_event *dvmh_omp_subevent_iterator_next(dvmh_omp_subevent_iterator *it);
void dvmh_omp_subevent_iterator_destroy(dvmh_omp_subevent_iterator *it);

#endif
