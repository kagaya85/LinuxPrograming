#include "../common/common.h"

using namespace std;

const char* namestr = "send-level5";

int main()
{
    int wfd;
    char* wfilename = "5to4.txt";
    int len, ret;
    char* data = NULL;

    string result = get_from_file(LEVEL_APP, "datalen");
    if (result.length() <= 0)
        len = 1024;
    else
        len = atoi(result.c_str());

    if (len < 1 || len > 1460) len = 1024;

    data = new (nothrow) char[len];
    if (data == NULL) {
        cerr << namestr << "new error" << endl;
        exit(EXIT_FAILURE);
    }

    generateData(data, len);

    wfd = open(wfilename, O_WRONLY);
    if (wfd < 0) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }
    flock(wfd, LOCK_EX);
    ret = write(wfd, data, strlen(data));
    if (ret < strlen(buf)) {
        cerr << namestr << " write error" << endl;
    } else {
        int pid;
        while ((pid = getPidByName("send-level-1")) == 0) {
            sleep(1);
        }
        kill(pid, SIGREADY);
    }
    close(wfd);
    cout << namestr << " complete" << endl;
    return 0;
}