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
    if (sscanf(argv[1], "%u", &withdraw) < 0)
    {
        perror("sscanf failed\n");
        exit(EXIT_FAILURE);
    }

    semid = GetGroup(SEMAPHORE_KEY);

    shmid = GetSegment(SEMAPHORE_KEY);

    memaddr = (char *)AttachSegment(shmid);

    wcount = *(unsigned int *)memaddr;
    balance = *(unsigned int *)(memaddr + sizeof(unsigned int));
    list = *(LinkedList *)(memaddr + 2*sizeof(unsigned int));

    Wait(semid, MUTEX);
    // if there are no other withdraw processes waiting and there is enough to withdraw
    if (wcount == 0 && balance > withdraw)
    {
        balance = balance - withdraw;
        Signal(semid, MUTEX);
    }
    // either there are other withdrawl requests waiting or there isn't enough balance
    else
    {
        // add a withdraw process to the wait list
        wcount = wcount + 1;
        AddEndOfList(&list, withdraw);
        // this process is waiting so more withdraw processes can start up
        Signal(semid, MUTEX);
        // wait for a deposit process to run and deposit enough for the first process waiting
        Wait(semid, WLIST);
        balance = balance - FirstRequestAmount(&list);
        DeleteFirstRequest(&list);
        // this withdraw is done waiting
        wcount = wcount - 1;
        // if there are still withdraw processes waiting and there is enough to withdraw
        if (wcount > 1 && FirstRequestAmount(&list) < balance)
        {
            // signal for the waiting withdraw process to go
            Signal(semid, WLIST);
        }
        // either there aren't any withdraw processes waiting or there isn't enough to withdraw
        else
        {
            // let another deposit or withdraw process run (although a withdraw process is just going to go straight to the queue
            Signal(semid, MUTEX);
        }
    }

Cleanup:

    DetachSegment(memaddr);

    exit(EXIT_SUCCESS);
}
