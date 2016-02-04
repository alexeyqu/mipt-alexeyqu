#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>

#define SIG_ONE_BIT SIGUSR1
#define SIG_ZERO_BIT SIGUSR2

#define BITS_IN_BYTE 8

#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
}

struct inarg
{
    char* filename;
};

static volatile sig_atomic_t sigflag;

static void bit_code (int signo)
{
    if (signo == SIG_ONE_BIT) sigflag = 1;
    else sigflag = 2;
}

void exit_child (int signo)
{
    exit (EXIT_SUCCESS);
}

void exit_parent (int signo)
{
    exit (EXIT_SUCCESS);
}

void empty_handler (int signo) {}

void rdy_to_rcv (pid_t pid)
{
    kill (pid, SIGUSR1);
}

int get_input (struct inarg* inargs, int argc, char *argv[])
{
    if (inargs == NULL)
        ERROR ("inargs error");

    if (argc != 2) ERROR ("Usage: ./signals filename");

    strcpy (inargs->filename, argv[1]);

    return 0;
}

int byte_to_bits (int8_t byte, int8_t bits[])
{
    int i = 0;

    for (i = 0; i < BITS_IN_BYTE; i++)
    {
        bits[i] = byte >> 7;
        byte = byte << 1;
    }

    return 0;
}

int bits_to_byte (int8_t bits[], int8_t* byte)
{
    if (byte == NULL) ERROR ("NULL pointer");

    *byte = 1;
    int i = 0;

    for (i = 0; i < BITS_IN_BYTE; i++)
    {
        *byte = (*byte << 1) + bits[i];
    }

    return 0;
}

int main (int argc, char *argv[])
{
    int i = 0;
    sigset_t set, newmask, oldmask, zeromask;

    int8_t cur_byte = 0, cur_len = 0;
    int8_t bits[BITS_IN_BYTE] = {}; 

    struct sigaction act_exitchld;
    memset (&act_exitchld, 0, sizeof (act_exitchld));
    act_exitchld.sa_handler = exit_child;
    sigfillset (&act_exitchld.sa_mask);
    sigaction (SIGCHLD, &act_exitchld, NULL);

    struct sigaction act_bit;
    memset (&act_bit, 0, sizeof (act_bit));
    act_bit.sa_handler = bit_code;
    sigfillset (&act_bit.sa_mask);
    sigaction (SIGUSR1, &act_bit, NULL);
    sigaction (SIGUSR2, &act_bit, NULL);

    sigemptyset (&zeromask);
    sigemptyset (&newmask);

    sigaddset (&newmask, SIGUSR1);
    sigaddset (&newmask, SIGUSR2);
    sigaddset (&newmask, SIGCHLD);

    if (sigprocmask (SIG_BLOCK, &newmask, &oldmask) < 0)
        ERROR ("sigprocmask prepare SIG_BLOCK");

    int pid = fork();

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//__________________________________SENDER_____________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
    if (pid == 0)
    {
        perror ("SENDER");

        struct sigaction act_empty;
        memset (&act_empty, 0, sizeof (act_empty));
        act_empty.sa_handler = empty_handler;
        sigfillset (&act_empty.sa_mask);
        sigaction (SIGUSR1, &act_empty, NULL);

        struct sigaction act_exitparent;
        memset (&act_exitparent, 0, sizeof (act_exitparent));
        act_exitparent.sa_handler = exit_parent;
        sigfillset (&act_exitparent.sa_mask);
        sigaction (SIGHUP, &act_exitparent, NULL);

        if (prctl (PR_SET_PDEATHSIG, SIGHUP) < 0) //catch death of parent
            ERROR ("prctl");

        struct inarg inargs = {0};

        if ((inargs.filename = (char*) calloc (FILENAME_MAX, sizeof (char))) == NULL)
            ERROR ("Alloc");

        int res_input = get_input (&inargs, argc, argv);
        if (res_input != 0) ERROR ("Input error");

        FILE* input = fopen (inargs.filename, "r");
        if (input == NULL) ERROR ("fopen");

        int fd_in = fileno (input);

        pid_t ppid = getppid();

        while ((cur_len = read (fd_in, &cur_byte, 1)) == 1)
        {
            if (byte_to_bits (cur_byte, bits) < 0) ERROR ("byte_to_bits");

            for (i = 0; i < BITS_IN_BYTE; i++)
            {
                if (bits[i] == 0) kill (ppid, SIG_ZERO_BIT);
                else kill (ppid, SIG_ONE_BIT);

                sigsuspend (&zeromask);
            }
        }

        fclose (input);

        free (inargs.filename);

        perror ("SENT");

        exit (EXIT_SUCCESS);
    }

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//_________________________________RECEIVER____________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

    perror ("RECEIVER");

    int bitscounter = 0;

    while (1)
    {
        sigsuspend (&zeromask);

        bits[bitscounter++] = sigflag % 2;

        if (bitscounter == 8) 
        {
            bits_to_byte (bits, &cur_byte);

            write (STDOUT_FILENO, &cur_byte, 1);

            bitscounter = 0;
        }

        sigflag = 0;

        rdy_to_rcv (pid);
    }

    perror ("GOT");

    return 0;
}
