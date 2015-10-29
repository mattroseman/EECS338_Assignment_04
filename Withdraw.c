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
#define NUM_SEM 2
#define MUTEX 0
#define FIRST 1
#define SECOND 2
#define THIRD 3

// Pushes the new variables to shared memory
void UpdateSHM();
// Pulls the variables from shared memory
void GetSHM();

unsigned int sleepScale;

char * signature;
char * cssignature;

int semid;
int shmid;
char * memaddr;

unsigned int withdraw;

unsigned int wcount;
unsigned int balance;
unsigned int nextWithdraw;
unsigned int waitingForThird;

unsigned int oldbalance;

// Input: An unsigned int amount that will be taken from balance
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
    
    // Read in arguments
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

SemaphoreAlgorithm:

    Wait(semid, MUTEX);

    printf("\n%sEntering Critical Section\n", cssignature);
    sleep(2 * sleepScale);

    GetSHM();

    // If there are no waiting semaphores and there is enough to withdraw
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

        printf("%sExiting Critical Section\n\n", cssignature);
        sleep(2 * sleepScale);

        UpdateSHM();

        Signal(semid, MUTEX);
    }
    // Either there are waiting semaphores or there is not enough to withdraw
    else
    {
        wcount = wcount + 1;

        printf("%sNot enough in balance (%u) to withdraw (%u)\n", cssignature, balance, withdraw);
        sleep(2 * sleepScale);

        printf("\n%sExiting Critical Section\n", cssignature);
        sleep(2 * sleepScale);

        Signal(semid, MUTEX);

        // first acts as a second mutex semaphore
        Wait(semid, FIRST);

        nextWithdraw = withdraw;

        printf("%sThe next waiting withdraw process needs amount %u\n", signature, nextWithdraw);

        if(waitingForThird)
        {
            Signal(semid, THIRD);
        }

        UpdateSHM();
        sleep(2 * sleepScale);

        Wait(semid, SECOND);

        printf("\n%sEntering Critical Section\n", cssignature);
        sleep(2 * sleepScale);

        GetSHM();

        oldbalance = balance;

        balance = balance - withdraw;
        printf("%sWithdrawing %u\n", cssignature, withdraw);
        sleep(2 * sleepScale);

        printf("%s%u - %u = %u\n", cssignature, oldbalance, withdraw, balance);
        sleep(2 * sleepScale);

        printf("%sNew Balance = %u\n", cssignature, balance);
        sleep(2 * sleepScale);

        wcount = wcount - 1;

        // If there is another process waiting and there is enough for it to withdraw
        if (wcount > 1)
        {
            waitingForThird = 1;
            Signal(semid, FIRST);
            Wait(semid, THIRD);
            waitingForThird = 0;

            if (nextWithdraw < balance)
            {
                printf("%sExiting Critical Section\n\n", cssignature);
                sleep(2 * sleepScale);

                UpdateSHM();

                Signal(semid, SECOND);
            }
        }
        // If there is no other process waiting or there isn't enough for it to withdraw
        else
        {
            printf("%sExiting Critical Section\n\n", cssignature);
            sleep(2 * sleepScale);

            UpdateSHM();

            Signal(semid, FIRST);

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

void UpdateSHM()
{
    *(unsigned int *)memaddr = wcount;
    *(unsigned int *)(memaddr + sizeof(unsigned int)) = balance;
    *(unsigned int *)(memaddr + 2 * sizeof(unsigned int)) = nextWithdraw;
    *(unsigned int *)(memaddr + 3 * sizeof(unsigned int)) = waitingForThird;
}

void GetSHM()
{
    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    nextWithdraw = *(unsigned int *)(memaddr + 2 * sizeof(unsigned int));
    waitingForThird = *(unsigned int *)(memaddr + 3 * sizeof(unsigned int));
}
