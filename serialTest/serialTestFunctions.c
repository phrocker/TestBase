#include <unistd.h>
#include "../CommonLibrary/Common.h"
#include "serialTestFunctions.h"
#include "serialFunctions.h"

unsigned int baudRates[] = {B1200,B2400,B4800,B9600,B19200,B38400,B57600,B115200 };
unsigned char *baudRateStrings[] = {"1200","2400","4800","9600","19200","38400","57600","115200"};



unsigned char patterns[] = {0x55,0x7f}; //,0x55,0xff,0xa0,0x0f};


/************************************************************************************
*
*	gracefulSerialShutdown
*
*	Attempts to gracefully restore system resources for the communication port
*      
*	Arguments:
*
*		short port - port
*
*	Return Value:
*
*		short
*
*************************************************************************************/
void gracefulSerialShutdown(int port)
{
    //  open the serial port
    int fd = openSerialPort(port);
    if (fd >= 0)
    {
        // reset the port's attributes
        resetPort(fd);
        // now, close the serial port
        closeSerialPort(fd);
    }

}

/************************************************************************************
*
*	test_com
*
*	Executes communications test
*      
*	Arguments:
*
*		short port - port
*       short retry - retry flag
*       short debug - debug flag
*
*	Return Value:
*
*		short
*
*************************************************************************************/
void test_com(int port, int retry, short debug)
{
    short result;
	

    consolePrint("\nNow testing COM %d\n",port+1);
    
    do
    {
        // execute serial test
    	result = serial_test(port,debug); // 1 is returned on error, 0 otherwise
        
    	if(retry == YES)
    		{
    		if(result == 1)
    			{
        			consolePrint("\nSerial Port Test FAILED!\n");
                    // if the com test fialed, then we ask the user if he/she wishes
                    // to retry the test
                    if (askRetryFailQuestion("Would you like to retry the serial test?")!=RETRY )
                    {
                        break;
                    }
                    else
                        gracefulSerialShutdown(port);
    			
    		    }
                else
                    break;
    		}
            else
                break;

    }while(1);

	testPrint("Serial Port Test (COM %d)",port+1);

	if (result == 1)
		failedMessage();
	else
		passedMessage();
}

void usage()
{
	testPrint("Serial Test Parameter Check");
	failedMessage();

	consolePrint("Tests a single serial port using and external serial loopback plug.\n");
	consolePrint("Example: serialTest 0 Y\n\n");
	consolePrint("USAGE: SERIAL1 <Port> <Retry>\n");
	consolePrint("  <Port>  : Zero based DOS COM port number.\n");
	consolePrint("  <Retry> : Y - Allows user retry if test fails (default).\n");
	consolePrint("            N - Does not allow user to retry if test fails.\n");
	consolePrint("You can override the test message with the TESTMSG TSET variable.\n");
}


/************************************************************************************
*
*	serial_test
*
*	Performs serial test on port
*      
*	Arguments:
*
*		short port - port
*       short debug - debug flag
*
*	Return Value:
*
*		short
*
*************************************************************************************/
short serial_test(short port, short debug)
{
    iopl(3); // give ourselves permission to open port
    // open port and obtain file descriptor
    int localPortFileDescriptor = openSerialPort(port);
    
    short status=0x00;
    short i,j,k,l;
    unsigned char receiveChar;
    unsigned char sendChar;
    
    diagnosticPrint("Beginning Serial Test\n");
    for (i=0; i < sizeof(baudRates)/sizeof(int); i++) // baud rate
    {

        for (j=0; j < 3; j++) // parity
        {

            for (k=0; k < 2; k++) // stop bits
            {
                // set the attributes for our port
                setSerialPortAttributes(localPortFileDescriptor,baudRates[i],baudRates[i],(char)j,(char)k);
                for (l=0; l < sizeof(patterns)/sizeof(char); l++)
                {

                    // write to serial port
                    int writeAmount = serialPortWrite(localPortFileDescriptor,&patterns[l],1);
                    
                    if (writeAmount == -1)
                    {
                        return 1;
                    }
                    // since we are not using an interrupt, there is a 25 millisecond
                    // delay between writing to the file descriptor and reading from
                    // the file descriptor
                    usleep(25 * 1000);

                    // read from serial port
                    int ret = serialPortRead(localPortFileDescriptor,&receiveChar,1);

                    diagnosticLinePrint("Baud: %s, Parity: %X (0=none,1=odd,2=even), Stop Bits: %X, Pattern %X\n",baudRateStrings[i],j,k,patterns[l]);

                    // verify the pattern that was written, and read
                    if (patterns[l] != receiveChar || ret ==-1)
                    {
                       return 1;
                    }
                    
                    

                    
                }
            }
        }
    }

    diagnosticPrint("Serial Test Finished\n");

    // flush the serial port, assuming
    // that we actually reach this point, of course
    tcflush (localPortFileDescriptor, TCIFLUSH);
    // close the serial port
    closeSerialPort(localPortFileDescriptor);
    return 0;
}
