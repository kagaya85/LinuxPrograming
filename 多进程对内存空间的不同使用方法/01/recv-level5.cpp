#include "../common/common.h"

using namespace std;

int main() {
    int rfd, wfd;
    char* rfilename = "4to5.txt";
    char* wfilename = "receiver.dat"

        signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    wfd = open(wfilename, O_WRONLY);
    if (wfd < 0) {
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    int ret, len, bufsize = 2000;
    char* buf[bufsize];
    flock(rfd, LOCK_SH);
    len = read(rfd, buf, bufsize);
    if (len < 0) {
        cerr << "read error" << endl;
    }
    close(rfd);

    flock(wfd, LOCK_EX);
    ret = write(wfd, buf, len) if (ret < 0) { cerr << "write error" << endl; }
    close(wfd);
    cout << "recv-level5 complete" << endl;
}

void signal_handle(int signal) {
    cout << "recv-level5 "
         << "get ready signal" << endl;
}