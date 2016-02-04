#ifndef MATRIX_MAX_LIBRARY
#define MATRIX_MAX_LIBRARY

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <sys/wait.h>
#include <limits.h>
#include <float.h>
#include <sys/time.h>
#include <omp.h>
#include "/usr/include/mpi/mpi.h"

#define MASTER 0
#define FROM_MASTER 1
#define FROM_SLAVE 2
#define MIN_TAG 3
#define MAX_TAG 4

#define EXIT_FAILURE 1

#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
}

typedef struct complex 
{
	double real, imag;
} complex_t;

typedef struct cell 
{
	int index;
	double Rsqr;
} cell_t;

typedef struct matrix 
{
	int N, size;
	complex_t *cells;
} matrix_t;

typedef struct thread_data 
{
	pthread_t thread_id;
	int thread_index;
	int n_of_procs;
	cell_t max, min;
	matrix_t *matrix;
} thread_data_t;

typedef struct mpi_data
{
	cell_t cell;
	int task_id;
	int type;
} mpi_data_t;

double radius (complex_t number);

int index_to_coordinates (int N, int index, int *col, int *row);

int generate_integer (int lower, int upper);

int generate_matrix (int n, matrix_t *matrix);

int read_matrix (matrix_t *matrix);

int print_matrix (matrix_t *matrix);

int find_limits_consecutive (matrix_t *matrix, cell_t *max, cell_t *min);

void *thread_limits (void *thread_arg);

void master_task (matrix_t *matrix, int num_slaves, cell_t *max, cell_t *min);

void slave_task (int n_of_procs, int slave_id);

int find_limits_mpi (matrix_t *matrix, int num_tasks, int task_id, int n_of_procs, cell_t *max, cell_t *min);

#endif

