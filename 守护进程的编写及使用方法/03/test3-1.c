#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>

int main()
{
    pid_t pid, ppid, cpid, cppid;
    daemon(0, 1);

    // 获取父进程的id和ppid
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
            for(j = 0; j < 3; j++)
            {
                printf("ppid:%d pid:%d 1650275 sub\n",getppid(),getpid());
                sleep(25);
            }
            //打印三次后退出
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
