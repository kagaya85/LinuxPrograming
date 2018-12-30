#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_PORT 8000
#define MAXLINE 4096

int main(int argc, char** argv) {
    int sockHandle;
    int port = -1;
    struct sockaddr_in servaddr;
    char recvbuff[4096], sendBuff[4096];
    int msglen;

    if(argc != 3) {
        printf("need server ip address and server port\n");
        exit(0);
    }

    // ��ʼ��
    if((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // servaddr��0
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // ����ip��ַ
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }
    port = atoi(argv[2]);
    if(port < 1024 || port > 65535) {
        printf("port ivalid for %s\n",argv[2]);
        exit(0);
    }
    if(port > 0)
        servaddr.sin_port = htons(port);//���õĶ˿�Ϊ�������
    else
        servaddr.sin_port = htons(DEFAULT_PORT);//���õĶ˿�ΪDEFAULT_PORT


    // ��������
    if(connect(sockHandle, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    
    // ������Ϣ
    if((msglen = recv(sockHandle, recvbuff, MAXLINE, 0)) == -1) {
       perror("receive error: ");
       exit(1);
    }
    recvbuff[msglen]  = '\0';
    printf("Received from server : %s\n", recvbuff);

    // send hello
    strcpy(sendBuff, "Hello!\n");
    
    if(send(sockHandle, sendBuff, strlen(sendBuff), 0) < 0) {
        printf("send message error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // // ������Ϣ
    // printf("Send message to server: ");
    // fgets(sendBuff, 4096, stdin);

    // if(send(sockHandle, sendBuff, strlen(sendBuff), 0) < 0) {
    //     printf("send message error: %s(errno: %d)\n", strerror(errno), errno);
    //     exit(0);
    // }

    // ������Ϣ
    if((msglen = recv(sockHandle, recvbuff, MAXLINE, 0)) == -1) {
       perror("receive error");
       exit(1);
    }
    recvbuff[msglen]  = '\0';
    printf("Received from server : %s\n", recvbuff);
    
    printf("Socket closed\n");
    close(sockHandle);
    return 0;
}