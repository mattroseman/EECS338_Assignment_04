#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "Semaphore.c"
#include "SharedMemory.c"

// the ID of a semaphore group
int semid;
// the ID of the shared memory segment
int shmid;

void main (int argc, char *argv[])
{
    printf("The Withdraw Program with process id %d has started\n", getpid());

    if (sscanf(argv[1], "%d", &semid) == EOF)
    {
        perror("sscanf failed");
        exit(EXIT_FAILURE);
    }

    // signals the first semphore in the semaphore group
    Signal(semid, 0);
    printf("The first semaphore in semaphore group %d has been signaled\n", semid);

    exit(EXIT_SUCCESS);
}
