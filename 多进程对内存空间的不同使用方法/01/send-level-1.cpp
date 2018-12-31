#include "../common/common.h"

using namespace std;

void signal_handle(int signal) {
    cout << "send-level-1 "
         << "get ready signal" << endl;
}

int main() {
    int rfd;
    char* wfilename = "network.dat";
    char* rfilename = "2to1.dat";
    int ret, bufsize = 2048;
    unsigned char buf[bufsize];

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(rfd, LOCK_SH);

    ret = read(rfd, buf, bufsize);
    cout << "get " << ret << " byte(s)" << endl;
    close(rfd);
    remove(rfilename);

    formatToFile(buf, ret, wfilename);

    int pid;
    while ((pid = getPidByName("recv-level-1")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "send-level-1 complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
