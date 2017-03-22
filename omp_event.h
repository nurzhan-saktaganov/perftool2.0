#ifndef EVENT_H
#define EVENT_H

#include "list.h"

typedef enum {
	OMP_EVENT_PARALLEL_REGION,
} omp_event_type;

typedef struct _omp_event {
	omp_event_type type;
	pthread_mutex_t *mutex;
	double begin_time;
	double end_time;
	long thread_id;
	list *subevents;
} omp_event;


omp_event *omp_event_create(omp_event_type event_type);
void omp_event_destroy(omp_event *event_p);
void omp_event_add_subevent(omp_event *event_p, omp_event *subevent_p);
void omp_event_set_begin_time(omp_event *event_p, double begin_time);
double omp_event_get_begin_time(omp_event *event_p);
void omp_event_set_end_time(omp_event *event_p, double end_time);
double omp_event_get_end_time(omp_event *event_p);
void omp_event_set_thread_id(omp_event *event_p, long thread_id);
long omp_event_get_thread_id(omp_event *event_p);

#endif
