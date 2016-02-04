#include "matrix_max.h"

int main (int argc, char *argv[])
{
	if (argc != 2) ERROR ("Usage: ./matrmax n_of_procs_assigned_to_each_MPI_node");

	unsigned int n_of_threads = atoi (argv[1]);
	matrix_t matrix = {};
	cell_t max = {}, min = {};
	int max_col = 0, max_row = 0, min_col = 0, min_row = 0;
	register int i = 0;

	unsigned int num_tasks = 0, task_id = 0;

	int res = MPI_Init (&argc, &argv);
	if (res != MPI_SUCCESS)
	{
		printf ("Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, res);
	}

	MPI_Comm_rank (MPI_COMM_WORLD, &task_id);
	MPI_Comm_size (MPI_COMM_WORLD, &num_tasks);

	if (num_tasks < 2)
	{
		printf("Need at least two MPI tasks. Quitting...\n");
		MPI_Abort(MPI_COMM_WORLD, res);
		exit(1);
	}

	find_limits_mpi (&matrix, num_tasks, task_id, n_of_threads, &max, &min);

	MPI_Finalize();

	if (task_id != 0) return 0;

	index_to_coordinates (matrix.N, max.index, &max_col, &max_row);
	index_to_coordinates (matrix.N, min.index, &min_col, &min_row);

	printf ("\nMAXIMUM: index=%d, col=%d, row=%d, radius=%lg\n",
			max.index, max_col, max_row, max.Rsqr);

	printf ("MINIMUM: index=%d, col=%d, row=%d, radius=%lg\n",
			min.index, min_col, min_row, min.Rsqr);

	free (matrix.cells);

	return 0;
}