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

/* binary semaphore mutex = 1 
 * nonbinary semaphore wlist = 0
 * int wcount = 0
 * int balance = 500
 * linked-list LIST = NULL
 */

void CatchError(int, char *);
void StartWithdraw(unsigned int amount);
void StartDeposit(unsigned int amount);

int pid;
// What every message by this process starts with
char * signature;

// The identifier for the semaphore group that will be made
int semid;
// The identifier for the shared memory segment that will be made
int shmid;

// The address in memory of the shared memory 
void * memaddr;

// initial starting values of the semaphores
unsigned short initValues[] = {0,0};

char * pargs[2];

// Assignment variables
unsigned int wcount;
unsigned int balance;
LinkedList list;

int main() 
{ 
    signature = malloc(100);
    if (sprintf(signature, "%s%d%s", "--- PID: ", getpid(), ": ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }
    printf("%sStarting Main process\n", signature);

    // IPC_CREAT signals to make new group if key doesn't already exist
    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);
    printf("%sNew Semaphore Group %d has been created\n", signature, semid);
    // mutex starts as 1
    Signal(semid, MUTEX);

    // creates a new shared memory segment
    // The memory follows format wcount, then balance, then list pointer
    shmid = CreateSegment(SEMAPHORE_KEY, SHM_SIZE);
    printf("%sNew Shared Memory Segment %d has been created\n", signature, shmid);

    // Attach the memory segment to this process and get the address
    memaddr = AttachSegment(shmid);
    printf("%sThe memory segment %d has been attached to this process at address %p\n", signature, shmid, memaddr);

    // Initialize and put the data into shared memory
    wcount = 0;
    printf("%swcount = %u\n", signature, wcount);
    balance = 500;
    printf("%sbalance = %u\n", signature, balance);

    *(unsigned int *)memaddr = wcount;
    *(unsigned int *)(memaddr + sizeof(unsigned int)) = balance;
    // the third data element is a pointer to a linked list pointer
    *(LinkedList **)(memaddr + 2*sizeof(unsigned int)) = &list;


Cleanup:

    DetachSegment(memaddr);

    DestroySegment(shmid);

    DestroyGroup(semid);

    exit(EXIT_SUCCESS);
}

void CatchError(int x, char * errorMsg)
{
    if (x < 0)
    {
        perror(errorMsg);
        exit(EXIT_FAILURE);
    }
}

void StartWithdraw(unsigned int withdrawAmount)
{
    unsigned int amount = withdrawAmount;
    pargs[0] = "withdraw";
    sscanf(pargs[1], "%u", &amount);
    
    CatchError((pid = fork()), "withdraw fork failed\n");
    // If the process is the child process
    if (pid == 0)
    {
        // the child process is now running the withdraw program 
        CatchError(execvp("./withdraw", pargs), "withdraw execvp failed\n");
    }
    // else continue like normal
}

void StartDeposit(unsigned int depositAmount)
{
    unsigned int amount = depositAmount;
    pargs[0] = "deposit";
    sscanf(pargs[1], "%u", &amount);

    CatchError((pid = fork()), "deposit fork failed\n");

    if (pid == 0)
    {
        CatchError(execvp("./deposit", pargs), "deposit execvp failed\n");
    }
}
