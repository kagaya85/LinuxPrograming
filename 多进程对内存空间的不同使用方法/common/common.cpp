#include "common.h"

pid_t getPidByName(const char* const task_name) {
    int pid = 0;
    char shell[100] = "ps -e | grep \'";

    strcat(shell, task_name);
    strcat(shell, "\' | awk \'{print $1}\'");

    FILE* fp = popen(shell, "r");
    char ret[10] = {0};

    while (NULL != fgets(ret, 10, fp)) {
        pid = atoi(ret);
        break;
    }
    pclose(fp);

    return pid;
}

unsigned char toHex(unsigned char bit4) {
    if (bit4 % 16 < 10) {
        return bit4 % 16 + '0';
    } else {
        return bit4 % 16 - 10 + 'a';
    }
}

unsigned char* format(const unsigned char* const data, const int len) {
    char* res = new (nothrow) char[2 * len + 1];
    for (int i = 0; i < len; i++) {
        res[2 * i] = toHex(data[i] >> 4);        // 高4bit
        res[2 * i + 1] = toHex(data[i] & 0xf0);  // 低4bit
    }
}

// unsigned char* readFormatString(const unsigned char* const data) {

// }

string readConf(string layer, string type) {
    ifstream fin;
    fin.open(config, ios::in);
    if (!fin.is_open()) exit(-1);
    string result;
    string temp;
    string temp_type;
    while (fin.good()) {
        getline(fin, temp);
        if (temp[0] != '[') continue;
        if (temp == layer) {
            while (fin.good()) {
                getline(fin, temp_type);

                if (temp_type[0] == '[') {
                    result.clear();
                    return result;
                }
                int del_pos = temp_type.find('#');
                if (del_pos < temp_type.length()) {
                    temp_type.erase(del_pos, temp_type.length() - del_pos);
                }
                int del_space = temp_type.find(' ');
                while (del_space < temp_type.length()) {
                    temp_type.erase(del_space, 1);
                    del_space = temp_type.find(' ');
                }
                int pos = temp_type.find(type);
                if (pos == 0) {
                    result.append(temp_type, temp_type.find('=') + 1,
                                  temp_type.length() - temp_type.find('='));
                    return result;
                } else
                    ;
            }
        }
    }
    fin.close();
    result.clear();
    return result;
}
