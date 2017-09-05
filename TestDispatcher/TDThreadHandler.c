///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDThreadHandler.c
 *
 *  @brief      Test Execution suite
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
#include "TDThreadHandler.h"
#include "TDServer.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/time.h>

#include <sys/types.h>

#include <sys/select.h> 

/*! \file
    \brief Test Dispatcher IPC definitions

      
*/   


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn     void writeCpuProfileFile(char *location)
 */
///////////////////////////////////////////////////////////////////////////////
void writeCpuProfileFile(void *loc)
{
    char *location = (char*)loc;
    char buffer[255 + strlen(location)];
    sprintf(buffer,"testDispatcherCpuInfo > %s",location);
    system(buffer);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn     void createCpuProfile(char *location)
 */
///////////////////////////////////////////////////////////////////////////////
void createCpuProfile(char *location)
{
    // fork so that the system(...) call in writeCpuProfileFile
    // does not block the parent process with its IO call
    
    int pid = fork();
    if (pid == 0)
    {
        writeCpuProfileFile(location);
        exit(0);
    }
    else if ( pid > 0)
    {
        cpuProfileWriterPid = pid; // child process id
    }
    

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn     void closeCpuProfile()
 */
///////////////////////////////////////////////////////////////////////////////
void closeCpuProfile()
{
    if (cpuProfileWriterPid > 0)
        kill(cpuProfileWriterPid,SIGQUIT);
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int createIpcListenerThread()
 */
///////////////////////////////////////////////////////////////////////////////
int createIpcListenerThread()
{

    IPCSTATUS=IPCSTOPPED;
	qID = 233321;
    testType=FUNCTIONAL;

	// create key for message Queue
	ipcMakeKey(&mykey); 

    mykey = TEST_DISPATCHER_PROJECT_ID;

    ipcOpenQueue(mykey, &qID);

    mykey = TEST_DISPATCHER_PROJECT_ID;
    qID=ipcGetKey(mykey);


  
    IPCEXIT=FALSE;

    if (pthread_create (&ipcListener, NULL, testListener, &qID))
    {
        consolePrint("ERROR!! COULD NOT CREATE IPC LISTENER THREAD!\n");
        exit(1);
    }

    return qID;

}

/************************************************************************************
*
*	createServerThread
*
*	Creates all server threads
*      
*	Arguments:
*
*		void
*
*	Return Value:
*
*		TRUE/FALSE
*
*************************************************************************************/
int createServerThread()
{
    serverStatus=SERVERSTOPPED;
    openThreads=-1;
    if (pthread_create(&serverListener,NULL,createServer,NULL))
    {
        consolePrint("ERROR! COULD NOT CREATE SERVER THREAD!\n");
        exit(1);
    }

    return TRUE;   

}

/************************************************************************************
*
*	killServerThread
*
*	Destroys all server threads
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
void killServerThread()
{


    char *buf = "close";
    
   destroyServerThreads();

    //pthread_cancel(serverListener);

    
    
    
}


/************************************************************************************
*
*	testListener
*
*	IPC thread to listen for and read message on the message queue
*      
*	Arguments:
*
*	    void *qid - System's queue ID
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void *testListener(void *qid)
{
    int localQid = *((int*)qid);

    previousTestData=NONE;
    IPCSTATUS=IPCSTARTED;
    IPCPACKET userResponse = {};
    while(1)
    {
        if (IPCEXIT) {
            break;
        }
        // create IPC packet
        IPCPACKET incomingPacket = {};
        // set release listener to false
        incomingPacket.data.releaseListener=FALSE;
        
        incomingPacket.mtype=0;
        // read a message from the queue, if one is not available block until
        // one is receievd
        int err = ipcReadMessage(localQid, 0, &incomingPacket,sizeof(IPCPACKET), TRUE); 
        // check the type of the packet
        switch( (incomingPacket.mtype&0x0f) )
        {
            case    IPCSERVERRESTART:
                {
                    // on server restart, store the current database
                    // information 
                    storeCurrentTestDataInDatabase();


                    key_t mykey = TEST_SHUTDOWN_SERVER_QUEUE;

                    // get Queue ID for question queue
                    int rebootQid=ipcGetKey(mykey);

                    
                    // send a message that we are finished, and the user can shutdown
                    ipcSendMessage(rebootQid,&incomingPacket,sizeof(IPCPACKET));

                }
                break;
            case QUESTION:
                {
                    // check the question type
                    if ( (incomingPacket.mtype) == (QUESTION | YESNO) )
                    {
                        answerQuestion(incomingPacket.data.info,"YESNO");
                    }
                    else if ( (incomingPacket.mtype) == (QUESTION | RETRYFAIL) )
                    {
                        answerQuestion(incomingPacket.data.info,"RETRYFAIL");
                    }
                    else
                    {
                
                        // request that each user answer the question
                        answerQuestion(incomingPacket.data.info,"USRIN");
                    }
                }
                break;
            case IPCSERVERTOSERVER:
                {
                    // if a the server was told to release the listener
                    // this likely means that we are exiting
                    if (incomingPacket.data.releaseListener==TRUE) {

                        IPCEXIT=TRUE;
                        IPCSTATUS=IPCSTOPPED;
                        return NULL;
                    }
                     
                }
                break;
            case IPCSERVERTOCLIENT:
                {
                 {
                  
                                         
                        if ( (incomingPacket.mtype) == (IPCSERVERTOCLIENT | IPCGETATTEMPTFROMUSER) )
                        {
                            answerQuestion(incomingPacket.data.info,"SELA");
                        }
                        else if ( (incomingPacket.mtype) == (IPCSERVERTOCLIENT | IPCGETREPAIRUSERLIST) )
                        {
                            answerQuestion(incomingPacket.data.info,"REPR");
                        }
                        else if ( (incomingPacket.mtype) == (IPCSERVERTOCLIENT | IPCDIAGNOSTICPRINT) )
                        {
                            previousTestData=DIAG;
                            queueDiagnosticData(incomingPacket.data.info);
                        }
                        else
                        if ( (incomingPacket.mtype) == (IPCSERVERTOCLIENT | IPCREGULARPRINTSTATUS) ) 
                        {
                            updateStatus(incomingPacket.data.info,previousTestData);
                        }
                        else
                        if ( (incomingPacket.mtype) == (IPCSERVERTOCLIENT | IPCREGULARPRINTUPDATE) )
                        {
                            updateTestDataQueue(incomingPacket.data.info);
                        }
                        else
                        if ( (incomingPacket.mtype) == (IPCSERVERTOCLIENT | IPCDIAGNOSTICPRINTUPDATE) )
                        {
                            updateDiagnosticTestDataQueue(incomingPacket.data.info);
                        }
                        else
                        {
                            previousTestData=TEST;
                            queueTestData(incomingPacket.data.info);
                        }
                            
                            
                    }

                }
            default:
                break;
        };
        
        
    }

    
    IPCSTATUS=IPCSTOPPED;
    return (NULL);

}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void waitForThreadRunningStatus()
 */
///////////////////////////////////////////////////////////////////////////////
void breakIPCListener()
{
    if (IPCSTATUS==IPCSTOPPED) {
        return;
    }
    IPCPACKET disconnectPacket ={};
    disconnectPacket.data.releaseListener=TRUE;
    //disconnectPacket.data.info = (PTESTBOARDINFO)malloc(sizeof(TESTBOARDINFO)*1);
    disconnectPacket.mtype=IPCSERVERTOSERVER;
   
    // send a message to local to kill IPC Listening

    ipcSendMessage(qID,&disconnectPacket,sizeof(IPCPACKET));

    // wait until the thread has exited
    while(IPCSTATUS==IPCSTARTED)
        {               sleep(1);       }

    ipcRemoveQueue(qID);

    

}

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void waitForThreadRunningStatus()
 */
///////////////////////////////////////////////////////////////////////////////
void waitForThreadRunningStatus()
{
    // simply polls until the threads are running
    while (IPCSTATUS==IPCSTOPPED);

    while (serverStatus==SERVERSTOPPED);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void waitForThreadRunningStatus()
 */
///////////////////////////////////////////////////////////////////////////////
void TdShutDownSequence()
{
    consolePrint("Attempting to release Message Queue...");
    // kill the IPC listener first
    breakIPCListener();
    consolePrint("Complete\n\n");

    // close the cpu profile
    closeCpuProfile();

    consolePrint("\nAttempting to shutdown server...");
    // now, kill the thread
    killServerThread();
    consolePrint("Complete\n\n");

}
