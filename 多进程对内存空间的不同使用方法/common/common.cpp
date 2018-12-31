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

void formatToFile(const unsigned char *data, int len, const char *filename) {
    // ����ļ�������ɾ��
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
    cout << "�汾+��λ����:" << hex << (unsigned short)(iphead.verlen) << dec
         << '(' << (unsigned short)(iphead.verlen) << ")" << endl;
    cout << "��������:" << hex << (unsigned short)(iphead.tos) << dec << '('
         << (unsigned short)(iphead.tos) << ")" << endl;
    cout << "�ܳ���:" << hex << ntohs(iphead.iplen) << dec << '('
         << ntohs(iphead.iplen) << ")" << endl;
    cout << "��ʶ:" << hex << ntohs(iphead.identification) << dec << '('
         << ntohs(iphead.identification) << ")" << endl;
    cout << "Flag:" << hex << ntohs(iphead.flag) << dec << '('
         << ntohs(iphead.flag) << ")" << endl;
    cout << "����ʱ��:" << hex << (unsigned short)(iphead.ttl) << dec << '('
         << (unsigned short)(iphead.ttl) << ")" << endl;
    cout << "Э��:" << hex << (unsigned short)(iphead.proto) << dec << '('
         << (unsigned short)(iphead.proto) << ")" << endl;
    cout << "У���" << hex << ntohs(iphead.cksum) << dec << '('
         << ntohs(iphead.cksum) << ")" << endl;
    cout << "Դip:";
    struct in_addr inAddr;
    inAddr.s_addr = iphead.srcip;
    cout << hex << ntohl(iphead.srcip) << " - " << inet_ntoa(inAddr) << endl;
    cout << "Ŀ��ip:";
    inAddr.s_addr = iphead.dstip;
    cout << hex << ntohl(iphead.dstip) << " - " << inet_ntoa(inAddr) << endl;
}

void analyzeTcphead(TCPHead tcphead, const unsigned char* buf) {
    cout << "Դ�˿ں�  :" << hex << ntohs(tcphead.srcport) << dec << '('
         << ntohs(tcphead.srcport) << ")" << endl;
    cout << "Ŀ��˿ں�:" << hex << ntohs(tcphead.dstport) << dec << '('
         << ntohs(tcphead.dstport) << ")" << endl;
    cout << "���:" << hex << ntohl(tcphead.seq) << dec << '('
         << ntohl(tcphead.seq) << ")" << endl;
    cout << "ȷ�����:" << hex << ntohl(tcphead.ack) << dec << '('
         << ntohl(tcphead.ack) << ")" << endl;
    cout << "tcp����:" << hex << Tcp_offset(tcphead) << dec << '('
         << Tcp_offset(tcphead) << ")" << endl;
    cout << "code:" << hex << (ntohs(tcphead.head) & (0x003F)) << dec << '('
         << (ntohs(tcphead.head) & (0x003F)) << ")" << endl;
    cout << "���ڴ�С:" << hex << ntohs(tcphead.windowsize) << dec << '('
         << ntohs(tcphead.windowsize) << ")" << endl;
    cout << "У���:" << hex << ntohs(tcphead.chsum) << dec << '('
         << ntohs(tcphead.chsum) << ")" << endl;
    cout << "����ָ��:" << hex << ntohs(tcphead.urg) << dec << '('
         << ntohs(tcphead.urg) << ")" << endl;
    cout << "ѡ�" << endl;
    int offset = tcpOffset(tcphead) * 4 - 20;
    for (int i = 1; i <= offset; i++) {
        cout.fill('0');
        cout << setw(2) << hex << (unsigned short)buf[i - 1 + 20] << " ";
        if (i % 16 == 0) cout << endl;
    }
    cout << endl;
}

void analyzeMachead(MACHead machead) {
    cout << "ԴMAC��ַ��";

    for (int i = 0; i < 6; i++) {
        cout << hex << (unsigned short)machead.src[i];
        if (i != 5) cout << ":";
    }
    cout << endl;

    cout << "Ŀ��MAC��ַ��";
    for (int i = 0; i < 6; i++) {
        cout << hex << (unsigned short)machead.dst[i];
        if (i != 5) cout << ":";
    }
    cout << endl;
    cout << "����:" << hex << ntohs(machead.type) << '('
         << ntohs(machead.type) << ")" << endl;
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
        cout << "У����ȷ" << endl;
    else
        cout << "У�����" << endl;
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
        cout << "У����ȷ" << endl;
    else
        cout << "У�����" << endl;
}
