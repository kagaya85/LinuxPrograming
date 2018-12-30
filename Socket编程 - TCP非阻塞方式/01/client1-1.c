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
#define MAXLINE 4096

int main(int argc, char** argv) {
    int sockHandle;
    int servport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvBuff[4096], sendBuff[4096];
    int msglen;
    int flag;

    if(argc != 3) {
        printf("need <server ip address> <server port> \n");
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

    // ������
    flag = fcntl(sockHandle, F_GETFL, 0);       //��ȡ�ļ���flagsֵ��
    fcntl(sockHandle, F_SETFL, flag | O_NONBLOCK);     //���óɷ�����ģʽ��

    // read
    // if((msglen = read(sockHandle, recvBuff, 10)) <= 0) {
    //     printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
    // }

    // recv
    if((msglen = recv(sockHandle, recvBuff, 10, 0)) <= 0) {
        printf("recv socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
    }

    // int i;
    // for(;;){
    // // for(i = 0; i < 10; i++){
    //     if((msglen = read(sockHandle, recvBuff, rNum)) <= 0) {
    //         printf("read socket error\n");
    //         exit(0);
    //     }
    //     else {
    //         recvBuff[msglen] = '\0';
    //         printf("read from server: %s\n", recvBuff);
    //     }
    //     if (write(sockHandle, sendBuff, wNum) <= 0) {
    //         printf("send message error\n");
    //         exit(0);
    //     }
    // }

    printf("Socket closed\n");
    close(sockHandle);
    return 0;
}