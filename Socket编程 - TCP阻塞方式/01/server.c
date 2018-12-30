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
    int sockHandle, conn;
    int port = -1;
    struct sockaddr_in servaddr;
    char buff[4096];
    int msglen;

    if(argc != 2) {
        printf("need server listen port(1024~65535)\n");
        exit(0);
    }

    port = atoi(argv[1]);
    if(port < 1024 || port > 65535) {
        printf("invalid port, 1024~65535 is recommended.\n");
        exit(0);
    }
    
    // ��ʼ��
    if((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    int flag = 1;
    if (setsockopt(sockHandle, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
        perror("setsockopt error");
    }

    // servaddr��0
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP��ַ���ó�INADDR_ANY,��ϵͳ�Զ���ȡ������IP��ַ��
    if(port > 0)
        servaddr.sin_port = htons(port);//���õĶ˿�Ϊs�������
    else
        servaddr.sin_port = htons(DEFAULT_PORT);//���õĶ˿�ΪDEFAULT_PORT

    // �����ص�ַ�󶨵�socket��
    if(bind(sockHandle, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // ��������
    if(listen(sockHandle, 10) < 0) {    // �ڶ�������Ϊ�����Ŷӵ����������
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    printf("waiting......\n");

    // ��������
    if(1) {
        if((conn = accept(sockHandle, (struct sockaddr*)NULL, NULL)) < 0) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
            // continue;
        }
        if(1) {
            char *msgContent = "Hello, you are connected!\n";
            if(send(conn, msgContent, strlen(msgContent), 0) < 0) {
                perror("send error");
                exit(0);
            }
            if(msglen = recv(conn, buff, MAXLINE, 0) < 0) {
            printf("receive socket error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
            }
            buff[msglen] = '\0';
            printf("Received from client: %s\n", buff);
            
        }

    }
    printf("Socket closed\n");
    close(conn);
    close(sockHandle);
    return 0;
}