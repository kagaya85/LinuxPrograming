#include "../common/common.h"

using namespace std;

int main()
{
    int rfd, wfd;
    char* rfilename = "5to4.txt";
    char* wfilename = "4to3.txt";

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

    ret = read(rfd, buf, bufsize);
    if (ret > 0) {
        cout << "send-level4 has read " << ret << " byte(s) from level5" << endl;
        /* do someting */
         
    }
    else {
        cout << "send-level4 read error" << endl;
    }


    cout << "send-level4 complete" << endl;
}

void signal_handle(int signal)
{
    cout << "send-level4 " << "get ready signal" << endl;
}