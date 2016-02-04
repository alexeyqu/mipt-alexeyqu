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