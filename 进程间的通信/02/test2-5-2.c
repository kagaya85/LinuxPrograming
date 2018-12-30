#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
// static void child(int signo);

int main(int argc, char** argv)
{
    int fd1, fd2;
    const char *fifoPath1 = "FIFO1", *fifoPath2 = "FIFO2";
    char cstr[] = "Hello World! from 2";
    char pstr[] = "Hello World! from 1";

    char readBuff[100];
    
    if(mkfifo(fifoPath1,0666) < 0 && errno != EEXIST) {
        fprintf(stderr,"Fail to mkfifo %s : %s.\n",fifoPath1,strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(mkfifo(fifoPath2,0666) < 0 && errno != EEXIST) {
        fprintf(stderr,"Fail to mkfifo %s : %s.\n",fifoPath2,strerror(errno));
        exit(EXIT_FAILURE);
    }

    if((fd1 = open(fifoPath1,O_RDONLY)) < 0) {
        fprintf(stderr,"Fail to open %s : %s.\n", fifoPath1, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if((fd2 = open(fifoPath2,O_WRONLY)) < 0) {
        fprintf(stderr,"Fail to open %s : %s.\n", fifoPath2, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("I am %s: ", argv[0]);
    if(read(fd1, readBuff, sizeof(readBuff)) > 0)
        printf("%s\n", readBuff);
    if(write(fd2, cstr, sizeof(cstr)) < 0) {
        fprintf(stderr,"Fail to write %s : %s.\n",fifoPath2,strerror(errno));
        exit(0);
    }
    
    close(fd1);
    close(fd2);
    unlink(fifoPath1);
    unlink(fifoPath2);

    return 0;
}

// static void child(int signo)
// {
//     pid_t pid;

//     while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
//         ;
// }