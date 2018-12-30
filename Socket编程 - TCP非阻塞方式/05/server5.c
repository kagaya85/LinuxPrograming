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
    alarm(1);
}

int main(int argc, char** argv) {
    int lis_sockfd, con_sockfd, fd;
    int port = -1;
    struct sockaddr_in servaddr;
    char recvBuff[RECV_BUFF_SIZE], sendBuff[1024];
    int msglen;
    int flag = 1, selResult;
    fd_set readfds, testfds;

    if(argc != 2) {
        printf("need <listen port(1024~65535)>\n");
        exit(0);
    }
    
    // ��װ�ź�
    if(signal(SIGALRM, myWrite) == SIG_ERR)
    {
        perror("signal");
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
    FD_ZERO(&readfds);   
    FD_SET(lis_sockfd, &readfds);//������socket���뵽������ 
    printf("waiting for connecting......\n");
    
    // ��������
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    alarm(1);
    while(1){
        testfds = readfds;
        selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(selResult < 0) {
            // ����alarm���
            if (wFlag == 1) {
                for(fd = 0; fd < FD_SETSIZE; fd++) {
                    if(FD_ISSET(fd, &testfds) && fd != lis_sockfd) {
                        if ((msglen = write(fd, sendBuff, 10)) <= 0) {
                            perror("write message error\n");
                            exit(0);
                        }
                        else {
                            printf("write %d Bytes to fd %d\n", msglen, fd);
                        }
                        wFlag = 0;
                    }
                }
            }
            continue;
        }
        else if(selResult == 0) {
            perror("select timeout");
            exit(0);
        }
        // ����һ��
        for(fd = 0; fd < FD_SETSIZE; fd++){
            if(FD_ISSET(fd, &testfds)) {
                // ����Ǽ���socket��������µ�����
                if(fd == lis_sockfd) {
                    if((con_sockfd = accept(lis_sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
                        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
                        exit(0);
                    }
                    // ��������socket������
                    flag = fcntl(con_sockfd, F_GETFL, 0);              //��ȡ�ļ���flagsֵ��
                    fcntl(con_sockfd, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��
                    int recv_min_size = RECV_BUFF_SIZE;
                    // if(setsockopt(con_sockfd, SOL_SOCKET, SO_RCVLOWAT, (void *)&recv_min_size, sizeof(int)) < 0){
                    //     perror("setsockopt error");
                    // }

                    //�����ͻ��˵�ַ
                    char ipbuff[INET_ADDRSTRLEN + 1] = {0};
                    inet_ntop(AF_INET, &cliaddr.sin_addr, ipbuff, INET_ADDRSTRLEN);
                    uint16_t cli_port = ntohs(cliaddr.sin_port);
                    printf("connection from %s, port %d\n", ipbuff, cli_port);
                    
                    // ������socket���뵽������ 
                    FD_SET(con_sockfd, &readfds);
                }
                /*���������ͻ���������*/
                else {
                    // ioctl(fd,?FIONREAD,?&nread);//ȡ������������nread??
                    if((msglen = read(fd, recvBuff, sizeof(recvBuff))) < 0) {
                        printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
                        exit(0);
                    }
                    else if(msglen == 0) {
                        /*�ͻ�����������ϣ��ر��׽��֣��Ӽ����������Ӧ������?*/
                        close(fd);
                        FD_CLR(fd, &readfds);
                        printf("removing client on fd %d\n", fd);
                    }
                    else {
                        printf("read from fd %d %d Bytes\n", fd, msglen);
                    }
                }
            } // end of if
        } // end of for
        
        // wait for write
        if(wFlag == 1) {
            for(fd = 0; fd < FD_SETSIZE; fd++) {
                if(FD_ISSET(fd, &readfds) && fd != lis_sockfd) {
                    if ((msglen = write(fd, sendBuff, 10)) <= 0) {
                        perror("write message error\n");
                        exit(0);
                    }
                    else {
                        printf("write %d Bytes to fd %d\n", msglen, fd);
                    }
                    wFlag = 0;
                }
            }
        }
    } // end of while
    
    // close socket
    for(fd = 0; fd < FD_SETSIZE; fd++) {
        if(FD_ISSET(fd, &readfds)) {
            close(fd);
        }
    }
    printf("Socket closed\n");
    return 0;
}