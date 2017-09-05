#include "../CommonLibrary/Common.h"
#include "serialTestFunctions.h"
#include <sys/io.h>
#include <pthread.h>
#include "argtable2.h"
pthread_t serialThread;

void *beginSerialTest(void *);

void killPort(short portNumber);


/*! \file
    \brief Serial Test implementation file

    serialTest functions in a similar manner as its DOS counterpart. Linux treats everything like
    a file, and the serial connections are no exception. Each serial connection is located at 
    at /dev/ttyS#, where # indicates the COM port number.Essentially, the test simply reads and
    writes to the serial port file, verifying that the data written equals the data read. This,
    of course, relies on a serial loopback plug. The kernel driver handles the interrupts. 

    One drawback is that there must be a small delay between reading and writting to the serial
    port file. This is because we are not dealing directly with the interrupt, rather a write
    triggers the driver which must trigger, and ultimately, handle the interrupt. Several layers
    exist with the linux driver. These layers allow the user space driver to know nothing
    about the hardware.

    In this test, the baud rate, parity, stop bit ( on/off ), and the data are alternated during
    the test. If the data is read back exactly, a fail is not triggered, but if a hardware problem
    exists, or the serial loopback plug is removed, the test will immediately failed.

    To set the attributes fcntl(...) is used. This function allows certain registers to be set
    for a device. setSerialPortAttributes outlines the processor of setting the attributes
    for a certain port. openSerialPort accepts a COM port number, where all other functions
    accept a file descriptor for the file
*/
int main(int argc, char *argv[])
{
    short i;
    short retry=1;

    struct arg_lit *debug,*help;
    struct arg_int *port;
    struct arg_str *retryarg;
    struct arg_end *end;
    
	setTestVersion(0.01);

     void *argtable[] = {
        port = arg_int0(NULL,"port","[0-9]","Zero based COM port number."),
        retryarg = arg_str0("rR","retry","Y/N","Y - Allows user to retry if test fails (default)."),
                arg_rem(NULL,"N - Does not allow user to retry if test fails"),
        debug = arg_lit0(NULL,"debug","Displays debug information."),
        help = arg_lit0("h","help","Displays usage information"),
         end = arg_end(20),

    };

    if (arg_nullcheck(argtable) != 0)
    {
        consolePrint("ERROR! Insufficient memory\n");
        exit(1);
    }

    // parse command line
    arg_parse(argc,argv,argtable);

    if (port->count == 0 || help->count > 0)
    {
        arg_print_syntax(stdout,argtable,"\n");
        arg_print_glossary(stdout,argtable,"  %-25s %s\n");

        goto exit;
    }

    

    if (retryarg->count > 0)
    {
        makeUpperCase((char *)retryarg->sval[0]);
        if( strcmp(retryarg->sval[0], "Y") )
        {
            consolePrint("got %s\n",retryarg->sval[0]);
            retry =0;
        }
    }
    
   
    test_com(port->ival[0], retry,debug->count);
    gracefulSerialShutdown(port->ival[0]);
exit:
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    
    return 0;
}
