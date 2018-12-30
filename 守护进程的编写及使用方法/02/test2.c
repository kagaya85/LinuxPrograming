#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>

void create_daemon();

int main()
{
    daemon(0, 1);
    while(1)
    {
        printf("1650275\n");
        sleep(5);
    }

    return 0;
}

void create_daemon()
{
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

    if(setsid() == -1)
    {
        printf("setsid error\n");
        exit(1);
    }
    
    return;
}