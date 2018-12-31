#include "../common/common.h"

using namespace std;

void signal_handle(int signal) {
    cout << "recv-level-3 "
         << "get ready signal" << endl;
}

int main() {
    int rfd, wfd;
    char* rfilename = "2to3.dat";
    char* wfilename = "3to4.dat";
    int len, bufsize = 2000;
    unsigned char buf[bufsize];
    IPHead iphead;
    TCPHead tcphead;

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    
    flock(rfd, LOCK_SH);
    len = read(rfd, buf, bufsize);
    if(len <= 0) {
        cerr << "recv-level-3 read error" << endl;
        exit(EXIT_FAILURE);
    }
    close(rfd);
    remove(rfilename);

    memcpy(&iphead, buf, IP_HEAD_LEN);  //  获取ip头
    memcpy(&tcphead, &buf[IP_HEAD_LEN], TCP_HEAD_MIN_LEN);  // 获取下一层的tcp头

    cout << "recv-level-3 ip head:" << endl;
    analyzeIphead(iphead);

    cout << "recv-level-3 ip cksum check: ";
    analyzeIpCksum(iphead);

    cout << "recv-level-3 tcp cksum check: ";
    analyzeTcpCksum(iphead, tcphead);

    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if (wfd < 0) {
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(wfd, LOCK_EX);
    write(wfd, &buf[IP_HEAD_LEN], len - IP_HEAD_LEN);
    close(wfd);

    int pid;
    while ((pid = getPidByName("recv-level-4")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level-3 complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
