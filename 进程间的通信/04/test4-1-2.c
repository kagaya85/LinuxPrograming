#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
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
    long int msgtype = 0; 
    
    msgid = msgget(114, 0666 | IPC_CREAT);
    if(msgid < 0) {
        perror("msgid get error");
        exit(EXIT_FAILURE);
    }

    if(msgrcv(msgid, &data, BUFSIZ, msgtype, 0) == -1) {
        fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Recv: %s\n",data.text);

    if(msgctl(msgid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
    
    return 0;
}
