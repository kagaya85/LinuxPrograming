#include "../common/common.h"

using namespace std;

const char* namestr = "send-level-2";

int main()
{
    int rfd, wfd;
    char* rfilename = "3to2.txt";
    char* wfilename = "2to1.txt";
    int len, bufsize = 2048;
    char* buf[bufsize];
    IPHead ipHead;

    signal(SIGREADY, signal_handle);

    pause();

    ipHead = getIpHead();
    memcpy(buf, &ipHead, IP_HEAD_LEN);
    
    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, &buf[IP_HEAD_LEN], bufsize);
    if(len <= 0) {
        cerr << namestr << " read error" << endl;
        exit(EXIT_FAILURE);
    }
    close(rfd);
    remove(rfilename);

    wfd = open(wfilename, O_WRONLY);
    if(wfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    int ret;
    ret = write(wfd, buf, IP_HEAD_LEN + len);
    if (ret <= 0) {
        cerr << namestr << " write error" << endl;
        exit(EXIT_FAILURE);
    }
    close(wfd);

    int pid;
    while ((pid = getPidByName("send-level-1")) == 0) {
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