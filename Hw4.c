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
// Binary Semaphore
#define MUTEX 0
// Nonbinary Semaphores
#define WLIST 1

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

char * pargs[3];

// Assignment variables
unsigned int wcount;
unsigned int balance;
LinkedList *list;

/*
 * TODO 
 * Have the main function take in a flag of either preset test or random test
 * Maybe ./sap preset, or ./sap random
 */
int main() 
{ 
    signature = malloc(100);
    if (sprintf(signature, "%s%d%s", "--- PID: ", getpid(), " (main): ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }
    printf("%sStarting Main process\n", signature);
    sleep(2);

    // IPC_CREAT signals to make new group if key doesn't already exist
    semid = CreateGroup(SEMAPHORE_KEY, NUM_SEM, initValues);
    printf("%sNew Semaphore Group %d has been created\n", signature, semid);
    sleep(2);
    // mutex starts as 1
    Signal(semid, MUTEX);

    // Initialize
    wcount = 0;
    printf("%swcount = %u\n", signature, wcount);
    sleep(2);
    balance = 500;
    printf("%sbalance = %u\n", signature, balance);
    sleep(2);
    list = NewLinkedList();
    printf("%sThe linked list has been initialized\n", signature);
    sleep(2);

    // creates a new shared memory segment
    // The memory follows format wcount, then balance, then list pointer
    shmid = CreateSegment(SEMAPHORE_KEY, (2*sizeof(unsigned int) + sizeof(list)));
    printf("%sNew Shared Memory Segment %d has been created\n", signature, shmid);
    sleep(2);

    // Attach the memory segment to this process and get the address
    memaddr = AttachSegment(shmid);
    printf("%sThe memory segment %d has been attached to this process at address %p\n", signature, shmid, memaddr);
    sleep(2);

    // Put data into shared memory
    *(unsigned int *)memaddr = wcount;
    *(unsigned int *)(memaddr + sizeof(unsigned int)) = balance;
    // the third data element is a pointer to a linked list
    *(LinkedList **)(memaddr + 2*sizeof(unsigned int)) = list;

    printf("%sVariables have been put into shared memory\n", signature);

    // An infinite loop that randomly deposits and withdraws at most every 10 seconds and least 1 second
    while(1==1)
    {
        // come up with a random time between 1 and 10 seconds
        unsigned int time = (rand()%10) + 1;
        CatchError(sleep(time), "sleep failed\n");
        // come up with a random dollar amount between 1 and 300
        unsigned int amount = (rand()%300) + 1;
        // randomly decide between deposit or withdraw
        unsigned int x = (rand()%2);
        // make a deposit
        if (x == (unsigned int)0)
        {
            //printf("%sStarting Deposit Process with amount %u\n", signature, amount);
            sleep(2);
            StartDeposit(amount);
        }
        // make a withdraw
        else
        {
            //printf("%sStarting Withdraw Process with amount %u\n", signature, amount);
            sleep(2);
            StartWithdraw(amount);
        }
    }

    /* 
     * TODO
     * Have a preset sequential calls to StartWithdrawProcess and StartDepositProcess
     * be called in a specific order to test different arrangements
     */


Cleanup:

    free(signature);

    DestroyLinkedList(list);

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
    // If the process is the child process
    if (pid == 0)
    {
        // the child process is now running the withdraw program 
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
