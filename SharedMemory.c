#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "SharedMemory.h"

// Creates a new shared memory segment based off of key and number of bytes size, and returns the segment id
static int CreateSegment(size_t size)
{
    int shmid;
    if ((shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget IPC_CREAT failed\n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Destroys the shared memory segment with the given id
static void DestroySegment(int shmid)
{
    if (shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0) < 0)
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

static size_t GetSegmentSize(int shmid)
{
    struct shmid_ds buf;
    if (shmctl(shmid, IPC_STAT, &buf) < 0)
    {
        perror("shmctl IPC_STAT failed\n");
        exit(EXIT_FAILURE);
    }
    return buf.shm_segsz;
}
