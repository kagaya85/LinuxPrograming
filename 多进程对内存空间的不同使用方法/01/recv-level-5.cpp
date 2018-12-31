#include "../common/common.h"

using namespace std;

void signal_handle(int signal) {
    cout << "recv-level-5 "
         << "get ready signal" << endl;
}

int main() {
    int rfd;
    const char* rfilename = "4to5.dat";
    const char* wfilename = "receiver.dat";

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if (rfd < 0) {
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    int ret, len, bufsize = 2048;
    unsigned char buf[bufsize];
    flock(rfd, LOCK_SH);
    len = read(rfd, buf, bufsize);
    if (len < 0) {
        cerr << "read error" << endl;
    }
    close(rfd);
    remove(rfilename);

    formatToFile(buf, len, wfilename);
    
    cout << "recv-level-5 complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
