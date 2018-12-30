#include "../common/common.h"

using namespace std;

int main()
{
    int wfd;
    char* wfilename = "5to4.txt";

    signal(SIGREADY, signal_handle);

    pause();

    wfd = open(rfilename, O_WRONLY);
    if(wfd < 0){
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    int ret, bufsize = 1000;

    char* buf[bufsize] = "test";

    ret = write(wfd, buf, strlen(buf));
    if (ret < strlen(buf)) {
        cerr << "send-level5 write error" << endl;
    }
    else{
        int pid = getPidByName("d-level4");
        kill(pid, SIGREADY);
    }

    cout << "send-level5 complete" << endl;
}

void signal_handle(int signal)
{
    cout << "send-level5 " << "get ready signal" << endl;
}