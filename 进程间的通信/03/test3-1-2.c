#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MYSIG1 __SIGRTMIN+10
#define MYSIG2 __SIGRTMIN+11
#define MYSIGKILL __SIGRTMIN+12

void handleSignal(int signo)
{
    switch(signo){
        case SIGHUP:
            printf("Get a signal -- SIGHUP\n");
            break;
        case SIGINT:
            printf("Get a signal -- SIGINT\n");
            break;
        case SIGQUIT:
            printf("Get a signal -- SIGHQUIT\n");
            break;
        case MYSIG1:
            printf("Get a signal -- MYSIG1\n");
            break;
        case MYSIG2:
            printf("Get a signal -- MYSIG2\n");
            break;
        case MYSIGKILL:
            printf("Get a signal -- MYSIGKILL\n");
            exit(0);
            break;
        default:
            printf("ERROR\n");
            exit(0);
    }
}

int main()
{
    printf("This is %d\n", getpid());
    signal(SIGHUP, handleSignal);
    signal(SIGINT, handleSignal);
    signal(SIGQUIT, handleSignal);
    signal(MYSIG1, handleSignal);
    signal(MYSIG2, handleSignal);
    signal(MYSIGKILL, handleSignal);
    for(;;)
        ;

    return 0;
}