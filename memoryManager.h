#ifndef MEMMAN_H
#define MEMMAN_H 1
#include "definitions.h"


memPointer *allocateMemoryPointer(void *ptr,unsigned int lowAddr, unsigned int highAddr);
short deAllocateMemoryPointer(memPointer *);


#endif
