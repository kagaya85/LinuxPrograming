#include <iostream>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/file.h>
#include <sys/stat.h>

#define SIGREADY (SIGRTMIN + 1)

pid_t getPidByName(const char * const);
