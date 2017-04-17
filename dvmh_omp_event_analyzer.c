#include <stdio.h>
#include "dvmh_omp_event.h"
#include "dvmh_omp_interval.h"
#include "dvmh_omp_event_analyzer.h"


void dvmh_omp_event_analyzer(dvmh_omp_event *global_omp_event)
{
	dvmh_omp_interval *program = dvmh_omp_interval_build(global_omp_event);
	dvmh_omp_interval_destroy(program);
}

