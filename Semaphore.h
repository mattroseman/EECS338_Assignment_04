#ifndef _Semaphore_h
#define _Semaphore_h

// the outline for the operation to increase a semaphore value by one (missing the semaphore number)
struct sembuf signal;
// same as signal but for wait
struct sembuf wait;

// Signal takes in the semaphore group id and the semaphore number in that group
static void Signal(int, int);

// Wait takes same arguments as Signal
static void Wait(int, int);

// Get Val takes in semaphore group id and semaphore number and returns that semaphore number
static int GetVal(int, int);

#endif
