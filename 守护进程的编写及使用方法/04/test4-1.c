#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <signal.h>

static void child(int signo);

int main()
{
    pid_t pid, ppid, cpid, cppid;
    daemon(0, 1);
    signal(SIGCHLD, child);
    // ��ȡ�����̵�id��ppid
    pid = getpid();
    ppid = getppid();

    int i;
    for(i = 0; i < 10; i++)
    {
        cpid = fork();
        if(cpid < 0)
        {
            printf("fork error\n");
            exit(1);
        }
        // child process
        else if(cpid == 0)
        {
            int j;
            while(1)
            {
                printf("ppid:%d pid:%d 1650275 sub\n",getppid(),getpid());
                sleep(15);
            }
            //��ӡ���κ��˳�
            exit(0);
        }
        sleep(3);
    }
    
    while(1)
    {
        printf("ppid:%d pid:%d 1650275 main\n",getppid(),getpid());
        sleep(5);
    }

    return 0;
}

static void child(int signo)
{
    pid_t pid;

    while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
        ;
}