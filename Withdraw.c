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
char * memaddr;

unsigned int withdraw;

unsigned int wcount;
unsigned int balance;
LinkedList list;

// takes one argument, the amount to be withdrawn
void main (int argc, char * argv[])
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

    if (sscanf(argv[1], "%u", &withdraw) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    printf("%sStarting Withdraw Process with amount %u\n", signature, withdraw);

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (char *)AttachSegment(shmid);

    printf("%sAttached to shared memory segment %d at address %p\n", signature, shmid, memaddr);

    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    list = *(LinkedList *)(memaddr + 2*sizeof(unsigned int));

    Wait(semid, MUTEX);
    printf("%sEntering Critical Section\n", cssignature);
    // if there are no other withdraw processes waiting and there is enough to withdraw
    if (wcount == 0 && balance > withdraw)
    {
        balance = balance - withdraw;
        printf("%sWithdrawing %u\n", signature, withdraw);
        printf("%sNew Balance = %u\n", signature, balance);

        printf("%sExiting Critical Section\n", cssignature);
        Signal(semid, MUTEX);
    }
    // either there are other withdrawl requests waiting or there isn't enough balance
    else
    {
        // add a withdraw process to the wait list
        wcount = wcount + 1;
        AddEndOfList(&list, withdraw);

        // this process is signaling so more withdraw/deposit processes can start up
        Signal(semid, MUTEX);
        // wait for a deposit process to run and deposit enough for the first process waiting
        printf("%sNot enough in balance (%u) to withdraw (%u)\n", signature, balance, withdraw);
        printf("%sExiting Critical Section\n", cssignature);

        Wait(semid, WLIST);
        printf("%sEntering Critical Section\n", cssignature);
        balance = balance - FirstRequestAmount(&list);
        printf("%sWithdrawing %u\n", signature, FirstRequestAmount(&list));
        printf("%sNew Balance = %u\n", signature, balance);
        DeleteFirstRequest(&list);
        // this withdraw is done waiting
        wcount = wcount - 1;

        // if there are still withdraw processes waiting and there is enough to withdraw
        if (wcount > 1 && FirstRequestAmount(&list) < balance)
        {
            // signal for the waiting withdraw process to go
            printf("%sExiting Critical Section\n", cssignature);
            Signal(semid, WLIST);
        }
        // either there aren't any withdraw processes waiting or there isn't enough to withdraw
        else
        {
            // let another deposit or withdraw process run (although a withdraw process is just going to go straight to the queue
            printf("%sExiting Critical Section\n", cssignature);
            Signal(semid, MUTEX);
        }
    }

Cleanup:

    printf("%sWithdraw is complete\n", signature);

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}
