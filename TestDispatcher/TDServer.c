///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDServer.c
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Definitions for the Test Dispatcher Server, which runs as its own thread. 
 *              createServer(...) is used in pthread_create. It is kept in its own thread
 *              because its calls for the multiplexing select(...) call are blocking. This
 *              enables us to listen to the TCP and broadcast ports in non-polling loops.
 *              
 *              Once data has been received, it is pushed onto a queue, on associated file
 *              descriptor's queue. Please see TDServerThreads for more information about
 *              the reasoning, and design of the individual threads and queues.
 *
 *              In addition to coordinating the server, TDServer.c is responsible for handling
 *              the directives and commands from clients, in handle_thread_event(...)
 *              
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn     void sigchld_handler(int ss)
 */
///////////////////////////////////////////////////////////////////////////////
void sigchld_handler(int ss)
{
    if (ss!=17) {
        printf("received %i",ss);
        exit(0);
    }
    
}

/**********************************************************
*	sendall
*		Takes a long array needing to be sent over a socket 
*		connection and sends the entire array. 
*		BACKGROUND:	the send function will not always send the
*		entire array if it is very long. This function keeps 
*		calling send until the entire array has been sent. 
*
*	Arguments:
*		int s  - the socket number
*		char buf -  the array to be sent
*		int len -  the length of the array to be sent
*
*	Returns:
*		int 0 or -1 , success or fail
************************************************************/
int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;
    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here
  
    return n==-1?-1:0; // return -1 on failure, 0 on success
} 


/**********************************************************
*	getUnusedPort
*		Attempts to find the lowest available port at
*		or beyond startPort
*
*	Arguments:
*		int startPort - port at which to begin
*
*	Returns:
*		int  - smallest unused port at or beyond startPort
************************************************************/
int getUnusedPort(int startPort)
{
	int port = startPort; //start val
	int sock; 
	struct sockaddr_in tempaddr; 

	//create a temp UDP socket
	tempaddr.sin_family = AF_INET;         // host byte order

    tempaddr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(tempaddr.sin_zero), '\0', 8); // zero the rest of the struct


	if((sock = socket(AF_INET, SOCK_DGRAM,0)) == -1)
		return -1; //error
	
	//call bind until an open port is found
	do{
		tempaddr.sin_port = htons(port); // set the port for the socket

		if (bind(sock, (struct sockaddr *)&tempaddr,sizeof(struct sockaddr)) == -1) {
			if(errno == EADDRINUSE) //port is in use
			{
				port++; //try next port
			}
		}
		else //successfully bound
			break; 
	}while(port < 65000);
	
	//close the socket to free up the port
	close(sock);
	//return the port number 
	return port;
}

/************************************************************************************
*
*	createServer
*		Creates socket server
*      
*      
*	Arguments:
*
*       void *input -- input for the server
*
*	Return Value:
*
*		void * - can be cast to return any data type
*
*************************************************************************************/

void *createServer(void *input)
{
    // set the status now!
    isBurnout=FALSE;

    repairCounter = -1;
    handleDefaultMessage=NOTLOCKED; // start server
	// initialize sub threads and allocate memory for them    
    initializeServerThreads();
    repairLock=0;

    // initialize thread descriptors
    initializeThreadDescriptors();
    
    
    memset(streamPort,'0',6);
    
    
    struct sockaddr_in their_addr={}, bc_addr={}; // connector's address information
    
	struct in_addr myinaddr={};
    int sin_size=0, recv_len=0, addr_len=0, fdmax=0, returnStatus=0;
	int numbytes= -1;
    struct sigaction sa;
	// all purpose buffer
    char bcBuffer[MAXBUFLEN]="";
    int yes=1,j=0,i=0;	
	
	

	
	

	
	// basic socket initialization
    myBC_addr.sin_family = AF_INET;        
    myBC_addr.sin_port = htons(BROADCAST_PORT);   
    myBC_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(myBC_addr.sin_zero), '\0', 8);


	//create local socket for broadcast response
	if((bcSockFd = socket(AF_INET, SOCK_DGRAM,0)) == -1){
        
		perror("broadcast socket");
		return NULL;
	}
	if (setsockopt(bcSockFd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return NULL;
    }

    
	//bind to broadcasting port
	if (bind(bcSockFd, (struct sockaddr *)&myBC_addr,sizeof(struct sockaddr)) == -1) {
        perror("BC bind");
        return NULL;
    }



	// create local socket for stream talking...yes, streams talk
	// they do, I promise!
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return NULL;
        
    }
	
	// Set non-blocking
	long arg=0;
	if( (arg = fcntl(sockfd, F_GETFL, NULL)) < 0)
		exit(0);

//	arg |= O_NONBLOCK;
	if( fcntl(sockfd, F_SETFL, arg) < 0)
		exit(1);

	if( (arg = fcntl(bcSockFd , F_GETFL, NULL)) < 0)
		exit(1);
	

	arg |= O_NONBLOCK;
	
	if( fcntl(bcSockFd, F_SETFL, arg) < 0)
		exit(1);

    
	// get first unused port at or beyond TCP_PORT
	int streamPortNumber = getUnusedPort(TCP_PORT); 

	sprintf(streamPort, "%i", streamPortNumber); //put this value into the global variable
    
	prefixExpand(streamPort,5); //make the array 5 places. ie. 00345

    int exit=0;
	//stream file setup
    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(streamPortNumber);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    /* Using the SO_KEEPALIVE flag will cause problems if you try to end and restart
     * the server. Generally, though, this will not happen unless testing Test Dispatcher.
     * In that case, simply remove it. In the course of normal use, this will not be a
     * problem as the only restart would occur when the system restarts, alleviating
     * any binding errors
     */

    
    if (setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return NULL;
    }
    
    

    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return NULL;
    }
    
	//bind socket to listen to that one port
    int tCount=0;

    /* The following procedure is only needed during testing, as explained above.
     * It will attempt to bind a socket over the course of 30 seconds. If it 
     * cannot bind the socket within that time frame, the program will exit
     */
    for (; tCount < 60; tCount++)
    {
        if ( (tCount%10) == 0 && tCount > 0)
        {
            consolePrint("still attempting to bind socket...\n");
        }
        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1) {
            if (tCount==59)
            {
                perror("bind");
                return NULL;
            }
            usleep(500000); // sleep for 500k microseconds ( 10^-6 )
            
        }
        else
            break;
    
    }


	//listen for connection requests
    if (listen(sockfd, MAXCONNECTIONS) == -1) {
        consolePrint("too many connections?\n");
        return NULL;
    }


	// reap all dead processes. 

    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        return NULL;
    }

	// add sockets to set for select statement to use
	FD_ZERO(&fileDescriptorList); //clear FD sets
    FD_ZERO(&exceptfds);
	FD_SET(bcSockFd, &fileDescriptorList);  //add both open sockets to the set
	FD_SET(sockfd, &fileDescriptorList);

	//the socket is basically an INT, and the select function needs to know
	//how high to count to when it is checking for sockets to read
	fdmax = bcSockFd;

	if(sockfd > fdmax) fdmax = sockfd;  //make highest fd the "max" 

	// set the status of the server. At this point, we can consider ourselves
	// running.......I was Running.....
    serverStatus=SERVERRUNNING;
    int foundCount=0;
    
    do
	{

    

		// this will probably never cause the server to exit
		if ( serverStatus==SERVERSTOPPED )
                    break;

        readfds = fileDescriptorList; //copy the set each go-round because the function changes what's in the set.
		
		// select is a software MUX, thus the name select. select blocks until a socket reads
		// at this point, we can begin checking each socket to see if it is set, then act accordingly
        
		returnStatus = select(fdmax+1, &readfds, NULL,NULL,NULL); //check only for sockets ready to read
        
		if ( serverStatus == SERVERSTOPPING)
                    break;

		// uh oh, a bad error ....me scared
        if(returnStatus ==-1)
			return NULL;

        if (returnStatus==0)
        {
            
            return NULL;
        }

		// check all file descriptors
        
            
        for(i=0,foundCount=0;i<= fdmax, foundCount< returnStatus;i++)  
		{
            // if i is set in the read file descriptor list
			// check more
            
            if (FD_ISSET(i, &readfds)) //if i is the number of a set descriptor, see if it is one we're interested in
			{
                foundCount++;
                /*
                arg=0;
                    if( (arg = fcntl(i, F_GETFL, NULL)) < 0)
                        break;

                    arg |= O_NONBLOCK;
                    if( fcntl(i, F_SETFL, arg) < 0)
                        break;
                    */
                
				if(i==sockfd) //we've reached the stream socket
				{
                
                    int new_fd=0;
                    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
						continue;  //screwed up this time, give up and wait until next connect attempt
					}

                    
                    

                  //FD_CLR(sockfd,&readfds);
                  //FD_CLR(sockfd,&fileDescriptorList);
                    FD_SET(new_fd,&fileDescriptorList);
/*                    
                    

                    

                    readfds = fileDescriptorList;
                    */

                    if (new_fd > fdmax)
                    {
                        fdmax = new_fd;
                    }

                    addr_len = sizeof(struct sockaddr);

					// get the ip address of the user attempting to connect	
                    ulong ip = inet_lnaof( their_addr.sin_addr );
					
					// lookup the mac address in our fancy lookup table
                    char  *mac =getMacLookup(ip);

                    //consolePrint("ip is %ul %lu is is %i\n",ip,ip,i);
					// attempt to get a new (or possibly old ) thread
					// for the file descriptor mac combinatoin
                    int l = getThreadForFileDescriptor(ip,new_fd,mac);

					// if the thread is not -1, set openThreads equal
					// to l
                    if (l!=-1) {

                        if (l > openThreads) {
                            openThreads=l;
                        }

                                          
                    }
                    
                    
                    memset(bcBuffer,0x00,MAXBUFLEN);
					// attempt to receive data into bcBuffer
                    if(( recv_len = recv(new_fd, bcBuffer, sizeof(MAXBUFLEN-1), 0))==0)
                    {
                        // user has probably disconnected
                        if (recv_len == -1)
                        {
                            removeThreadListener(l,new_fd);
                        }
                        
                        
                        
                        if (recv_len==-1)
                        {
                        
                            close(i);
                            FD_CLR(i,&fileDescriptorList);
                            readfds = fileDescriptorList;
                        }
                        


                    }
                    else
                    {
                        // push command onto queue
                        int j= pushThreadCommand(TCP,l,new_fd,ip,bcBuffer);
                    //    FD_CLR(i,&readfds);
                    }
                    //FD_CLR(new_fd,&fileDescriptorList);
                   

                }
                else if (i==bcSockFd) { // the broadcast socket tripped select
						
                        addr_len = sizeof(struct sockaddr);
                        memset(bcBuffer,0x00,MAXBUFLEN);
    					numbytes = recvfrom(bcSockFd, bcBuffer, MAXBUFLEN-1, 0,(struct sockaddr *)&their_addr, &addr_len);
						// receive broadcast packet into bcBuffer, it the length
						// is zero, we skip the following code
                        if (strlen(bcBuffer)==0) {
                            continue;
                        }
						// the packet is a packet which tells the server
						// the user has disconnected
                        if (getCommandDecision(bcBuffer) == TBS_EXIT) {
                            
								// obtain the ip address and mac for the user
                                ulong ip = inet_lnaof( their_addr.sin_addr );
                                char  *mac =getMacLookup(ip);
                                if (mac != NULL) 
                                {
									// get the file descriptor for the user
                                    
                                    int fd = getFileDescriptorByMac(mac);
                                    if (fd >= 0) {
										// now, obtain the thread for the user
                                        int l = getThreadForFileDescriptor(ip,fd,mac);
										// with the thread number and file descriptor
										// destroy the listening thread for this
										// connection
                                        // the only time we'll completely destroy the thread
                                        // listener is when the user closes the client
                                        removeThreadListener(l,ip);

										// close out the socket
                                          close(fd);
                                          FD_CLR(fd,&fileDescriptorList);
                                          readfds = fileDescriptorList;
                                    }
                                }
                                
                        }
                        else // if not an exit packet, handle the packet elsewhere
                            handle_thread_event(NULL,0,0,0,BROADCAST,bcSockFd,&their_addr,bcBuffer);
                    
                }
                else
                {
                   

                    // set blank IP address so we can, hopefully,
                    // populate it later
                    ulong ip = 0x0000;
                    
                    // set NULL MAC address. We'll set it later,
                    // hopefully
                    char  *mac =NULL;
                    
					// obtain thread from file descriptor and mac
                    
                    int l = getThreadForFileDescriptor(ip,i,mac);
                        
                    
                    ip = getIpForThread(l,i);
                    
                    
                    mac = getMacForThread(l,i);
                    
                    //consolePrint("ip is %ul %lu 2 i is %i\n",ip,ip,i);
                    
					// at this point, the thread should not be -1
					// if it is, continue
                    if (l==-1)
                        continue;
                    
                    
                      
                   memset(bcBuffer,0x00,MAXBUFLEN);

					// receive packet
                    
                    int recv_len = recv(i, bcBuffer, sizeof(MAXBUFLEN-1), 0);
					if(recv_len <= 0)
                    {

                        /*
                         * we may not want to perform this action when -1
                         * is received, though for now, we simply close
                         * the file descriptor and release the question queue
                         * Negative one usually indicates a problem in the client
                         * causing the user to disconnect
                         */

                        ulong ip = 0x0000;

                        // obtain mac from ip address through lookup
                        char  *mac =NULL;

                        // obtain thread from file descriptor and mac

                        int l = getThreadForFileDescriptor(ip,i,mac);
                        ip = getIpForThread(l,i);

                        removeThreadListener(l,ip);   
                        if (recv_len==-1)
                        {
                            
                            //consolePrint("received a -1 at 504 for %i\n",i);
                            
                            close(i);
                            FD_CLR(i,&fileDescriptorList);
                            //readfds = fileDescriptorList;
                        }
                        
                        //consolePrint("close at 504 for %i\n",i);
                        if (recv_len==0)
                        {
                            removeQuestionHold(l,i);
                              close(i);
                              FD_CLR(i,&fileDescriptorList);
                            readfds = fileDescriptorList;
                        }

                        //removeThreadListener(l,i);
                    }
                    else
                    {
                    
                    
                        // push command onto queue


                        if (bcBuffer!=NULL)
                        {
                            int j= pushThreadCommand(TCP,l,i,ip,bcBuffer);

							// this occurs when all data has been read and
							// a full command has been received
                            
                            if ((j&0xfffe)) {

                                
                                
                             }

                        }
                     }
                        // send signal to thread that we have something for it
                        // it may not like it...but we have something for it
                      
                  
                              
                 
                }
                 
            }
        }

    }while(1);
    
    return NULL;

}

/************************************************************************************
*
*	handle_thread_event
*		Handles data from a full packet command
*      
*      
*	Arguments:
*
*       char *MAC - user's mac address
*		int accessLevel - user's access level
*		int connection - connection number in thread
*		int thread - thread number
*		short type - type
*		int fd - file descriptor
*		socketAddress *inaddr - socket structure
*		char *data
*
*	Return Value:
*
*		char *data
*
*************************************************************************************/
char *handle_thread_event(char *MAC,int accessLevel,int connection,int thread,short type, int fd,socketAddress *inaddr, char *data )
{
    
	// obtain integer representation of command
    
    uint COMMAND = getCommandDecision(data);
    
	// statically allocate memory for return buffer command
	char returnBuffer[1024]=""; // I hate statically defining arrays

	// zero array
    memset(returnBuffer,0x00,1024);

    struct sockaddr_in their_addr, bc_addr; // connector's address information
    struct in_addr myinaddr;
    unsigned short send=FALSE;
    int numSent=0;
    
    switch(type)
    {
		case BROADCAST: // we have received a broadcast packet
        {
            
            switch(COMMAND)
            {
                
                case  TBS_AUTHENTICATE_USER:
                    {

                        user_authenticate(&their_addr,&bc_addr,&myinaddr,thread,fd,data,returnBuffer);

                        send=TRUE;
                    }
                    break;
				case TBS_BROADCAST:
                {
					// initial broadcast from user. this is to find test dispatchers which may be running
					if (initial_broadcast(referenceBoardInfo,inaddr,&bc_addr,&myinaddr,fd,data,returnBuffer) == TRUE)
                        send=TRUE;
                   
                    }
                    break;
				default:
                {
					// handle default broadcast when this board is in the user's local list
					if ( default_broadcast(referenceBoardInfo,inaddr,&bc_addr,&myinaddr,fd,data,returnBuffer) == TRUE ) 
						send=TRUE;
	            }
                    break;
            }; // end switch command

            if (send==TRUE) { // if send == true

				// if sendto failed, print any error information which may be
				// helpful to a debugger
                if ((numSent = sendto(fd, &returnBuffer, strlen(returnBuffer), 0,(struct sockaddr *)&bc_addr, sizeof(struct sockaddr))) == -1) {
                        // perror("sendto");
                        
                    }
                

            }

        }
			break;
		case TCP: // TCP broadcast
        {
            char *returnBuff = NULL;
            int functionReturn =0;
            switch(COMMAND)
            {
          
            case TBS_TCP_GET_FILE: 
                {
					// handle command to return data of file
                    send_filesystem_data(&their_addr,&bc_addr,&myinaddr,fd,data,&returnBuffer);
                }
                break;
            case TBS_TCP_GET_QUESTION_RESPONSE:
                {
                    
					// use has responded to question
                    short response = userRespondToQuestion(data);
                    int closeIterator=0;
                    char buffer[25];
					// tell the user to close his/her messagebox
                    if (response == DEFER)
                    {
                        
                        ServerThreads[thread].connectionStatus[connection]|=QUESTIONPAUSE;
                    }
                    if (response != DEFER)
                    {
                        sprintf(buffer,"00005MSGBOXCLOSE");
                        
                        int len = strlen(buffer);
                        int j=0;
                        for (; closeIterator <= openThreads; closeIterator++ ) 
                        {
                            
                            for (j=0; j < CONNECTIONSPERTHREAD; j++) 
                            {
    							// if the file descriptor is not null, and the closing thread
    							// was not this one, tell it to close its message box
                                if (j != connection && ServerThreads[closeIterator].threadIP[j] != 0x0000) 
                                {
                                    
                                    if (sendall(ServerThreads[closeIterator].threadFD[j], buffer, &len) == -1) 
                                    {
                                        perror("sendall");
                                    }
                                    
                                        
                                }
                                ServerThreads[closeIterator].connectionStatus[j]&=(QUESTIONPAUSE^0xffff);
                            }
                            
                        }
                        
                    }
                    // 
                }
                break;
            
            case TBS_TCP_GET_TEST_DATA: // this reinitializes the test count
				// and diagnostic count to zero, so to resend all data to this
				// user
               //requestToClearData(&their_addr,&bc_addr,&myinaddr,'1',fd);
                ServerThreads[thread].testCount[connection]=0;
                ServerThreads[thread].diagCount[connection]=0;
               // send_test_clear(&their_addr,&bc_addr,&myinaddr,fd);
//                requestToClearData(&their_addr,&bc_addr,&myinaddr,'2',fd);

            case TBS_TCP_GET_TEST_UPDATE_DATA:
                {
                    
                    int checkTestData=0,checkDiaglogData=0;
					// send test data to user
                    
                    checkTestData=send_test_data(&their_addr,&bc_addr,&myinaddr,'1',&(ServerThreads[thread].testCount[connection]),TESTCOUNT,fd,data,testData, returnBuff);
					// send diagnostic data to user
                    checkDiaglogData=send_test_data(&their_addr,&bc_addr,&myinaddr,'2',&(ServerThreads[thread].diagCount[connection]),DIAGCOUNT,fd,data,diagnosticData,returnBuff);
					// if either is zero, set the user's status to TESTVIEW
                    if (checkTestData ==0 || checkDiaglogData == 0)
                        ServerThreads[thread].threadStatus|=TESTVIEW;
                   
                }
                break;
                /*
            case TBS_TCP_AUTHENTICATE_USER:
                {
                    
                }
                break;
                 */
			case TBS_TCP_GET_LAST_TEST_UPDATE_DATA:
				{
					int checkTestData=-1,checkDiaglogData=-1;
					
					if (ServerThreads[thread].testCount[connection] >=1 ) {
                    
						// if user has already viewed test data
						// invalidation user's test count
                        
                        if (ServerThreads[thread].testCount[connection] >= TESTCOUNT) {
                            ServerThreads[thread].testCount[connection]-=1;
                            checkTestData=send_test_data(&their_addr,&bc_addr,&myinaddr,'3',&(ServerThreads[thread].testCount[connection]),TESTCOUNT,fd,data,testData, returnBuff);
                        }
                        else
                        if (ServerThreads[thread].testCount[connection] >= TESTCOUNT-1) {

                            checkTestData=send_test_data(&their_addr,&bc_addr,&myinaddr,'1',&(ServerThreads[thread].testCount[connection]),TESTCOUNT,fd,data,testData, returnBuff);
                        }

                        
                        

                            
                        
						
					    
					}
					if (ServerThreads[thread].diagCount[connection]>=1)
					{
						// if user has already viewed test data
						// invalidation user's diagnostic count
						if (ServerThreads[thread].diagCount[connection] >= DIAGCOUNT) {
							ServerThreads[thread].diagCount[connection]-=1;
                            checkDiaglogData=send_test_data(&their_addr,&bc_addr,&myinaddr,'4',&(ServerThreads[thread].diagCount[connection]),DIAGCOUNT,fd,data,diagnosticData, returnBuff);
                        }
                        else
                        if (ServerThreads[thread].diagCount[connection] >= DIAGCOUNT-1) {
                            checkDiaglogData=send_test_data(&their_addr,&bc_addr,&myinaddr,'2',&(ServerThreads[thread].diagCount[connection]),DIAGCOUNT,fd,data,diagnosticData, returnBuff);
                        }
                        
						
						
					    
					}

					// if either is zero, set the user's status to TESTVIEW
                    if (checkTestData ==0 || checkDiaglogData == 0)
                        ServerThreads[thread].threadStatus|=TESTVIEW;
                   
                }
                break;
            case TBS_TCP_REPAIR_LOCK_BOARD:
                {
                    requestToLockBoard(ServerThreads[thread].threadIP[connection],data);
                }
            default:
                {
                   
                }
                break;
            };
        }
        break;
    }
   
   
    
    
    return NULL;

        
    
}



/************************************************************************************
*
*	loadServer
*		Copies pointer of board info into referenceBoardInfo
*      
*      
*	Arguments:
*
*       PTESTBOARDINFO board -- pointer to this board info
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void loadServer(PBOARDSTATE board)
{
    referenceBoardInfo=board;
}



/************************************************************************************
*
*	getMacLookup
*		Attempts to locate the ip address and return the associated mac address.
*		If the mac is not found, it returns NULL
*      
*      
*	Arguments:
*
*       long ip - ip address of user
*
*	Return Value:
*
*		char * - value of mac search
*
*************************************************************************************/
char *getMacLookup(long ip)
{
    short i=0;
    for(; i <  macLookup; i++)
    {
        if (localArp[i].ipAddress == ip)  { // ip found
            return localArp[i].macAddress; // return mac address
        }
    }
    return NULL;
}


/************************************************************************************
*
*	setMacLookup
*		Adds an ip address/mac combination into the mac address lookup table
*      
*      
*	Arguments:
*
*       long ip - ip address of user
*		char *mac - user's mac address
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void setMacLookup(long ip, char *mac)
{
	// if mac address is already found, no reason
	// to add it again
	if (getMacLookup(ip))
		return;

    // copy ip address into localArp lookup table
    localArp[macLookup].ipAddress=ip;
	// now, copy mac address into table
    strcpy(localArp[macLookup].macAddress,mac);
	// increment the number of mac addresses
    macLookup++;
}
