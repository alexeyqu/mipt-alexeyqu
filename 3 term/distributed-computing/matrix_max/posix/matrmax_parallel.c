#include "matrix_max.h"

int main (int argc, char *argv[])
{
    if (argc != 2) ERROR ("Usage: ./matrmax_parallel n_of_threads");

    unsigned int n_of_threads = atoi (argv[1]);
    matrix_t matrix = {};
    cell_t max = {}, min = {};
    int max_col = 0, max_row = 0, min_col = 0, min_row = 0;
    register int i = 0;

    read_matrix (&matrix);

    find_limits_posix (&matrix, n_of_threads, &max, &min);

    index_to_coordinates (matrix.N, max.index, &max_col, &max_row);
    index_to_coordinates (matrix.N, min.index, &min_col, &min_row);

    printf ("\nMAXIMUM: index=%d, col=%d, row=%d, radius=%lg\n",
            max.index, max_col, max_row, max.Rsqr);

    printf ("MINIMUM: index=%d, col=%d, row=%d, radius=%lg\n",
            min.index, min_col, min_row, min.Rsqr);

    free (matrix.cells);

    return 0;
}