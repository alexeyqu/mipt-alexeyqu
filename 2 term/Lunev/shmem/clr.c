#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define DATA_BLOCK_SIZE 100

#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
}

int main (int argc, char *argv[])
{
    key_t sem_key = 0, shm_key = 0;
    int sem_id = 0, shm_id = 0;
    int shm_size = DATA_BLOCK_SIZE + 1;
    int sem_num = 6;

    if ((sem_key = ftok (".", 's')) < 0)
        ERROR ("ftok sem");

    if ((shm_key = ftok (".", 'm')) < 0)
        ERROR ("ftok shm");

    if ((sem_id = semget (sem_key, sem_num, 0666 | IPC_CREAT)) < 0)
        ERROR ("semget");

    if ((shm_id = shmget (shm_key, shm_size, 0666 | IPC_CREAT)) < 0)
        ERROR ("shmget");

    if (semctl (sem_id, 0, IPC_RMID) < 0)
        ERROR ("semctl");

    if (shmctl (shm_id, IPC_RMID, NULL) < 0)
        ERROR ("shmctl");

    return 0;
}


