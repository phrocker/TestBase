#ifndef SERIALFUNCTIONS_H
#define SERIALFUNCTIONS_H 1
#include <pty.h>

struct termios originalTerminal;
/*! \fn int serialBytesWaiting(int)
    \brief Returns the number of bytes to be read by serial port
    \param portFileDescriptor File descriptor
	\return Number of bytes waiting
*/
int serialBytesWaiting(unsigned int );

/*! \fn int resetPort(unsigned int)
    \brief Resets all flags associated witha  port
    \param portFileDescriptor File descriptor
	\return void
*/
void resetPort(unsigned int);

/*! \fn int clearPort(unsigned int)
    \brief Clears all data from the specified port
    \param portFileDescriptor File descriptor
	\return void
*/
void clearPort(unsigned int);

/*! \fn int openSerialPort(unsigned int)
    \brief Opens a com port and returns a file descriptor
    \param comPort Communications port
	\return File descriptor to com port
*/
int openSerialPort(unsigned int);

/*! \fn int closeSerialPort(unsigned int)
    \brief Closes a serial port
    \param portFileDescriptor File descriptor
	\return void
*/
void closeSerialPort(unsigned int);

/*! \fn int serialPortWrite(unsigned int,const void*,int)
    \brief Writes to serial port
    \param portFileDescriptor File descriptor
    \param data data to write
    \param size size of data to be written
	\return size of data actually written
*/
int serialPortWrite(unsigned int,void*,int);

/*! \fn int serialPortRead(unsigned int,const void*,int)
    \brief Reads from serial port
    \param portFileDescriptor File descriptor
    \param readBuffer buffer in which to place data
    \param size size of readBuffer
	\return size of data read
*/
int serialPortRead(unsigned int,void*,int);

/*! \fn int setSerialPortAttributes(unsigned int ,unsigned int ,unsigned int,unsigned char, unsigned char)
    \brief Sets the attribute for a given serial port
    \param portFileDescriptor File descriptor
    \param intputBaudRate input baud rate
    \param outputBaudRate ouput baud rate
    \param parity parity flag
    \param stopBit stop bit flag
	\return void
*/
void setSerialPortAttributes(unsigned int ,unsigned int ,unsigned int,unsigned char, unsigned char);

#endif
