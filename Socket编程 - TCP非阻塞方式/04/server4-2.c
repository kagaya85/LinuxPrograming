#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_PORT 8000
int wFlag = 0;

int main(int argc, char** argv) {
    int lis_sockfd, con_sockfd;
    int port = -1;
    struct sockaddr_in servaddr;
    char recvBuff[1024], sendBuff[1024];
    int msglen, rNum, wNum;
    int flag = 1, selResult;
    fd_set readfds, testfds;

    if(argc != 2) {
        printf("need <listen port(1024~65535)>\n");
        exit(0);
    }

    port = atoi(argv[1]);
    if(port < 1024 || port > 65535) {
        printf("invalid port, 1024~65535 is recommended.\n");
        exit(0);
    }

    // ��ʼ��
    if((lis_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if (setsockopt(lis_sockfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
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

    // ���ü���socket������
    flag = fcntl(lis_sockfd, F_GETFL, 0);              //��ȡ�ļ���flagsֵ��
    fcntl(lis_sockfd, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��

    // �����ص�ַ�󶨵�socket��
    if(bind(lis_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // ��������
    if(listen(lis_sockfd, 10) < 0) {    // �ڶ�������Ϊ�����Ŷӵ����������
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // �ȴ���������
    while(1){
        FD_ZERO(&readfds);   
        FD_SET(lis_sockfd, &readfds);//������socket���뵽������ 
        testfds = readfds;
        printf("waiting for connecting......\n");
        selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(selResult <= 0) {
            perror("select error");
            exit(0);
        }
        else if(FD_ISSET(lis_sockfd, &testfds))
            break;
    }

    // ��������
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    if((con_sockfd = accept(lis_sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
        // continue;
    }

    // ��������socket������
    flag = fcntl(con_sockfd, F_GETFL, 0);              //��ȡ�ļ���flagsֵ��
    fcntl(con_sockfd, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��

    //�����ͻ��˵�ַ
    char ipbuff[INET_ADDRSTRLEN + 1] = {0};
    inet_ntop(AF_INET, &cliaddr.sin_addr, ipbuff, INET_ADDRSTRLEN);
    uint16_t cli_port = ntohs(cliaddr.sin_port);
    printf("connection from %s, port %d\n", ipbuff, cli_port);

    getchar();

    // �ȴ��ͻ�����Ϣ
    FD_ZERO(&readfds);  // ��ʼ������ 
    FD_SET(con_sockfd, &readfds);//������socket���뵽������ 
    while(1){
        testfds = readfds;
        // printf("waiting for reading......\n");
        selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(selResult < 0){
            continue;
        }
        else if(selResult == 0) {
            perror("select error");
            exit(0);
        }
        else if(FD_ISSET(con_sockfd, &testfds)) {
            // read
            if((msglen = read(con_sockfd, recvBuff, 1000)) <= 0) {
                printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
                break;
            }
            else {
                printf("read from server %d Bytes\n", msglen);
                sleep(1);
            }
        }

    }

    printf("Socket closed\n");
    close(con_sockfd);
    close(lis_sockfd);
    return 0;
}