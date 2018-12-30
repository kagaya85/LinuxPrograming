#include "../common/common.h"

using namespace std;

int main()
{
    int rfd;
    char* rfilename = "4to5.txt";

    signal(SIGREADY, signal_handle);

    pause();

    rfd = open(rfilename, O_RDONLY);
    if(rfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    int ret, bufsize = 1000;
    char* buf[bufsize];
    while(true) {
        ret = read(rfd, buf, bufsize);

    }

    cout << "recv-level5 complete" << endl;
}

void signal_handle(int signal)
{
    cout << "recv-level5 " << "get ready signal" << endl;
}