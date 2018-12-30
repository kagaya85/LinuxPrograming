#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define DEFAULT_PORT 8000
int wFlag = 0;

void myWrite(int signo) {
    wFlag = 1;
    alarm(3);
}

int main(int argc, char** argv) {
    int sockfd;
    int servport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvBuff[100], sendBuff[1024];
    int msglen;
    int flag, selResult;
    fd_set readfds, testfds;
    
    // ����ֽ�
    memset(&sendBuff, '#', sizeof(sendBuff));
    
    if(argc != 3) {
        printf("need <server ip address> <server port> \n");
        exit(0);
    }
    printf("connect to %s %s\n",argv[1], argv[2]);

    // ��װ�ź�
    if(signal(SIGALRM, myWrite) == SIG_ERR)
    {
        perror("signal");
        exit(0);
    }

    // ��ʼ��
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // ������
    flag = fcntl(sockfd, F_GETFL, 0);       //��ȡ�ļ���flagsֵ��
    fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��
    FD_ZERO(&readfds);  // ��ʼ������ 
    FD_SET(sockfd, &readfds);//������socket���뵽������ 

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
    // ���ó�ʱ����
    struct timeval timeout = {10 ,0};
    while(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        // wait for server
        testfds = readfds;
        printf("waiting for connecting......\n");
        selResult = select(FD_SETSIZE, &testfds, &testfds, NULL, &timeout);
        if(selResult < 0) {
            perror("select error");
            exit(0);
        }
        else if(selResult == 0) {
            perror("select timeout");
            exit(0);
        }
        else if(FD_ISSET(sockfd, &testfds)){
            printf("connected\n");
            break;
        }
        else {
            printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
            exit(0);
        }
    }

    // ����3���д����
    alarm(3);
    
    while(1){
        testfds = readfds;
        // printf("waiting for reading......\n");
        selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(selResult < 0){
            if (wFlag == 1) {
                if ((msglen = write(sockfd, sendBuff, 10)) <= 0) {
                    perror("write message error\n");
                    exit(0);
                }
                else {
                    printf("write %d Bytes to %s\n", msglen, argv[1]);
                }
                wFlag = 0;
            }
            continue;
        }
        else if(selResult == 0) {
            perror("select error");
            exit(0);
        }
        else if(FD_ISSET(sockfd, &testfds)) {
            // read
            if((msglen = read(sockfd, recvBuff, 100)) <= 0) {
                printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
                exit(0);
            }
            else {
                printf("read from server %d Bytes\n", msglen);
            }
        }
        // wait for write
        if(wFlag == 1) {
            if ((msglen = write(sockfd, sendBuff, 15)) <= 0) {
                perror("write message error\n");
                exit(0);
            }
            else {
                printf("write %d Bytes to %s\n", msglen, argv[1]);
            }
            wFlag = 0;
        }

    }

    printf("Socket closed\n");
    close(sockfd);
    return 0;
}
