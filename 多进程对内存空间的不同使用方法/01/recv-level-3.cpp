#include "../common/common.h"

using namespace std;

int main()
{
    int rfd, wfd;
    char* rfilename = "2to3.txt";
    char* wfilename = "3to4.txt";

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    wfd = open(wfilename, O_WRONLY);
    if(wfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    int ret, bufsize = 1000;
    char* buf[bufsize];
    while(true) {
        ret = read(rfd, buf, bufsize);
        if (ret > 0)
            write(wfd, buf, ret);
        else
            break;
    }

    cout << "recv-level3 complete" << endl;
}

void signal_handle(int signal)
{
    cout << "recv-level3 " << "get ready signal" << endl;
}