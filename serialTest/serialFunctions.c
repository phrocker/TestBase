#include "serialFunctions.h"
#include <unistd.h>
#include <fcntl.h>
#include <pty.h>
#include <sys/ioctl.h>
#include <errno.h>


/************************************************************************************
*
*	openSerialPort
*
*	Opens a serial port and returns the file descriptor associated with that com port
*      
*	Arguments:
*
*		unsigned int comPort - com port
*
*	Return Value:
*
*		int - file descriptor
*
*************************************************************************************/
int openSerialPort(unsigned int comPort)
{
    char portFile[11];
    snprintf(portFile,sizeof(portFile),"/dev/ttyS%i",comPort);
    int fd = open(portFile,O_RDWR | O_NONBLOCK | O_ASYNC | O_NOCTTY);
    return fd;
}

/************************************************************************************
*
*	resetPort
*
*	Resets port attributes
*      
*	Arguments:
*
*		unsigned int portDescriptor -- port file descriptor
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void resetPort(unsigned int portDescriptor)
{
    tcsetattr (portDescriptor, TCSANOW, &originalTerminal);
}

/************************************************************************************
*
*	closeSerialPort
*
*	Closes the port/file
*      
*	Arguments:
*
*		unsigned int portDescriptor -- port file descriptor
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void closeSerialPort(unsigned int portDescriptor)
{
     
    close(portDescriptor);
}


/************************************************************************************
*
*	serialPortWrite
*
*	Writes to serial port/file; sets no-delay flag for port file
*      
*	Arguments:
*
*		unsigned int portDescriptor - port file descriptor
*       const void *data - data to be written to port
*       int size - size/amount of data to be written
*
*	Return Value:
*
*		int - size of data actually written
*
*************************************************************************************/
int serialPortWrite(unsigned int portDescriptor, void *data, int size)
{
    int ret=0;
    int fc = fcntl(portDescriptor, F_GETFL, NULL);
    fcntl(portDescriptor, F_SETFL,  fc|FNDELAY);
    if ( (ret = write(portDescriptor,data,size) ) ==-1 )
    {
        perror("write");
    }
    fcntl(portDescriptor, F_SETFL, fc);
    return ret;
}


/************************************************************************************
*
*	clearPort
*
*	Flushes port
*      
*	Arguments:
*
*		unsigned int portDescriptor - port file descriptor
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void clearPort(unsigned int port)
{
    tcdrain(port);
}

/************************************************************************************
*
*	serialPortRead
*
*	Reads data from the serial port
*      
*	Arguments:
*
*		unsigned int portDescriptor - port file descriptor
*       const void *readBuffer - pointer to read buffer
*       int size - size/amount of data to be read
*
*	Return Value:
*
*		int - size of data actually written
*
*************************************************************************************/
int serialPortRead(unsigned int portDescriptor, void *readBuffer, int size)
{
    int ret=0;
    int fc = fcntl(portDescriptor, F_GETFL, NULL);
    fcntl(portDescriptor, F_SETFL, fc|FNDELAY);
    
    if ( (ret = read(portDescriptor,readBuffer,size) ) ==-1 )
    {
        ret=-1;
    }
    fcntl(portDescriptor, F_SETFL, fc);
    return ret;
}


/************************************************************************************
*
*	setSerialPortAttributes
*
*	Reads data from the serial port
*      
*	Arguments:
*
*		unsigned int portDescriptor - port file descriptor
*       unsigned int inputBaudRate - input baud rate
*       unsigned int outputBaudRate - output baud rate
*       unsigned char parity - parity flag
*       unsigned char stopBit - stop bit flag
*
*	Return Value:
*
*		int - size of data actually written
*
*************************************************************************************/
void setSerialPortAttributes(unsigned int portDescriptor,
                            unsigned int inputBaudRate,
                            unsigned int outputBaudRate,
                            unsigned char parity,
                            unsigned char stopBit) 
{
	struct termios terminalSettings;
	// Get the current options for the port...
	tcgetattr(portDescriptor, &terminalSettings);
	// set input baud rate
	cfsetispeed(&terminalSettings, inputBaudRate);
    // set the output baud rate
	cfsetospeed(&terminalSettings, outputBaudRate);
	
	terminalSettings.c_cflag |= (CLOCAL | CREAD);

	if (!parity)
        terminalSettings.c_cflag &= ~PARENB;
    else
    {
        if (parity == 1)
        {
            terminalSettings.c_cflag |= PARENB;
            terminalSettings.c_cflag |= PARODD;
        }
        else
        {
            terminalSettings.c_cflag |= PARENB;
            terminalSettings.c_cflag &= ~PARODD;
        }

    }
    if (!stopBit)
    {   
        terminalSettings.c_cflag &= ~CSTOPB; // no stop bit
    }
    else
        terminalSettings.c_cflag |= CSTOPB; // no stop bit
	
	terminalSettings.c_cflag &= ~CSIZE;
    // make sure the characters are eight bits
	terminalSettings.c_cflag |= CS8;

    /*
    terminalSettings.c_iflag &= ~(BRKINT | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    terminalSettings.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON | ISIG | NOFLSH ); //| TOSTOP
    terminalSettings.c_cc[VMIN] = 0;
    terminalSettings.c_cc[VTIME] = 0;
    */
    fcntl(portDescriptor, F_SETFL, O_ASYNC|O_NONBLOCK);
	tcsetattr(portDescriptor, TCSANOW, &terminalSettings);

}

/************************************************************************************
*
*	serialBytesWaiting
*
*	returns the number of bytes waiting to be read from serial port/file
*      
*	Arguments:
*
*		unsigned int portDescriptor - port file descriptor\
*
*	Return Value:
*
*		int - size of data actually written
*
*************************************************************************************/
int
serialBytesWaiting (unsigned int portDescriptor)
{
    int bytes;
    ioctl (portDescriptor, FIONREAD, &bytes); /*read number of bytes in input buffer*/
    return bytes;
}
