#include "../common/common.h"

using namespace std;

int main() {
    int rfd, wfd;
    char* rfilename = "network.dat";
    char* wfilename = "1to2.dat";
    int ret, bufsize = 2048;
    char* buf[bufsize];

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    wfd = open(wfilename, O_WRONLY);
    if (wfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(rfd, LOCK_SH);
    flock(wfd, LOCK_EX);
    while (true) {
        ret = read(rfd, buf, bufsize);
        if (ret > 0)
            write(wfd, buf, ret);
        else
            break;
    }

    int pid;
    while ((pid = getPidByName("recv-level-2")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level1 complete" << endl;
    return 0;
}

void signal_handle(int signal) {
    cout << "recv-level1 "
         << "get ready signal" << endl;
}