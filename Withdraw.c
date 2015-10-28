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

#define SEMAPHORE_KEY 64043
// The number of semaphores used
#define NUM_SEM 2
// The size of shared memory
// size of two insigned integers and a pointer to a linked list
#define SHM_SIZE (2*sizeof(unsigned int) + sizeof(LinkedList *))
// Binary Semaphore
#define MUTEX 0
// Nonbinary Semaphores
#define FIRST 1
#define SECOND 2


void UpdateSHM();
void GetSHM();

unsigned int sleepScale;

// a string that precedes every output string
char * signature;
// a string to precede CS messages
char * cssignature;

// the ID of a semaphore group
int semid;
// the ID of the shared memory segment
int shmid;
char * memaddr;

unsigned int withdraw;

unsigned int wcount;
unsigned int balance;
unsigned int nextWithdraw;

unsigned int oldbalance;

// takes one argument, the amount to be withdrawn
void main (int argc, char * argv[])
{
    sleepScale = 1;

    signature = malloc(100);
    if (sprintf(signature, "%s%d%s", "--- PID: ", getpid(), " (withdraw): ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }

    cssignature = malloc(100);
    if (sprintf(cssignature, "%s%d%s", "*** PID: ", getpid(), " (withdraw): ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }
    
    if (sscanf(argv[1], "%u", &withdraw) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    printf("%sWithdraw Process with amount %u has started\n", signature, withdraw);
    sleep(2 * sleepScale);

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (char *)AttachSegment(shmid);

    GetSHM();

    Wait(semid, MUTEX);
    printf("\n%sEntering Critical Section\n", cssignature);
    GetSHM();

    sleep(4 * sleepScale);

    // if there are no other withdraw processes waiting and there is enough to withdraw
    if (wcount == 0 && balance > withdraw)
    {
        oldbalance = balance;

        balance = balance - withdraw;
        printf("%sWithdrawing %u\n", cssignature, withdraw);
        sleep(2 * sleepScale);
        printf("%s%u - %u = %u\n", cssignature, oldbalance, withdraw, balance);
        sleep(2 * sleepScale);
        printf("%sNew Balance = %u\n", cssignature, balance);
        sleep(2 * sleepScale);

        sleep(4 * sleepScale);

        printf("%sExiting Critical Section\n\n", cssignature);
        UpdateSHM();

        Signal(semid, MUTEX);
    }
    // either there are other withdrawl requests waiting or there isn't enough balance
    else
    {
        // add a withdraw process to the wait list
        wcount = wcount + 1;

        printf("\n%sExiting Critical Section\n", cssignature);
        // even though nextWithdraw is shared memory the FIRST semaphore protects it so only one withdraw process can access it
        Signal(semid, MUTEX);
        // if this is the first withdraw processs waiting it should be able to go through this
        // if the process isn't the first it should queue up here
        Wait(semid, FIRST);
        // the value of the next process waiting to go
        nextWithdraw = withdraw;

        // wait for a deposit process to run and deposit enough for the first process waiting
        printf("%sNot enough in balance (%u) to withdraw (%u)\n", cssignature, balance, withdraw);

        sleep(4 * sleepScale);

        UpdateSHM();
        sleep(2 * sleepScale);

        // this is signaled by deposit
        Wait(semid, SECOND);
        // signals the second process to move up as first process
        Signal(semid, FIRST);

        printf("\n%sEntering Critical Section\n", cssignature);
        GetSHM();

        sleep(4 * sleepScale);

        oldbalance = balance;

        balance = balance - withdraw;
        printf("%sWithdrawing %u\n", cssignature, withdraw);
        sleep(2 * sleepScale);
        printf("%s%u - %u = %u\n", cssignature, oldbalance, withdraw, balance);
        sleep(2 * sleepScale);
        printf("%sNew Balance = %u\n", cssignature, balance);
        sleep(2 * sleepScale);
        //DeleteFirstRequest(list);
        // this withdraw is done waiting
        wcount = wcount - 1;

        sleep(4 * sleepScale);

        // if there are still withdraw processes waiting and there is enough to withdraw
        if (wcount > 1 && nextWithdraw < balance)
        {
            // signal for the waiting withdraw process to go
            printf("%sExiting Critical Section\n\n", cssignature);
            UpdateSHM();
            sleep(2 * sleepScale);
            Signal(semid, SECOND);
        }
        // either there aren't any withdraw processes waiting or there isn't enough to withdraw
        else
        {
            // let another deposit or withdraw process run (although a withdraw process is just going to go straight to the queue)
            printf("%sExiting Critical Section\n\n", cssignature);
            UpdateSHM();
            sleep(2 * sleepScale);
            Signal(semid, MUTEX);
        }
    }

Cleanup:

    printf("%sWithdraw is complete\n", signature);
    sleep(2 * sleepScale);

    free(signature);
    free(cssignature);

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}

// uploads to the shared memory the new values
void UpdateSHM()
{
    // The value at the end of the unsigned integer pointer is wcount
    *(unsigned int *)memaddr = wcount;
    // The value at the end of the second unsigned integer pointer is balance
    *(unsigned int *)(memaddr + sizeof(unsigned int)) = balance;
    *(unsigned int *)(memaddr + 2 * sizeof(unsigned int)) = nextWithdraw;
}

// gets the new values from shared memory and assigns them to the appropriate variables
void GetSHM()
{
    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    nextWithdraw = *(unsigned int *)(memaddr + 2 * sizeof(unsigned int));
}
