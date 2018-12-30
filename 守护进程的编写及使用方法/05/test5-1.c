#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

static void child(int signo);
int sub();

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("只接受一个参数\n");
        exit(1);
    }
    
    int n = atoi(argv[1]);
    if(n == 0)
    {
        printf("参数错误\n");
        exit(1);
    }
    
    pid_t pid, ppid, cpid, cppid;
    daemon(0, 1);
    signal(SIGCHLD, child);
    // 获取父进程的id和ppid
    pid = getpid();
    ppid = getppid();

    int i;
    for(i = 0; i < n; i++)
    {
        cpid = fork();
        if(cpid < 0)
        {
            printf("分裂失败，已分裂%d个子进程\n", i);
            break;
        }
        // child process
        else if(cpid == 0)
        {
            prctl(PR_SET_PDEATHSIG, SIGKILL);
            sub();
        }
        if((i + 1) % 100 == 0)
            printf("已分裂%d个子进程\n", i + 1);
    }
    
    while(1)
    {
        ;
    }

    return 0;
}

static void child(int signo)
{
    pid_t pid;

    while ((pid = waitpid(-1, NULL, WNOHANG)) >0)
        ;
}

int sub()
{
    char str[1024*10] = "hello world!"; 
    for(;;)
        sleep(1);
}