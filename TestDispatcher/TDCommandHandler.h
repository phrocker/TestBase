///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDCommandHandler.h
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Definitions for the functions involved with analyzing and responding
 *              to packets send to and received from the Test Dispatcher. These functions
 *              handle the commands from the client, and are responsible for responding              
 *
 *              **** Note: Nothing in this file needs to be called directly. It should not
 *              be included in your source code. ****  
 *  
 *  @author     Marc Parisi
 *  @author     marc.parisi@gmail.com                                                
 *                                                              
 *  @attention
 *              This program is free software; you can redistribute it and/or modify  
 *              it under the terms of the GNU General Public License as published by  
 *              the Free Software Foundation; either version 2 of the License, or     
 *              (at your option) any later version.                                   
 *  @attention                                                                      
 *              This program is distributed in the hope that it will be useful,       
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *              GNU General Public License for more details.                          
 *  @attention                                                           
 *              You should have received a copy of the GNU General Public License     
 *              along with this program; if not, write to the                         
 *              Free Software Foundation, Inc.,                                       
 *              59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 */ 
/////////////////////////////////////////////////////////////////////////////
#ifndef TESTDISPATCHERCOMMANDHANDLER_H
#define TESTDISPATCHERCOMMANDHANDLER_H 1
#include "../CommonLibrary/definitions.h"
#include "TDTestFunctions.h"
#include "../CommonLibrary/BoardInfo.h"

/*! \file
	\brief Prototypes for those functions related to analyzing and responding
    to command packets 
*/





/*! \def BROADCAST_PORT
	\brief Preprocessor value for our broadcast port
*/

/*! \def BROADCAST_RETURN_PORT
	\brief Preprocessor value for the broadcast return port
*/

/*! \def BREAK_PORT
	\brief Preprocessor value for the break port (deprecated)
*/

/*! \def TCP_PORT
	\brief Preprocessor value for the lowest possible TCP/IP port
*/

/*! \def UNKNOWN
	\brief Preprocessor value for an unknown command
*/

/*! \def TBS_BROADCAST
	\brief Preprocessor value for the initial broadcast command
*/

/*! \def TBS_TCP_GET_FILE
	\brief Preprocessor value for the get file data command
*/

/*! \def TBS_TCP_GET_TEST_DATA
	\brief Preprocessor value for the command to retrieve test data

*/

/*! \def TBS_TCP_GET_TEST_UPDATE_DATA
	\brief Preprocessor value for the command to update our test data
*/

/*! \def TBS_TCP_GET_QUESTION_RESPONSE
	\brief Preprocessor value for the question response command
*/

/*! \def TBS_TCP_GET_LAST_TEST_UPDATE_DATA
	\brief Preprocessor value for the command which returns the previous test data
*/

/*! \def TBS_EXIT
	\brief Preprocessor value for the exit command

*/
#define BROADCAST_PORT 3490    // the port users will be connecting to
#define BROADCAST_RETURN_PORT 3491
#define BREAK_PORT  3492
#define TCP_PORT 3493
#define UNKNOWN 0x0000
#define TBS_BROADCAST 0x0001
#define TBS_TCP_GET_FILE 0x1001
#define TBS_TCP_GET_TEST_DATA 0x1002
#define TBS_TCP_GET_TEST_UPDATE_DATA 0x1003
#define TBS_TCP_GET_QUESTION_RESPONSE 0x1004
#define TBS_TCP_GET_LAST_TEST_UPDATE_DATA 0x1005
#define TBS_EXIT 0x1006
#define TBS_AUTHENTICATE_USER   0x1007
// used when the user locks the board
#define TBS_TCP_REPAIR_LOCK_BOARD 0x1008


/*! \var mySerial[10]
	\brief Variable for local board's serial number (deprecated)

*/
/*! \var myInitials[10]
	\brief Variable for local tester's initials (deprecated)

*/
/*! \var myFinGood[10]
	\brief Variable for local board's FG Number (deprecated)

*/
/*! \var myRev
	\brief Variable for local FG Revision (deprecated)

*/
/*! \var streamPort[6]
	\brief Variable for stream port

*/

char mySerial[10];
char myInitials[10];
char myFinGood[10];
char myRev;
char streamPort[6];




/*! \fn int getCommandDecision(char *data)
    \brief Returns the integer representation of the input command
    \param data Command being analyzed
	\return Integer representation of the input command
*/
uint getCommandDecision(char *);

/*! \fn int getFileByteCount(char myFile[])
    \brief Returns the number of characters in a file
    \param myFile array holding the file name to open
	\return The char count or a negative on failure
*/
int  getFileByteCount(char myFile[]);			//find file size

/*! \fn int checkEmpty(char myFile[])
    \brief This function is used to see if a file is empty.
	It functions the same as getFileByteCount, but stops
	after reading 10 chars so as not to take a very long 
	time to check a large file
    \param myFile array holding the file name to open
	\return The char count or a negative on failure
*/
int  checkEmpty(char myFile[]);					//check if file is empty

/*! \fn int addFileNames(char myArray[], int addSize)
    \brief hen the app receives a broadcast to respond to, the
	response is an array containing the MAC address, serial 
	number and then  a list of the nonempty files in 
	/var/log sperated by |. The client app parses the 
	array and  displays the file names for the user. 
	The array is a jumble of information, but it all needs to 
	be returned to the client after a broadcast
    \param myArray Array that will be returned to the client
	\param addSize 1 if if the file sizes are being placed into the buffer along with the filenames
*/
void addFileNames(char myArray[], int addSize);	//make a list of file names

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short initial_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
 *
 *  @arg    <b>board</b> @n 
 *          - Board data structure
 *
 *  @arg    <b>their_addr</b> @n 
 *          - socket structure for user
 *
 *  @arg    <b>bc_addr</b> @n 
 *          - broadcast socket structure
 *
 *  @arg    <b>myinaddr</b> @n 
 *          - socket structure
 *
 *  @arg    <b>myinaddr</b> @n 
 *          - socket structure for TestDispatcher
 *
 *  @arg    <b>fd</b> @n 
 *          - connection's file descriptor
 *
 *  @arg    <b>bcBuffer</b> @n 
 *          - input buffer
 *
 *  @arg    <b>myReturn</b> @n 
 *          - ipointer to return nput buffer
 *
 *  @brief  Handles the initial broadcoast from clients which are searching for boards
 *	        currently running Test Dispatcher
 * 
 *          Appends broadcast string into myReturn  
 *
 *  @note   Obtains the local machine's mac address from the eth0 device.
 *  
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short initial_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn);

/*! \fn int default_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
    \brief Handles the default broadcast which occurs when the local board is listed as a local board
	in the corresponding client
    \param board Board data structure
	\param their_addr socket structure
	\param bc_addr socket structure
	\param myinaddr socket structure
	\param fd connection's file descriptor
	\param bcBuffer input buffer
	\param myReturn pointer to return buffer
	\return TRUE/FALSE value
*/
short default_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn);

/*! \fn int initial_broadcast(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
    \brief Determines whether to send file data or a file listing to the user based on the command. Calls either
	send_file_information or send_file_data
	\param their_addr socket structure
	\param bc_addr socket structure
	\param myinaddr socket structure
	\param fd connection's file descriptor
	\param bcBuffer input buffer
	\param myReturn pointer to return buffer
	\return TRUE/FALSE value
*/
void send_file(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int new_fd, char *bcBuffer, char *myReturn);

/*! \fn int send_file_information(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
    \brief Returns a listing of files in /var/log
	\param their_addr socket structure
	\param bc_addr socket structure
	\param myinaddr socket structure
	\param fd connection's file descriptor
	\param bcBuffer input buffer
	\param myReturn pointer to return buffer
	\return TRUE/FALSE value
*/
void send_file_information(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int new_fd, char *bcBuffer, char *myReturn);

/*! \fn int send_file_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
    \brief Sends file data within the specified file
	\param their_addr socket structure
	\param bc_addr socket structure
	\param myinaddr socket structure
	\param fd connection's file descriptor
	\param bcBuffer input buffer
	\param myReturn pointer to return buffer
	\return TRUE/FALSE value
*/
void send_file_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int new_fd, char *bcBuffer, char *myReturn);

/*! \fn int send_test_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char type,int *startLocation,uint end, int new_fd, char *bcBuffer,test *array ,char *myReturn)
    \brief Sends clear command to client
	\param their_addr socket structure
	\param bc_addr socket structure
	\param myinaddr pointer to return buffer
	\param new_fd connection's file descriptor
	\return void
*/
void send_test_clear(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,int new_fd);

/*! \fn int send_test_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char type,int *startLocation,uint end, int new_fd, char *bcBuffer,test *array ,char *myReturn)
    \brief Sends file data within the specified file
	\param their_addr socket structure
	\param bc_addr socket structure
	\param type type of data to send user ( either test data or diagnostic data)
	\param startLocation Pointer to integer which holds the connection's current test data location
	\param end Number of the absolute last test data object
	\param myinaddr pointer to return buffer
	\param fd connection's file descriptor
	\param bcBuffer input buffer
	\param array test data array, from which the test data will be extracted
	\param myReturn pointer to return buffer
	\return TRUE/FALSE value
*/
short send_test_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char type,int *startLocation,uint end, int new_fd, char *bcBuffer,test *array ,char *myReturn);

/*! \fn int userRespondToQuestion(char *data)
    \brief Receives the user's response to the most recent question
    \param data Question response
	\return TRUE/FALSE or defer if the user is defering the question
*/
short userRespondToQuestion(char *data);

void requestToClearData(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char type,int new_fd);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short requestToLockBoard(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char *data)
 *
 *  @arg    <b>ulong</b> ip 
 *          - Connecting client's ip address
 *
 *  @brief  If the board is already not locked, requestToLockBoard answers a
 * 
 *          request from repair to lock the board from being edited by anyone
 *          else. Essentially, requestToLockBoard checks a flag to ensure
 *          that it's not already locked, then locks the board. If the board
 *          is already locked, a return is sent to the user notifying him/her
 *          that the board is already locked. 
 *  
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short requestToLockBoard(ulong ip,char *data);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void catchTimer(int sig)
 *
 *  @arg    <b>int</b> sig 
 *          - signal
 *
 *  @brief  signal handle which is tailored to handling the timer set by
 *          repairTimer. This timer, after five intervals, released
 *          the board from lock
 *          
 *  
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void catchTimer(int sig);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int repairTimer(unsigned int secondsToWait);
 *
 *  @arg    <b>unsigned int</b> secondsToWait 
 *          - timer wait, in seconds
 *
 *  @brief  If the board is already not locked, requestToLockBoard answers a
 * 
 *          sets an interval timer before SIGALRM is tripped, at which point
 *          a handler may be used to coordinate an action. If an action is
 *          not set, then our program will exit
 *  
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int repairTimer(unsigned int secondsToWait);

#endif



