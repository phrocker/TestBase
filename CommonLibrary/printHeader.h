#ifndef PRINTHEADER_H
#define PRINTHEADER_H 1
#include "BoardInfo.h"

/*! \file
	\brief Prototypes for the functions involved with printing the 
    TBS Test header
*/

/*! \fn void printDateAndTimeString()
    \brief Prints the current date and time
	\return void
*/
void printDateAndTimeString();

/*! \fn void printHeader()
    \brief Prints the TBS header
	\return void
*/
void printBoardHeader(PBOARDSTATE board);

/*! \fn void printBoardString(const char *boardName)
    \brief Prints the TBS board name header
    \param boardName Board's name
	\return void
*/
void printBoardString(const char *boardName);

/*! \fn void printSoftwareVersionString(short version)
    \brief Prints the TBS Test software version String
    \param version The current test attempt's version number
	\return void
*/
void printSoftwareVersionString(short version);

/*! \fn void printSerialNumberString(long serialNumber)
    \brief Prints the TBS Serial Number String
    \param serialNumber Board's Serial Number
	\return void
*/
void printSerialNumberString(long serialNumber);


/*! \fn void printTesterString(const char *Tester)
    \brief Prints the initials of the current tester
    \param Tester Initials of the current tester
	\return void
*/
void printTesterString(const char *Tester);

#endif 
