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
    char recvBuff[1024], sendBuff[1024];
    int msglen, rNum, wNum;

    if(argc != 4) {
        printf("need <listen port(1024~65535)> <read num> <write num>\n");
        exit(0);
    }

    port = atoi(argv[1]);
    if(port < 1024 || port > 65535) {
        printf("invalid port, 1024~65535 is recommended.\n");
        exit(0);
    }
    
    rNum = atoi(argv[2]);
    wNum = atoi(argv[3]);

    // ��ʼ��
    if((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    int flag = 1;
    if (setsockopt(sockHandle, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
        perror("setsockopt error");
    }

    // servaddr��ʼ��
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
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    if((conn = accept(sockHandle, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
        // continue;
    }
    //�����ͻ��˵�ַ
    char ipbuff[INET_ADDRSTRLEN + 1] = {0};
    inet_ntop(AF_INET, &cliaddr.sin_addr, ipbuff, INET_ADDRSTRLEN);
    uint16_t cli_port = ntohs(cliaddr.sin_port);
    printf("connection from %s, port %d\n", ipbuff, cli_port);
    
    memset(&sendBuff, '1', sizeof(sendBuff));
    int i;
    for(;;){
    // for(i = 0; i < 10; i++){
        if ((msglen = write(conn, sendBuff, wNum)) <= 0) {
            printf("send message error\n");
            exit(0);
        }
        else
            printf("write to client %d bytes\n", msglen);

        if((msglen = read(conn, recvBuff, rNum)) <= 0) {
            printf("read socket error\n");
            exit(0);
        }
        else {
            recvBuff[msglen] = '\0';
            printf("read from client %d bytes\n", msglen);
        }
    }

    printf("Socket closed\n");
    close(conn);
    close(sockHandle);
    return 0;
}