#include <assert.h>
#include <stdio.h>
#include "dvmh_omp_event.h"
#include "dvmh_omp_interval.h"
#include "dvmh_omp_event_analyzer.h"

static void write_characteristics_to_csv(const char *filename, dvmh_omp_interval *i);

void dvmh_omp_event_analyzer(dvmh_omp_event *global_omp_event)
{
	dvmh_omp_interval *program = dvmh_omp_interval_build(global_omp_event);
	write_characteristics_to_csv("intervals_stat.csv", program);
	dvmh_omp_interval_destroy(program);
}

static void _write_characteristics_to_csv(FILE *fd, dvmh_omp_interval *i, long parent_id)
{
	long id = dvmh_omp_interval_get_id(i);
	dvmh_omp_subinterval_iterator *it = dvmh_omp_subinterval_iterator_new(i);
	while (dvmh_omp_subinterval_iterator_has_next(it)){
		dvmh_omp_interval *s = dvmh_omp_subinterval_iterator_next(it);
		_write_characteristics_to_csv(fd, s, id);
	}
	dvmh_omp_subinterval_iterator_destroy(it);

	fprintf(fd, "%ld,", dvmh_omp_interval_get_id(i));
	fprintf(fd, "%d,", dvmh_omp_interval_get_calls_count(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_io_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_execution_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_sync_barrier_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_user_time(i));
	fprintf(fd, "%d,", dvmh_omp_interval_get_used_threads_number(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_idle_critical_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_sync_flush_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_idle_parallel_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_load_imbalance_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_thread_load_max(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_thread_load_min(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_thread_load_avg(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_total_time(i));
	fprintf(fd, "%lf", dvmh_omp_interval_get_lost_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_productive_time(i));
	fprintf(fd, "%lf,", dvmh_omp_interval_get_efficiency(i));
	fprintf(fd, "%d,", dvmh_omp_interval_is_in_parallel(i));
	fprintf(fd, "%d,", dvmh_omp_interval_begin_line(i));
	if (parent_id == -1){
		fprintf(fd, "%ld", parent_id);
	} else {
		fprintf(fd, "%ld\r\n", parent_id);
	}
}

static void write_characteristics_to_csv(const char *filename, dvmh_omp_interval *i)
{
	FILE *fd = fopen(filename, "w");
	assert(fd);

	/* write characteristics names */
	fprintf(fd, "id,");
	fprintf(fd, "calls count,");
	fprintf(fd, "io time,");
	fprintf(fd, "execution time,");
	fprintf(fd, "idle barrier time,");
	fprintf(fd, "user time,");
	fprintf(fd, "used threads,");
	fprintf(fd, "idle critical time,");
	fprintf(fd, "flush time,");
	fprintf(fd, "idle parallel,");
	fprintf(fd, "load imbalance time,");
	fprintf(fd, "thread load max,");
	fprintf(fd, "thread load min,");
	fprintf(fd, "thread load avg,");
	fprintf(fd, "total time,");
	fprintf(fd, "lost time,");
	fprintf(fd, "productive time,");
	fprintf(fd, "efficiency,");
	fprintf(fd, "in parallel,");
	fprintf(fd, "begin line,");
	fprintf(fd, "parent id\r\n");
	/* write corresponding charactertistics */
	_write_characteristics_to_csv(fd, i, -1);
	fclose(fd);
}
