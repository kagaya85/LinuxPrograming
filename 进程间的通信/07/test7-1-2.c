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
    char buf[100];

    fd = open(FILEPATH, O_RDONLY);
    // flock(fd, LOCK_SH);
    if((ret = read(fd, buf, sizeof(buf))) < 0) {
        perror("write error");
        exit(EXIT_FAILURE);
    }
    printf("Read from file: %s\n", buf);
    flock(fd, LOCK_UN);

    while(1)
        sleep(1);

    return 0;
}