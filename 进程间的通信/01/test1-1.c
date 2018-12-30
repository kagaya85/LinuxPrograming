#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

// static void child(int signo);

int main()
{
    pid_t pid, ppid;
    int fds[2];
    char str[] = "Hello World!";
    char readBuff[100];
    pipe(fds);

    int i;
    pid = fork();
    if(pid < 0)
    {
        printf("fork error\n");
        exit(1);
    }
    // child process
    else if(pid == 0)
    {
        close(fds[1]);
        printf("I am child: ");
        if(read(fds[0], readBuff, sizeof(readBuff)) > 0)
            printf("%s\n", readBuff);
        close(fds[0]);    
        exit(0);
    }
    // parents process
    else {
        close(fds[0]);
        write(fds[1], str, sizeof(str));
        waitpid(-1, NULL, 0);
        close(fds[1]);
    }

    return 0;
}

// static void child(int signo)
// {
//     pid_t pid;

//     while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
//         ;
// }