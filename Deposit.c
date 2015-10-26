#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "Semaphore.h"
#include "SharedMemory.h"
#include "LinkedList.h"

#define SEMAPHORE_KEY 64043
// The number of semaphores used
#define NUM_SEM 2
// The size of shared memory
// size of two insigned integers and a pointer to a linked list
#define SHM_SIZE (2*sizeof(unsigned int) + sizeof(LinkedList *))
// Binary Semaphore
#define MUTEX 0
// Nonbinary Semaphores
#define WLIST 1


// the ID of a semaphore group
int semid;
// the ID of the shared memory segment
int shmid;
void * memaddr;

// passed in as an argument
unsigned int deposit;

// found from storage
unsigned int wcount;
unsigned int balance;
LinkedList list;

void main(int argc, char * argv[])
{
    if (sscanf(argv[1], "%u", &deposit) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (void *)AttachSegment(shmid);

    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    list = *(LinkedList *)(memaddr + 2*sizeof(unsigned int));

    Wait(semid, MUTEX);
    balance = balance + deposit;
    if (wcount == 0)
    {
        Signal(semid, MUTEX);
    }
    else if (FirstRequestAmount(&list) > balance)
    {
        Signal(semid, MUTEX);
    }
    else
    {
        Signal(semid, WLIST);
    }

Cleanup:

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}
