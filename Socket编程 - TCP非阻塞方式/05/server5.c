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
    
    // 安装信号
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

    // 初始化
    if((lis_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // 参数最后一个0代表自动选择协议
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if (setsockopt(lis_sockfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
        perror("setsockopt error");
    }

    // servaddr初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    if(port > 0)
        servaddr.sin_port = htons(port);//设置的端口为s输入参数
    else
        servaddr.sin_port = htons(DEFAULT_PORT);//设置的端口为DEFAULT_PORT

    // 设置监听socket非阻塞
    flag = fcntl(lis_sockfd, F_GETFL, 0);              //获取文件的flags值。
    fcntl(lis_sockfd, F_SETFL, flag | O_NONBLOCK);     //设置成非阻塞模式；

    // 将本地地址绑定到socket上
    if(bind(lis_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 监听连接
    if(listen(lis_sockfd, 10) < 0) {    // 第二个参数为可以排队的最大连接数
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 等待连接阻塞
    FD_ZERO(&readfds);   
    FD_SET(lis_sockfd, &readfds);//将监听socket加入到集合中 
    printf("waiting for connecting......\n");
    
    // 接受连接
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    alarm(1);
    while(1){
        testfds = readfds;
        selResult = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
        if(selResult < 0) {
            // 由于alarm打断
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
        // 遍历一遍
        for(fd = 0; fd < FD_SETSIZE; fd++){
            if(FD_ISSET(fd, &testfds)) {
                // 如果是监听socket活动，接受新的连接
                if(fd == lis_sockfd) {
                    if((con_sockfd = accept(lis_sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0) {
                        printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
                        exit(0);
                    }
                    // 设置连接socket非阻塞
                    flag = fcntl(con_sockfd, F_GETFL, 0);              //获取文件的flags值。
                    fcntl(con_sockfd, F_SETFL, flag | O_NONBLOCK);     //设置成非阻塞模式；
                    int recv_min_size = RECV_BUFF_SIZE;
                    // if(setsockopt(con_sockfd, SOL_SOCKET, SO_RCVLOWAT, (void *)&recv_min_size, sizeof(int)) < 0){
                    //     perror("setsockopt error");
                    // }

                    //解析客户端地址
                    char ipbuff[INET_ADDRSTRLEN + 1] = {0};
                    inet_ntop(AF_INET, &cliaddr.sin_addr, ipbuff, INET_ADDRSTRLEN);
                    uint16_t cli_port = ntohs(cliaddr.sin_port);
                    printf("connection from %s, port %d\n", ipbuff, cli_port);
                    
                    // 将连接socket加入到集合中 
                    FD_SET(con_sockfd, &readfds);
                }
                /*处理其他客户数据请求*/
                else {
                    // ioctl(fd,?FIONREAD,?&nread);//取得数据量交给nread??
                    if((msglen = read(fd, recvBuff, sizeof(recvBuff))) < 0) {
                        printf("read socket error: %s(errno: %d, return: %d)\n", strerror(errno), errno, msglen);
                        exit(0);
                    }
                    else if(msglen == 0) {
                        /*客户数据请求完毕，关闭套接字，从集合中清除相应描述符?*/
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