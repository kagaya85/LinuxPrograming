#include "../common/common.h"

using namespace std;

int main() {
    int rfd, wfd;
    char* rfilename = "3to4.dat";
    char* wfilename = "4to5.dat";
    int len, bufsize = 2000;
    char *buf[bufsize], indexp;
    TCPHead tcpHead;

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    len = read(rfd, buf, bufsize);
    memcpy(&tcpHead, buf, TCP_HEAD_MIN_LEN);
    int offset = tcpOffset(tcphead);
    indexp = offset * 4;  // 指向数据部分

    cout << "recv-level-4 tcp head：" << endl;
    analyzeTcphead(tcphead, buf);

    wfd = open(wfilename, O_WRONLY);
    if (wfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(wfd, LOCK_EX);
    write(wfd, &buf[indexp], len - indexp);
    close(wfd);

    while ((pid = getPidByName("recv-level-5")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level-4 complete" << endl;
    return 0;
}

void signal_handle(int signal) {
    cout << "recv-level-4 "
         << "get ready signal" << endl;
}