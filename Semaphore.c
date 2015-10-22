#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "Semaphore.h"


static void Signal(int semid, int semnum)
{
    // initialize the semaphore operation structures
    signal.sem_op = 1;
    signal.sem_flg = 0;
    signal.sem_num = semnum;

    if(semop(semid, &signal, 1) < 0)
    {
        perror("Semop to signal semaphore failed\n"); 
        exit(EXIT_FAILURE);
    }
}

static void Wait(int semid, int semnum)
{
    // initialize the semaphore operation structures
    wait.sem_op = -1;
    wait.sem_flg = 0;
    signal.sem_num = semnum;

    wait.sem_num = semnum;
    if(semop(semid, &signal, 1) < 0) 
    {
        perror("Semop to wait semaphore failed\n");
        exit(EXIT_FAILURE);
    }
}

static int GetVal(int semid, int semnum)
{
    int val;
    if((val = semctl(semid, semnum, GETVAL)) < 0)
    {
        perror("semctl GETVAL for semaphore failed\n");
        exit(EXIT_FAILURE);
    }
    return val;
}
