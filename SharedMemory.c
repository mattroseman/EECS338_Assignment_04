#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "SharedMemory.h"

// Creates a new shared memory segment based off of key, and returns the segment id
static int CreateSegment(key_t key)
{
    int shmid;
    if (shmid = shmget(key, 0, IPC_CREAT | 0666) < 0)
    {
        perror("shmget IPC_CREAT failed\n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Destroys the shared memory segment with the given id
static void DestroySegment(int shmid)
{
    if (shmctl(shmid, IPC_RMID, 0) < 0)
    {
        perror("shmctl IPC_RMID failed\n");
        exit(EXIT_FAILURE);
    }
}

static void * AttachSegment(int shmid)
{
    void * shmaddr;
    if ((shmaddr = shmat(shmid, 0, 0)) < 0)
    {
        perror("shmat failed\n");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

static void DetachSegment(void * shmaddr)
{
    if (shmdt(shmaddr) < 0)
    {
        perror("shmdt failed\n");
        exit(EXIT_FAILURE);
    }
}
