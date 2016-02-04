#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
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
        ERROR ("Usage: %s filename {mode}");
    }

    strcpy (inargs->filename, argv[1]);

    return 0;
}

int main (int argc, char *argv[])
{
    struct inarg inargs = {0, 0};

    if ((inargs.filename = calloc (FILENAME_MAX, sizeof (char))) == NULL)
        ERROR ("Alloc");

    int res_input = get_input (&inargs, argc, argv);
    if (res_input != 0) ERROR ("Input error");

//    printf ("%d %d file:\'%s\' fifo:\'%s\'", (int) getpid(), (int) mode, filename, fifoname);

    if (mkfifo ("my_fifo", 0666) == -1 && errno != EEXIST)
        ERROR ("Unable to create FIFO");

    if (inargs.mode == 0) // receiver
    {
// SYNC_RCV_START
        
        if (mkfifo ("fifo_rcv", 0666) == -1 && errno != EEXIST)
            ERROR ("Unable to create FIFO");

        int rcv_fifo = 0;

        errno = 0;

        int data_rcv[PIPE_BUF] = {getpid()};

        if ((rcv_fifo = open ("fifo_rcv", O_RDWR | O_NONBLOCK)) < 0)
            ERROR ("Rcv sync fifo could not be opened");

        if (fcntl (rcv_fifo, F_SETPIPE_SZ, PIPE_BUF) < 0)
            ERROR ("Rcv fcntl error");

        if (write (rcv_fifo, data_rcv, PIPE_BUF) != PIPE_BUF)
            ERROR ("Rcv sync write error");

// SYNC_RCV_END

        int r_fifo = 0;

        if ((r_fifo = open ("my_fifo", O_RDONLY)) < 0)
            ERROR ("Unable to open FIFO: receiver");

        char* cur_str = calloc (PIPE_BUF, sizeof (char));
        int cur_size = 0;

        while ((cur_size = read (r_fifo, cur_str, PIPE_BUF)) > 0)
        {
            if (write (STDOUT_FILENO, cur_str, cur_size) != cur_size)
                ERROR ("Unable to print data");
        }

        perror ("PRINTED");

        free (cur_str);

        close (r_fifo);
        close (rcv_fifo);
    }

    if (inargs.mode == 1) // sender
    {
// SYNC_SND_START

        if (mkfifo ("fifo_snd", 0666) == -1 && errno != EEXIST)
            ERROR ("Unable to create FIFO");

        int snd_fifo = 0;

        errno = 0;

        int data_snd[PIPE_BUF] = {getpid()};

        if ((snd_fifo = open ("fifo_snd", O_RDWR | O_NONBLOCK)) < 0)
            ERROR ("Snd sync fifo could not be opened");

        if (fcntl (snd_fifo, F_SETPIPE_SZ, PIPE_BUF) < 0)
            ERROR ("Snd fcntl error");

        if (write (snd_fifo, data_snd, PIPE_BUF) != PIPE_BUF)
            ERROR ("Snd sync write error");

// SYNC_SND_END

        FILE* input = fopen (inargs.filename, "r");

        if (input == NULL)
            ERROR ("Invalid input file!");

        int fd_in = fileno (input);

        int w_fifo = 0;

        if ((w_fifo = open ("my_fifo", O_WRONLY)) < 0)
            ERROR ("Unable to open FIFO: sender");

        char* cur_str = calloc (PIPE_BUF, sizeof (char));
        int cur_size = 0;

        while ((cur_size = read (fd_in, cur_str, PIPE_BUF)) > 0)
        {
            if (write (w_fifo, cur_str, cur_size) != cur_size)
                ERROR ("Unable to print data");
        }

        perror ("SENT");

        free (cur_str);

        close (snd_fifo);
        close (w_fifo);
        fclose (input);
    }

    free (inargs.filename);

    return 0;
}
