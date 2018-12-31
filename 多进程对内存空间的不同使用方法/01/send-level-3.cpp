#include "../common/common.h"

using namespace std;

const char* namestr = "send-level-3";

void signal_handle(int signal)
{
    cout << namestr << " get ready signal" << endl;
}

int main()
{
    int rfd, wfd;
    char* rfilename = "4to3.dat";
    char* wfilename = "3to2.dat";
    int len, bufsize = 2048;
    unsigned char buf[bufsize];
    IPHead ipHead;

    signal(SIGREADY, signal_handle);

    pause();
    
    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << rfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(rfd, LOCK_SH);
    len = read(rfd, &buf[IP_HEAD_LEN], bufsize);
    if(len <= 0) {
        cerr << namestr << " read error" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "get " << len << " byte(s)" << endl;
    close(rfd);
    remove(rfilename);
    
    ipHead = getIpHead(len);
    memcpy(buf, &ipHead, IP_HEAD_LEN);
    
    cout << namestr << " ip head:" << endl;
    analyzeIphead(ipHead);

    wfd = open(wfilename, O_WRONLY | O_CREAT);
    if(wfd < 0){
        cerr << "open " << wfilename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    int ret;
    ret = write(wfd, buf, IP_HEAD_LEN + len);
    if (ret <= 0) {
        cerr << namestr << " write error" << endl;
        exit(EXIT_FAILURE);
    }
    close(wfd);

#ifdef DEBUG
    formatToFile(buf, IP_HEAD_LEN + len,"test3.dat");
#endif

    int pid;
    while ((pid = getPidByName("send-level-2")) == 0) {
        sleep(1);
    }

    kill(pid, SIGREADY);

    cout << namestr << " complete" << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
