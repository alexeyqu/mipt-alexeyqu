#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <resolv.h>
#include <ifaddrs.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <linux/if_arp.h>

#include <netinet/tcp.h>

#include <arpa/inet.h>

#define ERROR( str )        \
{                           \
    perror (str);           \
    fprintf(stderr, "%d\n", errno);\
    exit (EXIT_FAILURE);    \
}

#define MSG_LEN sizeof (int)

#define SRV_PORT 20000
#define CLT_PORT 30000

#define CLC_PORT 25000

#define HRT_PORT 35000

#define DELAY 5

#define KEY 7

typedef struct broadmsg
{
    int msg;
    int Nprocs;
} broadmsg_t;

typedef struct procthread
{
    long iproc;
	double localsum;
} procthread_t;

typedef struct sockmsg
{
    double a, b;
    int Ninter, Nprocs;
    double sum;
    double* sumaddr;
} sockmsg_t;

typedef struct serverthread
{
	double* sumaddr;
    double sum;
    double a, b;
    int Ninter;
    struct sockaddr_in server;
} serverthread_t;