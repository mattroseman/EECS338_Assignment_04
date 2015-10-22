#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
// creating a new semaphore group
// int semget(IPC_PRIVATE, int numsems, IPC_CREAT | 0666)

#define SEMAPHORE_KEY 0xFA2B

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
    
    if ((semid = semget(SEMAPHORE_KEY, 4, IPC_CREAT | 0666)) == -1)
    {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }
    printf("Semaphore group with id: %d was just created\n", semid);

    SemUnion.array = counters;
    
    if (semctl(SEMAPHORE_KEY, 0, SETALL, SemUnion) == -1)
    {
        perror("semctl failed");
        exit(EXIT_FAILURE);
    }
    printf("Each semaphore in: %d has been initialized to a certain value", semid); 


    return 0;
}
