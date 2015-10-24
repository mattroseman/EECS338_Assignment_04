#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "Semaphore.c"
#include "SharedMemory.c"

void main ()
{
    printf("The Withdraw Program with process id %d has started\n", getpid());
}
