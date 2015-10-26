#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "Semaphore.c"
#include "SharedMemory.c"

// the ID of a semaphore group
int semid;
// the ID of the shared memory segment
int shmid;
char * memaddr;

char * message;

void main (int argc, char *argv[])
{
    printf("The Withdraw Program with process id %d has started\n", getpid());

    if (sscanf(argv[1], "%d", &semid) == EOF)
    {
        perror("sscanf for first argument failed\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &shmid) == EOF)
    {
        perror("sscanf for second argument failed\n");
        exit(EXIT_FAILURE);
    }

    memaddr = (char *)AttachSegment(shmid);
    printf("Process %d has been attached to the shared memory\n", getpid());

    // find the size of the segment and allocate space for message
    message = malloc(GetSegmentSize(shmid));

    // signals the first semphore in the semaphore group
    Signal(semid, 0);
    printf("The first semaphore in semaphore group %d has been signaled\n", semid);
    // wait for Hw4 to put the message into shared memory
    Wait(semid, 2);
    printf("Withdraw has started up again\n");

    strcpy(message, memaddr);
    printf("The message should have been copied\n");

    printf("The message is %s\n", message);
    
    DetachSegment(memaddr);
    printf("Process %d has been detached from the shared memory\n", getpid());

    Signal(semid, 3);

    free(message);
    exit(EXIT_SUCCESS);
}
