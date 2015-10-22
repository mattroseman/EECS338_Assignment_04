#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#define SEMAPHORE_KEY 0xFA2B
#define ALL_READ_WRITE 0666

/* binary semaphore mutex = 1 
 * nonbinary semaphore wlist = 0
 * int wcount = 0
 * int balance = 500
 * linked-list LIST = NULL
 */

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int semid;

union semun SemUnion;
unsigned short counters[] = {1,2,3,4};

int main() {
    pid_t pid = getpid();
    printf("Process ID: %d\n", pid);

    // IPC_RMID signals to remove the specified group
    if (semctl(SEMAPHORE_KEY, 0, IPC_RMID) < 0)
    {
        perror("semctl IPC_RMID failed");
        exit(EXIT_FAILURE);
    }
    
    // IPC_CREAT signals to make new group if key doesn't already exist
    if ((semid = semget(SEMAPHORE_KEY, 4, IPC_CREAT | ALL_READ_WRITE)) < 0)
    {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }
    printf("Semaphore group with id: %d was just created\n", semid);

    SemUnion.array = counters;
    
    // sets all semaphores in the group to the coresponding values in SemUnion.array, or in counters
    if (semctl(SEMAPHORE_KEY, 0, SETALL, SemUnion) < 0)
    {
        perror("semctl failed");
        exit(EXIT_FAILURE);
    }
    printf("Each semaphore in: %d has been initialized to a certain value", semid); 


    if (semctl(SEMAPHORE_KEY, 0, IPC_RMID) < 0)
    {
        perror("semctl IPC_RMID failed");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
