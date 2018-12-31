#include "../common/common.h"

using namespace std;

int main() {
    int rfd, wfd;
    char* rfilename = "1to2.dat";
    char* wfilename = "2to3.dat";
    int len, bufsize = 2048;
    char* buf[bufsize];
    MACHead macHead;

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flcok(rfd, LOCK_SH);
    len = read(rfd, buf, bufsize);
    if(len <= 0){
        cerr << "recv-level2 read error" << endl;
        exit(EXIT_FAILURE);
    }
    close(fd);
    remove(rfilename);

    memcoy(&macHead, buf, MAC_HEAD_LEN);

    cout << "recv-level-2 mac head:" << endl;
    analyzeMachead(macHead);

    wfd = open(wfilename, O_WRONLY);
    if (wfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(wfd, LOCK_EX);
    write(wfd, &buf[MAC_HEAD_LEN], len - MAC_HEAD_LEN);
    close(wfd);

    int pid;
    while ((pid = getPidByName("recv-level-3")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level-2 complete" << endl;
    return 0;
}

void signal_handle(int signal) {
    cout << "recv-level-2 "
         << "get ready signal" << endl;
}