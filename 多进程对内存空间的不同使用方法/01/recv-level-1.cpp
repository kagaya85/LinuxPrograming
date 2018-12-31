#include "../common/common.h"

using namespace std;

void signal_handle(int signal) {
    cout << "recv-level-1 "
         << "get ready signal" << endl;
}

int main() {
    int wfd;
    char* rfilename = "network.dat";
    char* wfilename = "1to2.dat";
    int ret, bufsize = 2048;
    unsigned char buf[bufsize];

    signal(SIGREADY, signal_handle);

    pause();


    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if (wfd < 0) {
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    flock(wfd, LOCK_EX);
    ret = readFromFile(rfilename, buf);
    write(wfd, buf, ret);
    cout << "get " << ret << " byte(s)" << endl;
    
    int pid;
    while ((pid = getPidByName("recv-level-2")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << "recv-level-1 complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
