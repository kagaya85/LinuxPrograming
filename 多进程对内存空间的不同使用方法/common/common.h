#include <iostream>
#include <sys/type.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/sem.h>

#define SIG_READY (SIGRTMIN + 1)

pid_t getPidByName(const char * const);
