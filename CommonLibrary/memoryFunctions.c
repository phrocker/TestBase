#include <stdio.h>
#define __USE_LARGEFILE64
#include <sys/mman.h>
#include <sys/types.h>
#include  <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "memoryFunctions.h"
#include "memoryManager.h"


int memManFD=-1;

/*! \file
	\brief Memory read/write functions

    pointerToMemory is the only function which memory maps, since it returns
    a pointer. It is recommended that you perform your own memory mapping
    since pointerToMemory is quite basic. 

    Memory read and write functions open /dev/mem, as is required for memory
    access in user space, then they seek to the appropriate address. 

    remove the __USE_LARGEFILE64 declaration to remove large file support.

    The memory read functions below were altered to accept an unsigned long long argument
    This amounts to an unsigned long long value. lsee64 is now used to address
    memory beyond four gigabytes

    Warnings will arise when compiling because the c compiler thinks unsigned long long
    is a regular long variable. You can safely ignore these...I think

    In order to execute these functions, the calling user must have root
    access and iopl(3) must be used to change the the I/O access level
*/

memPointer *createFarPointer(unsigned short segment, unsigned short offset,unsigned long size)
{
    //unsigned int address = PHYSICAL_TO_VIRTUAL(segment,offset);
    //size =0x8000;
    unsigned int address =(segment<<4)+offset;//PHYSICAL_TO_VIRTUAL(segment,offset); //PHYSICAL_TO_VIRTUAL(segment,offset);
    return pointerToMemory(address,size);

}

void destroyMemoryPointer(memPointer *ptr)
{
    if (deAllocateMemoryPointer(ptr)) // no memory locations remaining
    {
        close(memManFD);
    }
}
/***************************************************************************
*	pointerToMemory
*	
*	Memory maps the input address to a pointer in user space.
*    
*	Arguments:
*		unsigned int address
*		
*	Returns:
*		Memory mapped void pointer
*
***************************************************************************/
memPointer *pointerToMemory(unsigned int address,unsigned long size)
{
    unsigned char returnBYTE=0x00;
    if (memManFD == -1)
    {
        memManFD = open("/dev/mem",O_NONBLOCK|O_SYNC|O_RDWR);
        if (memManFD < 0)
        {
            perror("open");
        }
    }
    unsigned int pageAlign = address/getpagesize();
    // pageAlign should be an integer
    
    pageAlign*= getpagesize();
    int difference = address-pageAlign;
    // memory map
    //consolePrint("page is %u, size is %u\n",pageAlign,size);
    size+=difference;
    
    void *pt = mmap(0,size,PROT_READ ,MAP_SHARED,memManFD,pageAlign);
    memPointer *returnPointer=0;
    if (pt != MAP_FAILED)
    {
        pt+=difference;
        
        returnPointer = allocateMemoryPointer(pt,address,address+size);
    }
    else
    {
        consolePrint("%i %i %i %i\n",errno,EACCES,EBADF,EINVAL);
        perror("mmap");
        pt=NULL;
        close(memManFD);
    }
    return returnPointer;
    
}

/***************************************************************************
*	memoryReadByte
*	
*	Reads a byte from memory
*    
*	Arguments:
*		unsigned int address
*		
*	Returns:
*		byte from memory
*
***************************************************************************/
unsigned char memoryReadByte(unsigned long long address)
{
    unsigned char returnBYTE=0x00;
    int fd = open("/dev/mem",O_NONBLOCK);
    lseek64(fd,address,SEEK_SET);
    //lseek(fd,address,SEEK_SET);
    read(fd,&returnBYTE,1);
    close(fd);
    return returnBYTE;
    
}

/***************************************************************************
*	memoryReadWord
*	
*	Reads a word from memory
*    
*	Arguments:
*		unsigned int address
*		
*	Returns:
*		word from memory
*
***************************************************************************/
unsigned short memoryReadWord(unsigned long long address)
{
    unsigned short returnWORD=0x0000;
    int fd = open("/dev/mem",O_NONBLOCK);
    lseek64(fd,address,SEEK_SET);
    //lseek(fd,address,SEEK_SET);
    read(fd,&returnWORD,sizeof(short));
    close(fd);
    return returnWORD;
    
}

/***************************************************************************
*	memoryReadWord
*	
*	Reads a double word from memory
*    
*	Arguments:
*		unsigned int address
*		
*	Returns:
*		double word from memory
*
***************************************************************************/
unsigned long int memoryReadDoubleWord(unsigned long long address)
{
    unsigned long int returnDWORD=0x0000;
    int fd = open("/dev/mem",O_NONBLOCK);
    lseek64(fd,address,SEEK_SET);
    //lseek(fd,address,SEEK_SET);
    read(fd,&returnDWORD,sizeof(short));
    close(fd);
    return returnDWORD;
    
}

/***************************************************************************
*	memoryReadWord
*	
*	writes a byte to memory
*    
*	Arguments:
*		unsigned int address
*		Data - byte to write ( I'm a poet )
*	Returns:
*		void
*
***************************************************************************/
void memoryWriteByte(unsigned long long address, unsigned char Data)
{
    int fd = open("/dev/mem",O_NONBLOCK);
    lseek64(fd,address,SEEK_SET);
    //lseek(fd,address,SEEK_SET);
    write(fd,&Data,sizeof(Data));
    close(fd);
}

/***************************************************************************
*	memoryWriteWord
*	
*	writes a word to memory
*    
*	Arguments:
*		unsigned int address
*		Data - word to write
*	Returns:
*		void
*
***************************************************************************/
void memoryWriteWord(unsigned long long address, unsigned short Data)
{
    int fd = open("/dev/mem",O_NONBLOCK);
    lseek64(fd,address,SEEK_SET);
    //lseek(fd,address,SEEK_SET);
    write(fd,&Data,sizeof(Data));
    close(fd);
}

/***************************************************************************
*	memoryWriteDoubleWord
*	
*	writes a double word to  memory
*    
*	Arguments:
*		unsigned int address
*		Data - double word  to write
*	Returns:
*		void
*
***************************************************************************/
void memoryWriteDoubleWord(unsigned long long address, unsigned long Data)
{
    int fd = open("/dev/mem",O_NONBLOCK);
    lseek64(fd,address,SEEK_SET);
    //lseek(fd,address,SEEK_SET);
    write(fd,&Data,sizeof(Data));
    close(fd);
}
