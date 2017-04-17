#include <stdlib.h>
#include <omp.h>
#include "list.h"
#include "context_descriptor.h"
#include "dvmh_omp_event.h"

struct _dvmh_omp_event {
	dvmh_omp_event_type type;
	omp_lock_t *mutex;
	double begin_time;
	double end_time;
	long thread_id;
	list *subevents;
	context_descriptor *descriptor;
};

struct _dvmh_omp_subevent_iterator {
	list_iterator *li;
};

dvmh_omp_event *dvmh_omp_event_create(dvmh_omp_event_type event_type)
{
	dvmh_omp_event *event_p = (dvmh_omp_event *) malloc(sizeof(dvmh_omp_event));
	event_p->type = event_type;
	if (event_p->type == DVMH_OMP_EVENT_BEFORE_PARALLEL) {
		event_p->mutex = (omp_lock_t *) malloc(sizeof(omp_lock_t));
		omp_init_lock(event_p->mutex);
	} else {
		event_p->mutex = NULL;
	}
	event_p->begin_time = 0.0;
	event_p->end_time = 0.0;
	event_p->thread_id = 0L;
	event_p->subevents = NULL;
	event_p->descriptor = NULL;
	return event_p;
}

void dvmh_omp_event_destroy(dvmh_omp_event *event_p)
{
	if (event_p == NULL){
		return;
	}
	if (event_p->type == DVMH_OMP_EVENT_BEFORE_PARALLEL) {
		omp_destroy_lock(event_p->mutex);
		free(event_p->mutex);
	}
	dvmh_omp_event *subevent_p;
	list_iterator *li_p = list_iterator_new(event_p->subevents);
	while (list_iterator_has_next(li_p)){
		subevent_p = (dvmh_omp_event *) list_iterator_next(li_p);
		dvmh_omp_event_destroy(subevent_p);
	}
	list_iterator_destroy(li_p);
	list_destroy(event_p->subevents, NULL);
	free(event_p);
	return;
}

void dvmh_omp_event_add_subevent(dvmh_omp_event *event_p, dvmh_omp_event *subevent_p)
{
	if (event_p->type == DVMH_OMP_EVENT_BEFORE_PARALLEL) {
		omp_set_lock(event_p->mutex);
	}
	
	if (event_p->subevents == NULL){
		event_p->subevents = list_create();
	}
	
	list_append_tail(event_p->subevents, subevent_p);
	
	if (event_p->type == DVMH_OMP_EVENT_BEFORE_PARALLEL) {
		omp_unset_lock(event_p->mutex);
	}
	return;
}

void dvmh_omp_event_set_begin_time(dvmh_omp_event *event_p, double begin_time)
{
	event_p != NULL ? event_p->begin_time = begin_time : 0;
}

double dvmh_omp_event_get_begin_time(dvmh_omp_event *event_p)
{
	return event_p != NULL ? event_p->begin_time : 0.0;
}

void dvmh_omp_event_set_end_time(dvmh_omp_event *event_p, double end_time)
{
	event_p != NULL ? event_p->end_time = end_time : 0;
}

double dvmh_omp_event_get_end_time(dvmh_omp_event *event_p)
{
	return event_p != NULL ? event_p->end_time : 0.0;
}

double dvmh_omp_event_duration(dvmh_omp_event *e)
{
	return e != NULL ? e->end_time - e->begin_time : 0.0;
}

void dvmh_omp_event_set_thread_id(dvmh_omp_event *event_p, long thread_id)
{
	event_p != NULL ? event_p->thread_id = thread_id : 0;
}

long dvmh_omp_event_get_thread_id(dvmh_omp_event *event_p)
{
	return event_p != NULL ? event_p->thread_id : 0;
}

int dvmh_omp_event_has_subevent(dvmh_omp_event *e)
{
	return e->subevents != NULL;
}

dvmh_omp_event_type dvmh_omp_event_get_type(dvmh_omp_event *e)
{
	return e->type;
}

void dvmh_omp_event_set_context_descriptor(dvmh_omp_event *e, context_descriptor *d)
{
	e->descriptor = d;
}

context_descriptor *dvmh_omp_event_get_context_descriptor(dvmh_omp_event *e)
{
	return e->descriptor;
}

dvmh_omp_subevent_iterator * dvmh_omp_subevent_iterator_new(dvmh_omp_event *e)
{
	if (e == NULL || e->subevents == NULL){
		return NULL;
	}
	dvmh_omp_subevent_iterator *it =
			(dvmh_omp_subevent_iterator *) malloc(sizeof(dvmh_omp_subevent_iterator));
	it->li = list_iterator_new(e->subevents);
	return it;
}

int dvmh_omp_subevent_iterator_has_next(dvmh_omp_subevent_iterator *it)
{
	return it != NULL ? list_iterator_has_next(it->li) : 0;
}

dvmh_omp_event *dvmh_omp_subevent_iterator_next(dvmh_omp_subevent_iterator *it)
{
	return (dvmh_omp_event *) list_iterator_next(it->li);
}

void dvmh_omp_subevent_iterator_destroy(dvmh_omp_subevent_iterator *it)
{
	if (it == NULL){
		return;
	}
	list_iterator_destroy(it->li);
	free(it);
}
