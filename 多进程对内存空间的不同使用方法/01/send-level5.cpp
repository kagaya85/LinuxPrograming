#include "../common/common.h"

#define SIGREADY (SIGRTMIN + 1)

using namespace std;

int main()
{
    int wfd;
    char* wfilename = "4to5.txt";

    signal(SIGREADY, signal_handle);

    pause();

    wfd = open(rfilename, O_WRONLY);
    if(wfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    int ret, bufsize = 1000;
    
    char* buf[bufsize];
    while(true) {
        ret = write(wfd, buf, bufsize);
        
    }

    cout << "send-level5 complete" << endl;
}

void signal_handle(int signal)
{
    cout << "send-level5 " << "get ready signal" << endl;
}