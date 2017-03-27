#include <stdio.h>
#include "dvmh_omp_event.h"
#include "dvmh_omp_event_analyzer.h"

double calculate_real_time(dvmh_omp_event *e);
void _calculate_real_time(dvmh_omp_event *e, double *t, long master_thread_id);

void dvmh_omp_event_analyzer(dvmh_omp_event *global_omp_event)
{
	double duration = dvmh_omp_event_duration(global_omp_event);
	double real_time = calculate_real_time(global_omp_event);
	fprintf(stderr, "GEA: program durarion: %lf\n", duration);
	fprintf(stderr, "GEA: program real time: %lf\n", real_time);
	return;
}

double calculate_real_time(dvmh_omp_event *e)
{
	double real_time = 0.0;
	long thread_id = dvmh_omp_event_get_thread_id(e);
	_calculate_real_time(e, &real_time, thread_id);
	real_time += dvmh_omp_event_duration(e);
	return real_time;
}

void _calculate_real_time(dvmh_omp_event *e, double *t, long master_thread_id)
{
	long thread_id = dvmh_omp_event_get_thread_id(e);
	if (thread_id != master_thread_id){
		*t += dvmh_omp_event_duration(e);
		return;
	}
	if (!dvmh_omp_event_has_subevent(e)){
		return;
	}
	dvmh_omp_event *subevent;
	dvmh_omp_subevent_iterator *it = dvmh_omp_subevent_iterator_new(e);
	while (dvmh_omp_subevent_iterator_has_next(it)){
		subevent = dvmh_omp_subevent_iterator_next(it);
		_calculate_real_time(subevent, t, master_thread_id);
	}
	dvmh_omp_subevent_iterator_destroy(it);
}
