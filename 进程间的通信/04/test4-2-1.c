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
    char buf[] = "Hello world! I am 1";

    msgid = msgget(114, 0666 | IPC_CREAT);
    if(msgid < 0) {
        perror("Message get error");
        exit(EXIT_FAILURE);
    }

    int i;
    for(i = 0; i < 10; i++) {
        // 向队列发送数据
        data.msg_type = 1;
        strcpy(data.text, buf);
        if(msgsnd(msgid, (void*)&data, BUFSIZE, 0) == -1)
        {
            fprintf(stderr, "msgsnd failed\n");
            exit(EXIT_FAILURE);
        }
        // 从队列读取
        if(msgrcv(msgid, &data, BUFSIZ, 2, 0) == -1) {
            fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        printf("Recv from 2: %s\n",data.text);
        sleep(1);
    }

    msgctl(msgid, IPC_RMID, NULL);
    printf("Message destroy done...\n");
    return 0;
}
