#ifndef _Semaphore_h
#define _Semaphore_h

// Signal takes in the semaphore group id and the semaphore number in that group
static void Signal(int, int);

// Wait takes same arguments as Signal
static void Wait(int, int);

// Get Val takes in semaphore group id and semaphore number and returns that semaphore number
static int GetVal(int, int);

#endif
