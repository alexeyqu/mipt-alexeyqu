#include "matrix_max.h"

void master_task (matrix_t *matrix, int num_slaves, cell_t *max, cell_t *min)
{
	// matrix_t matrix = {};
	// cell_t max = {}, min = {};
	mpi_data_t cur_extremum = {};
	int max_col = 0, max_row = 0, min_col = 0, min_row = 0;
	register int i = 0;

	unsigned int task_id = 0,
		msg_type = 0, cells_amount = 0,
		avgcells = 0, extracells = 0, offset = 0;

	MPI_Status status;

	read_matrix (matrix);

	// print_matrix (matrix);

	// printf ("Master started with %d slaves\n", num_slaves);

	avgcells = matrix->size / num_slaves;
	extracells = matrix->size % num_slaves;
	offset = 0;

	struct timeval t1 = {}, t2 = {};
    double elapsedTime = 0.0;

	gettimeofday(&t1, NULL);

	for (task_id = 1; task_id <= num_slaves; task_id++)
	{
		cells_amount = (task_id <= extracells)? avgcells + 1 : avgcells;

		// printf ("snd %d cells to %d slave\n", cells_amount, task_id + 1);

		MPI_Send (&cells_amount, sizeof (int), MPI_BYTE, task_id, FROM_MASTER, MPI_COMM_WORLD);

		int cur_cell = 0;
		for (cur_cell = 0; cur_cell < cells_amount; cur_cell++)
		{
			MPI_Send (&matrix->cells[offset + cur_cell], sizeof (complex_t), MPI_BYTE, task_id, FROM_MASTER, MPI_COMM_WORLD);
			offset++;
		}
	}

	for (task_id = 0; task_id < 2 * num_slaves; task_id++)
	{
		MPI_Recv (&cur_extremum, sizeof(mpi_data_t), MPI_BYTE, MPI_ANY_SOURCE, FROM_SLAVE, MPI_COMM_WORLD, &status);

		// printf ("%d %d %lg\n", cur_extremum.task_id, cur_extremum.type, cur_extremum.cell.Rsqr);

		if(cur_extremum.type == MIN_TAG)
		{
			if(cur_extremum.cell.Rsqr < min->Rsqr)
			{
				min->Rsqr = cur_extremum.cell.Rsqr;
				min->index = cur_extremum.task_id * avgcells + (cur_extremum.task_id < extracells)? cur_extremum.task_id : extracells;
			}
		}
		else if (cur_extremum.type == MAX_TAG)
		{
			if(cur_extremum.cell.Rsqr > max->Rsqr)
			{
				max->Rsqr = cur_extremum.cell.Rsqr;
				max->index = cur_extremum.task_id * avgcells + (cur_extremum.task_id < extracells)? cur_extremum.task_id : extracells;
			}
		}
		else ERROR ("Wrong tag!");
	}

	gettimeofday(&t2, NULL);

	elapsedTime =  (t2.tv_sec - t1.tv_sec)   * 1000.0;  
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; 

	fprintf (stdout, "Time elapsed: %lg\n", elapsedTime);
}

void slave_task (int n_of_procs, int slave_id)
{
	matrix_t matrix = {};
	cell_t min = {}, max = {};
	mpi_data_t min_data = {}, max_data = {};

	int n_of_cells = 0;
	MPI_Recv (&n_of_cells, sizeof (int), MPI_BYTE, MASTER, FROM_MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	matrix.N = 0;
	matrix.size = n_of_cells;
	matrix.cells = (complex_t *) calloc (n_of_cells, sizeof (complex_t));

	int cur_cell = 0;
	for (cur_cell = 0; cur_cell < n_of_cells; cur_cell++)
	{
		MPI_Recv (&matrix.cells[cur_cell], sizeof (complex_t), MPI_BYTE, MASTER, FROM_MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	find_limits_openmp (&matrix, n_of_procs, &max, &min);

	min_data.cell = min;
	min_data.task_id = slave_id;
	min_data.type = MIN_TAG;

	max_data.cell = max;
	max_data.task_id = slave_id;
	max_data.type = MAX_TAG;

	MPI_Send (&min_data, sizeof (mpi_data_t), MPI_BYTE, MASTER, FROM_SLAVE, MPI_COMM_WORLD);
	MPI_Send (&max_data, sizeof (mpi_data_t), MPI_BYTE, MASTER, FROM_SLAVE, MPI_COMM_WORLD);

	free(matrix.cells);
}

int find_limits_mpi (matrix_t *matrix, int num_tasks, int task_id, int n_of_procs, cell_t *max, cell_t *min)
{
	unsigned int num_slaves = num_tasks - 1;

	if (task_id == MASTER)
	{
		master_task (matrix, num_slaves, max, min);
	}
	if (task_id > MASTER)
	{
		slave_task (n_of_procs, task_id);
	}

	return 0;
}