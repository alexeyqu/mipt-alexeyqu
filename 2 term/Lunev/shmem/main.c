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
#define SEM_NUM 6

#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
}

#define ERROR_SEM( str )                                                    \
{                                                                           \
    perror (str);                                                           \
    if (semctl (sem_id, 0, SETVAL, 0) < 0 ||                                \
        semctl (sem_id, 1, SETVAL, 0) < 0 ||                                \
        semctl (sem_id, 2, SETVAL, 0) < 0 ||                                \
        semctl (sem_id, 3, SETVAL, 0) < 0 ||                                \
        semctl (sem_id, 4, SETVAL, 0) < 0 ||                                \
        semctl (sem_id, 5, SETVAL, 0) < 0) ERROR ("semctl error_sem");      \
    exit (EXIT_FAILURE);                                                    \
}

#define SEMCHK( str )                                               \
{                                                                   \
    if (semctl (sem_id, SEM_NUM, GETALL, debug) < 0)                \
        ERROR ("semchk");                                           \
    fprintf (stderr, "\n%s %d %d %d %d %d\n",                       \
        str, debug[0], debug[1], debug[2], debug[3], debug[4]);     \
}

struct inarg
{
    int mode;
    char* filename;
};

int get_input (struct inarg* inargs, int argc, char *argv[])
{
    if (inargs == NULL)
      ERROR ("inargs error");

    if (argc == 3)
    {
      inargs->mode = 1;
    }
    else if (argc == 2)
    {
       inargs->mode = 0;
    }
    else
    {
       ERROR ("Usage: ./shmem filename {mode}");
    }

    strcpy (inargs->filename, argv[1]);

    return 0;
}

int set_sem_op (struct sembuf buf[], int nofop, short sem_num, short sem_op, short sem_flg)
{
    if (buf == NULL) ERROR ("set_sem_op");

    buf[nofop].sem_num = sem_num;
    buf[nofop].sem_op  = sem_op;
    buf[nofop].sem_flg = sem_flg;

    return 0;
}

int main (int argc, char *argv[])
{
    struct inarg inargs = {0, 0};

    if ((inargs.filename = calloc (FILENAME_MAX, sizeof (char))) == NULL)
        ERROR ("calloc");

    int res_input = get_input (&inargs, argc, argv);
    if (res_input != 0) ERROR ("Input error");

    key_t sem_key = 0, shm_key = 0;
    int sem_id = 0, shm_id = 0;
    int shm_size = DATA_BLOCK_SIZE + 1;
    int8_t* shmem = NULL;
    struct sembuf op_buf[SEM_NUM];
    // [0] — empty, [1] — full, [2] — snd, [3] — rcv, [4] — deathifzero, [5] — deathone
    int8_t cur_str[DATA_BLOCK_SIZE];
    int8_t cur_size = 0;

    short debug[SEM_NUM] = {};

    if ((sem_key = ftok (".", 's')) < 0)
        ERROR ("ftok sem");

    if ((shm_key = ftok (".", 'm')) < 0)
        ERROR ("ftok shm");

    if ((sem_id = semget (sem_key, SEM_NUM, 0666 | IPC_CREAT)) < 0)
        ERROR ("semget");

    if ((shm_id = shmget (shm_key, shm_size, 0666 | IPC_CREAT)) < 0)
        ERROR ("shmget");

    if ((shmem = (int8_t *) shmat (shm_id, NULL, 0)) < 0)
        ERROR ("shmat");

    errno = 0;

    SEMCHK ("START");

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//_________________________________RECEIVER____________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
    if (inargs.mode == 0) // receiver
    {
    // RCV_CHOOSING_THE_ONE

        set_sem_op (op_buf, 0, 3, 0, IPC_NOWAIT); // only 1st rcv will pass it
        set_sem_op (op_buf, 1, 3, 1, SEM_UNDO); // thanks to
        if (semop (sem_id, op_buf, 2) < 0)
            ERROR ("rcv sync");

    // RCV_THE_ONE_WAS_CHOSEN

        set_sem_op (op_buf, 0, 4, 1, SEM_UNDO); // indicates that rcv is alive
        if (semop (sem_id, op_buf, 1) < 0)
            ERROR_SEM ("rcv birth");

        set_sem_op (op_buf, 0, 0, 1, 0);
        set_sem_op (op_buf, 1, 0, -1, SEM_UNDO); // this'll help snd to pass blocks in case of dead rcv
        if (semop (sem_id, op_buf, 2) < 0)
            ERROR_SEM ("rcv prevent death");

        set_sem_op (op_buf, 0, 4, -2, 0); // if snd is ready too
        set_sem_op (op_buf, 1, 4, 2, 0);
        if (semop (sem_id, op_buf, 2) < 0)
            ERROR_SEM ("rcv GO");

        perror ("RECEIVER STARTED");

        while (1)
        {
            set_sem_op (op_buf, 0, 4, -2, IPC_NOWAIT); // both alive
            set_sem_op (op_buf, 1, 4, 2, 0);
            set_sem_op (op_buf, 2, 1, -1, 0); // P (full)
            set_sem_op (op_buf, 3, 4, -2, IPC_NOWAIT); // still alive
            set_sem_op (op_buf, 4, 4, 2, 0);
            if (semop (sem_id, op_buf, 5) < 0)
                ERROR_SEM ("semop snd dead");

            cur_size = *shmem;

            if (cur_size == 0) break;

            memmove (cur_str, shmem + 1, cur_size);

            write (STDOUT_FILENO, cur_str, cur_size);

            *shmem = -1;

            set_sem_op (op_buf, 0, 4, -2, IPC_NOWAIT); // again alive
            set_sem_op (op_buf, 1, 4, 2, 0);
            set_sem_op (op_buf, 2, 0, 1, 0); // V (empty)
            set_sem_op (op_buf, 3, 4, -2, IPC_NOWAIT); // yet alive
            set_sem_op (op_buf, 4, 4, 2, 0);
            if (semop (sem_id, op_buf, 5) < 0)
                ERROR_SEM ("semop snd dead");
        }

        set_sem_op (op_buf, 0, 4, -2, IPC_NOWAIT);
        set_sem_op (op_buf, 1, 4, 2, 0);
        set_sem_op (op_buf, 2, 4, 1, 0); // waiting for snd to finish
        if (semop (sem_id, op_buf, 3) < 0)
            ERROR_SEM ("rcv end");

        perror ("GOT");
    }

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//__________________________________SENDER_____________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
    if (inargs.mode == 1) // sender
    {
// SND_CHOOSING_THE_ONE

        set_sem_op (op_buf, 0, 2, 0, IPC_NOWAIT);
        set_sem_op (op_buf, 1, 2, 1, SEM_UNDO);
        if (semop (sem_id, op_buf, 2) < 0)
            ERROR ("snd sync");

// SND_THE_ONE_WAS_CHOSEN

        if (semctl (sem_id, 0, SETVAL, 1) < 0)
            ERROR_SEM ("semctl snd start");
        if (semctl (sem_id, 1, SETVAL, 0) < 0)
            ERROR_SEM ("semctl snd start");

        FILE* input = fopen (inargs.filename, "r");
        if (input == NULL) ERROR_SEM ("fopen");

        int fd_in = fileno (input);

        set_sem_op (op_buf, 0, 4, 1, SEM_UNDO);
        if (semop (sem_id, op_buf, 1) < 0)
            ERROR_SEM ("snd birth");

        set_sem_op (op_buf, 0, 1, 1, 0);
        set_sem_op (op_buf, 1, 1, -1, SEM_UNDO);
        if (semop (sem_id, op_buf, 2) < 0)
            ERROR_SEM ("snd prevent death");

        set_sem_op (op_buf, 0, 4, -2, 0);
        set_sem_op (op_buf, 1, 4, 2, 0);
        if (semop (sem_id, op_buf, 2) < 0)
            ERROR_SEM ("snd GO");

        perror ("SENDER STARTED");

        while ((cur_size = read (fd_in, cur_str, DATA_BLOCK_SIZE)) >= 0)
        {
            set_sem_op (op_buf, 0, 4, -2, IPC_NOWAIT);
            set_sem_op (op_buf, 1, 4, 2, 0);
            set_sem_op (op_buf, 2, 0, -1, 0);
            set_sem_op (op_buf, 3, 4, -2, IPC_NOWAIT);
            set_sem_op (op_buf, 4, 4, 2, 0);
            if (semop (sem_id, op_buf, 5) < 0)
                ERROR_SEM ("semop rcv dead");

            *shmem = cur_size;

            memmove (shmem + 1, cur_str, cur_size);

            set_sem_op (op_buf, 0, 4, -2, IPC_NOWAIT);
            set_sem_op (op_buf, 1, 4, 2, 0);
            set_sem_op (op_buf, 2, 1, 1, 0);
            set_sem_op (op_buf, 3, 4, -2, IPC_NOWAIT);
            set_sem_op (op_buf, 4, 4, 2, 0);
            if (semop (sem_id, op_buf, 5) < 0)
                ERROR_SEM ("semop rcv dead");

            if (cur_size == 0) break;
        }

        if (cur_size < 0) ERROR_SEM ("read");

        set_sem_op (op_buf, 0, 4, -2, IPC_NOWAIT);
        set_sem_op (op_buf, 1, 4, 2, 0);
        set_sem_op (op_buf, 2, 4, 1, 0);
        if (semop (sem_id, op_buf, 3) < 0)
            ERROR_SEM ("snd end");

        set_sem_op (op_buf, 0, 4, -4, 0);
        if (semop (sem_id, op_buf, 1) < 0)
            ERROR_SEM ("snd end");

        perror ("SENT");

        fclose (input);
    }

    if (shmdt (shmem) < 0)
    ERROR_SEM ("shmdt");

    if (semctl (sem_id, 0, SETVAL, 0) < 0 ||
        semctl (sem_id, 1, SETVAL, 0) < 0 ||
        semctl (sem_id, 2, SETVAL, 0) < 0 ||
        semctl (sem_id, 3, SETVAL, 0) < 0 ||
        semctl (sem_id, 4, SETVAL, 0) < 0 ||
        semctl (sem_id, 5, SETVAL, 0) < 0) ERROR_SEM ("semctl end");

    free (inargs.filename);

    return 0;
}
