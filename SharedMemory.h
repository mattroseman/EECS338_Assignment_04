#ifndef _SharedMemory_h
#define _SharedMemory_h

static int CreateSegment(size_t);

static void DestroySegment(int);

static void * AttachSegment(int);

static void DetachSegment(void *);

static size_t GetSegmentSize(int);

#endif
