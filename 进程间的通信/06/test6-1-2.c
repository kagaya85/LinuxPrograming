#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

int main() {
    int sockfd, ret;
    struct sockaddr_un ser_un = {0};
    char *serFile = "\0server";
    char sendBuff[100] = "Hi, I am 6-1-2", recvBuff[100];
    int flag = 1;
    fd_set writefds, testfds;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }   

    // 非阻塞
    flag = fcntl(sockfd, F_GETFL, 0);       //获取文件的flags值。
    fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);     //设置成非阻塞模式；
    FD_ZERO(&writefds);  // 初始化集合 
    FD_SET(sockfd, &writefds);//将连接socket加入到集合中 

    ser_un.sun_family = AF_UNIX;
    strncpy(ser_un.sun_path, serFile, sizeof(ser_un.sun_path));

    // unlink(ser_un.sun_path);

    if(connect(sockfd, (struct sockaddr *)&ser_un, sizeof(ser_un)) < 0){
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    struct timeval timeout = {10 ,0};
    testfds = writefds;
    printf("waiting for connecting......\n");
    ret = select(FD_SETSIZE, NULL, &writefds, NULL, &timeout);
    if(ret < 0) {
        perror("select error");
        exit(0);
    }
    else if(ret == 0) {
        perror("select timeout");
        exit(0);
    }
    else if(FD_ISSET(sockfd, &testfds)){
        printf("connected\n");
    }
    else {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    int byteCount = 0;
    while(1) {
        // if((ret = recv(sockfd, recvBuff, sizeof(recvBuff), 0)) < 0) {
        //     perror("recv error");
        //     exit(EXIT_FAILURE);
        // }
        // else{
        //     printf("Read %d bytes from 6-1-1: %s\n", ret, recvBuff);
        // }
        if((ret = send(sockfd, sendBuff, 1, 0)) < 0) {
            perror("Send error");
            testfds = writefds;
            ret = select(FD_SETSIZE, NULL, &testfds, NULL, NULL);
            if (ret <= 0) {
                perror("select error");
                exit(0);
            }
            else if (FD_ISSET(sockfd, &testfds))   
                continue;
        }
        else {
            byteCount += ret;
            printf("send %d bytes to 6-1-1\r", byteCount);
        }
    }

    close(sockfd);
    
    return 0;
}