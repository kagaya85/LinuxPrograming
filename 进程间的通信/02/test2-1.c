#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
// static void child(int signo);

int main()
{
    pid_t pid, ppid;
    int fd;
    const char *fifoPath = "FIFO";
    char str[] = "Hello World!";
    char readBuff[100];
    
    if(mkfifo(fifoPath,0666) < 0 && errno != EEXIST) {
        fprintf(stderr,"Fail to mkfifo %s : %s.\n",fifoPath,strerror(errno));
        exit(EXIT_FAILURE);
    }

    int i;
    pid = fork();
    if(pid < 0) {
        fprintf(stderr,"fork error\n");
        exit(0);
    }
    // child process
    else if(pid == 0) {
        if((fd = open(fifoPath,O_RDONLY)) < 0) {
            fprintf(stderr,"Fail to open %s : %s.\n",fifoPath,strerror(errno));
            exit(EXIT_FAILURE);
        }
        printf("I am child: ");
        if(read(fd, readBuff, sizeof(readBuff)) > 0)
            printf("%s\n", readBuff);
        close(fd);
        unlink(fifoPath);
        exit(0);
    }
    // parents process
    else {
        if((fd = open(fifoPath,O_WRONLY)) < 0) {
            fprintf(stderr,"Fail to open %s : %s.\n",fifoPath,strerror(errno));
            exit(EXIT_FAILURE);
        }
        if(write(fd, str, sizeof(str)) < 0) {
            fprintf(stderr,"Fail to write %s : %s.\n",fifoPath,strerror(errno));
            exit(0);
        }
        close(fd);
        unlink(fifoPath);
        waitpid(-1, NULL, 0);
    }

    return 0;
}

// static void child(int signo)
// {
//     pid_t pid;

//     while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
//         ;
// }