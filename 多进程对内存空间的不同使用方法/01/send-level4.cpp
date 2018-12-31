#include "../common/common.h"

using namespace std;

const char* namestr = "send-level-4";

int main()
{
    int rfd, wfd;
    char* rfilename = "5to4.dat";
    char* wfilename = "4to3.dat";
    int len, bufsize = 2048;
    char* buf[bufsize];
    TCPHead tcpHead;

    signal(SIGREADY, signal_handle);

    pause();

    tcpHead = getTcpHead();    
    unsigned short offset = tcpOffset(tcpHead);
    memcpy(buf, &tcpHead, TCP_HEAD_MIN_LEN);

    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, &buf[TCP_HEAD_MIN_LEN + offset], bufsize);
    if(len <= 0) {
        cerr << namestr << " read error" << endl;
        exit(EXIT_FAILURE);
    }
    close(rfd);
    remove(rfilename);

    cout << namestr <<" tcp head:" << endl;
    analyzeTcphead(tcpHead, &buf[TCP_HEAD_MIN_LEN]);

    wfd = open(wfilename, O_WRONLY);
    if(wfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    write(wfd, buf, TCP_HEAD_MIN_LEN + offset + len);
    close(wfd);

    int pid;
    while ((pid = getPidByName("send-level-3")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << namestr << " complete" << endl;
    return 0;
}

void signal_handle(int signal)
{
    cout << namestr << " get ready signal" << endl;
}