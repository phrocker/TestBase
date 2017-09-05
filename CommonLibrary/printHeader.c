#include <time.h>
#include "Prompt.h"
#include "printHeader.h"
#include "BoardInfo.h"
#include "definitions.h"

/*! \file
	\brief Definitions for the functions related to printing the TBS header
*/

/************************************************************************************
*
*	printDateAndTimeString
*
*	Prints the current date and time
*      
*	Arguments:
*
*		void
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void printDateAndTimeString()
{

    time_t currentTime;

    struct tm *timeInfo;

    time( &currentTime );

    timeInfo = localtime( &currentTime );

    char dateTimeString[BUF_LEN]="";

    char hours[3],seconds[3],minutes[3];
    memset(hours,0x00,3);
    memset(seconds,0x00,3);
    memset(minutes,0x00,3);

    sprintf(hours,"%i",timeInfo->tm_hour);

    prefixExpand(hours,2);

    sprintf(minutes,"%i",timeInfo->tm_min);

    prefixExpand(minutes,2);

    sprintf(seconds,"%i",timeInfo->tm_sec);

    prefixExpand(seconds,2);
    
    sprintf(dateTimeString,"%s %s %i %s:%s:%s %i",getDayOfWeek(timeInfo->tm_wday),getMonth(timeInfo->tm_mon),timeInfo->tm_mday,hours,minutes,seconds,timeInfo->tm_year+1900);

    linePrint(dateTimeString);
  //  customMessage("\n");


}

/************************************************************************************
*
*	printBoardString
*
*	Prints the board String
*      
*	Arguments:
*
*		const char *boardName - Tested board's name
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void printBoardString(const char *boardName)
{
    centerPrint(53,"%s Single Board Computer Test",boardName);
   // customMessage("\n");
}

/************************************************************************************
*
*	printSoftwareVersionString
*
*	Prints the software version
*      
*	Arguments:
*
*		short version - Test Version
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void printSoftwareVersionString(short version)
{
    centerPrint(53,"Test Software Version 1.%i",version);
   // customMessage("\n");
}


/************************************************************************************
*
*	printSerialNumberString
*
*	Prints the serial number for the board
*      
*	Arguments:
*
*		long SerialNumber
*
*	Return Value:
*
*		void
*
*************************************************************************************/

void printSerialNumberString(long serialNumber)
{
    char ser[15];
    snprintf(ser,15,"%lu",serialNumber);
    prefixExpand(ser,8);
    centerPrint(53,"Serial Number: %s",ser);
   // customMessage("\n");

}

/************************************************************************************
*
*	printTesterString
*
*	Prints the initials of the current tester
*      
*	Arguments:
*
*		const char *Tester - Initials of the current tester
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void printTesterString(const char *Tester)
{
    centerPrint(53,"Tested By: %s",Tester);
    //customMessage("\n");
}

/************************************************************************************
*
*	printBoardHeader
*
*	Prints the entire TBS test Header
*      
*	Arguments:
*
*		void
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void printBoardHeader(PBOARDSTATE board)
{
    char *DASHED_LINE="----------------------------------------------------";    
    printDateAndTimeString();
    testPrint("");
    customMessage("\n");
    printBoardString(board->boardInfo.boardName);
    printSoftwareVersionString( board->attempt-1 );
    printSerialNumberString( board->boardInfo.serialNumber );
    printTesterString(board->boardInfo.tester);
    
 
    testPrint("    Function");

    customMessage("Test Results");

    centerPrint(53,"%s",DASHED_LINE);
}
