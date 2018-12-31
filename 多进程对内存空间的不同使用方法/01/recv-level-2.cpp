#include "../common/common.h"

using namespace std;

void signal_handle(int signal) {
    cout << "recv-level-2 "
         << "get ready signal" << endl;
}

int main() {
    int rfd, wfd;
    char* rfilename = "1to2.dat";
    char* wfilename = "2to3.dat";
    int len, bufsize = 2048;
    unsigned char buf[bufsize];
    MACHead macHead;

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, buf, bufsize);
    if(len <= 0){
        cerr << "recv-level2 read error" << endl;
        exit(EXIT_FAILURE);
    }
    close(rfd);
    remove(rfilename);

    memcpy(&macHead, buf, MAC_HEAD_LEN);

    cout << "recv-level-2 mac head:" << endl;
    analyzeMachead(macHead);

    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if (wfd < 0) {
        cerr << "open " << wfilename << " error" << endl;
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
    cout << "--------------------------------" << endl;
    return 0;
}
