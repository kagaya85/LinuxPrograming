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
    MACHead macHead;

    signal(SIGREADY, signal_handle);

    pause();

    macHead = getMacHead();
    memcpy(buf, &macHead, MAC_HEAD_LEN);

    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, &buf[MAC_HEAD_LEN], bufsize);
    if(len <= 0) {
        cerr << namestr << " read error" << endl;
        exit(EXIT_FAILURE);
    }
    close(rfd);
    remove(rfilename);

    cout << namestr << " mac head:" << endl;
    analyzeMachead(macHead);

    wfd = open(wfilename, O_WRONLY);
    if(wfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    write(wfd, buf, MAC_HEAD_LEN + len);
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