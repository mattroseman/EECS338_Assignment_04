#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
// creating a new semaphore group
// int semget(IPC_PRIVATE, int numsems, IPC_CREAT | 0666)

#define SEMAPHORE_KEY 0xFA2B


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};


int main(int argc, char *argv[]) {
    printf("Process ID: %d\n", getpid());
    return 0;
}
