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
    int sockHandle;
    int servport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvBuff[4096], sendBuff[4096];
    int msglen, rNum, wNum;

    if(argc != 5) {
        printf("need <server ip address> <server port> <read num> <write num>\n");
        exit(0);
    }

    // 初始化
    if((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // 参数最后一个0代表自动选择协议
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    
    rNum = atoi(argv[3]);
    wNum = atoi(argv[4]);
    
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
    if(connect(sockHandle, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }


    memset(&sendBuff, '1', sizeof(sendBuff));
    int i;
    for(;;){
    // for(i = 0; i < 10; i++){
        if ((msglen = write(sockHandle, sendBuff, wNum)) <= 0) {
            printf("send message error\n");
            exit(0);
        }
        else
            printf("write to server %d bytes\n", msglen);

        if((msglen = read(sockHandle, recvBuff, rNum)) <= 0) {
            printf("read socket error\n");
            exit(0);
        }
        else {
            recvBuff[msglen] = '\0';
            printf("read from server %d bytes\n", msglen);
        }
    }

    printf("Socket closed\n");
    close(sockHandle);
    return 0;
}