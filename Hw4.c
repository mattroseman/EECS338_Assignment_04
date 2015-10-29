#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "Semaphore.h"
#include "SharedMemory.h"

#define SEMAPHORE_KEY 64043
// The number of semaphores used
#define NUM_SEM 3
// Binary Semaphore
#define MUTEX 0
// Nonbinary Semaphores
#define FIRST 1
#define SECOND 2
#define THIRD 3

void CatchError(int, char *);
void StartWithdraw(unsigned int amount);
void StartDeposit(unsigned int amount);

unsigned int sleepScale;

int pid;
char * signature;

int semid;
int shmid;

void * memaddr;

// initial starting values of the semaphores
unsigned short initValues[] = {1,1,0,0};

char * pargs[3];

// Assignment variables
unsigned int wcount;
unsigned int balance;
// Amount of the next waiting withdraw process
unsigned int nextWithdraw;
unsigned int waitingForThird;

unsigned int seed;

/*
 * TODO 
 * Have the main function take in a flag of either preset test or random test
 * Maybe ./sap preset, or ./sap random
 */
int main() 
{ 
    sleepScale = 1;

    signature = malloc(100);
    CatchError( sprintf(signature, "%s%d%s", "--- PID: ", getpid(), " (main): "), "sprintf failed\n");

    printf("%sStarting Main process\n", signature);
    sleep(2 * sleepScale);

    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);
    printf("%sNew Semaphore Group %d has been created\n", signature, semid);
    sleep(2 * sleepScale);

Initialize:

    wcount = 0;
    printf("%swcount = %u\n", signature, wcount);
    sleep(2 * sleepScale);

    balance = 500;
    printf("%sbalance = %u\n", signature, balance);
    sleep(2 * sleepScale);

    nextWithdraw = 0;

    waitingForThird = 0;

SharedMemory:

    // creates a new shared memory segment
    // The memory follows format wcount, then balance
    shmid = CreateSegment(SEMAPHORE_KEY, (4*sizeof(unsigned int)));
    printf("%sNew Shared Memory Segment %d has been created\n", signature, shmid);
    sleep(2 * sleepScale);

    // Attach the memory segment to this process and get the address
    memaddr = AttachSegment(shmid);
    printf("%sThe memory segment %d has been attached to this process at address %p\n", signature, shmid, memaddr);
    sleep(2 * sleepScale);

    // Put data into shared memory
    *(unsigned int *)memaddr = wcount;
    *(unsigned int *)(memaddr + sizeof(unsigned int)) = balance;
    *(unsigned int *)(memaddr + 2 * sizeof(unsigned int)) = nextWithdraw;
    *(unsigned int *)(memaddr + 3 * sizeof(unsigned int)) = waitingForThird;

    printf("%sVariables have been put into shared memory\n", signature);
    sleep(2 * sleepScale);

Semaphores:

    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);
    printf("%sNew Semaphore Group %d has been created\n", signature, semid);

MainLoop:

    CatchError((seed = time(NULL)), "time failed\n");
    srand(seed);

    /* 
    TODO
    Have a listener for any key strokes that exits this and kills all running processes
    */
    while(1)
    {
        // Amount to be withdrawn/deposited
        unsigned int amount = (rand()%300) + 1;

        // Which process should be run: deposit = 0/withdraw = 1
        unsigned int x = (rand()%2);

        // Make a Deposit
        if (x == 0)
        {
            sleep(2 * sleepScale);
            StartDeposit(amount);
        }

        // Make a Withdraw
        else // x == 1
        {
            sleep(2 * sleepScale);
            StartWithdraw(amount);
        }

        // Time between process creations
        unsigned int time = (rand()%10) + 5;
        CatchError(sleep(time), "sleep failed\n");
    }

    /* 
     * TODO
     * Have a preset sequential calls to StartWithdrawProcess and StartDepositProcess
     * be called in a specific order to test different arrangements
     */


Cleanup:

    free(signature);

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
    pargs[1] = malloc(sizeof(char *));    
    CatchError(sprintf(pargs[1], "%u", amount), "sscanf failed\n");
    pargs[2] = NULL;
    
    CatchError((pid = fork()), "withdraw fork failed\n");
    if (pid == 0)
    {
        CatchError(execvp("./withdraw", pargs), "withdraw execvp failed\n");
    }
    free(pargs[1]);
}

void StartDeposit(unsigned int depositAmount)
{
    unsigned int amount = depositAmount;
    pargs[0] = "deposit";
    pargs[1] = malloc(sizeof(char *));
    CatchError(sprintf(pargs[1], "%u", amount), "sscanf failed\n");
    pargs[2] = NULL;

    CatchError((pid = fork()), "deposit fork failed\n");

    if (pid == 0)
    {
        CatchError(execvp("./deposit", pargs), "deposit execvp failed\n");
    }
    free(pargs[1]);
}
