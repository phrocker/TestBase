#define _GNU_SOURCE
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <ucontext.h>

#include <execinfo.h>
#include "Debug.h"
#include <dlfcn.h>

#define dumpHighRegisers(a,b,c,d,s,di,bp,sp) __asm__ volatile("mov %%eax,%0; mov %%ebx,%1; mov %%ecx,%2; mov %%edx,%3; mov %%esi,%4; mov %%edi,%5; mov %%ebp,%6; mov %%esp,%7;"::"m"(a),"m"(b),"m"(c),"m"(d),"m"(s),"m"(di),"m"(bp),"m"(sp):"eax","ebx","ecx","edx","esi","edi","ebp","esp")

#define dumpSegmentRegisters(es,ds,cs,ss,fs,gs) __asm__ volatile("movl %%es,%0; movl %%ds,%1; movl %%cs,%2; movl %%ss,%3; movl %%fs,%4; movl %%gs,%5;"  ::"m"(es),"m"(ds),"m"(cs),"m"(ss),"m"(fs),"m"(gs))

#define current_text_addr() ({ void *pc; __asm__("movl $1f,%0\n1:":"=g" (pc)); pc; })
/*! \file
	\brief Rudimentary debug functions. More can be added later, but
    these serve as a starting point for debugging hardware or software
    issues
*/

/***********************************************************************
*	printHighLowPointerRegisters
*   Prints lower word, high and low bytes of certain registers. Only here
*   to make things easier to read on debug ( for pointer registers )
*	
*	Arguments: 
*		char reg -- register identifier
*	Return Value:
*		VOID
*		
***********************************************************************/
void printHighLowPointerRegisters(char reg)
{
    unsigned long longreg=0;
    unsigned long eaxReg=0,ebxReg=0,ecxReg=0,edxReg=0,esiReg=0,ediReg=0,ebpReg=0,espReg=0;

    dumpHighRegisers(eaxReg,ebxReg,ecxReg,edxReg,esiReg,ediReg,ebpReg,espReg);

    switch(reg)
    {
    case 'b':
            longreg = ebpReg;
        break;
    case 's':
            longreg= espReg;
        break;
    default:
        return;
    };
    unsigned short upper16 = (unsigned short)( (longreg&0xffff0000)>>16);
    unsigned short lower16 = (unsigned short)(longreg&0x0000ffff);
    consolePrint("Reg: [e%cp: 0x%X], [ %cp: 0x%x ], [ %cs: 0x%x ]\n",reg,longreg,reg,upper16,reg,lower16);
}

/***********************************************************************
*	printHighLowRegister
*   Prints lower word, high and low bytes of certain registers. Only here
*   to make things easier to read on debug
*	
*	Arguments: 
*		char reg -- register identifier
*	Return Value:
*		VOID
*		
***********************************************************************/
void printHighLowRegister(char reg)
{
    unsigned long longreg = 0;
    unsigned long eaxReg=0,ebxReg=0,ecxReg=0,edxReg=0,esiReg=0,ediReg=0,ebpReg=0,espReg=0;

    dumpHighRegisers(eaxReg,ebxReg,ecxReg,edxReg,esiReg,ediReg,ebpReg,espReg);
    switch(reg)
    {
    case 'a':
        longreg= eaxReg;
        break;
    case 'b':
        longreg= ebxReg;
        break;
    case 'c':
        longreg= ecxReg;
        break;
    case 'd':
        longreg= edxReg;
        break;
    default:
        return;
    };
    unsigned short lower16 = (unsigned short)(longreg&0x0000ffff);
    consolePrint("Reg: [e%cx: 0x%X], [ %cx: 0x%x ], [ %ch: 0x%x ], [ %cl: 0x%x ]\n",reg,longreg,reg,lower16,reg, (( lower16&0xff00)>>8),reg, ( lower16&0x00ff));
}




/***********************************************************************
*	dumpRegisters
*   Dumps all pertinent registers on debug
*	
*	Arguments: 
*		void
*	Return Value:
*		VOID
*		
***********************************************************************/
void dumpRegisters()
{
    unsigned long eaxReg=0,ebxReg=0,ecxReg=0,edxReg=0,esiReg=0,ediReg=0,ebpReg=0,espReg=0,eipReg=0,flags=0;



    __asm__ volatile("pushfl; popl %0;" : : "m"(flags) );

    dumpHighRegisers(eaxReg,ebxReg,ecxReg,edxReg,esiReg,ediReg,ebpReg,espReg);

    consolePrint("\nRegister Dump\n");
    consolePrint("[eax: 0x%.8X] [ebx: 0x%.8X] [ecx: 0x%.8X] [edx: 0x%.8X]\n",eaxReg,ebxReg,ecxReg,edxReg);
    consolePrint("[esx: 0x%.8X] [edi: 0x%.8X] [ebp: 0x%.8X] [esp: 0x%.8X]\n",esiReg,ediReg,ebpReg,espReg);
    eipReg = current_text_addr();
    consolePrint("[eip: 0x%.8X] [e-flags: 0x%.8X]\n\n",eipReg,flags);


    unsigned long es=0,ds=0,cs=0,ss=0,fs=0,gs=0;

    dumpSegmentRegisters(es,ds,cs,ss,fs,gs);

    consolePrint("[cs: 0x%.4x] [ds:  0x%.4x] [es:  0x%.4x] [ss:  0x%.4x] [fs:  0x%.4x]\n",cs,ds ,es,ss,fs);
    consolePrint("[gs:  0x%.4x]\n",gs);

    consolePrint("\n");
}

void handle_segfault(int sig, siginfo_t* info, void* stacktrace)
{

    unsigned int i=0,j=0;
    Dl_info dlinfo;
    void **bytePointer = 0;
    void *instructionPointer = 0;

    consolePrint("\nA Seg-Fault occured, causing your program to exit abruptly.\n");
    dumpRegisters();
    void *buffer[2048];
    int size = sizeof(buffer);
    char **symbols = NULL;

    i = backtrace(buffer,2048);
    if (i > 0)
    {
        symbols = backtrace_symbols (buffer, size);
    }
    else
        consolePrint("A stack trace could not be obtained...\n");
    
    if (symbols != NULL)
    {
        consolePrint("Program Stack-Trace\n");
        for (; j < i; j++)
              consolePrint("%s\n",symbols[j]);
        free(symbols);
    }
    exit(1);
}
