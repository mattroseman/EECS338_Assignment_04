#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "Semaphore.h"

// Makes a semaphore group from key key of n semaphores and returns the new id
// Sets all semaphores to 0
// the size of the array and n should be the same
static int CreateGroup(key_t key, int n, unsigned short * initVal)
{
    int returnVal;
    if((returnVal = semget(key, n, IPC_CREAT | 0666)) < 0)
    {
        perror("Semget with IPC_CREAT failed\n");
        exit(EXIT_FAILURE);
    }

    union semun SemUnion;
    SemUnion.array = initVal;
    if (semctl(returnVal, 0, SETALL, SemUnion) < 0)
    {
        perror("Semctl with SETALL failed\n");
        exit(EXIT_FAILURE);
    }

    return returnVal;
}

// Destroys the group with id semid
static void DestroyGroup(int semid)
{
    if(semctl(semid, 0, IPC_RMID) < 0)
    {
        perror("semctl IPC_RMID failed");
        exit(EXIT_FAILURE);
    }
}

static void Signal(int semid, int semnum)
{
    // initialize the semaphore operation structures
    struct sembuf signal = {semnum, 1, 0};

    if(semop(semid, &signal, 1) < 0)
    {
        perror("Semop to signal semaphore failed\n"); 
        exit(EXIT_FAILURE);
    }
}

static void Wait(int semid, int semnum)
{
    // initialize the semaphore operation structures
    struct sembuf wait = {semnum, -1, 0};

    wait.sem_num = semnum;
    if(semop(semid, &wait, 1) < 0) 
    {
        perror("Semop to wait semaphore failed\n");
        exit(EXIT_FAILURE);
    }
}

static int GetVal(int semid, int semnum)
{
    int returnVal;
    if((returnVal = semctl(semid, semnum, GETVAL)) < 0)
    {
        perror("semctl GETVAL for semaphore failed\n");
        exit(EXIT_FAILURE);
    }
    return returnVal;
}
