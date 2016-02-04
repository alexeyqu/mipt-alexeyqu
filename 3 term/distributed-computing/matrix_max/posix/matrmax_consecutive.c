#include "matrix_max.h"

int main (int argc, char *argv[])
{
    if (argc != 1) ERROR ("Usage: ./matrmax_consecutive");

    matrix_t matrix = {};
    cell_t max = {}, min = {};
    int max_col = 0, max_row = 0, min_col = 0, min_row = 0;

    read_matrix (&matrix);

    find_limits_consecutive (&matrix, &max, &min);

    index_to_coordinates (matrix.N, max.index, &max_col, &max_row);
    index_to_coordinates (matrix.N, min.index, &min_col, &min_row);

    printf ("\nMAXIMUM: index=%d, col=%d, row=%d, radius=%lg\n",
            max.index, max_col, max_row, max.Rsqr);

    printf ("MINIMUM: index=%d, col=%d, row=%d, radius=%lg\n",
            min.index, min_col, min_row, min.Rsqr);

    free (matrix.cells);

    return 0;
}