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
char * memaddr;

unsigned int withdraw;

unsigned int wcount;
unsigned int balance;
LinkedList list;

// takes one argument, the amount to be withdrawn
void main (int argc, char * argv[])
{
    if (sscanf(argv[1], "%u", &withdraw) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (char *)AttachSegment(shmid);

    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    list = *(LinkedList *)(memaddr + 2*sizeof(unsigned int));

    Wait(semid, MUTEX);
    if (wcount == 0 && balance > withdraw)
    {
        balance = balance - withdraw;
        Signal(semid, MUTEX);
    }
    else
    {
        wcount = wcount + 1;
        AddEndOfList(&list, withdraw);
        Signal(semid, MUTEX);
        Wait(semid, WLIST);
        balance = balance - FirstRequestAmount(&list);
        DeleteFirstRequest(&list);
        wcount = wcount - 1;
        Signal(semid, MUTEX);
    }

Cleanup:

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}
