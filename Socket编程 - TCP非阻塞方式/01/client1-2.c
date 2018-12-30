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
    int sockfd;
    int servport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvBuff[4096], sendBuff[4096];
    int msglen;
    int flag, selResult;
    fd_set readfds, testfds;
    if(argc != 3) {
        printf("need <server ip address> <server port> \n");
        exit(0);
    }

    printf("connect to %s %s\n",argv[1], argv[2]);

    // 初始化
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // 参数最后一个0代表自动选择协议
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // servaddr初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // 设置ip地址
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
        servaddr.sin_port = htons(servport);//设置的端口为输入参数
    else
        servaddr.sin_port = htons(DEFAULT_PORT);//设置的端口为DEFAULT_PORT

    // 请求连接
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    // 非阻塞
    flag = fcntl(sockfd, F_GETFL, 0);       //获取文件的flags值。
    fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);     //设置成非阻塞模式；

    // 初始化描述符集合
    FD_ZERO(&readfds);   
    FD_SET(sockfd, &readfds);//将服务端socket加入到集合中 
    testfds = readfds;
    selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
    if(selResult <= 0) {
        perror("select error");
        exit(0);
    }
    // read
    if((msglen = read(sockfd, recvBuff, 10)) <= 0) {
        printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
    }

    // recv
    // if((msglen = recv(sockfd, recvBuff, 10, 0)) <= 0) {
    //     printf("recv socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
    // }

    // int i;
    // for(;;){
    // // for(i = 0; i < 10; i++){
    //     if((msglen = read(sockfd, recvBuff, rNum)) <= 0) {
    //         printf("read socket error\n");
    //         exit(0);
    //     }
    //     else {
    //         recvBuff[msglen] = '\0';
    //         printf("read from server: %s\n", recvBuff);
    //     }
    //     if (write(sockfd, sendBuff, wNum) <= 0) {
    //         printf("send message error\n");
    //         exit(0);
    //     }
    // }

    printf("Socket closed\n");
    close(sockfd);
    return 0;
}