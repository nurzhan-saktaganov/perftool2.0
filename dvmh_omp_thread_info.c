#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "dvmh_omp_event.h"
#include "dvmh_omp_thread_info.h"

struct _dvmh_omp_thread_info {
	stack *active_events;
};

dvmh_omp_thread_info *dvmh_omp_thread_info_create()
{
	dvmh_omp_thread_info *info =
		(dvmh_omp_thread_info *) malloc(sizeof(dvmh_omp_thread_info));
	info->active_events = stack_create();
	return info;
}

void dvmh_omp_thread_info_destroy(dvmh_omp_thread_info *info)
{
	stack_destroy(info->active_events, NULL);
	free(info);
	return;
}

void dvmh_omp_thread_info_event_occured(
		dvmh_omp_thread_info *info,
		dvmh_omp_event *event)
{
	stack_push(info->active_events, event);
}

dvmh_omp_event *dvmh_omp_thread_info_event_finished(dvmh_omp_thread_info *info)
{
	return (dvmh_omp_event *) stack_pop(info->active_events);
}

dvmh_omp_event *dvmh_omp_thread_info_get_active_event(dvmh_omp_thread_info *info)
{
	return (dvmh_omp_event *) stack_peek(info->active_events);
}

int dvmh_omp_thread_info_is_alive(dvmh_omp_thread_info *info)
{
	return stack_size(info->active_events) > 0;
}
