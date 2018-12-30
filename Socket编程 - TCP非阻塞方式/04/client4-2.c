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

int main(int argc, char** argv) {
    int sockfd;
    int servport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvBuff[100], sendBuff[1024];
    int msglen;
    int flag, selResult;
    fd_set writefds, testfds;
    
    // ����ֽ�
    memset(&sendBuff, '#', sizeof(sendBuff));
    
    if(argc != 3) {
        printf("need <server ip address> <server port> \n");
        exit(0);
    }
    printf("connect to %s %s\n",argv[1], argv[2]);

    // ��ʼ��
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // �������һ��0�����Զ�ѡ��Э��
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // ������
    flag = fcntl(sockfd, F_GETFL, 0);       //��ȡ�ļ���flagsֵ��
    fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��
    FD_ZERO(&writefds);  // ��ʼ������ 
    FD_SET(sockfd, &writefds);//������socket���뵽������ 

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
        testfds = writefds;
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

    int count = 0;
    while(1){
        // wait for write
        if ((msglen = write(sockfd, sendBuff, 1)) <= 0) {
            perror("\nwrite message error");
            testfds = writefds;
            // printf("waiting for reading......\n");
            selResult = select(FD_SETSIZE, NULL, &testfds, NULL, NULL);
            if (selResult <= 0) {
                perror("select error");
                exit(0);
            }
            else if (FD_ISSET(sockfd, &testfds))   
                continue;
        }
        else {
            count++;
            printf("write %d Bytes to %s\r", count, argv[1]);
        }
    }
    printf("\n");
    printf("Socket closed\n");
    close(sockfd);
    return 0;
}
