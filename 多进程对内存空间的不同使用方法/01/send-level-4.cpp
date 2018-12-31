#include "../common/common.h"

using namespace std;

const char* namestr = "send-level-4";

void signal_handle(int signal)
{
    cout << namestr << " get ready signal" << endl;
}

int main()
{
    int rfd, wfd;
    char* rfilename = "5to4.dat";
    char* wfilename = "4to3.dat";
    int len, bufsize = 2048;
    unsigned char* buf;
    unsigned char data[bufsize];
    TCPHead tcpHead;

    signal(SIGREADY, signal_handle);

    pause();
    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, data, bufsize);
    if(len <= 0) {
        cerr << namestr << " read error" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "get " << len << " byte(s)" << endl;
    close(rfd);
    remove(rfilename);

    buf = getTcpHead(data, len);
    // cout << "-offset : " << (ntohs((*(TCPHead*)buf).head) >> 12) << endl;    
    memcpy(&tcpHead, buf, TCP_HEAD_MIN_LEN);
    // cout << "--offset : " << (ntohs(tcpHead.head) >> 12) << endl;    
    unsigned short offset = tcpOffset(tcpHead);
    // cout << "+offest : " << offset << endl;

    cout << namestr <<" tcp head:" << endl;
    analyzeTcphead(tcpHead, &buf[TCP_HEAD_MIN_LEN]);

    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if(wfd < 0){
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    write(wfd, buf, offset * 4 + len);
    close(wfd);

#ifdef DEBUG
    formatToFile(buf, offset * 4 + len,"test4.dat");
#endif

    int pid;
    while ((pid = getPidByName("send-level-3")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);
    delete buf;
    cout << namestr << " complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
