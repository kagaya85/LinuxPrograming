#include "../common/common.h"

using namespace std;

void signal_handle(int signal) {
    cout << "recv-level-4 "
         << "get ready signal" << endl;
}

int main() {
    int rfd, wfd;
    char* rfilename = "3to4.dat";
    char* wfilename = "4to5.dat";
    int len, bufsize = 2000;
    unsigned char buf[bufsize], indexp;
    TCPHead tcpHead;

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, buf, bufsize);
    close(rfd);
    remove(rfilename);

    memcpy(&tcpHead, buf, TCP_HEAD_MIN_LEN);
    int offset = tcpOffset(tcpHead);
    indexp = offset * 4;  // 指向数据部分

    cout << "recv-level-4 tcp head:" << endl;
    analyzeTcphead(tcpHead, buf);

    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if (wfd < 0) {
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(wfd, LOCK_EX);
    write(wfd, &buf[indexp], len - indexp);
    close(wfd);

    int pid;
    while ((pid = getPidByName("recv-level-5")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level-4 complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
