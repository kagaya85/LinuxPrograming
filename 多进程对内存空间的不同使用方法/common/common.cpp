#include "common.h"

using namespace std;

pid_t getPidByName(const char *const task_name) {
    int pid = 0;
    char shell[100] = "ps -e | grep \'";

    strcat(shell, task_name);
    strcat(shell, "\' | awk \'{print $1}\'");

    FILE *fp = popen(shell, "r");
    char ret[10] = {0};

    while (NULL != fgets(ret, 10, fp)) {
        pid = atoi(ret);
        break;
    }
    pclose(fp);

    return pid;
}

void generateData(unsigned char *data, int len) {
    srand((unsigned int)time(0));

    if (data == NULL) return;

    for (int i = 0; i < len; i++) {
        data[i] = rand() % 256;
    }
}

unsigned short ip_chksum(unsigned short checksum, char *ptr, int len) {
    unsigned long long cksum;
    int idx;
    int odd;

    cksum = (unsigned long long)checksum;
    odd = len & 1;
    len -= odd;

    for (idx = 0; idx < len; idx += 2) {
        cksum += ((unsigned int)ptr[idx] << 8) |
                 ((unsigned int)ptr[idx + 1] & 0x00FF);
        // printf("%x :",(unsigned int)cksum);
        // printf("%x %x\n",ptr[idx],ptr[idx+1]);
        // cout<<dec<<cksum<<endl;
    }

    if (odd) {
        cksum += ((unsigned int)ptr[idx] << 8);
    }

    while (cksum >> 16) {
        cksum = (cksum & 0x0000FFFF) + (cksum >> 16);
    }
    return cksum;
}

unsigned short tcp_checksum(char *tcphead, int tcplen, unsigned int *srcaddr,
                            unsigned int *destaddr) {
    char pseudoheader[12];
    unsigned int calccksum;
    memcpy(&pseudoheader[0], srcaddr, IP_ADDR_LEN);
    memcpy(&pseudoheader[4], destaddr, IP_ADDR_LEN);
    pseudoheader[8] = 0;
    pseudoheader[9] = IPPROTO_TCP;
    pseudoheader[10] = (tcplen >> 8) & 0xFF;
    pseudoheader[11] = (tcplen & 0xFF);

    calccksum = ip_chksum(0, pseudoheader, sizeof(pseudoheader));
    calccksum = ip_chksum(calccksum, tcphead, tcplen);
    calccksum = ~calccksum;
    return calccksum;
}

unsigned char hex_to_char(char temp) {
    unsigned char result;
    if (temp >= '0' && temp <= '9')
        result = temp - '0';
    else if (temp >= 'a' && temp <= 'f')
        result = 10 + temp - 'a';
    else if (temp >= 'A' && temp <= 'F')
        result = 10 + temp - 'A';
    else
        return 255;
    return result;
}

string readConf(string layer, string type) {
    ifstream fin;
    fin.open(CONFIG, ios::in);
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

MACHead getMacHead() {
    MACHead head;
    string dstmac = readConf(LAYER_DAT, "dstmac");
    string srcmac = readConf(LAYER_DAT, "srcmac");
    short type = htons(0x0800);
    unsigned char src[6];
    unsigned char dst[6];
    for (int i = 0, j = 0; i < 6; i++, j = j + 3) {
        unsigned char h = hex_to_char(srcmac[j]);
        unsigned char l = hex_to_char(srcmac[j + 1]);
        src[i] = (h << 4) | l;
    }
    for (int i = 0, j = 0; i < 6; i++, j = j + 3) {
        unsigned char h = hex_to_char(dstmac[j]);
        unsigned char l = hex_to_char(dstmac[j + 1]);
        dst[i] = (h << 4) | l;
    }
    memcpy(&head.src, &src, sizeof(src));
    memcpy(&head.dst, &dst, sizeof(dst));
    memcpy(&head.type, &type, sizeof(type));

    return head;
}

IPHead getIpHead(int length) {
    IPHead head;
    srand((unsigned int)time(0));

    unsigned short verlen = 0x45;
    unsigned short tos = 0x00;
    unsigned short temp_ver_tos = htons(verlen << 8 | tos);
    unsigned short iplen = htons(length + IP_HEAD_LEN);
    unsigned short identification = htons(rand());

    string flag = readConf(LAYER_NET, "flag");
    unsigned short flag_temp = 1;
    int len = flag.length();
    for (int i = 0; i < len; i++)
        if (flag[i] == '1')
            flag_temp = (flag_temp << 1) | 1;
        else
            flag_temp = (flag_temp << 1);

    unsigned short offset;
    string result = readConf(LAYER_NET, "offset");
    if (result.length() <= 0)
        offset = rand() % 8192;
    else
        offset = atoi(result.c_str());

    flag_temp = htons((flag_temp << 13) | offset);

    unsigned char ttl;
    result = readConf(LAYER_NET, "ttl");
    if (result.length() <= 0)
        ttl = rand() % 256;
    else
        ttl = atoi(result.c_str());

    unsigned char proto = 6;
    unsigned short cksum = 0;
    unsigned int srcip = (inet_addr(readConf(LAYER_NET, "srcip").c_str()));
    unsigned int desip = (inet_addr(readConf(LAYER_NET, "dstip").c_str()));

    cksum = htons(~ip_chksum(cksum, (char*)&head, 20));

    memcpy((char *)&head, &temp_ver_tos, 2);
    memcpy((char *)&head + 2, &iplen, 2);
    memcpy((char *)&head + 4, &identification, 2);
    memcpy((char *)&head + 6, &flag_temp, 2);
    memcpy((char *)&head + 8, &ttl, 1);
    memcpy((char *)&head + 9, &proto, 1);
    memcpy((char *)&head + 10, &cksum, 2);
    memcpy((char *)&head + 12, &srcip, 4);
    memcpy((char *)&head + 16, &desip, 4);
    memcpy((char *)&head + 10, &cksum, 2);

    return head;
}

unsigned char* getTcpHead(unsigned char* data, int datalen) {
    string result;
    srand((unsigned int)time(0));
    unsigned short target;
    result = readConf(LAYER_TRS, "srcport");
    if (result.length() <= 0)
        target = rand() % 65545 + 1;
    else
        target = htons(atoi(result.c_str()));

    unsigned short source;
    result = readConf(LAYER_TRS, "dstport");
    if (result.length() <= 0)
        source = rand() % 65545 + 1;
    else
        source = htons(atoi(result.c_str()));

    unsigned seq = htonl(rand());
    unsigned ack = htonl(rand());

    unsigned short offset = atoi(readConf(LAYER_TRS, "offset").c_str());
    unsigned short reserved = 0;
    string flag = readConf(LAYER_TRS, "flag");
    unsigned short flag_temp = 0;
    int len = flag.length();
    for (int i = 0; i < len; i++)
        if (i != len - 1) {
            if (flag[i] == '1')
                flag_temp = (flag_temp | 1) << 1;
            else
                flag_temp = (flag_temp << 1);
        } else {
            if (flag[i] == '1')
                flag_temp = (flag_temp | 1);
            else
                flag_temp = (flag_temp);
        }

    unsigned short head = htons((offset << 12) | (reserved << 6) | flag_temp);

    unsigned short window = htons(rand());
    unsigned short cksum = 0;
    unsigned urg;
    urg = flag[0] == '0' ? 0 : rand();
    
    char* buf = new(nothrow) char[offset * 4 + datalen];
    memcpy(buf + offset * 4, data, datalen);  // copy data
    memcpy(buf, &target, 2);
    memcpy(buf + 2, &source, 2);
    memcpy(buf + 4, &seq, 4);
    memcpy(buf + 8, &ack, 4);
    memcpy(buf + 12, &head, 2);
    memcpy(buf + 14, &window, 2);
    memcpy(buf + 16, &cksum, 2);
    memcpy(buf + 18, &urg, 2);
    for (int i = TCP_HEAD_MIN_LEN; i < offset * 4; i++) buf[i] = rand() % 256;

    unsigned int srcip =
        (inet_addr(readConf(LAYER_NET, "srcip").c_str()));
    unsigned int desip =
        (inet_addr(readConf(LAYER_NET, "dstip").c_str()));

    cksum = htons(
        tcp_checksum(buf, datalen + offset * 4, &srcip, &desip));
    memcpy(buf + 16, &cksum, 2);

    return (unsigned char *)buf;
}

int readFromFile(const char* filename, unsigned char *buf) {
    ifstream fin;
    fin.open(filename, ios::in);
    if (!fin.is_open()) return -1;
    int len = 0;
    while (!fin.eof()) {
        short temp;
        fin >> hex >> temp;
        unsigned char temp_char = temp;
        memcpy(buf + len, &temp_char, 1);
        len++;
    }
    return len;
}

void formatToFile(const unsigned char *data, int len, const char *filename) {
    // 结果文件存在则删除
    if (access(filename, F_OK) == 0) {
        remove(filename);
    }

    FILE *wfp;
    wfp = fopen(filename, "a");
    if (wfp == NULL) {
        cerr << "open " << filename << " error" << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len; i++) {
        fprintf(wfp, "%02x ", data[i]);
        if ((i + 1) % 16 == 0) fprintf(wfp, "\n");
    }

    fclose(wfp);
}

int tcpOffset(TCPHead tcphead) {
    unsigned short head = 0;
    memcpy(&head, (char*)&tcphead + 12, 2);
    unsigned char offset = 0;
    head = ntohs(head);
    offset = head >> 12;
    return offset;
}

void analyzeIphead(IPHead iphead) {
    cout << "version and len: " << hex << (unsigned short)(iphead.verlen) << dec
         << '(' << (unsigned short)(iphead.verlen) << ")" << endl;
    cout << "tos: " << hex << (unsigned short)(iphead.tos) << dec << '('
         << (unsigned short)(iphead.tos) << ")" << endl;
    cout << "length: " << hex << ntohs(iphead.iplen) << dec << '('
         << ntohs(iphead.iplen) << ")" << endl;
    cout << "identification: " << hex << ntohs(iphead.identification) << dec << '('
         << ntohs(iphead.identification) << ")" << endl;
    cout << "Flag: " << hex << ntohs(iphead.flag) << dec << '('
         << ntohs(iphead.flag) << ")" << endl;
    cout << "ttl: " << hex << (unsigned short)(iphead.ttl) << dec << '('
         << (unsigned short)(iphead.ttl) << ")" << endl;
    cout << "protocol: " << hex << (unsigned short)(iphead.proto) << dec << '('
         << (unsigned short)(iphead.proto) << ")" << endl;
    cout << "sheck sum: " << hex << ntohs(iphead.cksum) << dec << '('
         << ntohs(iphead.cksum) << ")" << endl;
    cout << "source ip: ";
    struct in_addr inAddr;
    inAddr.s_addr = iphead.srcip;
    cout << hex << ntohl(iphead.srcip) << " - " << inet_ntoa(inAddr) << endl;
    cout << "dest ip: ";
    inAddr.s_addr = iphead.dstip;
    cout << hex << ntohl(iphead.dstip) << " - " << inet_ntoa(inAddr) << endl;
}

void analyzeTcphead(TCPHead tcphead, const unsigned char *buf) {
    cout << "source port: " << hex << ntohs(tcphead.srcport) << dec << '('
         << ntohs(tcphead.srcport) << ")" << endl;
    cout << "dest port: " << hex << ntohs(tcphead.dstport) << dec << '('
         << ntohs(tcphead.dstport) << ")" << endl;
    cout << "seq: " << hex << ntohl(tcphead.seq) << dec << '('
         << ntohl(tcphead.seq) << ")" << endl;
    cout << "ack: " << hex << ntohl(tcphead.ack) << dec << '('
         << ntohl(tcphead.ack) << ")" << endl;
    cout << "offset: " << hex << tcpOffset(tcphead) << dec << '('
         << tcpOffset(tcphead) << ")" << endl;
    cout << "code: " << hex << (ntohs(tcphead.head) & (0x003F)) << dec << '('
         << (ntohs(tcphead.head) & (0x003F)) << ")" << endl;
    cout << "winsow size: " << hex << ntohs(tcphead.windowsize) << dec << '('
         << ntohs(tcphead.windowsize) << ")" << endl;
    cout << "check sum: " << hex << ntohs(tcphead.chsum) << dec << '('
         << ntohs(tcphead.chsum) << ")" << endl;
    cout << "urg: " << hex << ntohs(tcphead.urg) << dec << '('
         << ntohs(tcphead.urg) << ")" << endl;
    cout << "extern：" << endl;
    int offset = tcpOffset(tcphead) * 4 - 20;
    for (int i = 1; i <= offset; i++) {
        cout.fill('0');
        cout << setw(2) << hex << (unsigned short)buf[i - 1 + 20] << " ";
        if (i % 16 == 0) cout << endl;
    }
    cout << endl;
}

void analyzeMachead(MACHead machead) {
    cout << "source MAC：";

    for (int i = 0; i < 6; i++) {
        cout << hex << (unsigned short)machead.src[i];
        if (i != 5) cout << ":";
    }
    cout << endl;

    cout << "dest MAC：";
    for (int i = 0; i < 6; i++) {
        cout << hex << (unsigned short)machead.dst[i];
        if (i != 5) cout << ":";
    }
    cout << endl;
    cout << "type:" << hex << ntohs(machead.type) << '(' << ntohs(machead.type)
         << ")" << endl;
}

void analyzeData(unsigned char *buf, int length) {
    for (int i = 1; i <= length; i++) {
        cout.fill('0');
        cout << setw(2) << hex << (unsigned short)buf[i - 1] << " ";
        if (i % 16 == 0) cout << endl;
    }
}

void analyzeIpCksum(IPHead iphead) {
    unsigned short result = htons(ip_chksum(iphead.cksum, (char*)&iphead, IP_HEAD_LEN));
    cout << result << endl;
    if (result == ntohs(iphead.cksum))
        cout << "校验正确" << endl;
    else
        cout << "校验错误" << endl;
}

void analyzeTcpCksum(IPHead iphead, TCPHead tcphead) {
    int offset = tcpOffset(tcphead);

    unsigned short cksum;
    cksum = tcphead.chsum;

    unsigned short zero = 0;
    tcphead.chsum = zero;

    unsigned short result = tcp_checksum((char*)&tcphead,
                                         ntohs(iphead.iplen) - IP_HEAD_LEN,
                                         &iphead.srcip, &iphead.dstip);
    cout << "result: " << result << endl;

    if (result == ntohs(tcphead.chsum))
        cout << "校验正确" << endl;
    else
        cout << "校验错误" << endl;
}
