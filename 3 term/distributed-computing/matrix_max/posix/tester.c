 #include "matrix_max.h"

#define N_OF_SZS 8
#define N_OF_THR 9

int main (int argc, char *argv[])
{
    int seed = 0;

    if (argc == 2)
    {
        seed = atoi (argv[1]);
    }
    else
    {
        seed = time (NULL);
    }

    srand (seed);

    int matrix_szs[] = {1, 3, 10, 100, 500, 1000, 3000, 5000};
    int matrix_thr[] = {1, 2, 3, 4, 6, 8, 16, 24, 32};

    matrix_t matrix = {};
    cell_t max = {},        min = {};
    cell_t check_max = {},  check_min = {};
    int max_col = 0, max_row = 0, min_col = 0, min_row = 0;
    int N = 0;
    register int i_sz = 0, i_thr = 0;

    struct timeval t1 = {}, t2 = {};
    double elapsedTime = 0.0;

    FILE *stats = fopen ("stats.csv", "w");
    if (stats == NULL)
    {
        ERROR ("fopen error");
    }

    fprintf (stats, "\t0\t");

    for (i_thr = 0; i_thr < N_OF_THR; i_thr++)
    {
        fprintf (stats, "%d\t", matrix_thr[i_thr]);
    }

    for (i_sz = 0; i_sz < N_OF_SZS; i_sz++) 
    {
        // printf ("\n=========================================\n\nMATRIX_SIZE = %d\n", matrix_szs[i_sz]);

        fprintf (stats, "\n%d\t", matrix_szs[i_sz]);

        N = matrix_szs[i_sz];

        generate_matrix (N, &matrix);

        gettimeofday(&t1, NULL);

        find_limits_consecutive (&matrix, &check_max, &check_min);

        gettimeofday(&t2, NULL);

        elapsedTime =  (t2.tv_sec - t1.tv_sec)   * 1000.0;  
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; 

        fprintf (stats, "%lg\t", elapsedTime);

        for (i_thr = 0; i_thr < N_OF_THR; i_thr++) // testfunc
        {
            // printf ("\nN_OF_THREADS = %d\n", matrix_thr[i_thr]);

            gettimeofday(&t1, NULL);

            find_limits_posix (&matrix, matrix_thr[i_thr], &max, &min);

            gettimeofday(&t2, NULL);

            if (max.index != check_max.index && max.Rsqr != check_max.Rsqr ||
                min.index != check_min.index && min.Rsqr != check_min.Rsqr)
            {
                printf ("\ngot:%d %d\nexpected: %d %d\n", max.index, check_max.index, min.index, check_min.index);
                ERROR ("Wrong result");
            }

            elapsedTime  = (t2.tv_sec - t1.tv_sec)   * 1000.0;  
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; 

            fprintf (stats, "%lg\t", elapsedTime);
        }
    }

    fclose (stats);

    return 0;
}

int generate_integer (int lower, int upper)
{
    return rand() % (upper - lower + 1) + lower;
}

int generate_matrix (int N, matrix_t *matrix)
{
    int index = 0;

    register int i = 0;

    matrix->N       = N;
    matrix->size    = N * N;

    matrix->cells = (complex_t*) calloc (matrix->size, sizeof (complex_t));
    if (matrix->cells == NULL)
    {
        ERROR ("Callor error");
    }

    // cell_t max = {}, min = {};

    // max.index = 0;
    // min.index = 0;

    // max.Rsqr = 0.0;
    // min.Rsqr = DBL_MAX;
    // double cur_radius = 0.0;

    for (i = 0; i < matrix->size; i++)
    {
        matrix->cells[i].real = generate_integer (-100, 100);
        matrix->cells[i].imag = generate_integer (-100, 100);

        // cur_radius = radius (matrix->cells[i]);

        // if (cur_radius > max.Rsqr)
        // {
        //  max.index = i;
        //  max.Rsqr = cur_radius;
        // }

        // if (cur_radius < min.Rsqr)
        // {
        //  min.index = i;
        //  min.Rsqr = cur_radius;
        // }
    }

    // printf ("Generated: max %d\t%lg\tmin %d\t%lg\n", max.index, max.Rsqr, min.index, min.Rsqr);
    // getchar();

    return 0;
}