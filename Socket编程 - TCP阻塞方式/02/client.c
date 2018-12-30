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
    int servport = -1, cliport = -1;
    struct sockaddr_in servaddr, cliaddr;
    char recvbuff[4096], sendBuff[4096];
    int msglen;

    if(argc != 4) {
        printf("need <local port> <server ip address> <server port>\n");
        exit(0);
    }

    cliport = atoi(argv[1]);
    if(cliport < 1024 || cliport > 65535) {
        printf("client port ivalid for %s\n",argv[1]);
        exit(0);
    }

    // 初始化
    if((sockHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // 参数最后一个0代表自动选择协议
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // servaddr初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // 设置ip地址
    if(inet_pton(AF_INET, argv[2], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n",argv[2]);
        exit(0);
    }
    servport = atoi(argv[3]);
    if(servport < 1024 || servport > 65535) {
        printf("server port ivalid for %s\n",argv[3]);
        exit(0);
    }
    if(servport > 0)
        servaddr.sin_port = htons(servport);//设置的端口为输入参数
    else
        servaddr.sin_port = htons(DEFAULT_PORT);//设置的端口为DEFAULT_PORT

    // cliaddr初始化
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    if(cliport > 0)
        cliaddr.sin_port = htons(cliport);//设置的端口为输入参数
    else
        cliaddr.sin_port = htons(0);//设置的端口随机

    // 将本地地址绑定到socket上
    if(bind(sockHandle, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 请求连接
    if(connect(sockHandle, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    
    // 接受消息
    if((msglen = recv(sockHandle, recvbuff, MAXLINE, 0)) == -1) {
       perror("receive error: ");
       exit(1);
    }
    recvbuff[msglen]  = '\0';
    printf("Received from server : %s\n", recvbuff);

    // send hello
    strcpy(sendBuff, "Hello!\n");
    
    if(send(sockHandle, sendBuff, strlen(sendBuff), 0) < 0) {
        printf("send message error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // // 发送消息
    // printf("Send message to server: ");
    // fgets(sendBuff, 4096, stdin);

    // if(send(sockHandle, sendBuff, strlen(sendBuff), 0) < 0) {
    //     printf("send message error: %s(errno: %d)\n", strerror(errno), errno);
    //     exit(0);
    // }

    // 接受消息
    if((msglen = recv(sockHandle, recvbuff, MAXLINE, 0)) == -1) {
       perror("receive error");
       exit(1);
    }
    recvbuff[msglen]  = '\0';
    printf("Received from server : %s\n", recvbuff);
    
    printf("Socket closed\n");
    close(sockHandle);
    return 0;
}