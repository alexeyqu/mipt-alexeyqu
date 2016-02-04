#include "matrix_max.h"

int find_limits_openmp (matrix_t *matrix, int n_of_threads, cell_t *max, cell_t *min)
{
	if (matrix == NULL || 
		max == NULL ||
		min == NULL) ERROR ("Null pointer");

	omp_set_num_threads (n_of_threads); 

	max->index = 0; //maxmin_init
	min->index = 0;

	max->Rsqr = 0.0;
	min->Rsqr = DBL_MAX;

	#pragma omp parallel
	{
		cell_t current_min = {}, current_max = {};

		current_max.index = 0;
		current_min.index = 0;

		current_max.Rsqr = 0.0;
		current_min.Rsqr = DBL_MAX;

		register int i = 0;
		double cur_radius = 0.0;

		#pragma omp for
		for (i = 0; i < matrix->size; i++) 
		{
			cur_radius = radius (matrix->cells[i]);

			if (cur_radius > current_max.Rsqr) // seek_maxmin
			{
				current_max.index = i;
				current_max.Rsqr  = cur_radius;
			}

			if (cur_radius < current_min.Rsqr) 
			{
				current_min.index = i;
				current_min.Rsqr  = cur_radius;
			}
		}

		#pragma omp critical
		{
			if (current_max.Rsqr > max->Rsqr)
			{
				max->index = current_max.index;
				max->Rsqr = current_max.Rsqr;
			}

			if (current_min.Rsqr < min->Rsqr)
			{
				min->index = current_min.index;
				min->Rsqr = current_min.Rsqr;
			}
		}
	}

	return 0;
}