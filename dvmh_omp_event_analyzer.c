#include <stdio.h>
#include "dvmh_omp_event.h"
#include "dvmh_omp_event_analyzer.h"


void dvmh_omp_event_analyzer(dvmh_omp_event *global_omp_event)
{
	double duration = dvmh_omp_event_get_end_time(global_omp_event);
	duration -= dvmh_omp_event_get_begin_time(global_omp_event);
	fprintf (stderr, "GEA: program durarion: %lf\n", duration);
	return;
}
