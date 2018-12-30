#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define BUFSIZE 100

struct Message {
    long int msg_type;
    char text[BUFSIZE];
};

int main()
{
    int msgid = -1;
    struct Message data;
    char buf[] = "Hello world!";

    msgid = msgget(114, 0666 | IPC_CREAT);
    if(msgid < 0) {
        perror("Message get error");
        exit(EXIT_FAILURE);
    }

    data.msg_type = 1;
    strcpy(data.text, buf);
    //向队列发送数据
    if(msgsnd(msgid, (void*)&data, BUFSIZE, 0) == -1)
    {
        fprintf(stderr, "msgsnd failed\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
