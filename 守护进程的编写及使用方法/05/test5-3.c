#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

static void child(int signo);
int sub();

int recycled;

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("ֻ����һ������\n");
        exit(1);
    }
    
    int n = atoi(argv[1]);
    if(n == 0)
    {
        printf("��������\n");
        exit(1);
    }
    
    pid_t pid, ppid, cpid, cppid, maxpid;
    daemon(0, 1);
    signal(SIGCHLD, child);
    // ��ȡ�����̵�id��ppid
    pid = getpid();
    ppid = getppid();
    maxpid = pid;

    int i;
    for(i = 0; i < n; i++)
    {
        cpid = fork();
        if(cpid < 0)
        {
            printf("����ʧ��1s�����ԣ��ѷ���%d���ӽ��̣������̺�%d, �ѻ���%d������\n", i, maxpid, recycled);
            sleep(1);
            i--;
            continue;
        }
        // child process
        else if(cpid == 0)
        {
            prctl(PR_SET_PDEATHSIG, SIGKILL);
            sub();
            exit(0);
        }
        if(maxpid < cpid)
            maxpid = cpid;
        if((i + 1) % 100 == 0)
            printf("�ѷ���%d���ӽ��̣������̺�%d���ѻ���%d������\n", i + 1, maxpid, recycled);
    }

    while(1)
    {
        printf("����ɷ���%d�����̣������̺�%d���ѻ���%d������\n", n, maxpid, recycled);
        sleep(1);
    }

    return 0;
}

static void child(int signo)
{
    pid_t pid;

    while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
        recycled++;        
}

int sub()
{
    char str[1024] = "hello world!"; 
    sleep(20);
    return 0;
}