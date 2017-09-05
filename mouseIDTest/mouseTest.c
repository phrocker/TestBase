#include "../CommonLibrary/Common.h"
#include "argtable2.h"
#include <sys/io.h>

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <ucontext.h>

#include <execinfo.h>

#include <dlfcn.h>



#define MYVERSION 0.01

//*****************************
// Keyboard controller equates
//*****************************

#define PORT60                0x60 // 8742 Data port
#define PORT64                0x64 // 8742 Command port

#define OUTPUT_BUFFER_FULL    0x01 // K/C output buffer full
#define INPUT_BUFFER_FULL     0x02 // K/C input buffer full

void WaitObf();

void WaitIbe();

void Send64(short Command);

void KCCommandWrite(short Command, short Data);

void dumpRegisters();


/*! \file
    \brief Mouse Test Implementation

    Very simple I/O procedure to determine if mouse is present
*/

void crash()
{
    consolePrint("crashing\n");
    char *p = NULL;
    *p = 'a';
}

int main (int argc, char *argv[])
{
    // you must set the internal version of your test

    
    setTestVersion(MYVERSION);

    // give us permission to the ports
    ioperm(PORT60,1,1);
    ioperm(PORT64,1,1);


    struct arg_lit *debug,*help;
    struct arg_end *end;

    void *argtable[] = {
        debug = arg_lit0(NULL,"debug","Displays debug information."),
        help = arg_lit0("h","help","Displays usage information"),
         end = arg_end(20),

    };

    if (arg_nullcheck(argtable) != 0)
    {
        consolePrint("ERROR! Insufficient memory\n");
        exit(1);
    }

    arg_parse(argc,argv,argtable);

    int i,passed=1;
    unsigned char data[3];

    if (help->count > 0)
    {
        displayUsage(argtable,"Mouse ID Test",MYVERSION);
    }

    




	KCCommandWrite(0xD4, 0xE8); // Set mouse resolution to 2 = 50dpi
	WaitObf(); if((inb(PORT60) & 0xFF) != 0xFA){ passed = 0; goto mouse_end; }
	KCCommandWrite(0xD4, 0x02);
	WaitObf(); if((inb(PORT60) & 0xFF) != 0xFA){ passed = 0; goto mouse_end; }

	KCCommandWrite(0xD4, 0xF3); // Set sample rate to 0x64 = 100 reports per second
	WaitObf(); if((inb(PORT60) & 0xFF) != 0xFA){ passed = 0; goto mouse_end; }
	KCCommandWrite(0xD4, 0x64);
	WaitObf(); if((inb(PORT60) & 0xFF) != 0xFA){ passed = 0; goto mouse_end; }

	KCCommandWrite(0xD4, 0xE9);
	WaitObf(); if((inb(PORT60) & 0xFF) != 0xFA){ passed = 0; goto mouse_end; }
	for(i=0; i<3; i++)
		{
		WaitObf();			// Wait for data to come back
		data[i] = inb(PORT60);		// Read data returned
		}

mouse_end:

    
	testPrint("Mouse Test");
	if((data[1] != 0x02) || (data[2] != 0x64) || (passed == 0))
    {
        failedMessage();
        if (debug->count > 0)
            dumpRegisters();
    }
	else
		passedMessage();


      

    ioperm(PORT60,1,0);
    ioperm(PORT64,1,0);

    
    return 0;
}

//**************************************************************************
// This procedure waits for the 8742 to be ready to accept data.
//**************************************************************************
void WaitObf()
{
	while( (inb(PORT64) & OUTPUT_BUFFER_FULL) != OUTPUT_BUFFER_FULL );
}

//**************************************************************************
// This procedure waits for the 8742 to be ready to accept data.
//**************************************************************************
void WaitIbe()
{
	while( (inb(PORT64) & INPUT_BUFFER_FULL) == INPUT_BUFFER_FULL );
}


//**************************************************************************
// This procedure sends a keyboard command.
//**************************************************************************
void Send64(short Command)
{
	WaitIbe();					// Wait for keyboard empty
    outb(Command, PORT64);	// Send command to keyboar
	WaitIbe();					// Wait for keyboard empty
}



//**************************************************************************
// This procedure sends 8742 command and data.
//**************************************************************************
void KCCommandWrite(short Command, short Data)
{
	Send64(Command);			// Send command to 8742
    outb(Data, PORT60);	// Send command to keyboard
	WaitIbe();					// Wait for keyboard empty
}


