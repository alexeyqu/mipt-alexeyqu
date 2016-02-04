#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/select.h>

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#define MIN_BUF_SIZE 1111
#define READ 0
#define WRITE 1
#define DEBUGJ 0

#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
}

struct inarg
{
    char* filename;
    long N;
};

struct piper
{
    int size, effective_size;
    int8_t* buf;
    int din, dout;
    int alive;
};

int get_input (struct inarg* inargs, int argc, char *argv[])
{
    if (inargs == NULL)
        ERROR ("inargs error");

    if (argc != 3) ERROR ("Usage: ./select filename num_of_children\n")//;
    else
    {
        char* str = argv[2], *endptr = NULL;
        long val = 0;

        errno = 0;
        val = strtol(str, &endptr, 10);

        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
         || (errno != 0 && val == 0))
            ERROR ("strtol");

        if (endptr == str)
            ERROR ("No digits")

        if (*endptr != '\0') printf("Further characters after number: %s\n", endptr);

        inargs->N = val;
        strcpy (inargs->filename, argv[WRITE]);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct inarg inargs = {0, 0};

    if ((inargs.filename = calloc (FILENAME_MAX, sizeof (char))) == NULL)
        ERROR ("Alloc");

    int res_input = get_input (&inargs, argc, argv);
    if (res_input != 0) ERROR ("Input error");

    int i = 0, j = 0;
    int pid = 0;

    struct piper pipers[inargs.N]; // array of parent
    struct piper little_piper; // every child has it

    fd_set inset, outset;

    FD_ZERO (&inset);
    FD_ZERO (&outset);

    for (i = 0; i < inargs.N; i++)
    {
        int partoch[2] = {};
        int chtopar[2] = {};

        if (pipe (partoch) == -1 || pipe (chtopar) == -1)
            ERROR ("pipe");

        pid = fork();

        if (pid < 0) ERROR ("fork");

        if (pid == 0) //child
        {
            if (i == 0) little_piper.din = STDIN_FILENO;
            else little_piper.din = partoch[READ];
            little_piper.dout = chtopar[WRITE];
            little_piper.size = MIN_BUF_SIZE*(inargs.N);
            little_piper.effective_size = 0;
            little_piper.alive  = 1;
            little_piper.buf  = calloc (little_piper.size, 1);
            if (little_piper.buf == NULL) ERROR ("calloc child buf");

            close (partoch[WRITE]);
            close (chtopar[READ]);

            break;
        }
        else //parent
        {
            pipers[i].din = chtopar[READ];
            if (i > 0) pipers[i - 1].dout = partoch[WRITE];
            if (i == inargs.N - 1) pipers[i].dout = STDOUT_FILENO;
            pipers[i].size = MIN_BUF_SIZE*(inargs.N - i);
            pipers[i].effective_size = 0;
            pipers[i].alive  = 1;
            pipers[i].buf = calloc (pipers[i].size, 1);
            if (pipers[i].buf == NULL) ERROR ("calloc parent");

            if (fcntl (pipers[i].din,  F_SETFL, O_NONBLOCK))
                ERROR ("fcntl parent nonblock");

            if (i > 0 && fcntl (pipers[i - 1].dout, F_SETFL, O_NONBLOCK))
                ERROR ("fcntl parent dout nonblock");

            close (partoch[READ]);
            close (chtopar[WRITE]);
        }
    }

    if (pid == 0) //child
    {
        for (j = 0; j < i; j++)
        {
            if (pipers[j].alive == 1)
            {
                close (pipers[j].din);
                if (j > 0) close (pipers[j - 1].dout);

                pipers[j].alive = 0;
            }
        }

        FILE* input = NULL;

        if (i == 0)
        {
            input = fopen (inargs.filename, "r");

            if (input == NULL)
                ERROR ("fopen");

            little_piper.din = fileno (input);
        }

        int cur_len = 0;

        while ((cur_len = read (little_piper.din, little_piper.buf, little_piper.size)) > 0)
        {
            if (write (little_piper.dout, little_piper.buf, cur_len) < 0)
                ERROR ("write child");
        }

        if (cur_len < 0) ERROR ("read child");

        close (little_piper.din);
        close (little_piper.dout);
    }
    else //parent
    {
        int retval = 0;
        int maxfd = 0;
        int res_rd = 0;
        int still_alive = 0;

        while (1)
        {
            maxfd = 0;
            still_alive = 0;
            FD_ZERO (&inset);
            FD_ZERO (&outset);

            for (j = 0; j < inargs.N; j++)
            {
                if (pipers[j].alive == 1)
                {
                    if (pipers[j].effective_size == 0)
                    {
                        FD_SET (pipers[j].din, &inset);
                        maxfd = max (maxfd, pipers[j].din);
                    }

                    if (pipers[j].effective_size != 0)
                    {
                        FD_SET (pipers[j].dout, &outset);
                        maxfd = max (maxfd, pipers[j].dout);
                    }

                    still_alive++;
                }
            }

            if (still_alive == 0) break;

            retval = select (maxfd + 1, &inset, &outset, NULL, NULL);

            if (retval > 0)
            {
                for (j = 0; j < inargs.N; j++)
                {
                    if (FD_ISSET (pipers[j].din, &inset))
                    {
                        if ((res_rd = read (pipers[j].din, pipers[j].buf, pipers[j].size)) > 0)
                        {
                            pipers[j].effective_size = res_rd;
                        }

                        if (res_rd == 0)
                        {
                            pipers[j].alive = 0;
                            pipers[j].effective_size = 0;
                            close (pipers[j].din);
                            close (pipers[j].dout);
                        }
                    }

                    if (FD_ISSET (pipers[j].dout, &outset))
                    {
                        int res_wr = 0;

                        if ((res_wr = write (pipers[j].dout, pipers[j].buf, pipers[j].effective_size)) < 0)
                            ERROR ("write parent");

                        pipers[j].effective_size -= res_wr;

                        if (res_wr != pipers[j].effective_size)
                        {
                            memmove (pipers[j].buf, pipers[j].buf + res_wr, pipers[j].effective_size);
                        }
                    }
                }
            }
            else ERROR ("select");
        }

        for (i = 0; i < inargs.N; i++)
        {
            int state = 0;
            wait(&state);
        }
    }

    return 0;
}
