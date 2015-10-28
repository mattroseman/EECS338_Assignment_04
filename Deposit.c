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
#include "Array.h"

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


void UpdateSHM();
void GetSHM();


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
unsigned int *list;
unsigned int size;

unsigned int oldbalance;

void main(int argc, char * argv[])
{
    signature = malloc(100);
    if (sprintf(signature, "%s%d%s", "--- PID: ", getpid(), " (deposit): ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }

    cssignature = malloc(100);
    if (sprintf(cssignature, "%s%d%s", "*** (deposit) PID: ", getpid(), " (deposit): ") < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }

    if (sscanf(argv[1], "%u", &deposit) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    printf("%sDeposit Process with amount %u has started\n", signature, deposit);
    sleep(2);

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (void *)AttachSegment(shmid);

    //printf("%sAttached to shared memory segment %d at address %p\n", signature, shmid, memaddr);
    //sleep(2);

    GetSHM();

    Wait(semid, MUTEX);
    printf("\n%sEntering Critical Section\n", cssignature);
    GetSHM();

    sleep(4);

    oldbalance = balance;

    balance = balance + deposit;
    printf("%sDepositing %u\n", cssignature, deposit);
    sleep(2);
    printf("%s%u + %u = %u", cssignature, oldbalance, deposit, balance);
    sleep(2);
    printf("%sNew Balance = %u\n", cssignature, balance);
    sleep(2);


    sleep(4);

    // if there aren't any withdraw processes waiting
    if (wcount == 0)
    {
        // signal the next withdraw or deposit process
        printf("%sExiting Critical Section\n\n", cssignature);
        UpdateSHM();
        sleep(2);
        Signal(semid, MUTEX);
    }
    else 
    {
        // if there are some waiting and there is not enough to withdraw now
        if (FirstRequestAmount(list, &size) > balance)
        {
            // keep them waiting for a bigger deposit
            printf("%sExiting Critical Section\n\n", cssignature);
            UpdateSHM();
            sleep(2);
            Signal(semid, MUTEX);
        }
        // if there are some waiting and there is enough to withdraw
        else
        {
            // signal the waiting withdraw process to proceed and withdraw
            printf("%sExiting Critical Section\n\n", cssignature);
            UpdateSHM();
            sleep(2);
            Signal(semid, WLIST);
        }
    }

Cleanup:

    printf("%sDeposit is complete\n", signature);
    sleep(2);

    free(signature);
    free(cssignature);

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}

void UpdateSHM()
{
    *(unsigned int *)memaddr = wcount;
    *(unsigned int *)(memaddr + sizeof(unsigned int)) = balance;
    *(unsigned int **)(memaddr + 2*sizeof(unsigned int)) = list;
    *(unsigned int *)(memaddr + 2*sizeof(unsigned int) + sizeof(unsigned int *)) = size;
}

void GetSHM()
{
    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    list = *(unsigned int **)(memaddr + 2*sizeof(unsigned int));
    size = *(unsigned int *)(memaddr + 2*sizeof(unsigned int) + sizeof(unsigned int *));
}
