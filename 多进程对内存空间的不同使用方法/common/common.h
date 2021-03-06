#ifndef COMMON
#define COMMON
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <iomanip>

// #define DEBUG

#define SIGREADY (SIGRTMIN + 1)
#define LAYER_PHY "[物理层]"
#define LAYER_NET "[网络层]"
#define LAYER_DAT "[数据链路层]"
#define LAYER_TRS "[传输层]"
#define LAYER_APP "[应用层]"
#define MAC_HEAD_LEN 14
#define TCP_HEAD_MIN_LEN 20
#define IP_HEAD_LEN 20
#define IP_ADDR_LEN 4
#define CONFIG "network.conf"

using namespace std;

struct TCPHead {
    unsigned short srcport;
    unsigned short dstport;
    unsigned int seq;
    unsigned int ack;
    unsigned short head;
    unsigned short windowsize;
    unsigned short chsum;
    unsigned short urg;
};

struct IPHead {
    unsigned char verlen;
    unsigned char tos;
    unsigned short iplen;
    unsigned short identification;
    unsigned short flag;
    unsigned char ttl;
    unsigned char proto;
    unsigned short cksum;
    unsigned int srcip;
    unsigned int dstip;
};

struct MACHead {
    unsigned char src[6];
    unsigned char dst[6];
    short type;
};

void generateData(unsigned char *data, int len);
pid_t getPidByName(const char *const);
unsigned short tcp_checksum(char *tcphead, int tcplen, unsigned int *srcip,
                            unsigned int *destip);
unsigned short ip_chksum(unsigned short checksum, char *ptr, int len);

string readConf(string layer, string type);
unsigned char hex_to_char(char temp);
MACHead getMacHead();
IPHead getIpHead(int length);
unsigned char* getTcpHead(unsigned char* data, int datalen);
int readFromFile(const char* filename, unsigned char *buf);
void formatToFile(const unsigned char *data, int len, const char *filename);
int tcpOffset(TCPHead tcphead);
void analyzeTcphead(TCPHead tcphead, const unsigned char *buf);
void analyzeMachead(MACHead machead);
void analyzeIphead(IPHead iphead);
void analyzeData(unsigned char *buf, int length);
void analyzeIpCksum(IPHead iphead);
void analyzeTcpCksum(IPHead iphead, TCPHead tcphead);

#endif