#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#define MYSIG1 __SIGRTMIN+10
#define MYSIG2 __SIGRTMIN+11
#define MYSIGKILL __SIGRTMIN+12
#define BUF_SIZE 1024

pid_t getPidByName(char *task_name)
{
    DIR *dir;
    pid_t pid;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];
    char cur_task_name[50];
    char buf[BUF_SIZE];

    dir = opendir("/proc"); 
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL) //ѭ����ȡ/proc�µ�ÿһ���ļ�/�ļ���
        {
            //�����ȡ������"."����".."����������ȡ���Ĳ����ļ�������Ҳ����
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;
        
            sprintf(filepath, "/proc/%s/status", ptr->d_name);//����Ҫ��ȡ���ļ���·��
            fp = fopen(filepath, "r");
            if (NULL != fp)
            {
                if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%*s %s", cur_task_name);
                // printf("%s\n", cur_task_name);
                // printf("%s\n", ptr->d_name);

                //����ļ���������Ҫ�����ӡ·�������֣������̵�PID��
                if (!strcmp(task_name, cur_task_name)){
                    pid = atoi(ptr->d_name);
                    // sscanf(ptr->d_name, "%d", pid);
                    return pid;
                }
                fclose(fp);
            }
        }
        closedir(dir);
    }
    pid = -1;
    return pid;
}

int main()
{
    pid_t pid;

    pid = getPidByName("test3-1-2");
    if(pid == -1) {
        printf("No such process\n");
        exit(0);
    }

    printf("Get target pid %d\n", pid);
    kill(pid, SIGHUP);
    printf("Send a signal -- SIGHUP\n");
    sleep(1);
    kill(pid, SIGINT);
    printf("Send a signal -- SIGINT\n");
    sleep(1);    
    kill(pid, SIGQUIT);
    printf("Send a signal -- SIGQUIT\n");
    sleep(1);
    kill(pid, MYSIG1);
    printf("Send a signal -- MYSIG1\n");    
    sleep(1);
    kill(pid, MYSIG2);
    printf("Send a signal -- MYSIG2\n");    

    return 0;
}
