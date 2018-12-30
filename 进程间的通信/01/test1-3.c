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
    int fds1[2], fds2[2];
    char pstr[] = "Hello from parent!";
    char cstr[] = "Hello from child!";
    char readBuff[100];
    pipe(fds1);
    pipe(fds2);

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
        close(fds1[1]);
        close(fds2[0]);
        printf("I am child: ");
        
        write(fds2[1], cstr, sizeof(cstr));
        if(read(fds1[0], readBuff, sizeof(readBuff)) > 0)
            printf("%s\n", readBuff);

        close(fds1[0]);    
        close(fds2[1]);    
        exit(0);
    }
    // parents process
    else {
        close(fds1[0]);
        close(fds2[1]);
        printf("I am parent: ");
        
        write(fds1[1], pstr, sizeof(pstr));
        if(read(fds2[0], readBuff, sizeof(readBuff)) > 0)
            printf("%s\n", readBuff);
        waitpid(-1, NULL, 0);
        
        close(fds1[1]);
        close(fds2[0]);
    }

    return 0;
}

// static void child(int signo)
// {
//     pid_t pid;

//     while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
//         ;
// }