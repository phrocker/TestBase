///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDServer.h
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Header file for Test Dispatcher Server
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
#ifndef TESTDISPATCHERSERVER_H
#define TESTDISPATCHERSERVER_H 1

/*! \file
	\brief Function prototypes for the Test Dispatcher server
*/


#include "../CommonLibrary/BoardInfo.h"
#include "TDTestFunctions.h"

#include "../CommonLibrary/definitions.h"
#define BROADCAST_PORT 3490    // the port users will be connecting to
#define BROADCAST_RETURN_PORT 3491
  


#define BREAK_PORT  3492


/*! \def TCP_PORT
	\brief Minimal TCP Port number
*/
#define TCP_PORT 3493


/*! \def MAXCONNECTIONS
	\brief Maximum number of allowed connections
*/

#define MAXCONNECTIONS 150

/*! \def CONNECTIONSPERTHREAD
	\brief Number of connections per thread
*/
#define CONNECTIONSPERTHREAD 10

#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <dirent.h>

char *ownerMAC;
ulong ownerAddr;





fd_set fileDescriptorList;
fd_set readfds;
fd_set exceptfds;
int sockfd,bcSockFd, new_fd;  // listen on sock_fd, new connection assigned to new_fd
struct sockaddr_in myBC_addr, my_addr;    // my address information
char streamPort[6];
unsigned long repairLock;
char repairCounter;
//char repairLock,

short isBurnout;

#define NOTLOCKED 0x03

short handleDefaultMessage; // 

#define MAXBUFLEN 512

#include "DataStructures/queue.h"

#define DEFER 0x0fff
#define NOTSTARTED 0x0000
#define STARTED 0x0001
#define ENABLED 0x0010
#define TESTVIEW 0x0100
#define QUESTIONPAUSE 0x1000
#define BROADCAST 0x00
#define TCP       0x01

/*! \def NOTSTARTED
	\brief Preprocessor value to indicate a thread connection has not started
*/

/*! \def NOTSTARTED
	\brief Preprocessor value to indicate a thread connection  has started
*/

/*! \def ENABLED
	\brief Preprocessor value to indicate a thread connection is enabled
*/

/*! \def TESTVIEW
	\brief Preprocessor value to indicate a thread connection is in testView Mode
*/

/*! \def BROADCAST
	\brief Preprocessor value to signify a broadcast command
*/


/*! \def BROADCAST
	\brief Preprocessor value to signify a TCP command
*/

#define SERVERRUNNING 0x00
#define SERVERSTOPPED 0x03
#define SERVERSTOPPING 0x02


/*! \def SERVERRUNNING
	\brief Preprocessor value to indicate the server has started
*/

/*! \def SERVERSTOPPED
	\brief Preprocessor value to indicate the server has not started
*/

/*! \def SERVERSTOPPED
	\brief Preprocessor value to indicate the server is in the process of stopping
*/
short serverStatus;


/*! \struct threads TDServer.h
   \brief Thread structure which allows for easier socket multiplexing
 
	All data in this structure corresponds to the thread
	connections
*/
typedef struct {

	/*! \var myThread
		\brief posix thread
	*/

	/*! \var messageQueue
		\brief local thread's queue
	*/

	/*! \var threadStatus
		\brief local thread's status
	*/
	
	/*! \var threadMutex
		\brief local thread's mutex
	*/

	/*! \var threadNotifier
		\brief local thread's signal notifier
	*/

	/*! \var BUFFER[CONNECTIONSPERTHREAD]
		\brief local thread's command buffer
	*/

	/*! \var macAddress[CONNECTIONSPERTHREAD][18]
		\brief thread connection's mac address
	*/

	/*! \var testCount[CONNECTIONSPERTHREAD]
		\brief local connection's current test data count
	*/

	/*! \var diagCount[CONNECTIONSPERTHREAD]
		\brief local connection's current diagnostic data count
	*/

	/*! \var threadFD[CONNECTIONSPERTHREAD]
		\brief local connection's file descriptor
	*/

    pthread_t myThread;

    Queue messageQueue;

    ushort threadStatus;

    pthread_mutex_t *threadMutex;

    pthread_cond_t *threadNotifier;

    char *BUFFER[CONNECTIONSPERTHREAD];

    char macAddress[CONNECTIONSPERTHREAD][18];

    uint testCount[CONNECTIONSPERTHREAD];

    uint diagCount[CONNECTIONSPERTHREAD];

    ulong   threadIP[CONNECTIONSPERTHREAD];

    ushort threadFD[CONNECTIONSPERTHREAD];

    ushort connectionStatus[CONNECTIONSPERTHREAD];

} threads;

/*! \var typedef threads
    \brief Thread variable structure
*/


threads ServerThreads[(MAXCONNECTIONS/CONNECTIONSPERTHREAD)];

/*! \var ServerThreads[(MAXCONNECTIONS/CONNECTIONSPERTHREAD)]
    \brief Server thread structure list
*/


/*! \struct arp TDServer.h
   \brief Structure used for lookup of ip address/mac address information
 
	Structure contains long representation of ip address and mac address
*/
typedef struct
{
	/*! \var ipAddress
		\brief user's IP address
	*/

	/*! \var macAddress[18]
		\brief user's mac address
	*/


    ulong ipAddress;
    char macAddress[18];

} arp;

/*! \var typedef arp 
	\brief Variable structure for mac address lookup
*/

/*! \var arp
	\brief Variable structure for mac address lookup
*/
arp localArp[MAXCONNECTIONS*2];




/*! \var macLookup
	\brief Count of ip addresses in mac address lookup table
*/
unsigned short macLookup;


/*! \var mainThread
	\brief Main thread mutex
*/
pthread_mutex_t mainThread;

/*! \var threadHandler
	\brief Main thread mutex
*/

pthread_mutex_t *threadHandler;

/*! \var questionMutex
	\brief Question mutex to lock question buffer
*/

pthread_mutex_t *questionMutex;

/*! \var questionCondition
	\brief Question condition to indicate question buffer has been modified
*/

pthread_cond_t  *questionCondition;

/*! \var question
	\brief Question buffer
*/

char *question;

/*! \var questionType
	\brief Indicates type of question in questoin buffer
*/

char questionType[6];


/*! \var TESTCOUNT
	\brief Test data count
*/

uint TESTCOUNT;

/*! \var DIAGCOUNT
	\brief Diagnostic data count
*/


uint DIAGCOUNT;


/*! \var testData
	\brief Test data
*/

test *testData;

/*! \var diagnosticData
	\brief Diagnostic data
*/

test *diagnosticData;

/*! \var referenceBoardInfo
	\brief Pointer to board info
*/
PBOARDSTATE referenceBoardInfo;


/*! \var openThreads
	\brief Indicates number of open threads
*/
int openThreads;


/*! \var serverVersion
	\brief Test Dispatcher Server version
*/
float serverVersion;


/*! \fn int sendall (int s, char *buf, int *len)
    \brief This function repeatedly carries out the socket send() function on a long buffer to be sure all the data gets sent
    \param s the socket descriptor
	\param buf Pointer to the buffer that will be sent
	\param len Length of the buffer being sent
	\return 0 or -1 , success or fail
*/
int sendall(int s, char *buf, int *len);

/*! \fn int getUnusedPort(int startPort)
    \brief Attempts to find an unused port at startPort or higher by
	trying to bind a temp socket to each port. Upon success
	the socket is closed and the port number is returned
	\param startPort Port at which to begin searching for open ports
	\return an open port number or -1 on error
*/

int getUnusedPort(int startPort);

/*! \fn void *createServer(void *input)
    \brief Creates socket listener for TCP and datagram sockets
	\return void
*/
void *createServer(void *);

/**********************************************************
*	@fn sigchld_handler
*		Captures signals for the socket server. If anything
*		other than 17 is received, exit 
*
*	Arguments:
*		int ss - signal
*
*	Returns:
*		void
************************************************************/
void sigchld_handler(int);


/*! \fn char *handle_thread_event(char*,int,int connection,int thread,short type, int fd,socketAddress* inaddr, char *data)
    \brief Handles commands from packets
	\param connection Connection number within thread
	\param thread Thread number/ID
	\param type Type of command
	\param fd Socket file descriptor
	\param inaddr Socket structure
	\param data command
	\return char *
*/
char *handle_thread_event(char*,int,int connection,int thread,short type, int fd,socketAddress* inaddr, char *data);



/*! \fn loadServer(PBOARDSTATE)
    \brief Creates pointer to external structure for current test board
	\return void
*/
void loadServer(PBOARDSTATE);

/*! \fn setMacLookup(long ip, char *mac);
    \brief Sets the mac address to the ip address in mac lookup table
	\param ip User's ip address
	\param mac User's MAC address
	\return void
*/
void setMacLookup(long ip, char *mac);

/*! \fn setMacLookup(long ip, char *mac);
    \brief Attempts to lookup the user's MAC address via the input IP address
	\param ip User's ip address
	\return char* Pointer to MAC address if found, NULL if not found
*/
char *getMacLookup(long ip);

/*! \fn exitServer
    \brief Attempts to kill the server
	\return void
*/
void exitServer();

#endif


