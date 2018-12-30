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
    int servport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvbuff[4096], sendBuff[4096];
    int msglen;

    if(argc != 3) {
        printf("need <server ip address> <server port>\n");
        exit(0);
    }

    // ��ʼ��
    if((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // servaddr��ʼ��
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // ����ip��ַ
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }
    servport = atoi(argv[2]);
    if(servport < 1024 || servport > 65535) {
        printf("server port ivalid for %s\n",argv[2]);
        exit(0);
    }
    if(servport > 0)
        servaddr.sin_port = htons(servport);//���õĶ˿�Ϊ�������
    else
        servaddr.sin_port = htons(DEFAULT_PORT);//���õĶ˿�ΪDEFAULT_PORT

    // ��������
    if(connect(sockHandle, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    // send 
    int counter = 0;
    while(1) {
        strcpy(sendBuff, "#");
        // printf("write: %s\n", sendBuff);
        if (write(sockHandle, sendBuff, 1) <= 0) {
            printf("send message error (errno: %d)\n", errno);
            exit(0);
        }
        counter++;
        printf("%d bytes wrote\r", counter);
    }

    printf("Socket closed\n");
    close(sockHandle);
    return 0;
}