#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>
#include <time.h>

// 子进程对应pid，可用标记，子进程数量
pid_t cpids[20];
int Flags = 0;
int childNum = 5;
char originName[20];

int readConfig(char *fileName);

void create_daemon() {
    pid_t pid;

    pid = fork();

    if(pid == -1)
    {
        printf("fork error\n");
        exit(1);
    }
    else if(pid)
    {
        exit(0);
    }
    
    return;
}

int locateOne(int num) {
    int i = 0;
    
    while(!(num & 1)){
        i++;
        num = num >> 1;
        if(i > childNum)
            break;
    }
    return i;
}

void addFlags(int* Flags, int n) {
    if(n < 0 || n >= 32)
        return;
    else{
        *Flags = *Flags | (1 << n); 
    }
}

void rmFlags(int* Flags, int n) {
    if(n < 0 || n >= 32)
        return;
    else{
        *Flags = *Flags & ~(1 << n); 
        // printf("%X %X\n", ~(1 << n), *Flags);
    }
}

static void child(int signo) {
    pid_t pid;
    int i;
    while ((pid = waitpid(-1, NULL, WNOHANG)) >0){
        for(i = 0; i < childNum; i++){
            if(cpids[i] == pid)
                break;
        }
        cpids[i] = 0;
        addFlags(&Flags, i);
    }
}

void transTime(int* h, int* m, int* s, time_t time1, time_t time2) {
    int delt = time2 - time1;
    
    *h = delt / 3600;
    delt %= 3600;
    *m = delt / 60;
    delt %= 60;
    *s = delt;

    return;
}

int sub(int id, char** argv) {
    time_t timeStart, timeNow;
    int h, m, s;
    timeStart = time((time_t*)NULL);
    
    while(1){
        sleep(1);
        timeNow = time((time_t*)NULL);
        transTime(&h, &m, &s, timeStart, timeNow);
        sprintf(argv[0],"./%s [Sub-%02d %02d:%02d:%02d]", originName, id+1, h, m, s);
    }
        
}


int main(int argc, char** argv) {
    create_daemon();
    signal(SIGCHLD, child);

    childNum = readConfig("/etc/1650275.conf");
    
    time_t pTimeStart, pTimeNow;
    pid_t pid;
    int i, j;

    char validName[20];

    pTimeStart = time((time_t*)NULL);
    // change parent ps name
    prctl(PR_GET_NAME, originName);
    if(strlen(originName) > 6)
        sprintf(originName, "%s", &originName[strlen(originName)-4]);
    sprintf(validName,"./%s [Main]", originName);
    prctl(PR_SET_NAME, validName);

    // initial flag 1 valid 0 used
    for(i = 0; i < childNum; i++){
        Flags = Flags << 1;
        Flags = Flags | 1;
    }

    while(1){
        sleep(1);
        if(Flags && (i = locateOne(Flags)) < childNum){
            pid = fork();
            cpids[i] = pid;
            rmFlags(&Flags, i);
            // printf("Flags:%X\n", Flags);

            // change child ps name        
            sprintf(validName,"./%s [Sub-%02d]", originName, i+1);
                
            if(pid < 0)
            {
                fprintf(stderr, "fork error\n");
                exit(-1);
            }
            // child process
            else if(pid == 0)
            {
                prctl(PR_SET_PDEATHSIG, SIGKILL);
                prctl(PR_SET_NAME, validName);
                sub(i, argv);
                return 0;
            }
            
        }
        else{
            int h, m, s;
            pTimeNow = time((time_t*)NULL);
            transTime(&h, &m, &s, pTimeStart, pTimeNow);
            // printf("%d\n", pTimeNow - pTimeStart);
            sprintf(argv[0],"./%s [Main %02d:%02d:%02d]", originName, h, m, s);
        }
    }

    return 0;
}