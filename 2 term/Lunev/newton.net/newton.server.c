 #include "newton.h"

int mutex = 0;

int get_msqid()
{
    int msqid = 0;

    key_t key = ftok ("/tmp", 'n');
    int msgflg = IPC_CREAT | 0666;

    if ((msqid = msgget (key, msgflg)) == -1)
    {
        perror ("msgget error");
        exit(1);
    }

    return msqid;
}

double compute_interval (double a, double b) // uses Newton-Cotes formula (n = 3)
{
	double h = (double) (b - a)/3.0;

	double sum = (double) (sin (a) + 3.0*sin(a + (double) h/3.0) + 3.0*sin(a + (double) 2.0*h/3.0) + sin (b))/8.0*3.0*h;

	return sum;
}

int calculate (sockmsg_t* task)
{
    int a = task->a, b = task->b;
    int Ninter = task->Ninter, Nprocs = task->Nprocs;
    double localsum = 0, width = 0;
    int msqid = get_msqid();
    register int iproc = 0, pid = 0;

    if (Nprocs == -1)
    {
        if (msgctl (msqid, IPC_RMID, 0) == -1)
        {
            ERROR ("msgctl error");
        }

        return 0;
    }

    size_t msglen = sizeof (procthread_t);

    width = (double) (b - a)/Ninter;

    for (iproc = 0; iproc < Nprocs; iproc++)
    {
        pid = fork();

        if (pid < 0)
        {
            ERROR ("Error in fork\n");
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

        procthread_t msglocalsum = {0, 0};

        msglocalsum.localsum = localsum;
        msglocalsum.iproc = iproc;

      fprintf(stderr, "%d \t %g \t %g \t %g\n", iproc, cur_a, cur_b, localsum);

        if (msgsnd (msqid, &msglocalsum, msglen, 0) == -1)
        {
            ERROR ("msgsnd error");
        }

        exit (EXIT_SUCCESS);
    }
    else // collector
    {
        procthread_t msg = {0, 0};

        register int i = 0;
        double sum = 0;

        for (i = 0; i < Nprocs; i++)
        {
            if (msgrcv (msqid, &msg, msglen, 0, 0) == -1)
            {
                ERROR ("msgrcv error");
            }

            sum += msg.localsum;
        }

        printf("integrate [sin(x), {x, 0, 10}] = %g (estimated 1.83907152908)\n", sum);
        // printf("Time elapsed: %g\n", elapsedTime);

        task->sumaddr = &sum;
        task->sum = sum;

        wait (NULL);

        return 0;
    }
}

void heartbeat_server (int sock_client)
{
    int bytes = 1;
    char msg[MSG_LEN] = "";
    int res_snd = 0;

    struct timeval timeout;
    timeout.tv_sec  = DELAY;
    timeout.tv_usec = 0;

    if (setsockopt (sock_client, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        close (sock_client);
        ERROR ("HRT setsockopt error");
    }

    while (bytes > 0)
    {
        bytes = recv (sock_client, msg, MSG_LEN, 0);

        sleep (1);

        if ( bytes > 0 )
        {
            res_snd = send (sock_client, msg, MSG_LEN, 0);
            if (res_snd < 0)
            {
                close (sock_client);
                ERROR ("HRT send error");
            }
        }
    }

    close(sock_client);
    sleep (1);

    killpg (getpgid(0), SIGKILL);

    return;
}

int main (int argc, char *argv[])
{
    if (setpgid (getpid(), 0) < 0)
    {
        ERROR ("setgpid error");
    }

    int Nprocs = 0;

    if (argc == 2)
    {
        Nprocs = atoi (argv[1]);
    }
    else
    {
        ERROR ("Usage: ./server %%d processes");
    }

    broadmsg_t brmsg = {};
    brmsg.msg = KEY;
    brmsg.Nprocs = Nprocs;

    struct sockaddr_in server, client, heartbeat;
    int sock_desc = 0, sock_client = 0, sock_heartbeat = 0, read_size = 0;
    sockmsg_t task = {};

    sock_desc = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1)
    {
        ERROR ("socket error");
    }
    fprintf(stderr, "socket created\n");

    sock_client = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_client == -1)
    {
        ERROR ("socket error");
    }
/*
    struct timeval timeout_retry;
    timeout_retry.tv_sec  = 2*DELAY;
    timeout_retry.tv_usec = 0;

    if (setsockopt (sock_client, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_retry,
                sizeof(timeout_retry)) < 0)
    {
        close (sock_client);
        ERROR ("HRT setsockopt error");
    }

    sleep (10);*/

    server.sin_family       = AF_INET;
    server.sin_addr.s_addr  = htonl ( INADDR_ANY );
    server.sin_port         = htons ( CLC_PORT );

    int server_perms = 1;
    if (setsockopt (sock_desc, SOL_SOCKET, SO_REUSEADDR, &server_perms, sizeof(server_perms)) < 0)
    {
        ERROR ("setsockopt reuseaddr");
    }

    if (bind (sock_desc, (struct sockaddr*) &server, sizeof (server)) < 0)
    {
        ERROR ("bind error");
    }
    fprintf(stderr, "binding done\n");

    listen (sock_desc, 255);

//-----------------------------HEARTBEAT

    sock_heartbeat = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_heartbeat == -1)
    {
        ERROR ("HRT socket error");
    }
    fprintf(stderr, "HRT socket created\n");

    heartbeat.sin_family        = AF_INET;
    heartbeat.sin_addr.s_addr   = htonl ( INADDR_ANY );
    heartbeat.sin_port          = htons ( HRT_PORT );

    if (setsockopt (sock_heartbeat, SOL_SOCKET, SO_REUSEADDR, &server_perms, sizeof(server_perms)) < 0)
    {
        ERROR ("HRT setsockopt reuseaddr");
    }

    if (bind (sock_heartbeat, (struct sockaddr*) &heartbeat, sizeof (heartbeat)) < 0)
    {
        ERROR ("HRT bind error");
    }
    fprintf(stderr, "HRT binding done\n");

    listen (sock_heartbeat, 255);

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//________________________________BROADCAST____________________________________
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

    struct ifaddrs *ifaddr;

    if (getifaddrs(&ifaddr) == -1) 
    {
        close (sock_heartbeat);
        close (sock_client);
        ERROR ("getifaddrs error");
    }

    int sock_broadcast = socket (AF_INET, SOCK_DGRAM, 0);
    if (sock_broadcast == -1)
    {
        ERROR ("socket broadcast error");
    }
    fprintf(stderr, "broadcast socket created\n");

    int port_snd = SRV_PORT;
    struct sockaddr_in addr_broadcast_rcv   = {};
    addr_broadcast_rcv.sin_family           = AF_INET;
    addr_broadcast_rcv.sin_addr.s_addr      = INADDR_ANY;
    addr_broadcast_rcv.sin_port             = htons (port_snd);

    if (bind (sock_broadcast, 
            (struct sockaddr* ) &addr_broadcast_rcv, 
            sizeof (addr_broadcast_rcv)) < 0)
    {
        close (sock_broadcast);
        ERROR ("broadcast bind error");
    }

    // char* broadcast_msg_snd = "integrate";

  //  char* broadcast_msg_rcv = calloc (MSG_LEN, sizeof (char));

    socklen_t addr_len = sizeof (struct sockaddr_in);

    while (1)
    {
        int res_read = recvfrom (sock_broadcast, 
                            &brmsg, 
                            sizeof (broadmsg_t),
                            MSG_TRUNC,
                            (struct sockaddr* ) &client, 
                            &addr_len);

        fprintf(stderr, "%d\n", brmsg.msg);

        brmsg.Nprocs = Nprocs;

        if (res_read == sizeof (broadmsg_t) &&
            brmsg.msg == KEY)
        {
            if (sendto (sock_broadcast, 
                    &brmsg,
                    sizeof (broadmsg_t),
                    0,
                    (struct sockaddr* ) &client, 
                    addr_len) < 0)
            {
                close (sock_broadcast);
                ERROR ("broadcast sendto error"); 
            }

            fprintf(stderr, "%s\n", "SENT");

            break;
        }
    }

    //OK, we've been chosen to calculate

    int addr_size = sizeof (struct sockaddr_in);

    sock_client = accept (sock_desc, (struct sockaddr*) &client, (socklen_t*) &addr_size);
    if (sock_client < 0)
    {
        ERROR ("accept error");
    }
    fprintf(stderr, "accept done\n");

    pid_t childProcess = fork();

    if (childProcess == (pid_t)-1) 
    {
        ERROR ("Unable to create new process for client connection");
    }
    else if (childProcess == 0) 
    {
        sock_client = accept (sock_heartbeat, (struct sockaddr*) &heartbeat, (socklen_t*) &addr_size);

        fprintf(stderr, "HRT. Got %s on %d port.\n", inet_ntoa (heartbeat.sin_addr), htons (heartbeat.sin_port));

        heartbeat_server (sock_client);
    }
    else 
    {
        fprintf(stderr, "CALC. Got %s on %d port.\n", inet_ntoa (client.sin_addr), htons (client.sin_port));

        if ((read_size = recv (sock_client, &task, sizeof (task), 0)) < 0)
        {
            ERROR ("recv error");
        }

        task.Nprocs = Nprocs;

        fprintf (stderr, "got message from client: %g %g %d %d\n", task.a, task.b, task.Ninter, task.Nprocs);

        if (calculate (&task) < 0)
        {
            ERROR ("calculation error");
        }
        fprintf(stderr, "calculated my sum: %g\n", task.sum);

        if (send (sock_client, &task, sizeof (task), 0) < 0)
        {
            ERROR ("send error");
        }

        kill (childProcess, SIGKILL);
    }

    close (sock_client);
    close (sock_broadcast);
    close (sock_desc);

	return 0;
}