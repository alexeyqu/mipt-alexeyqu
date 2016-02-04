#include "matrix_max.h"

int read_matrix (matrix_t *matrix)
{
	if (matrix == NULL) ERROR ("Null pointer");

	register int i = 0;

	int res_input = fscanf (stdin, "%d", &matrix->N);
	assert (res_input == 1);

	matrix->size  = matrix->N * matrix->N;
	matrix->cells = (complex_t*) calloc (matrix->size, sizeof (complex_t));
	if (matrix->cells == NULL) 
	{
		ERROR ("Callor error");
	}

	for (i = 0; i < matrix->size; i++) 
	{
		res_input = fscanf (stdin, "%lg %lg", &matrix->cells[i].real, &matrix->cells[i].imag);
		assert (res_input == 2);
	}

	return 0;
}

double radius (complex_t number)
{
	return number.real*number.real + number.imag*number.imag;
}

int index_to_coordinates (int N, int index, int *col, int *row)
{
	*col = index % N;
	*row = index / N;

	return 0;
}

int print_matrix (matrix_t *matrix)
{
	if (matrix == NULL) ERROR ("Null pointer");

	printf ("MATRIX: N = %d, size = %d\n", matrix->N, matrix->size);

	register int i = 0;
	for (i = 0; i < matrix->size; i++) 
	{
		printf ("(%lg %lg) ", matrix->cells[i].real, matrix->cells[i].imag);

		if ((i + 1) % matrix->N == 0) printf ("\n");
	}

	return 0;
}

int find_limits_consecutive (matrix_t *matrix, cell_t *max, cell_t *min)
{
	if (matrix == NULL || 
		max == NULL ||
		min == NULL) ERROR ("Null pointer");

	max->index = 0;
	min->index = 0;

	max->Rsqr = 0.0;
	min->Rsqr = DBL_MAX;

	register int i = 0;
	double cur_radius = 0.0;

	for (i = 0; i < matrix->size; i++) 
	{
		cur_radius = radius (matrix->cells[i]);

		if (cur_radius > max->Rsqr) 
		{
			max->index = i;
			max->Rsqr  = cur_radius;
		}

		if (cur_radius < min->Rsqr) 
		{
			min->index = i;
			min->Rsqr  = cur_radius;
		}
	}

	return 0;
}

int find_limits (matrix_t *matrix, int n_of_threads, cell_t *max, cell_t *min)
{
	if (matrix == NULL || 
		max == NULL ||
		min == NULL) ERROR ("Null pointer");

	register int i = 0;

	thread_data_t *thr_data = (thread_data_t*) calloc (n_of_threads, sizeof (thread_data_t));
	if (thr_data == NULL) ERROR ("calloc error");

	for (i = 0; i < n_of_threads; i++) 
	{
		thr_data[i].thread_index = i;
		thr_data[i].n_of_threads = n_of_threads;
		thr_data[i].matrix 		 = matrix;

		pthread_create (&thr_data[i].thread_id, NULL, &thread_limits, &thr_data[i]);
	}

	for (i = 0; i < n_of_threads; i++) 
	{
		pthread_join (thr_data[i].thread_id, NULL);

		// printf ("%d\t%d\t%lg\t%d\t%lg\n", thr_data[i].thread_index, thr_data[i].max.index, thr_data[i].max.Rsqr, thr_data[i].min.index, thr_data[i].min.Rsqr);
	}

	cell_t MAX = {}, MIN = {};

	MAX.index = 0;
	MIN.index = 0;

	MAX.Rsqr = 0.0;
	MIN.Rsqr = DBL_MAX;

	for (i = 0; i < n_of_threads; i++) 
	{
		if (thr_data[i].max.Rsqr > MAX.Rsqr) 
		{
			MAX.index = thr_data[i].max.index;
			MAX.Rsqr  = thr_data[i].max.Rsqr;
		}

		if (thr_data[i].min.Rsqr < MIN.Rsqr) 
		{
			MIN.index = thr_data[i].min.index;
			MIN.Rsqr  = thr_data[i].min.Rsqr;
		}
	}

	// printf ("Total: %d\t%lg\t%d\t%lg\n", MAX.index, MAX.Rsqr, MIN.index, MIN.Rsqr);

	*max = MAX;
	*min = MIN;

	free (thr_data);

	return 0;
}

void *thread_limits (void *thread_arg)
{
	if (thread_arg == NULL) ERROR ("Null pointer");

	thread_data_t *data = (thread_data_t*) thread_arg;

	cell_t max = {}, min = {};

	max.index = data->thread_index;
	min.index = data->thread_index;

	max.Rsqr = 0.0;
	min.Rsqr = DBL_MAX;

	register int i = 0;
	double cur_radius = 0.0;

	for (i = data->thread_index; i < data->matrix->size; i += data->n_of_threads)
	{
		cur_radius = radius (data->matrix->cells[i]);

		if (cur_radius > max.Rsqr)
		{
			max.index = i;
			max.Rsqr  = cur_radius;
		}

		if (cur_radius < min.Rsqr)
		{
			min.index = i;
			min.Rsqr  = cur_radius;
		}
	}

	// printf ("thread_ %d\t%d\t%lg\t%d\t%lg\n", data->thread_index, max.index, max.Rsqr, min.index, min.Rsqr);

	data->max.index = max.index;
	data->max.Rsqr  = max.Rsqr;

	data->min.index = min.index;
	data->min.Rsqr  = min.Rsqr;

	return NULL;
}