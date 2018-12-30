#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>

int main()
{
    pid_t pid, ppid, cpid, cppid;
    daemon(0, 1);

    // ��ȡ�����̵�id��ppid
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
            int j;
            for(j = 0; j < 3; j++)
            {
                printf("ppid:%d pid:%d 1650275 sub\n",getppid(),getpid());
                sleep(25);
            }
            //��ӡ���κ��˳�
            exit(0);
        }
        sleep(3);
    }



    while(1)
    {
        printf("ppid:%d pid:%d 1650275 main\n",getppid(),getpid());
        
        // �����ӽ��� ������
        for (i = 0; i < 10; i++)
		    waitpid(pids[i], NULL, WNOHANG);
        sleep(5);
    }

    return 0;
}
