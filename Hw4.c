#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
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

// The number of processes that will be started
unsigned int numProcesses;

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
unsigned int processRunning;

unsigned int seed;


int main(int argc, char * argv[]) 
{ 
    if(argc > 1) 
    {
        CatchError(sscanf(argv[1], "%u", &numProcesses), "sscanf failed\n");
    }
    else
    {
        numProcesses = 10;
    }

    sleepScale = 1;

    signature = malloc(100);
    CatchError( sprintf(signature, "%s%d%s", "--- PID: ", getpid(), " (main): "), "sprintf failed\n");

    printf("%sStarting Main process\n", signature);
    sleep(2 * sleepScale);

    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);
    printf("%sNew Semaphore Group %d has been created\n", signature, semid);
    sleep(2 * sleepScale);

/* Initialize */

    wcount = 0;
    printf("%swcount = %u\n", signature, wcount);
    sleep(2 * sleepScale);

    balance = 500;
    printf("%sbalance = %u\n", signature, balance);
    sleep(2 * sleepScale);

    nextWithdraw = 0;

    waitingForThird = 0;

    processRunning = 0;

/* SharedMemory */

    // creates a new shared memory segment
    // The memory follows format wcount, then balance
    shmid = CreateSegment(SEMAPHORE_KEY, (5*sizeof(unsigned int)));
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
    *(unsigned int *)(memaddr + 4 * sizeof(unsigned int)) = processRunning;

    printf("%sVariables have been put into shared memory\n", signature);
    sleep(2 * sleepScale);

/* Semaphores */

    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);
    printf("%sNew Semaphore Group %d has been created\n", signature, semid);

/* MainLoop */

    CatchError((seed = time(NULL)), "time failed\n");
    srand(seed);

    int i;
    for (i = 0; i < numProcesses; i++)
    {
        // Amount to be withdrawn/deposited
        unsigned int amount = (rand()%300) + 1;

        // Favours Withdraw for more interesting output
        // Which process should be run: deposit = 0/withdraw = 1, 2
        unsigned int x = (rand()%3);

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

    siginfo_t infop;
    // the max possible processes still running
    for (i=0; i<numProcesses; i++) 
    {
        CatchError(waitid(P_ALL, 0, &infop,  WEXITED | WNOHANG), "wait failed\n");
    }

/* Cleanup */

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
