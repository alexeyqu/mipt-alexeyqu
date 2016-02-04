#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

struct mymsg
{
    long mytype;
};

int get_N (int argc, char *argv[])
{
    int base;
    char *endptr, *str;
    long val;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s str [base]\n", argv[0]);
        exit(1);
    }

    str = argv[1];
    base = (argc > 2) ? atoi(argv[2]) : 10;

    errno = 0;
    val = strtol(str, &endptr, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
     || (errno != 0 && val == 0))
    {
        perror("strtol");
        exit(1);
    }

    if (endptr == str)
    {
        fprintf(stderr, "No digits were found\n");
        exit(1);
    }

    if (*endptr != '\0') printf("Further characters after number: %s\n", endptr);

    return val;
}

int get_msqid()
{
    int msqid = 0;

    key_t key = ftok ("/tmp", 'a');
    int msgflg = IPC_CREAT | 0666;

    if ((msqid = msgget (key, msgflg)) == -1)
    {
        perror ("msgget failed");
        exit(1);
    }

    return msqid;
}

int main(int argc, char *argv[])
{
    int N = get_N (argc, argv);
    int msqid = get_msqid();
    int i = 0;
    int pid = 0;

    if (N == -1)
    {
        if (msgctl (msqid, IPC_RMID, 0) == -1)
        {
            perror ("msgctl failed");
            exit (1);
        }

        return 0;
    }

    size_t msglen = 0; //sizeof (struct mymsg) - sizeof (long);

    for (i = 0; i < N; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror ("Error in fork\n");
            exit(1);
        }

        if (pid == 0)
        {
          //  fprintf (stdout, "[%d, %d]\n", i, getpid());
          //  fflush (stdout);
            break;
        }
    }

    if (pid == 0)
    {
        struct mymsg msgc = {0};

        if (msgrcv (msqid, &msgc, msglen, N - i, 0) == -1)
        {
            perror ("msgrcv failed");
            exit (1);
        }

//        fflush  (stdout);
        fprintf (stdout, "<%d, %d> ", i, getpid());
        fflush  (stdout);

        if (i < N)
        {
            msgc.mytype = N - i + 1;

            if (msgsnd (msqid, &msgc, msglen, 0) == -1)
            {
                perror ("2msgsnd failed");
                exit (1);
            }
        }
    }
    else
    {
        struct mymsg msgp = {1};

        if (msgsnd (msqid, &msgp, msglen, 0) == -1)
        {
            perror ("msgsnd failed");
            exit(1);
        }

        if (msgrcv (msqid, &msgp, msglen, N + 1, 0) == -1)
        {
            perror ("msgrcv failed");
            exit (1);
        }
    }

    return 0;
}
