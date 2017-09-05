///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDServerThreads.c
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              **** Note: Nothing in this file needs to be called directly. It should not
 *              be included in your source code. ****
 *
 *               A little history about why threads are used. The Test Dispatcher
 *              Server is located in TDServer.c. It executes and uses the socket multiplexer
 *              select(...). This functions just as a hardware MUX. You provide it a list of
 *              file descriptors which function as the select lines. When one is high, i.e 
 *              an individual file descriptor in the file descriptor list is 1, then we
 *              can begin to do our magic. Note, that select blocks until a read on a socket
 *              occurs. Select can act upon write and exception file descriptors as well, but
 *              for the purpose of Test Dispatcher, they are not needed. 
 *
 *              Once a socket connection is accepted via accept, a thread is creatd for that
 *              socket descriptor. This allows the thread running Test Dispatcher to only
 *              recv data, not process it. Generally, the processing is done via forking a new
 *              process, but this is obviously more expensive than creating a thread. 
 *
 *              There are a myriad of reasons why fork was not used. For example, each thread 
 *              uses conditions to signal each other that data's been received, modified,etc.
 *              Doing this between forked processes can become cumbersome. Suffice it to say,
 *              threads were chosen for their power. Unfortunately, posix threads pose a significant
 *              design problem. Mutexes are widely utilized to prevent data corruption, and to
 *              successfully allocate/deallocate memory when needed. Valgrind was used to 
 *              ensure that there were no memory leaks and the threads were behaving properly.
 *
 *              Threading allows for a significant level of communication over individual processes.
 *              Message queuing is used extensively already, so there is no need to worry about
 *              further complication by introducing more message queues, especially during socket
 *              communication. As already stated, one major benefit of threading is the ability to
 *              easily utilize mutual exclusion structures to prevent multiple access to a multitude
 *              of data object. In addition to mutexes, posix conditions are used to signal 
 *              locked threads which are waiting for data to arrive. 
 *
 *              It was decided that threading was a far better alternative to fork, after all, we
 *              want a test environment that can handle multiple requests while still performing
 *              tests on our board. This can be done with fork, but would be overly complicated. 
 *              CONNECTIONSPERTHREAD is a definition in TDServer.h which specifies how many 
 *              socket connections can be allocated per thread. As of design time, this is set
 *              to two. This can be increased to allow for more connections; however, it's probably
 *              better to increase the number of maximum connections. 
 *
 *              I have tested the threading across two computers, but it has NOT been verified 
 *              across more than two. This should not be a problem, however.            
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
#include "TDServerThreads.h"
#include "TDServer.h"
#include "TDCommandHandler.h"

/*! \file
    \brief Test Dispatcher Thread function definitions

    
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


/************************************************************************************
*
*	initializeServerThreads
*
*	Initializes all server threads and allocates memory for mutexes and conditions
*      
*	Arguments:
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void initializeServerThreads()
{
    
	// allocate memory for thread mutex
    threadHandler = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	if (threadHandler == NULL)
		noMemoryHalt(); // not enough memory
	
	// initialize posix thread mutex
	pthread_mutex_init (threadHandler, NULL);
    
    int i,j;
	
    for (i=0; i < (MAXCONNECTIONS/CONNECTIONSPERTHREAD); i++) {
        // create notifier for thread
        ServerThreads[i].threadNotifier= (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
        // make sure the server thread is not started
        ServerThreads[i].threadStatus=NOTSTARTED;
        // create thread mutex, specifically for thread condition
        ServerThreads[i].threadMutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
        // now, intitialize thread mutex and conditions
        pthread_mutex_init (ServerThreads[i].threadMutex, NULL);
        pthread_cond_init (ServerThreads[i].threadNotifier, NULL);  
    }

	
	questionMutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	if (questionMutex==NULL)
		noMemoryHalt(); // not enough memory
    questionCondition = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	if (questionCondition==NULL)
		noMemoryHalt(); // not enough memory
	// initialize thread mutex
	pthread_mutex_init (questionMutex, NULL);
	// initialize question condition to NULL
    pthread_cond_init (questionCondition, NULL);   
    testData = NULL; // initialize test data to NULL
    question=NULL; // initialize question to NULL
    TESTCOUNT=DIAGCOUNT=0; // set counts to zero
    
    macLookup=0;
}


/************************************************************************************
*
*	destroyServerThreads
*
*	Destroys all server threads, deallocates memory assigned to mutexes and conditions.
*	In addition, all buffers (test data, diagnostic data, and command buffers ) are
*	empties, and released. Message/Command Queues under each thread are destroyed as well
*      
*	Arguments:
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void destroyServerThreads()
{
    
    int i=0,j=0;  // initialize iterators to zero

    // if test data is null, we free its data
    
    if (testData!= NULL) {
        /*
        for (i=0; i < TESTCOUNT; i++) {
            if (testData[i].testData != NULL) {
                free(testData[i].testData);
            }

            if (testData[i].status != NULL) {
                free(testData[i].status);
            }
         
        }*/   
		// if test count is greater than or equal to one
		// we free testData
        if (TESTCOUNT>=1) {
            free(testData);
        }
        
    }
	
	// if diagnostic data is not null
    
    if (diagnosticData!= NULL) {
        /*
        for (i=0; i < DIAGCOUNT; i++) {
            if (diagnosticData[i].testData != NULL) {
                free(diagnosticData[i].testData);
            }

            if (diagnosticData[i].status != NULL) {
                free(diagnosticData[i].status);
            }
         
        }*/   
		// if dialog data count is greater than or equal to one
		// we free diagnosticData
        if (DIAGCOUNT>=1) {
            free(diagnosticData);
        }
        
    }

    // move through all connections
    for (i=0; i < (MAXCONNECTIONS/CONNECTIONSPERTHREAD); i++) {
        
		// set all file descriptors to zero, so to disconnect
		// all threads
        for (j=0; j < CONNECTIONSPERTHREAD; j++) {
            ServerThreads[i].connectionStatus[j]=0x0000;
            ServerThreads[i].threadFD[j]=0x0000;
            ServerThreads[i].threadIP[j]=0x0000;

        }
        // notify the thread to exit
                
        
        
		// access only open threads
        if (i <= openThreads) {
			
        	// signal all threads that each file descriptor is zero
			// and as a result, they should get outta dodge ( or exit )
            pthread_cond_signal(ServerThreads[i].threadNotifier);
           

            while(1)
            {
            	// okey dokey, lock the mutex, so we have
				// exclusive access to the thread
            	pthread_mutex_lock(threadHandler);
				
				/*
				 * wait until the thread is started. This should only
			     * be a formality, but with threading, you unfortunately
				 * cannot be certain of each thread's granularity. Put simply,
			     * you cannot force each thread to begin in the same time frame
				 */
                if ( ( ServerThreads[i].threadStatus&STARTED) != STARTED)
                {
                	// if a thread is started, unlock 
                    pthread_mutex_unlock(threadHandler);
                    break;
                }
				// unlock the thread mutex
                pthread_mutex_unlock(threadHandler);
                
            }           
        }

        // traverse each connection and check its buffer
		// if it's not null, free it
        for (j=0; j < CONNECTIONSPERTHREAD; j++) {
            if (ServerThreads[i].BUFFER[j] != NULL) {
                free(ServerThreads[i].BUFFER[j]);
            }
        }
       
        // wait for the thread to exit
    }
    
	// free the question if it's not null
     if (question!=NULL) {
        free(question);
    }
    
   
	// destroy question mutex
    pthread_mutex_destroy (questionMutex);
	
	// free the question mutex
    free(questionMutex);
    
	// now, destroy and free the question condition
    pthread_cond_destroy(questionCondition);
    free(questionCondition);


    for (i=0; i < (MAXCONNECTIONS/CONNECTIONSPERTHREAD); i++) {
		// dispose the queue for this thread        
        destroyQueue( ServerThreads[i].messageQueue );
    
//        if (serverStatus == SERVERRUNNING ) 
        {
            
            // destroy thread's notifier
            pthread_cond_destroy(ServerThreads[i].threadNotifier);
			// lock the thread mutex to gain exclusive access
            pthread_mutex_lock(ServerThreads[i].threadMutex);
			// unlock it
            pthread_mutex_unlock(ServerThreads[i].threadMutex);
			// free it, then destroy it
            pthread_mutex_destroy (ServerThreads[i].threadMutex);
            free(ServerThreads[i].threadNotifier);
			// wow, this is kinda like the ronco food dehydrator...set it
			// and forget it! Is that copyright infringement?
            free(ServerThreads[i].threadMutex);

            
        }


           
    }
    
   
    // go ahead and lock the mutex
    //if (serverStatus == SERVERRUNNING ) 
    {
        //pthread_mutex_lock(threadHandler);
		// lock the main mutex, to gain exclusive access
		// set it, then forget it
        pthread_mutex_lock(threadHandler);
        pthread_mutex_unlock(threadHandler);
		// err, I mean, destroy it,then free it
        pthread_mutex_destroy (threadHandler);
        free(threadHandler);
    }
    threadHandler=NULL;
    int sockdie = SO_KEEPALIVE;

    
    close(sockfd);
}


/************************************************************************************
*
*	initializeThreadDescriptors
*
*	Initializes each thread connection's file descriptor list
*      
*	Arguments:
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void initializeThreadDescriptors()
{
    
    int i=0,j=0;
	// lock the thread mutex
    pthread_mutex_lock(threadHandler);
	
    for (i=0; i < (MAXCONNECTIONS/CONNECTIONSPERTHREAD); i++) {
		// traverse each connection
        for (; j < CONNECTIONSPERTHREAD; j++) {
			// set the file descriptor to zero
            ServerThreads[i].threadFD[j]=0x0000;
            ServerThreads[i].threadIP[j]=0x0000;
            
			// set the local buffer to NULL
            ServerThreads[i].BUFFER[j] = NULL;
            // set the test and diagnostic data counts to zero
            ServerThreads[i].testCount[j]=0;
            ServerThreads[i].diagCount[j]=0;
        }
		// initialize the thread statuses to not started
        ServerThreads[i].threadStatus=NOTSTARTED;
		// create queue
        ServerThreads[i].messageQueue = createAndInitializeQueue(MAXCONNECTIONS,1);
               
    }
	// unlock the mutex
    pthread_mutex_unlock(threadHandler);
	// set the server status to SERVERUNNING
    serverStatus=SERVERRUNNING;
}




/************************************************************************************
*
*	removeThreadListener
*
*	Removes a thread listener by setting a connection's file descriptor to zero
*	and signaling that thread
*      
*	Arguments:
*		uint ThreadNumber - local thread number to search
*		ulong in_addr - local file descriptor ( for now )
*	Return Value:
*
*		void
*
*************************************************************************************/
void  removeThreadListener(uint threadNumber,ulong in_addr)
{
    int i=0;
	// if the thread number is beyond the number of possible connections
	// exit
    if (threadNumber >= (MAXCONNECTIONS/CONNECTIONSPERTHREAD)) {
        return;
    }

    for (i=0; i < CONNECTIONSPERTHREAD; i++) {

        if (ServerThreads[threadNumber].threadIP[i]==in_addr) {
            
				// set the file descriptor to zero, and the test counts
				// to zero
                ServerThreads[threadNumber].threadFD[i]=0x0000;
                ServerThreads[threadNumber].threadIP[i]=0x0000;
                ServerThreads[threadNumber].testCount[i]=0;
                ServerThreads[threadNumber].diagCount[i]=0;
                ServerThreads[threadNumber].connectionStatus[i]=0x0000;
                break;

        }
    }
	// if we are at the final connection, that means that 
    if (i==CONNECTIONSPERTHREAD) {
        return;
    }
    
   	// notify the thread
    pthread_cond_signal ( ServerThreads[threadNumber].threadNotifier );

}

/************************************************************************************
*
*	pushThreadCommand
*
*	Pushes the command onto the local buffer, or if a full command has been served,
*	the command is appended to the queue
*      
*	Arguments:
*		uint type - type of command
*		uint threadNumber - number of local thread
*		uint fd - file descriptor
*		ulong in_addr  - file descriptor
*		char *command - command to push/append onto queue
*	Return Value:
*
*		TRUE/FALSE/ERROR - status of pushing the thread onto user's queue
*
*************************************************************************************/
short pushThreadCommand(short type,uint threadNumber,uint fd, ulong in_addr,char *command)
{
    // if the command we are trying to push is null
	// return an error
    if (command==NULL) {
        return ERROR;
    }

	// if the string length of our command is zero, this means
	// that our command is essentially void, and thus, we should return an error
    if (strlen(command)==0) {
        return ERROR;
    }
    int i=0;
    if (threadNumber >= (MAXCONNECTIONS/CONNECTIONSPERTHREAD)) {
        return ERROR;
    }
    
    /* at this point, we check to see if our in_addr, which for the time
	 * being is the file descriptor, is within our threadNumber. If it is
	 * this means that the thread connection is active
	 */
    for (i=0; i < CONNECTIONSPERTHREAD; i++) {
        if (ServerThreads[threadNumber].threadIP[i]==in_addr) {
                break;
        }
    }

    
	// if i equals CONNECTIONSPERTHREAD, this means that 
	// we were unable to locate in_addr within the file descriptor list
	// as such, return an error
    if (i>=CONNECTIONSPERTHREAD) {
        return ERROR;
    }

    

	// create buffer and command lengths
    int buflen=0,commandlen=0;
	
	if (ServerThreads[threadNumber].BUFFER[i] != NULL)
	{
		// obtain the length of the current Buffer, if the buffer
		// is not NULL
		buflen = strlen( ServerThreads[threadNumber].BUFFER[i] );
    
		/* at this point, a hardcoded arbitrary length is set
		 * for the buffer. This should be external, but of course,
		 * we should also never reach 2048 characters. This would
		 * normall indicate an error
		 */
		if (buflen >= 2048) {
			// if we do exceed the buffer maximum, free the memory
			free(ServerThreads[threadNumber].BUFFER[i]);

            // ...set the buffer to NULL
			ServerThreads[threadNumber].BUFFER[i]=NULL;

			// ...finally, return error
			return ERROR;
		}
	}

	commandlen = strlen(command); // obtain our command's length
    

    if (commandlen==0) return ERROR; // return error if the length is zero

	// okay, here comes the meat of what we need to do
	// we allocate enough memory for our current buffer
	// and the input command

	int size = buflen+commandlen; // calculate the size
      
	// use realloc to allocate our memory. 
    ServerThreads[threadNumber].BUFFER[i] = (char*)realloc(ServerThreads[threadNumber].BUFFER[i],(size+1)*sizeof(char));
    
	if (ServerThreads[threadNumber].BUFFER[i]==NULL) {
			exit(1); // die disgracefully, as there is really no other alternative
	}

	if (buflen == 0) // there is really no reason to do this now
		memset(ServerThreads[threadNumber].BUFFER[i],0x00,commandlen);
    
    buflen=size;
 	
    // copy the command into our buffer
    strncat(ServerThreads[threadNumber].BUFFER[i],command,commandlen);

    // do the final check if, and only if, our buffer length is greater
	// than or equal to three
    if (buflen >= 3) {


		/* this is a terrible hack. Instead of actually checkign the size against the input size, three
		 * characters are appended to the command: three 0x0a. This should probably be done differently.
		 * However, checking the size can be problematic as well. In the case of log files, the number of
		 * bytes sent may be different than what the size indicates. This method works, and is commonly
		 * used in programs such as MySQL and Oracle, for socket transmissions. If it's good enough for them,
		 * surely, it's good enough for me....or is it? hmmmm........
		 */
        if ( ServerThreads[threadNumber].BUFFER[i][buflen-1] == 0x0a  && ServerThreads[threadNumber].BUFFER[i][buflen-2] == 0x0a &&
             ServerThreads[threadNumber].BUFFER[i][buflen-3] == 0x0a )
        {

                
                int j=0;
                // if the three 0x0a characters are found, set them all to NULL  
                for (j = buflen-3; j < buflen+1;j++) {
                    ServerThreads[threadNumber].BUFFER[i][j] = 0x00;
                }
                    {
                       
                    
    				// lock the dreaded mutex, so we have exclusive access to this thread's
    				// queue
                    
                    pthread_mutex_lock(threadHandler);               
    				// add the buffer to the queue, minus the two last characters ( we only need one null
    				// Terminator (TM) 
                    addElementToQueue(ServerThreads[threadNumber].BUFFER[i],buflen-2,i,type,threadNumber,fd,ServerThreads[threadNumber].messageQueue);
    				// release the mutex, and thus release the queue
                    pthread_mutex_unlock(threadHandler);
                    
    				// signal to the thread that its queue is not empty...but this, of course,
    				// does not imply the thread was empty alrady
                    
                    pthread_cond_signal ( ServerThreads[threadNumber].threadNotifier );
    				// free the buffer, as it really is a temporary buffer between the socket
    				// and the queue
                    free(ServerThreads[threadNumber].BUFFER[i]);
    				// set the buffer equal to NULL
                    ServerThreads[threadNumber].BUFFER[i]=NULL;
                    }
                // why is this here? don't know really. I did it for debugging
                // this should indicate that our process of grabbing the command
                // is complete.
                i=0xfffe;
        }
        
        
    }
    
        
    return i;
}


/************************************************************************************
*
*	getThreadForFileDescriptor
*
*	Attempts to locate the thread number associated with the provided file descriptor
*	In addition, if a mac address is present in the thread connection that is checked
*	as well.
*      
*	Arguments:
*		uint in_addr - file descriptor
*		char *mac - MAC address associated with this thread connection
*
*	Return Value:
*
*		uint - thrad number
*
*************************************************************************************/
uint getThreadForFileDescriptor(ulong in_addr,uint fd, char *mac)
{
	// if openThreads is currently negative one, set it to zero
    
    if (openThreads==-1) {
        openThreads=0;
    }
    int i,j;
    if ( threadHandler == NULL) return ERROR;
	// lock the main thread mutex to prevent any external access
    
    //pthread_mutex_lock(threadHandler);
    
	/* this is ugly. Here's an overview: we go through each open thread
	 * and check each connection. If the connection's file descriptor matches
	 * in_addr, we check to see if the thread's macAddress is NULL. if it isn't
	 * we then compare the mac input to the mac address in thread's connection
	 * if they aren't equal, continue to the next connection
	 */
    for (i=0; i <= openThreads && i < (MAXCONNECTIONS/CONNECTIONSPERTHREAD); i++) {
        for (j=0; j < CONNECTIONSPERTHREAD; j++) {
            if (ServerThreads[i].threadIP[j]==in_addr  && in_addr!=0) {
                if (mac != NULL)
                {
                    if (strlen(ServerThreads[i].macAddress[j]) > 0) {
                        if ( strcmp(ServerThreads[i].macAddress[j],mac) ) {
                            continue;
                        }
                    }
                }
				// unlock the mutex
               // pthread_mutex_unlock(threadHandler);
				// if macAddress[j] is empty, we either copy mac into
				// it, or set it all to 0x00
                if (strlen(ServerThreads[i].macAddress[j]) == 0  ) {
                    if (mac != NULL) {
                        strncpy(ServerThreads[i].macAddress[j],mac,18);
                    }
                    else
                        memset(ServerThreads[i].macAddress[j],0x00,18);
                }
                
                return i;
            }
            else if (ServerThreads[i].threadFD[j]==fd  && ServerThreads[i].threadFD[j] != 0)
            {
             //  pthread_mutex_unlock(threadHandler);
                
                return i;
            }
            
            
            
			// okay, this is the scenario if the file descriptor for
			// this connection is zero. This should only occur on an uninitialized
			// connection
            if (ServerThreads[i].threadIP[j]==0x0000) {

                // go ahead and take it. assign in_addr to the file descrptor
                
                ServerThreads[i].threadIP[j] = in_addr;
                ServerThreads[i].threadFD[j] = fd;
				// if the input mac is not NULL, copy it into 
				// the connection's macAddress
                if (mac != NULL) {
                    strncpy(ServerThreads[i].macAddress[j],mac,18);
                }
                else
                    memset(ServerThreads[i].macAddress[j],0x00,18);
                
				// allocate  memory for the thread number. this will be
				// freed when the thread created by this function exits
                int *p = malloc(1*sizeof(int));
                *p=i;
				// create the thread 
                if (j > 0)
                {
                    return i;
                }
                if (pthread_create(&(ServerThreads[i].myThread),NULL,threadCommandHandler,p))
                {
                    
                    exit(1);
                }
                uint x=0;
				/*
				 * wait until that thread is started. As stated elsewhere, the granularity
				 * of thread creation is not clearly defined, nor is it guaranteed. Thus
				 * we must have a marker in the thread's status to help us determine when
				 * the thread has started
				 */
                
                while((ServerThreads[i].threadStatus&STARTED) !=STARTED);
                
				// unlock the mutex
               // pthread_mutex_unlock(threadHandler);
				                             
                // return i, which at this point is the thread number
                return i;
            }
        }
        
    }
    pthread_mutex_unlock(threadHandler);
    return ERROR; // eww, this shouldn't happen
}


/************************************************************************************
*
*	isThreadDescriptor
*
*	Returns TRUE/FALSE, depending on whether or not the provided file descriptor is
*	part of thread specified by threadNumber
*      
*	Arguments:
*		int	threadNumber - thread number
*		ulong in_addr - file descriptor
*
*	Return Value:
*
*		TRUE/FALSE
*
*************************************************************************************/
short isThreadDescriptor(int threadNumber, ulong in_addr)
{
	// if the thread number exceeds the number of possible
	// threads, return false, which indicates that in_addr
	// is not a file descriptor
    if (threadNumber >=(MAXCONNECTIONS/CONNECTIONSPERTHREAD)) {
        return FALSE;
    }
    int j=0;
	// lock the main thread
    pthread_mutex_lock(threadHandler);
    for (; j < CONNECTIONSPERTHREAD; j++) {
		// hey, we've found in_addr in the file descriptor, located
		// at connection j on thread number 'threadNumber'
        if (ServerThreads[threadNumber].threadIP[j]==in_addr)
        {
			// unlock the mutex and return true
            pthread_mutex_unlock(threadHandler);
            return TRUE;
        }
    }
	// unlock the mutex and return false, as
	// the file descriptor was not found
    pthread_mutex_unlock(threadHandler);
    return FALSE;
}


/************************************************************************************
*
*	threadCommandHandler
*
*	Function which handles execution of local thread data. Analyzes command data
*	to handle any responses
*      
*	Arguments:
*		void *threadNumber - input thread number
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void *threadCommandHandler(void *threadNumber)
{

    // begin handling this bugger
    int i=0; 
    // dereference threadNumber and assign the value to thread
    int thread = *((int*)threadNumber);

    // tell whoever is listening that this thread has STARTED
    ServerThreads[thread].threadStatus|=STARTED;     

    struct userdata
    {
        char *MACAddress;
        int accessLevel;
    }localUserData;

	// allocate memory for MACAddress ( this is here for posterity's sake)
    localUserData.MACAddress=(char*)malloc(sizeof(char));
    localUserData.accessLevel=-1;

    do
    {
        
        
        short isEmpty=TRUE;
        
        // check each connection for the thread to ensure
        // it is valid
        for (i=0; i < CONNECTIONSPERTHREAD; i++) {
            if (ServerThreads[thread].threadIP[i]!=0x0000) {
                isEmpty=FALSE;
                break;
            }
        }
        // if neither connection is valid, exit
        if (isEmpty==TRUE) {

            break;
        }                

        /* wait on condition signal, which will ignite this all powerful
         * thread. A little background on what will signal this thread: 
		 * The queue has been altered, and thus we need to check the queue
		 * to determine if any commands need to be handled. Or, a question
		 * has been placed in the question buffer. This must be handled until
		 * the question has been answered
		 */
        
        pthread_mutex_lock(ServerThreads[thread].threadMutex);
        pthread_cond_wait (ServerThreads[thread].threadNotifier, ServerThreads[thread].threadMutex);

        pthread_mutex_unlock(ServerThreads[thread].threadMutex);
        
       

        // check each connection for the thread to ensure
        // it is valid
        for (i=0; i < CONNECTIONSPERTHREAD; i++) {
            if (ServerThreads[thread].threadIP[i]!=0x0000) {
                
                isEmpty=FALSE;
                break;
            }
        }
        // if neither connection is valid, exit
        if (isEmpty==TRUE) {
            
            break;
        }

          

        /*
         * lock the main thread mutex while
         * we attempt to retrieve the front 
         * queue element
         */


        
        pthread_mutex_lock(threadHandler);
        
        command * cmd = (command*)peekAtFrontElement(ServerThreads[thread].messageQueue);
        
        pthread_mutex_unlock(threadHandler);
        
        // automatically assume there is a question
        short skipToQuestion=TRUE;
        // traverse through commands while they exist
        while (cmd != NULL) 
        {
              
            
            if (!question || (getCommandDecision(cmd->data) == TBS_TCP_GET_QUESTION_RESPONSE && question))
                {
                
                    skipToQuestion=FALSE;               
                }
                else
                {
                    
                    skipToQuestion=TRUE;
                    
                }

                
            
                
            
            {
            
                // move through each CONNECTION that belongs to this thread
                
                for (i =0; i < CONNECTIONSPERTHREAD; i++) {
                    // ensure the command is not null

                    
                    if (i==cmd->connection && thread==cmd->threadNumber && cmd->data!= NULL) 
                    {

                        if (ServerThreads[thread].threadIP[i]==0x0000) {
                        
                            getFrontElement(ServerThreads[thread].messageQueue);
                            continue;
                        }
                        
                        // if we are destined to skip to the question
                        if (skipToQuestion==TRUE)
                        {
                            

                            
                            if ((ServerThreads[thread].connectionStatus[i]&QUESTIONPAUSE) != QUESTIONPAUSE)
                            {
                                if (repairLock != 0)
                                {
                                    
                                    if (repairLock != ServerThreads[thread].threadIP[i])
                                    {
                                        if (repairCounter == -1)
                                        {
                                            repairCounter = 0;
                                            repairTimer(2);
                                        }
                                        
                                        skipToQuestion=FALSE;
                                    }
                                    else
                                    {
                                        pthread_mutex_lock(threadHandler);
                                        getFrontElement(ServerThreads[thread].messageQueue);
                                        pthread_mutex_unlock(threadHandler);
                                        break;
                                    }
                                    
                                }
                                else
                                {
                                
                                    pthread_mutex_lock(threadHandler);
                                    getFrontElement(ServerThreads[thread].messageQueue);
                                    pthread_mutex_unlock(threadHandler);
                                    break;
                                }
                                
                            }
                            else
                            {

                                
                                skipToQuestion=FALSE;
                                
                            }
                            
                        }

                        
                        // dequeue element if it is valid
                        
                        pthread_mutex_lock(threadHandler);
                        
                        getFrontElement(ServerThreads[thread].messageQueue);
                        pthread_mutex_unlock(threadHandler);
                        
                        /*
                         * handle queue command by checking it against
                         * a list of known commands, then executing the contents
                         * of the command
                         */
                        
                        handle_thread_event(localUserData.MACAddress,
                                            localUserData.accessLevel,
                                            i,
                                            thread,cmd->type,
                                            *cmd->fd ,
                                            NULL,cmd->data);
                        break;
                    }
                    
                }
            }

            if (question && skipToQuestion)
            {
                
                break;
            }

            
            /*
             * re-lock main thread mutex to obtain any additional
             * commands, if and, and only if, they exist
             */
            
            pthread_mutex_lock(threadHandler);
            
            cmd = (command*)peekAtFrontElement(ServerThreads[thread].messageQueue);
            pthread_mutex_unlock(threadHandler);
            
            
        }
        
        /*
         * Since we aren't editing question right here, 
         * there should be no reason to lock it. 
         */
        if (question != NULL && skipToQuestion) {

            userAnswerQuestion(thread);
        }
        
        
        
        


    }while(1);
    // empty queue, and free memory within it
    //makeEmpty(ServerThreads[thread].messageQueue);

    free(localUserData.MACAddress);
    // free threadNumber?
    free((int*)threadNumber);

    // tell whoever is reading threadExit that this thread
    // and its associated connections are exiting
    pthread_mutex_lock(threadHandler);
    ServerThreads[thread].threadStatus=NOTSTARTED;     
    pthread_mutex_unlock(threadHandler);
    // finally, return NULL
    return NULL;

}

/************************************************************************************
*
*	getFileDescriptorByMac
*
*	Attempts to return a file descriptor based on the provided mac address
*      
*	Arguments:
*		char *mac
*
*	Return Value:
*
*		int - file descriptor, if found. Error if not found
*
*************************************************************************************/
int getFileDescriptorByMac(char *mac)
{
	// first check mac to see if it's a valid string
	if (mac==NULL)
		return ERROR;
	if (strlen(mac) == 0)
		return ERROR;
	// search through all thread connections and locate the
	// input mac
    int i,j;
    for (i=0; i <= openThreads && i < (MAXCONNECTIONS/CONNECTIONSPERTHREAD); i++) {
        for (j=0; j < CONNECTIONSPERTHREAD; j++) {
			// compare mac to what we have in the thread structure
			// If it's found, return the file descriptor
            if (!strcmp(ServerThreads[i].macAddress[j],mac)) {
                
                return ServerThreads[i].threadFD[j];
            }
        }
    }
	// if we're here, the file descriptor wasn't found
    return ERROR;
}

/************************************************************************************
*
*	getIpForThread
*
*	Returns the IP address associated with the input thread and file descriptor
*      
*	Arguments:
*       uint thread - Thread to search
*       uint fd - File descriptor
*
*	Return Value:
*
*		unsigned long - Long representation of the found IP address
*
*************************************************************************************/
unsigned long getIpForThread(uint thread, uint fd)
{
    int j=0;
    for (j=0; j < CONNECTIONSPERTHREAD; j++) {
        // compare mac to what we have in the thread structure
        // If it's found, return the file descriptor
        
        if (ServerThreads[thread].threadFD[j]==fd )
        {
            
            return ServerThreads[thread].threadIP[j];
        }
        
    }
    return 0;
}

/************************************************************************************
*
*	removeQuestionHold
*
*	Removes any question holds/pauses which may be in this user's connectionStatus
*      
*	Arguments:
*       uint thread - Thread to search
*       uint fd - File descriptor
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void removeQuestionHold(uint thread, uint fd)
{
    int j=0;
    for (j=0; j < CONNECTIONSPERTHREAD; j++) {
        // compare mac to what we have in the thread structure
        // If it's found, return the file descriptor
        
        if (ServerThreads[thread].threadFD[j]==fd )
        {
            
            ServerThreads[thread].connectionStatus[j]&=(QUESTIONPAUSE^0xffff);
        }
        
    }
}

/************************************************************************************
*
*	getMacForThread
*
*	Returns a pointer to the connection's mac address
*      
*	Arguments:
*       uint thread - Thread to search
*       uint fd - File descriptor
*
*	Return Value:
*
*		char* - Pointer to mac address string
*
*************************************************************************************/
char *getMacForThread(uint thread, uint fd)
{
    int j=0;
    for (j=0; j < CONNECTIONSPERTHREAD; j++) {
        // compare mac to what we have in the thread structure
        // If it's found, return the file descriptor
        
        if (ServerThreads[thread].threadFD[j]==fd )
        {
            return ServerThreads[thread].macAddress[j];
        }
        
    }
    return NULL;
}
