#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

#define MAXSIZE 128
#define WRITTEN 1
#define BLANK 0

// static int semid;

struct ShareMem {
    int flag;
    char text[MAXSIZE];
};

// static int set_sem(void) {
//     union semun sem_union;

//     sem_union.val = 1;
//     if(semctl(semid, 0, SETVAL, sem_union) == -1) 
//         return 0;
//     return 1;
// }

// static void del_sem(void)
// {
//     union semun sem_union;

//     if(semctl(semid, 0, IPC_RMID, sem_union) == -1)
//         fprintf(stderr, "Failed to delete semaphore/n");
// }

// static int sem_p(void)
// {
//     struct sembuf sem_b;

//     sem_b.sem_num = 0;
//     sem_b.sem_op = -1;
//     sem_b.sem_flag = SEM_UNDO;
//     if(semop(semid, &sem_b, 1) == -1)
//     {
//         fprintf(stderr, "semaphore_p failed/n");
//         return 0;
//     }
//     return 1;
// }

// static int sem_v(void)
// {
//     struct sembuf sem_b;

//     sem_b.sem_num = 0;
//     sem_b.sem_op = 1;
//     sem_b.sem_flag = SEM_UNDO;
//     if(semop(semid, &sem_b, 1) == -1)
//     {
//         fprintf(stderr, "semaphore_v failed/n");
//         return 0;
//     }
//     return 1;
// }

int main()
{
    void *shmp = NULL;
    struct ShareMem *shared;
    int shmid;
    char str[] = "HI, this is test5-2!";

    shmid = shmget(114, sizeof(struct ShareMem), 0666 | IPC_CREAT);
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

    int i;
    for(i = 0; i < 5; i++) {
        while(shared->flag != WRITTEN) {
            sleep(1);
            printf("waiting...\n");
        }
        printf("Shared Mem is: %s\n", shared->text);
        shared->flag == BLANK;
        // sem_p();
        strncpy(shared->text, str, MAXSIZE);
        shared->flag = WRITTEN;
        sleep(3);
        // sem_v();
    }

    if(shmdt(shmp) == -1) {
		perror("shmdt error");
		exit(EXIT_FAILURE);
	}

    return 0;
}