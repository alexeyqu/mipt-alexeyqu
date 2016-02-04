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
#include <math.h>
#include <sys/time.h>
// Try Vim
#define ERROR( str )        \
{                           \
    perror (str);           \
    exit (EXIT_FAILURE);    \
}

typedef struct my_msg
{
    long iproc;
	long double localsum;
} my_msg;

int get_msqid()
{
    int msqid = 0;

    key_t key = ftok ("/tmp", 'n');
    int msgflg = IPC_CREAT | 0666;

    if ((msqid = msgget (key, msgflg)) == -1)
    {
        perror ("msgget failed");
        exit(1);
    }

    return msqid;
}

long double compute_interval (long double a, long double b) // uses Newton-Cotes formula (n = 3)
{
	long double h = (double) (b - a)/3.0;

	long double sum = (double) (sin (a) + 3.0*sin(a + (double) h/3.0) + 3.0*sin(a + (double) 2.0*h/3.0) + sin (b))/8.0*3.0*h;

	return sum;
}

int main (int argc, char *argv[]) // integrate [sin(x), {x, 0, 10}] = 1.83907152908
{
    struct timeval t1, t2;
    double elapsedTime;

   	gettimeofday(&t1, NULL);

	int a = 0, b = 10;
	int Ninter = 0, Nprocs = 0;
	long double localsum = 0, width = 0;
	int msqid = get_msqid();
	register int iproc = 0, pid = 0;

	if (argc == 3)
	{
		Ninter = atoi (argv[1]);
		Nprocs = atoi (argv[2]);
	}
	else
	{
		ERROR ("Usage: newton %%d intervals %%d processes");
	}

	if (Nprocs == -1)
    {
        if (msgctl (msqid, IPC_RMID, 0) == -1)
        {
            perror ("msgctl failed");
            exit (1);
        }

        return 0;
    }

    size_t msglen = sizeof (my_msg);

	width = (double) (b - a)/Ninter;

    for (iproc = 0; iproc < Nprocs; iproc++)
    {
        pid = fork();

        if (pid < 0)
        {
            perror ("Error in fork\n");
            exit(1);
        }

        if (pid == 0)
        {
        	iproc++;
            break;
        }
    }

    if (pid == 0) // computer
    {
    	register int i = 0;
    	register double cur_a = 0, cur_b = 0;

    	localsum = 0;

    	for (i = iproc; i <= Ninter; i += Nprocs)
    	{
    		cur_a = a + width*(i - 1);
    		cur_b = cur_a + width;

    		localsum += compute_interval (cur_a, cur_b);
    	}

  		my_msg msglocalsum = {0, 0};

    	msglocalsum.localsum = localsum;
    	msglocalsum.iproc = iproc;

//    	fprintf(stderr, "%d \t %g \t %g \t %Lg\n", iproc, cur_a, cur_b, localsum);

        if (msgsnd (msqid, &msglocalsum, msglen, 0) == -1)
		{
			perror ("msgsnd failed");
			exit (1);
		}
    }
    else // collector
    {
        my_msg msg = {0, 0};

    	register int i = 0;
    	register long double sum = 0;

    	for (i = 0; i < Nprocs; i++)
    	{
	    	if (msgrcv (msqid, &msg, msglen, 0, 0) == -1)
			{
				perror ("msgrcv failed");
				exit (1);
			}

			sum += msg.localsum;
    	}

    	wait (NULL);

    	gettimeofday(&t2, NULL);

		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;  
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0; 

    	// printf("integrate [sin(x), {x, 0, 10}] = %Lg (estimated 1.83907152908)\n", sum);
    	// printf("Time elapsed: %g\n", elapsedTime);

    	printf("%g\n", elapsedTime);
    }

	return 0;
}
