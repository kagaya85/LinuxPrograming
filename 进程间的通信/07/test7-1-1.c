#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILEPATH "test.txt"

int main()
{
    int fd, ret;
    char buf[] = "Hello world, this is 7-1-1";

    fd = open(FILEPATH, O_WRONLY | O_CREAT);
    flock(fd, LOCK_EX);
    sleep(5);
    if((ret = write(fd, buf, sizeof(buf))) < 0) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
    flock(fd, LOCK_UN);

    while(1) 
        sleep(1);

    return 0;
}