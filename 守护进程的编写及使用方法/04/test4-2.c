#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

int main()
{
    pid_t pid, ppid, cpid, cppid;
    daemon(0, 1);

    // 获取父进程的id和ppid
    pid = getpid();
    ppid = getppid();

    pid_t pids[10];
    int i;
    for(i = 0; i < 10; i++)
    {
        pids[i] = fork();
        if(pids[i] < 0)
        {
            printf("fork error\n");
            exit(1);
        }
        // child process
        else if(pids[i] == 0)
        {
            prctl(PR_SET_PDEATHSIG, SIGKILL);
            int j;
            while(1)
            {
                printf("ppid:%d pid:%d 1650275 sub\n",getppid(),getpid());
                sleep(15);
            }
            //打印三次后退出
            exit(0);
        }
        sleep(3);
    }

    while(1)
    {
        printf("ppid:%d pid:%d 1650275 main\n",getppid(),getpid());
        
        // 回收子进程 非阻塞
        for (i = 0; i < 10; i++)
		    waitpid(pids[i], NULL, WNOHANG);
        sleep(5);
    }

    return 0;
}
