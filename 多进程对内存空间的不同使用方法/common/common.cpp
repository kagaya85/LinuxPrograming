#include "common.h"

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

unsigned short tcp_checksum(unsigned short checksum, char *tcphead, int tcplen,
                            unsigned int *srcaddr, unsigned int *destaddr) {
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

MACHead getMacHead() {
    MACHead head;
    string dstmac = readConf(LEVEL_DATALINK, "dstmac");
    string srcmac = readConf(LEVEL_DATALINK, "srcmac");
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
    head.src = src;
    head.dst = dst;
    head.type = type;

    return head;
}

IPHead getIpHead() {
    IPHead head;
    srand((unsigned int)time(0));

    unsigned short verlen = 0x45;
    unsigned short tos = 0x00;
    unsigned short temp_ver_tos = htons(verlen << 8 | tos);
    unsigned short iplen = htons(length + 20);
    unsigned short identification = htons(rand());

    string flag = readConf(LEVEL_NETWORK, "flag");
    unsigned short flag_temp = 1;
    int len = flag.length();
    for (int i = 0; i < len; i++)
        if (flag[i] == '1')
            flag_temp = (flag_temp << 1) | 1;
        else
            flag_temp = (flag_temp << 1);

    unsigned short offset;
    string result = readConf(LEVEL_NETWORK, "offset");
    if (result.length() <= 0)
        offset = rand() % 8192;
    else
        offset = atoi(result.c_str());

    flag_temp = htons((flag_temp << 13) | offset);

    unsigned char ttl;
    result = readConf(LEVEL_NETWORK, "ttl");
    if (result.length() <= 0)
        ttl = rand() % 256;
    else
        ttl = atoi(result.c_str());

    unsigned char proto = 6;
    unsigned short cksum = 0;
    unsigned int srcip =
        (inet_addr(readConf(LEVEL_NETWORK, "srcip").c_str()));
    unsigned int desip =
        (inet_addr(readConf(LEVEL_NETWORK, "dstip").c_str()));

    cksum = htons(~ip_chksum(cksum, *buf_temp, 20));
    
    memcpy(&head, &temp_ver_tos, 2);
    memcpy(&head + 2, &iplen, 2);
    memcpy(&head + 4, &identification, 2);
    memcpy(&head + 6, &flag_temp, 2);
    memcpy(&head + 8, &ttl, 1);
    memcpy(&head + 9, &proto, 1);
    memcpy(&head + 10, &cksum, 2);
    memcpy(&head + 12, &srcip, 4);
    memcpy(&head + 16, &desip, 4);
    memcpy(&head + 10, &cksum, 2);

    return head;
}

TCPHead getTcpHead() {
    TCPHead tcphead;
    string result;
    srand((unsigned int)time(0));
    unsigned short target;
    result = get_from_file(LEVEL_TRANS, "srcport");
    if (result.length() <= 0)
        target = rand() % 65545 + 1;
    else
        target = htons(atoi(result.c_str()));

    unsigned short source;
    result = get_from_file(LEVEL_TRANS, "dstport");
    if (result.length() <= 0)
        source = rand() % 65545 + 1;
    else
        source = htons(atoi(result.c_str()));

    unsigned seq = htonl(rand());
    unsigned ack = htonl(rand());

    unsigned short offset = atoi(get_from_file(LEVEL_TRANS, "offset").c_str());
    unsigned short reserved = 0;
    string flag = get_from_file(LEVEL_TRANS, "flag");
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

    memcpy(&tcphead + offset * 4, buf, length);  // copy buf

    memcpy(&tcphead, &target, 2);
    memcpy(&tcphead + 2, &source, 2);
    memcpy(&tcphead + 4, &seq, 4);
    memcpy(&tcphead + 8, &ack, 4);

    memcpy(&tcphead + 12, &head, 2);

    memcpy(&tcphead + 14, &window, 2);
    memcpy(&tcphead + 16, &cksum, 2);
    memcpy(&tcphead + 18, &urg, 2);

    // for (int i = 20; i < offset * 4; i++) &head[i] = rand() % 256;

    unsigned int srcip =
        (inet_addr(get_from_file(LEVEL_NETWORK, "srcip").c_str()));
    unsigned int desip =
        (inet_addr(get_from_file(LEVEL_NETWORK, "dstip").c_str()));

    cksum = htons(
        tcp_checksum(cksum, *buf_temp, length + offset * 4, &srcip, &desip));
    memcpy(&tcphead + 16, &cksum, 2);

    return tcphead;
}

void formatToFile(const unsigned char *data, int len, const char *filename) {
    // 结果文件存在则删除
    if (access(filename) == 0) {
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
    memcpy(&head, &tcphead + 12, 2);
    unsigned char offset = 0;
    head = ntohs(head);
    offset = head >> 12;
    return offset;
}

void analyzeIphead(IPHead iphead) {
    cout << "版本+首位长度:" << hex << (unsigned short)(iphead.verlen) << dec
         << '(' << (unsigned short)(iphead.verlen) << ")" << endl;
    cout << "服务类型:" << hex << (unsigned short)(iphead.tos) << dec << '('
         << (unsigned short)(iphead.tos) << ")" << endl;
    cout << "总长度:" << hex << ntohs(iphead.iplen) << dec << '('
         << ntohs(iphead.iplen) << ")" << endl;
    cout << "标识:" << hex << ntohs(iphead.identification) << dec << '('
         << ntohs(iphead.identification) << ")" << endl;
    cout << "Flag:" << hex << ntohs(iphead.flag) << dec << '('
         << ntohs(iphead.flag) << ")" << endl;
    cout << "生存时间:" << hex << (unsigned short)(iphead.ttl) << dec << '('
         << (unsigned short)(iphead.ttl) << ")" << endl;
    cout << "协议:" << hex << (unsigned short)(iphead.proto) << dec << '('
         << (unsigned short)(iphead.proto) << ")" << endl;
    cout << "校验和" << hex << ntohs(iphead.cksum) << dec << '('
         << ntohs(iphead.cksum) << ")" << endl;
    cout << "源ip:";
    struct in_addr inAddr;
    inAddr.s_addr = iphead.srcip;
    cout << hex << ntohl(iphead.srcip) << " - " << inet_ntoa(inAddr) << endl;
    cout << "目的ip:";
    inAddr.s_addr = iphead.dstip;
    cout << hex << ntohl(iphead.dstip) << " - " << inet_ntoa(inAddr) << endl;
}

void analyzeTcphead(TCPHead tcphead, const unsigned char *buf) {
    cout << "源端口号  :" << hex << ntohs(tcphead.srcport) << dec << '('
         << ntohs(tcphead.srcport) << ")" << endl;
    cout << "目标端口号:" << hex << ntohs(tcphead.dstport) << dec << '('
         << ntohs(tcphead.dstport) << ")" << endl;
    cout << "序号:" << hex << ntohl(tcphead.seq) << dec << '('
         << ntohl(tcphead.seq) << ")" << endl;
    cout << "确认序号:" << hex << ntohl(tcphead.ack) << dec << '('
         << ntohl(tcphead.ack) << ")" << endl;
    cout << "tcp长度:" << hex << Tcp_offset(tcphead) << dec << '('
         << Tcp_offset(tcphead) << ")" << endl;
    cout << "code:" << hex << (ntohs(tcphead.head) & (0x003F)) << dec << '('
         << (ntohs(tcphead.head) & (0x003F)) << ")" << endl;
    cout << "窗口大小:" << hex << ntohs(tcphead.windowsize) << dec << '('
         << ntohs(tcphead.windowsize) << ")" << endl;
    cout << "校验和:" << hex << ntohs(tcphead.chsum) << dec << '('
         << ntohs(tcphead.chsum) << ")" << endl;
    cout << "紧急指针:" << hex << ntohs(tcphead.urg) << dec << '('
         << ntohs(tcphead.urg) << ")" << endl;
    cout << "选项：" << endl;
    int offset = tcpOffset(tcphead) * 4 - 20;
    for (int i = 1; i <= offset; i++) {
        cout.fill('0');
        cout << setw(2) << hex << (unsigned short)buf[i - 1 + 20] << " ";
        if (i % 16 == 0) cout << endl;
    }
    cout << endl;
}

void analyzeMachead(MACHead machead) {
    cout << "源MAC地址：";

    for (int i = 0; i < 6; i++) {
        cout << hex << (unsigned short)machead.src[i];
        if (i != 5) cout << ":";
    }
    cout << endl;

    cout << "目标MAC地址：";
    for (int i = 0; i < 6; i++) {
        cout << hex << (unsigned short)machead.dst[i];
        if (i != 5) cout << ":";
    }
    cout << endl;
    cout << "类型:" << hex << ntohs(machead.type) << '(' << ntohs(machead.type)
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
    unsigned short result = htons(ip_chksum(iphead.cksum, (char *)buf, 20));
    cout << result << endl;
    if (result == ntohs(iphead.cksum))
        cout << "校验正确" << endl;
    else
        cout << "校验错误" << endl;
}

void analyzeTcpCksum(IPHead iphead, TCPHead tcphead) {
    int offset = tcpOffset(tcphead);

    unsigned short cksum;
    memcpy(&cksum, buf + IP_LEN + 16, 2);

    unsigned short zero = 0;
    memcpy(buf + IP_LEN + 16, &zero, 2);

    unsigned short result = tcp_checksum(tcphead.chsum, (char *)(buf + IP_LEN),
                                         ntohs(iphead.iplen) - IP_LEN,
                                         &iphead.srcip, &iphead.dstip);
    cout << result << endl;

    memcpy(buf + IP_LEN + 16, &cksum, 2);
    if (result == ntohs(head.chsum))
        cout << "校验正确" << endl;
    else
        cout << "校验错误" << endl;
}
