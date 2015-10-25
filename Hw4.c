#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "Semaphore.c"
#include "SharedMemory.c"

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
// The identifier for the shared memory segment that will be made
int shmid;

// The address in memory of the shared memory 
void * memaddr;

// initial starting values of the semaphores
unsigned short counters[] = {0,0,0,0};

// a message to be passed between processes
char * message;

char * pargs[3];

int main() 
{
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


    // creates a new shared memory segment the size of the message
    shmid = CreateSegment(sizeof(message));
    printf("The shared memory segment with id %d has been created\n", shmid);

    memaddr = AttachSegment(shmid);
    printf("The shared memory address at %p has been associated with process %d\n", memaddr, getpid());

    // Assign the arguments for the withdraw program
    pargs[0] = "withdraw";
    printf("%d\n", semid);
    printf("%s\n", pargs[1]);
    if (sprintf(pargs[1], "%d", semid) < 0)
    {
        perror("sprintf failed\n");
        exit(EXIT_FAILURE);
    }
    pargs[2] = NULL;

    //Create a child process and send it a string
    CatchError((pid = fork()), "fork failed\n");

    if (pid == 0)
    {
        printf("Created new process with pid: %d\n", getpid());
        // the child process is now running the withdraw program 
        CatchError(execvp("./withdraw", pargs), "execvp failed\n");
    }
    Wait(semid, 0);

    message = "The passed message worked great\n";


Cleanup:
    DetachSegment(memaddr);
    printf("The shared memory address at %p assiociated with process %d has been detached\n", memaddr, getpid());

    DestroySegment(shmid);
    printf("The shared memory segment with id %d has been removed\n", shmid);

    DestroyGroup(semid);
    printf("The semaphore group with id %d has been removed\n", semid);

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
