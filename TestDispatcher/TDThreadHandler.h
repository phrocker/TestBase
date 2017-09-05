
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDThreadHandler
 *
 *  @brief      Test Execution suite header file
 *
 *              Copyright (C) 2006 @n@n
 *              Receives board information from client, obtains test data from
 *              the database server, executes the individual tests, and returns
 *              any test information to the user TDThreadHandler 'handles' the 
 *              creating, destroying, and listening to threads. All IPC interactions
 *              are directed through this file
 *
 *              The Test Dispatcher IPC is a single thread since ipcReadMessage works
 *              better as a blocking msgget. A message queue is created and blockes until data is
 *              received, making it difficult to receive data with a single thread. Once
 *              the data is received, it is analyzed, and the action is performed by THIS THREAD.
 *
 *              Once the action is performed, if any new items have been pushed on the message queue,
 *              it will read them, sequentially. 
 *              
 *              **** Note: Nothing in this file needs to be called directly. It should not
 *              be included in your source code. ****
 *  
 *  @author     Marc Parisi
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

#ifndef TESTDISPATCHERIPC_H
#define TESTDISPATCHERIPC_H 1
#include "../CommonLibrary/IPCFunctions.h"


key_t mykey;
int serverQid;
pthread_t ipcListener,serverListener, cpuProfileWriter;

#define NONE    0 /**< zero definition to indicate nothing has occured  */
#define TEST    1
#define DIAG    2
short cpuProfileWriterPid;

short previousTestData; /**< previous test marker  */

/*! \var short IPCEXIT
    \brief message queue exit status. 

    used to indicate that our listener should exit
 */
short IPCEXIT; /**<   */

/*! \def IPCSTOPPED
    \brief Definition to indicate that message queue is stopped
 */
#define IPCSTOPPED 0x00 
/*! \def IPCSTOPPED
    \brief Definition to indicate that message queue is running
 */
#define IPCSTARTED  0x01
short testType;

/*! \var char IPCSTATUS
    \brief status variable for message queue

    Indicates listener status
 */
char IPCSTATUS;


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void breakIPCListener()
 * 
 *  @brief      Cause the IPC listener ( for the message queue ) to exit
 * 
 *              Queues a message that lets the IPC listener know that it should
 *              exit. Once the message is received, a flag is set to exit
 *              then breakIPCListner waits until the message queue has exited
 *
 *  @warning    <b>should not be called in tests</b>
 *
 */
///////////////////////////////////////////////////////////////////////////////
void breakIPCListener();

/*! \fn void testListener(void *)
    \brief Function for the IPC listener thread
	\return void
*/
void *testListener(void *);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int createIpcListenerThread()
 * 
 *  @brief      Instantiates IPC listener thread
 * 
 *              Starts the message queue listener, which is the segway for data
 *              transfer between the Test Dispatcher, tests, and listening clients 
 *
 *  @warning    <b>should not be called in tests</b>
 *
 */
///////////////////////////////////////////////////////////////////////////////
int createIpcListenerThread();



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void waitForThreadRunningStatus()
 * 
 *  @brief      Blocks Test Dispatcher until all threads are 'running'
 * 
 *              Running means that thread is set as ready,ready/block, or
 *              suspended/ready ( we're hoping that it is ready at this point) 
 *
 *  @warning    <b>should not be called in tests</b>
 *
 */
///////////////////////////////////////////////////////////////////////////////
void waitForThreadRunningStatus();


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void writeCpuProfileFile(char *loc)
 * 
 *  @arg        <b>void *</b> loc
 *                  - location of file to pipe output to
 *
 *  @brief      Pipes the output of testDispatcherCpuInfo to a file specified by location
 *              writeCpuProfileFile executes the system function
 *
 *  @warning    <b>should not be called in tests</b>
 *              
 */
///////////////////////////////////////////////////////////////////////////////
void writeCpuProfileFile(void *loc);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void writeCpuProfile(char *location)
 * 
 *  @arg        <b>char *</b> location
 *                  - location of file to pipe output to
 *
 *  @brief      Pipes the output of testDispatcherCpuInfo to a file specified by location
 *              createCpuProfileFile spawns a thread, using writeCpuProfileFile
 *
 *  @warning    <b>should not be called in tests</b>
 *
 */
///////////////////////////////////////////////////////////////////////////////
void createCpuProfile(char *location);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void closeCpuProfile()
 * 
 *
 *  @brief      Destroys the thread that was used to create the cpu profile file
 *
 *  @warning    <b>should not be called in tests</b>
 *
 */
///////////////////////////////////////////////////////////////////////////////
void closeCpuProfile();


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void TdShutDownSequence()
 * 
 *
 *  @brief      Shuts down the server by performing the following sequence
 *                  1: Closes the main message queue
 *                  2: Closes the thread responsible for creating the cpu profile
 *                  3: Closes the server thread. 
 *
 *  @note       The thread for the cpu profile should complete long before the
 *              test Dispatcher finished; however, in cases of testing or error
 *              we will want to ensure that this file is closed so that TD 
 *              closes gracefully without a lingering thread
 *
 *   @note      The function responsible for closing the server thread must
 *              ensure that all subordinate threads are closed. This may take
 *              a second or two, as there could be up to MAXCONNECTIONS/2 threads
 *
 *  @warning    <b>should not be called in tests</b>
 *
 */
///////////////////////////////////////////////////////////////////////////////
void TdShutDownSequence();


#endif
