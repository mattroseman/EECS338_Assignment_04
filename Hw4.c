#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "Semaphore.c"

#define SEMAPHORE_KEY 64043

/* binary semaphore mutex = 1 
 * nonbinary semaphore wlist = 0
 * int wcount = 0
 * int balance = 500
 * linked-list LIST = NULL
 */

void CatchError(int, char *);

// The identifier for the semaphore group that will be made
int semid;

// initial starting values of the semaphores
unsigned short counters[] = {0,0,0,0};

int main() {
    pid_t pid = getpid();
    printf("Process ID: %d\n", pid);

    int val;

    // IPC_CREAT signals to make new group if key doesn't already exist
    // CatchError((semid = semget(SEMAPHORE_KEY, 4, IPC_CREAT | ALL_READ_WRITE)), "semget failed\n");
    semid = CreateGroup(SEMAPHORE_KEY, 4, counters);
    printf("Semaphore group with id: %d was just created\n", semid);


    // gets the value of the 0th semaphore in the semid group
    val = GetVal(semid, 0);
    printf("The first semaphore has value %d\n", val);

    val = GetVal(semid, 1);
    printf("The second semaphore has value %d\n", val);

    val = GetVal(semid, 2);
    printf("The third semaphore has value %d\n", val);

    val = GetVal(semid, 3);
    printf("The fourth semaphore has value %d\n", val);


    // Signals the second semaphore (its value should change from 0 to 1)
    Signal(semid, 1);
    printf("The second semaphore has been signaled and has value %d\n", GetVal(semid, 1));


    DestroyGroup(semid);
    printf("The semaphore group %d has been removed\n", semid);

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
