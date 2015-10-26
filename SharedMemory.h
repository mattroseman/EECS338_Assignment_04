#ifndef SharedMemory_h
#define SharedMemory_h

int CreateSegment(key_t, size_t);

int GetSegment(key_t);

void DestroySegment(int);

void * AttachSegment(int);

void DetachSegment(void *);

size_t GetSegmentSize(int);

#endif
