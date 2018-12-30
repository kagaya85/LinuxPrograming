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
#define RECV_BUFF_SIZE 100

int wFlag = 0;

void myWrite(int signo) {
    wFlag = 1;
    alarm(3);
}

int main(int argc, char** argv) {
    int sockfd1, sockfd2, fd;
    int servport1 = -1, servport2 = -1;
    struct sockaddr_in servaddr1, servaddr2;
    char recvBuff[RECV_BUFF_SIZE], sendBuff[1024];
    int msglen;
    int flag, selResult;
    fd_set readfds, testfds;
    
    // ����ֽ�
    memset(&sendBuff, '#', sizeof(sendBuff));
    
    if(argc != 4) {
        printf("need <server ip address> <server port1> <server port2>\n");
        exit(0);
    }
    printf("connect to %s %s\n",argv[1], argv[2]);
    printf("connect to %s %s\n",argv[1], argv[3]);

    // ��װ�ź�
    if(signal(SIGALRM, myWrite) == SIG_ERR)
    {
        perror("signal");
        exit(0);
    }

    // ��ʼ��
    if((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    
    if((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // ������
    flag = fcntl(sockfd1, F_GETFL, 0);       //��ȡ�ļ���flagsֵ��
    fcntl(sockfd1, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��
    flag = fcntl(sockfd2, F_GETFL, 0);       //��ȡ�ļ���flagsֵ��
    fcntl(sockfd2, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��

    // servaddr��ʼ��
    memset(&servaddr1, 0, sizeof(servaddr1));
    memset(&servaddr2, 0, sizeof(servaddr2));

    servaddr1.sin_family = AF_INET;
    servaddr2.sin_family = AF_INET;

    // ����ip��ַ
    if(inet_pton(AF_INET, argv[1], &servaddr1.sin_addr) <= 0 || inet_pton(AF_INET, argv[1], &servaddr2.sin_addr) <= 0) {
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }
    servport1 = atoi(argv[2]);
    servport2 = atoi(argv[3]);

    if(servport1 < 1024 || servport1 > 65535) {
        printf("server port ivalid for %s\n",argv[2]);
        exit(0);
    }
    if(servport2 < 1024 || servport2 > 65535) {
        printf("server port ivalid for %s\n",argv[3]);
        exit(0);
    }
    if(servport1 > 0 && servport2 > 0) {
        servaddr1.sin_port = htons(servport1);//���õĶ˿�Ϊ�������
        servaddr2.sin_port = htons(servport2);
    }
    else {
        servaddr1.sin_port = htons(DEFAULT_PORT);//���õĶ˿�ΪDEFAULT_PORT
        servaddr2.sin_port = htons(DEFAULT_PORT+1);//���õĶ˿�ΪDEFAULT_PORT
    }
    
    // ��������
    connect(sockfd1, (struct sockaddr*)&servaddr1, sizeof(servaddr1));
    connect(sockfd2, (struct sockaddr*)&servaddr2, sizeof(servaddr2));
    printf("waiting for connecting......\n");

    struct timeval timeout = {10 ,0};

    FD_ZERO(&readfds);  // ��ʼ������ 
    FD_SET(sockfd1, &readfds);//������socket���뵽������ 
    testfds = readfds;
    selResult = select(FD_SETSIZE, &testfds, &testfds, NULL, &timeout);
    if(selResult > 0)
        printf("connected to fd %d\n", sockfd1);
    else
        perror("select1");

    FD_ZERO(&readfds);  // ��ʼ������ 
    FD_SET(sockfd2, &readfds);//������socket���뵽������ 
    testfds = readfds;
    selResult = select(FD_SETSIZE, &testfds, &testfds, NULL, &timeout);
    if(selResult > 0)
        printf("connected to fd %d\n", sockfd2);
    else
        perror("select2");

    FD_ZERO(&readfds);  // ��ʼ������ 
    FD_SET(sockfd1, &readfds);//������socket���뵽������ 
    FD_SET(sockfd2, &readfds);//������socket���뵽������ 
    // ����3���д����
    alarm(3);
    // ���ó�ʱ����
    while(1) {
        // wait for server
        testfds = readfds;
        selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(selResult < 0) {
            if (wFlag == 1) {
                for(fd = 0; fd < FD_SETSIZE; fd++) {
                    if(FD_ISSET(fd, &readfds)) {
                        if ((msglen = write(fd, sendBuff, 15)) <= 0) {
                            perror("write message error\n");
                            exit(0);
                        }
                        else {
                            printf("write %d Bytes to fd %d\n", msglen, fd);
                        }
                    }
                }
                wFlag = 0;
            }
            continue;
        }
        else if(selResult == 0) {
            perror("select timeout");
            exit(0);
        }
        for(fd = 0; fd < FD_SETSIZE; fd++) {
            if(FD_ISSET(fd, &testfds)) {
                // printf("connected to fd %d\n", fd);
                // break;
                // read
                if((msglen = read(fd, recvBuff, sizeof(recvBuff))) < 0) {
                    printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
                    exit(0);
                }
                else if(msglen == 0) {
                        /*�ͻ�����������ϣ��ر��׽��֣��Ӽ����������Ӧ������?*/
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("removing server on fd %d/n", fd);
                }
                else {
                    printf("read from fd %d %d Bytes\n", fd, msglen);
                }
            }
        }
        // wait for write
        if(wFlag == 1) {
            for(fd = 0; fd < FD_SETSIZE; fd++) {
                if(FD_ISSET(fd, &testfds)) {
                    if ((msglen = write(fd, sendBuff, 15)) <= 0) {
                        perror("write message error\n");
                        exit(0);
                    }
                    else {
                        printf("write %d Bytes to fd %d\n", msglen, fd);
                    }
                }
            }
            wFlag = 0;
        }
        // else {
        //     printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        //     exit(0);
        // }
    }

    // close socket
    for(fd = 0; fd < FD_SETSIZE; fd++) {
        if(FD_ISSET(fd, &readfds)) {
            close(fd);
        }
    }
    printf("Socket closed\n");
    return 0;
}
