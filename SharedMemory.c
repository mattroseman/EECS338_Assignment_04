#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "SharedMemory.h"

int CreateSegment(key_t key, size_t size)
{
    int shmid;
    if ((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget IPC_CREAT failed\n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

int GetSegment(key_t key)
{
    int shmid;
    if ((shmid = shmget(key, 0, 0)) < 0)
    {
        perror("shmget no flag failed\n");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

void DestroySegment(int shmid)
{
    if (shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0) < 0)
    {
        perror("shmctl IPC_RMID failed\n");
        exit(EXIT_FAILURE);
    }
}

void * AttachSegment(int shmid)
{
    void * shmaddr;
    if ((shmaddr = shmat(shmid, 0, 0)) < 0)
    {
        perror("shmat failed\n");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

void DetachSegment(void * shmaddr)
{
    if (shmdt(shmaddr) < 0)
    {
        perror("shmdt failed\n");
        exit(EXIT_FAILURE);
    }
}

size_t GetSegmentSize(int shmid)
{
    struct shmid_ds buf;
    if (shmctl(shmid, IPC_STAT, &buf) < 0)
    {
        perror("shmctl IPC_STAT failed\n");
        exit(EXIT_FAILURE);
    }
    return buf.shm_segsz;
}
