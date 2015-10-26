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

#define SEMAPHORE_KEY 64043
// the maximum size of the string of the semid in bytes
#define MAX_ID_SIZE 100
// The number of semaphores used
#define NUM_SEM 2
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

int main() 
{
    // IPC_CREAT signals to make new group if key doesn't already exist
    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);

    // creates a new shared memory segment the size of the of one unsigned int
    shmid = CreateSegment(SEMAPHORE_KEY, sizeof(unsigned int));

    // Attach the memory segment to this process and get the address
    memaddr = AttachSegment(shmid);


    if (pid == 0)
    {
        // the child process is now running the withdraw program 
        CatchError(execvp("./withdraw", pargs), "execvp failed\n");
    }

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
    
    CatchError((pid = fork()), "fork failed\n");
    // If the process is the child process
    if (pid == 0)
    {
        // the child process is now running the withdraw program 
        CatchError(execvp("./withdraw", pargs), "execvp failed\n");
    }
    // else continue like normal
}

void StartDeposit(unsigned int amount)
{

}
