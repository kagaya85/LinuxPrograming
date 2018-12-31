#include "../common/common.h"

using namespace std;

const char* namestr = "send-level-2";

void signal_handle(int signal)
{
    cout << namestr << " get ready signal" << endl;
}

int main()
{
    int rfd, wfd;
    char* rfilename = "3to2.dat";
    char* wfilename = "2to1.dat";
    int len, bufsize = 2048;
    unsigned char buf[bufsize];
    MACHead macHead;

    signal(SIGREADY, signal_handle);

    pause();

    macHead = getMacHead();
    memcpy(buf, &macHead, MAC_HEAD_LEN);

    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, &buf[MAC_HEAD_LEN], bufsize);
    if(len <= 0) {
        cerr << namestr << " read error" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "get " << len << " byte(s)" << endl;
    close(rfd);
    remove(rfilename);

    cout << namestr << " mac head:" << endl;
    analyzeMachead(macHead);

    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if(wfd < 0){
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    write(wfd, buf, MAC_HEAD_LEN + len);
    close(wfd);

#ifdef DEBUG
    formatToFile(buf, MAC_HEAD_LEN + len,"test2.dat");
#endif

    int pid;
    while ((pid = getPidByName("send-level-1")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << namestr << " complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
