#include "newton.h"
#include <pthread.h>

int* mutex;

void heartbeat_client (struct sockaddr_in server)
{
	int bytes = 1;
	int msg = KEY;
	int sock_server = socket (PF_INET, SOCK_STREAM, 0);
	int res_snd = 0;

	server.sin_port = htons ( HRT_PORT );

	fprintf(stderr, "HRT. Got %s on %d port.\n", inet_ntoa (server.sin_addr), htons (server.sin_port));

	struct timeval timeout;
    timeout.tv_sec 	= DELAY;
    timeout.tv_usec = 0;

    if (setsockopt (sock_server, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        close (sock_server);
        ERROR ("HRT setsockopt hrtbt error\n");
    }

	if (connect (sock_server, (struct sockaddr* ) &server, sizeof(server)) == 0)
	{
		while (bytes > 0)
		{
			sleep (1);

			res_snd = send (sock_server, &msg, MSG_LEN, 0);
			if (res_snd < 0)
			{
				close (sock_server);
				ERROR ("HRT send error");
			}

			bytes = recv (sock_server, &msg, MSG_LEN, 0);
		}
	}
	else
	{
		close (sock_server);
		ERROR ("HRT connect error");
	}

	close(sock_server);
	sleep (1);

    killpg (getpgid(0), SIGKILL);

    return;
}

void* thread_server (void* arg)
{
	serverthread_t* task = (serverthread_t*) arg;
	int sock_server = 0;
	struct sockaddr_in server = {};
	memset (&server, 0, sizeof (server));
	server = task->server;

	sock_server = socket (AF_INET, SOCK_STREAM, 0);
	if (sock_server == -1)
	{
		ERROR ("socket error");
	}
	fprintf(stderr, "socket created\n");

	int server_perms = 1;
    if (setsockopt (sock_server, SOL_SOCKET, SO_REUSEADDR, &server_perms, sizeof(server_perms)) < 0)
    {
        ERROR ("setsockopt reuseaddr");
    }

    // task->server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons ( CLC_PORT );

	fprintf(stderr, "OK. Got %s on %d port.\n", inet_ntoa (server.sin_addr), htons (server.sin_port));

	// if (bind (sock_server, (struct sockaddr* ) &server, sizeof (server)) < 0)
	// {
	// 	close (sock_server);
	// 	ERROR ("bind sock server");
	// }

	// task->server.sin_addr.s_addr = INADDR_ANY;

	fprintf(stderr, "Connecting to %s on %d port.\n", inet_ntoa (task->server.sin_addr), htons (task->server.sin_port));

	if (connect (sock_server, (struct sockaddr *) &server , sizeof(server)) < 0)
	{
		close (sock_server);
		ERROR ("connect error");
	}
	fprintf(stderr, "connect ok\n");

	sockmsg_t send_task = {};

	send_task.a 		= task->a;
	send_task.b 		= task->b;
	send_task.Ninter 	= task->Ninter;
	send_task.Nprocs 	= -1;

	pid_t childProcess = fork();

    if (childProcess == (pid_t)-1) 
    {
        ERROR ("Unable to create new process for client connection");
    }
    else if (childProcess == 0) 
    {   
    	heartbeat_client (task->server);
    }
    else 
    {
   		fprintf(stderr, "sending values %g %g %d %d\n", send_task.a, send_task.b, send_task.Ninter, send_task.Nprocs);

		if (send (sock_server, &send_task, sizeof (send_task), 0) < 0)
		{
			close (sock_server);
			ERROR ("send error");
		}

		if (recv (sock_server, &send_task, sizeof (send_task), 0) <= 0)
		{
			close (sock_server);
			ERROR ("recv error");
		}

		fprintf(stderr, "got sum = %g\n", send_task.sum);

		memmove (task->sumaddr, &send_task.sum, sizeof (double));

        kill (childProcess, SIGKILL);
    }

	close (sock_server);

	return NULL;
}

int main (int argc, char *argv[]) // integrate [sin(x), {x, 0, 10}] = 1.83907152908
{
	mutex = (int*) calloc (1, sizeof (int));

	if (setpgid (getpid(), 0) < 0)
	{
		ERROR ("setgpid error");
	}

    struct timeval t1, t2;
    double elapsedTime;

   	gettimeofday(&t1, NULL);

	int a = 0, b = 10;
	int Ninter = 0, Nservs = 0;

	if (argc == 3)
	{
		Ninter = atoi (argv[1]);
		Nservs = atoi (argv[2]);
	}
	else
	{
		ERROR ("Usage: ./client %%d intervals %%d servers");
	}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//________________________________BROADCAST____________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	int sock_broadcast = socket (AF_INET, SOCK_DGRAM, 0);
	if (sock_broadcast == -1)
	{
		ERROR ("socket broadcast error");
	}

	// int sock_broadcast_snd = socket (AF_INET, SOCK_DGRAM, 0);
	// if (sock_broadcast_snd == -1)
	// {
	// 	ERROR ("socket broadcast error");
	// }
	fprintf(stderr, "broadcast socket created\n");

	int port_rcv = CLT_PORT;
	struct sockaddr_in addr_broadcast_rcv 	= {};
	addr_broadcast_rcv.sin_family 			= AF_INET;
	addr_broadcast_rcv.sin_addr.s_addr 		= INADDR_ANY;
	addr_broadcast_rcv.sin_port 			= htons (port_rcv);

	if (bind (sock_broadcast, 
			(struct sockaddr* ) &addr_broadcast_rcv, 
			sizeof (addr_broadcast_rcv)) < 0)
	{
		close (sock_broadcast);
		ERROR ("broadcast bind error");
	}

	int port_snd = SRV_PORT;
	struct sockaddr_in addr_broadcast_snd 	= {};
	addr_broadcast_snd.sin_family 			= AF_INET;
	addr_broadcast_snd.sin_addr.s_addr 		= htonl (INADDR_BROADCAST);
	addr_broadcast_snd.sin_port 			= htons (port_snd);

	int setsockopt_perms = 1;
	if (setsockopt (sock_broadcast, 
			SOL_SOCKET, 
			SO_BROADCAST, 
			&setsockopt_perms, 
			sizeof (setsockopt_perms)) < 0)
	{
		close (sock_broadcast);
		ERROR ("broadcast setsockopt error");
	}

	// char* broadcast_msg_snd = "integrate";

/*	struct ifaddrs* ifaddr = NULL, *ifa = NULL;

	if (getifaddrs (&ifaddr) < 0)
	{
		close (sock_broadcast);
		ERROR ("getifaddrs error");
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET &&
			((ifa->ifa_flags & IFF_BROADCAST) != 0))
		{
            struct sockaddr_in* sa = (struct sockaddr_in *) ifa->ifa_addr;
            char* addr = inet_ntoa(sa->sin_addr);
            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);

			((struct sockaddr_in* ) ifa->ifa_broadaddr)->sin_port = htons (port_snd);

			if (sendto (sock_broadcast_snd, 
					broadcast_msg_snd,
					MSG_LEN,
					0,
					(struct sockaddr* ) ifa->ifa_broadaddr,
					sizeof (struct sockaddr_in)) < 0)
			{
				close (sock_broadcast);
				ERROR ("broadcast sendto error");
			}
		}
	}
*/
	broadmsg_t brmsg = {KEY, 0};

	if (sendto (sock_broadcast, 
			&brmsg,
			sizeof (broadmsg_t),
			0,
			(struct sockaddr* ) &addr_broadcast_snd,
			sizeof (struct sockaddr_in)) < 0)
	{
		close (sock_broadcast);
		ERROR ("broadcast sendto error");
	}

	fcntl (sock_broadcast, F_SETFL, O_NONBLOCK);

	fd_set read_set;
	FD_ZERO (&read_set);
	FD_SET (sock_broadcast, &read_set);

	struct sockaddr_in* servers = (struct sockaddr_in* ) calloc (Nservs, sizeof (struct sockaddr_in));

	struct timeval timeout = {};
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	int cur_server = 0;
	// char* broadcast_msg_rcv = calloc (MSG_LEN, sizeof (char));
	socklen_t addr_len = sizeof (struct sockaddr_in);

	int* Nprocs = (int*) calloc (Nservs, sizeof (int));

	while (select (sock_broadcast + 1, &read_set, NULL, NULL, &timeout) > 0 &&
		cur_server < Nservs)
	{
		//memset (&servers[cur_server], 0, sizeof (servers[cur_server]));
		int res_read = recvfrom (sock_broadcast, 
							&brmsg, 
							sizeof (broadmsg_t),
							MSG_TRUNC,
							(struct sockaddr* ) &servers[cur_server], 
							&addr_len);

		fprintf(stderr, "%d\n", brmsg.Nprocs);

		Nprocs[cur_server] = brmsg.Nprocs;

		if (res_read == sizeof (broadmsg_t) &&
			brmsg.msg == KEY)
		{
			cur_server++;

			if (cur_server == Nservs) break;
		}
	}

	if (cur_server != Nservs)
	{
		close (sock_broadcast);
		ERROR ("broadcast recvfrom error");
	}

	int Nprocs_total = 0;

	register int i = 0;
	for (i = 0; i < Nservs; i++)
	{
		Nprocs_total += Nprocs[i];
		fprintf(stderr, "OK. Got %s on %d port. %d procs\n", inet_ntoa (servers[i].sin_addr), htons (servers[i].sin_port), Nprocs[i]);
	}

	close (sock_broadcast);

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//_________________________________CLIENT______________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

	pthread_t *tids = (pthread_t* ) calloc(Nservs, sizeof(pthread_t));
	double min_width = (double) (b - a)/Nprocs_total;
	double* localsums = (double* ) calloc (Nservs, sizeof (double));

	// fprintf(stderr, "each server has %g width\n", width);

	double cur_a = 0.0, cur_b = 0.0;

	for (i = 0; i < Nservs; i++)
	{
		cur_a = cur_b;
		cur_b += (double) Nprocs[i]*min_width;

		serverthread_t* task 	= (serverthread_t* ) calloc (1, sizeof (serverthread_t));
		task->sumaddr 			= &localsums[i];
		task->a 				= cur_a;
		task->b 				= cur_b;
		task->Ninter 			= Ninter;
		//task->Nprocs 			= -1;
		memset (&task->server, 0, sizeof (task->server));
		task->server 			= servers[i];

		if (pthread_create (&tids[i], NULL, thread_server, task) < 0)
		{
			ERROR ("pthread_create error");
		}
	}

	double sum = 0.0;

	for (i = 0; i < Nservs; i++)
	{
		pthread_join (tids[i], NULL);
		sum += localsums[i];
	}

	gettimeofday(&t2, NULL);

	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    printf("integrate [sin(x), {x, 0, 10}] = %g (estimated 1.83907152908)\n", sum);
	printf("%g\n", elapsedTime);

	return 0;
}