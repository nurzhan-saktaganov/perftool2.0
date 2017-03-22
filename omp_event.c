#include <stdlib.h>
#include <pthread.h>
#include "list.h"
#include "omp_event.h"

omp_event *omp_event_create(omp_event_type event_type)
{
	omp_event *event_p = (omp_event *) malloc(sizeof(omp_event));
	event_p->type = event_type;
	if (event_p->type == OMP_EVENT_PARALLEL_REGION) {
		event_p->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(event_p->mutex, NULL);
	} else {
		event_p->mutex = NULL;
	}
	event_p->begin_time = 0.0;
	event_p->end_time = 0.0;
	event_p->subevents = NULL;
	return event_p;
}

void omp_event_destroy(omp_event *event_p)
{
	if (event_p == NULL){
		return;
	}
	omp_event *subevent_p;
	list_iterator *li_p = list_iterator_new(event_p->subevents);
	while (list_iterator_has_next(li_p)){
		subevent_p = (omp_event *) list_iterator_next(li_p);
		omp_event_destroy(subevent_p);
	}
	list_iterator_destroy(li_p);
	list_destroy(event_p->subevents, NULL);
	free(event_p);
	return;
}

void omp_event_add_subevent(omp_event *event_p, omp_event *subevent_p)
{
	if (event_p->type == OMP_EVENT_PARALLEL_REGION) {
		pthread_mutex_lock(event_p->mutex);
	}
	
	if (event_p->subevents == NULL){
		event_p->subevents = list_create();
	}
	
	list_append_tail(event_p->subevents, subevent_p);
	
	if (event_p->type == OMP_EVENT_PARALLEL_REGION) {
		pthread_mutex_unlock(event_p->mutex);
	}
	return;
}

void omp_event_set_begin_time(omp_event *event_p, double begin_time)
{
	event_p != NULL ? event_p->begin_time = begin_time : 0;
}

double omp_event_get_begin_time(omp_event *event_p)
{
	return event_p != NULL ? event_p->begin_time : 0.0;
}

void omp_event_set_end_time(omp_event *event_p, double end_time)
{
	event_p != NULL ? event_p->end_time = end_time : 0;
}

double omp_event_get_end_time(omp_event *event_p)
{
	return event_p != NULL ? event_p->end_time : 0.0;
}

void omp_event_set_thread_id(omp_event *event_p, long thread_id)
{
	event_p != NULL ? event_p->thread_id = thread_id : 0;
}

long omp_event_get_thread_id(omp_event *event_p)
{
	return event_p != NULL ? event_p->thread_id : 0;
}