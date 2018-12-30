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
    int lis_fd, cli_fd, ret;
    struct sockaddr_un ser_un = {0};
    struct sockaddr_un cli_un = {0};
    char *serFile = "\0test";
    char sendBuff[100] = "Hi, I am 6-1-1", recvBuff[100];
    int flag = 1;
    fd_set readfds, testfds;

    if ((lis_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }   

    // 设置监听socket非阻塞
    flag = fcntl(lis_fd, F_GETFL, 0);              //获取文件的flags值。
    fcntl(lis_fd, F_SETFL, flag | O_NONBLOCK);     //设置成非阻塞模式；

    ser_un.sun_family = AF_UNIX;
    strncpy(ser_un.sun_path, serFile, sizeof(ser_un.sun_path));

    // unlink(ser_un.sun_path);
    // if (setsockopt(lis_fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
    //     perror("setsockopt error");
    // }

    if (bind(lis_fd, (struct sockaddr *)&ser_un, sizeof(ser_un)) == -1) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if(listen(lis_fd, 20) < 0){
        perror("listen error");
        exit(EXIT_FAILURE);
    }
    
    FD_ZERO(&readfds);   
    FD_SET(lis_fd, &readfds);//将监听socket加入到集合中 
    testfds = readfds;
    printf("waiting for connecting......\n");
    ret = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
    if(ret <= 0) {
        perror("select error");
        exit(0);
    }
    else 
        FD_ISSET(lis_fd, &testfds);

    cli_fd = accept(lis_fd,NULL,NULL);
    if(cli_fd < 0) {
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");

    getchar();

    // 等待客户端消息
    FD_ZERO(&readfds);  // 初始化集合 
    FD_SET(cli_fd, &readfds);//将连接socket加入到集合中 

    while(1) {
        testfds = readfds;
        ret = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(ret < 0) {
            continue;
        }
        else if(FD_ISSET(cli_fd, &testfds)) {
            if((ret = recv(cli_fd, recvBuff, sizeof(recvBuff), 0)) < 0) {
                perror("recv error");
                exit(EXIT_FAILURE);
            }
            else{
                printf("Read %d bytes from 6-1-2: %s\n", ret, recvBuff);
            }
        }
        // if((ret = send(cli_fd, sendBuff, sizeof(sendBuff), 0)) < 0) {
        //     perror("send error");
        //     exit(EXIT_FAILURE);
        // }
        // else
        //     printf("Send %d bytes\n", ret);

    }

    close(cli_fd);
    close(lis_fd);
    
    return 0;
}