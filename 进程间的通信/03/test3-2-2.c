#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#define MYSIGREAD __SIGRTMIN+10
#define BUF_SIZE 1024

void handleSignal(int signo)
{
    switch(signo){
        case MYSIGREAD:
            printf("Get a signal -- MYSIGREAD\n");
            break;
        default:
            printf("ERROR\n");
            exit(0);
    }
}

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
        while ((ptr = readdir(dir)) != NULL) //循环读取/proc下的每一个文件/文件夹
        {
            //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;
        
            sprintf(filepath, "/proc/%s/status", ptr->d_name);//生成要读取的文件的路径
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

                //如果文件内容满足要求则打印路径的名字（即进程的PID）
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
    char buf[100];
    FILE *fp;

    signal(MYSIGREAD, handleSignal);
    
    printf("This is %d\n", getpid());
    while(pid <= 0) {
        pid = getPidByName("test3-2-1");
        if(pid == -1) {
            printf("No such process\n");
            sleep(1);
        }
    }
    printf("Get target pid %d\n", pid);
    
    while(1) {
        printf("waiting...\n");
        pause();
        
        fp = fopen("test.txt", "r");
        if(fp == NULL) {
            fprintf(stderr, "Open read file error\n");
            exit(0);
        }
        printf("Start reading\n");
        while(!feof(fp)) {
            fgets(buf, sizeof(buf)-1, fp);
            printf("%s", buf);
        } 
        fclose(fp);
        printf("Stop reading\n");

        fp = fopen("test.txt", "w");

        if(fp == NULL) {
            fprintf(stderr, "Open write file error\n");
            exit(0);
        }
        printf("Start writing\n");
        fprintf(fp, "Hello I am from %d\n", getpid());
        sleep(1);
        fprintf(fp, "Hello I am from %d\n", getpid());
        printf("Stop writing\n");
        fclose(fp);
        kill(pid, MYSIGREAD);
        printf("Send a signal -- MYSIGREAD\n");
    }

    return 0;
}
