#include <stdio.h>
#include "memoryManager.h"
// good ole fashioned LLs



memPointer *memoryList = 0;
unsigned short memoryListSize = 0;


memPointer *allocateMemoryPointer(void *ptr,unsigned int lowAddr, unsigned int highAddr)
{
   
    unsigned short i=0;
    int marker=-1;
    for(; i < memoryListSize; i++)
    {
        if (memoryList[i].ptr == 0)
        {
            marker = i;
            break;
        }
    }

    if (marker >= 0)
    {
        memoryList[i].ptr = ptr;
        memoryList[i].lowAddr = lowAddr;
        memoryList[i].highAddr = highAddr;
        marker = memoryList[i].marker = i;
        
        //memoryListSize++;
    }
    else
    {
        // grow by 1
        memPointer *tempList = (memPointer*)malloc( (memoryListSize+1)*sizeof(memPointer));
        for(i=0; i < memoryListSize; i++)
        {
            tempList[i].highAddr=memoryList[i].highAddr;
            tempList[i].lowAddr=memoryList[i].lowAddr;
            tempList[i].ptr=memoryList[i].ptr;
            tempList[i].marker = memoryList[i].marker;
        }
        tempList[i].ptr = ptr;
        tempList[i].lowAddr = lowAddr;
        tempList[i].highAddr = highAddr;
        marker = tempList[i].marker = i;
        free(memoryList);
        memoryListSize++;
        memoryList=tempList;
        // no entries are free
    }

    return &memoryList[marker];

}

short deAllocateMemoryPointer(memPointer *memEntry)
{
    if (memoryList == NULL)
        return TRUE;
    
    munmap(memoryList[memEntry->marker].ptr,memoryList[memEntry->marker].highAddr-memoryList[memEntry->marker].lowAddr);
    memoryList[memEntry->marker].ptr = 0;
    memoryList[memEntry->marker].lowAddr = 0;
    memoryList[memEntry->marker].highAddr = 0;
    memoryList[memEntry->marker].marker = 0;
    unsigned short zeroCount=0;
    unsigned int i=0;
    for(; i < memoryListSize; i++)
    {
        if (memoryList[i].ptr == 0)
        {
            zeroCount++;
        }
    }
    
    if (zeroCount == memoryListSize)
    {
        free(memoryList);
        memoryListSize = 0;
        return TRUE;        
    }
    else
        return FALSE;
    
    
}
