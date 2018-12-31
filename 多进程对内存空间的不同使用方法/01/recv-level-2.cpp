#include "../common/common.h"

using namespace std;

int main() {
    int rfd, wfd;
    char* rfilename = "1to2.dat";
    char* wfilename = "2to3.dat";
    int ret, bufsize = 1000;
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


    int pid;
    while ((pid = getPidByName("recv-level-3")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level-2 complete" << endl;
}

void signal_handle(int signal) {
    cout << "recv-level-2 "
         << "get ready signal" << endl;
}