/*
 * File: send-level1.cpp
 * Project: 02
 * File Created: Wednesday, 12th December 2018 1:21:53 am
 * Author: kagaya (kagaya85@foxmail.com)
 * -----
 * Last Modified: Wednesday, 12th December 2018 9:04:36 am
 * Modified By: kagaya (kagaya85@foxmail.com>)
 * -----
 * Copyright (c) 2018 Tongji University
 */

#include <iostream>
#include "common.h"
#include <sys/types.h>

using namespace std;

#define MAXSIZE 128
#define WRITTEN 1
#define WRITABLE 0
#define IPC_ID 1551
// static int semid;

struct ShareMem {
    int flag;
    char text[MAXSIZE];
};

int shm_size;

static void sig_ready_handle(int signal, siginfo_t* siginfo, void* context)
{
    if (signal == SIG_READY) {
        shm_size = siginfo->si_int;
    }
    else {
        shm_size = 0;
    }
}

int main()
{
    pid_t prior_pid, next_pid;
    shm_size = 0;
    struct sigaction act;
    act.sa_sigaction = sig_ready_handle;    // 使用带附加参数的信号处理函数
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIG_READY, &act, NULL);
    // signal(SIG_READY, sig_ready_handle);

    while(true) {
        prior_pid = getPidByName("send-level2");
        if (prior_pid == 0)
            sleep(1);
        else
            break;
    }

    void *shmp = NULL;
    struct ShareMem *shared;
    int shmid;

    // pause here
    while (shm_size == 0) {
        pause();
    }

    shmid = shmget(IPC_ID, sizeof(struct ShareMem), 0666);
    if(shmid == -1) {
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    // 连接共享内存
    shmp = shmat(shmid, 0, 0);
    if(shmp == (void *)-1) {
        perror("shmat error");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %x\n", (int)shmp);
    shared = (struct ShareMem *) shmp;
    shared->flag = BLANK;

    // 读共享内存，写入文件

    // 分离共享内存
    if(shmdt(shmp) == -1) {
		perror("shmdt error");
		exit(EXIT_FAILURE);
	}

    // 删除共享内存
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl error");
        exit(EXIT_FAILURE);
    }

    /* write to file */
    while(true) {
        next_pid = getPidByName("recv-level1");
        if (next_pid == 0)
            sleep(1);
        else
            break;
    }
    
    kill(next_pid, SIG_READY);

    return 0;
}