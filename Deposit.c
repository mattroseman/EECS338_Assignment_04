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

// a string that precedes every output string
char * signature;
// a string to precede CS messages
char * cssignature;

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
    signature = malloc(100);
    if (sprintf(signature, "%s%d%s", "--- PID: ", getpid(), ": ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }

    cssignature = malloc(100);
    if (sprintf(cssignature, "%s%d%s", "*** PID: ", getpid(), ": ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }

    if (sscanf(argv[1], "%u", &deposit) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    printf("%sStarting Deposit Process with amount %u\n", signature, deposit);

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (void *)AttachSegment(shmid);

    printf("%sAttached to shared memory segment %d at address %p\n", signature, shmid, memaddr);

    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    list = *(LinkedList *)(memaddr + 2*sizeof(unsigned int));


    Wait(semid, MUTEX);
    printf("%sEntering Critical Section", cssignature);

    balance = balance + deposit;
    printf("%sDepositing %u\n", signature, deposit);
    printf("%sNew Balance = %u\n", signature, balance);

    // if there aren't any withdraw processes waiting
    if (wcount == 0)
    {
        // signal the next withdraw or deposit process
        printf("%sExiting Critical Section\n", cssignature);
        Signal(semid, MUTEX);
    }
    else 
    {
        // if there are some waiting and there is not enough to withdraw now
        if (FirstRequestAmount(&list) > balance)
        {
            // keep them waiting for a bigger deposit
            printf("%sExiting Critical Section\n", cssignature);
            Signal(semid, MUTEX);
        }
        // if there are some waiting and there is enough to withdraw
        else
        {
            // signal the waiting withdraw process to proceed and withdraw
            printf("%sExiting Critical Section\n", cssignature);
            Signal(semid, WLIST);
        }
    }

Cleanup:

    printf("%sDeposit is complete\n", signature);

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}
