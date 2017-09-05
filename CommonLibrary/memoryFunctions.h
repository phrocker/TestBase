#ifndef MEMORYFUNCTIONS_H
#define MEMORYFUNCTIONS_H 1
#include "definitions.h"

#define PHYSICAL_TO_VIRTUAL(s,o) ((s<<sizeof(short)*8))|o // essentially MK_FP// essentially MK_FP//
#define FAR_TO_PHYSICAL(s,o) (s<<sizeof(short)*8)|o
/*! \def PHYSICAL_TO_VIRTUAL
    \brief Macro to convert physical address to a virtual address
*/


/*! \fn unsigned char memoryReadByte(unsigned long long)
    \brief Reads specified data type from memory
    \param address
    \return data from memory
*/


void destroyMemoryPointer(memPointer *ptr);
memPointer *createFarPointer(unsigned short segment, unsigned short offset,unsigned long size);
memPointer *pointerToMemory(unsigned int address,unsigned long size);
unsigned char memoryReadByte(unsigned long long);
/*! \fn unsigned short memoryReadWord(unsigned long long address)
    \brief Reads specified data type from memory
    \param address
    \return data from memory
*/
unsigned short memoryReadWord(unsigned long long address);
/*! \fn unsigned long int memoryReadDoubleWord(unsigned long long address)
    \brief Reads specified data type from memory
    \param address
    \return data from memory
*/
unsigned long int memoryReadDoubleWord(unsigned long long address);


/*! \fn void memoryWriteByte(unsigned long long, unsigned char)
    \brief Writes data to memory
    \param address
    \param data - data to write to memory
    \return void
*/
void memoryWriteByte(unsigned long long, unsigned char);

/*! \fn void memoryWriteWord(unsigned long long, unsigned short)
    \brief Writes data to memory
    \param address
    \param data - data to write to memory
    \return void
*/
void memoryWriteWord(unsigned long long, unsigned short);

/*! \fn void memoryWriteDoubleWord(unsigned long long, unsigned long)
    \brief Writes data to memory
    \param address
    \param data - data to write to memory
    \return void
*/
void memoryWriteDoubleWord(unsigned long long, unsigned long);
#endif 
