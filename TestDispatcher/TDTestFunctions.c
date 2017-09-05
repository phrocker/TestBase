///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDTestFunctions.c
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Main Test Functions for Test Dispatcher. In addition to functional
 *              and burnin test functions, Printout update functions and test queuing
 *              is performed here.
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
#include "TDTestFunctions.h"
#include "TDServerThreads.h"
#include "TDServer.h"
#include "../CommonLibrary/IPCFunctions.h"
#include "../CommonLibrary/Database/DBFunc.h"

    
/*! \file
    \brief Contains testing functions

   
*/    


/************************************************************************************
*
*	singleUpdate
*
*	Causes a single update
*      
*	Arguments:
*
*		short *dialog - diagnostic flag 
*		int *iIterator - thread iterator
*		int *jIterator - connection iterator
*		char *buffer - buffer for local connection
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void singleUpdate(short *dialog, int *iIterator, int *jIterator, char *buffer)
{
	// create single command	
	sprintf(buffer,"00000SINGLX%c%c%c",0x0a,0x0a,0x0a);
}


/************************************************************************************
*
*	singleNonUpdate
*
*	Causes a single update
*      
*	Arguments:
*
*		short *dialog - diagnostic flag 
*		int *iIterator - thread iterator
*		int *jIterator - connection iterator
*		char *buffer - buffer for local connection
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void singleNonUpdate(short *dialog, int *iIterator, int *jIterator, char *buffer)
{ 
	// default type is X, which is to update
   	char type = 'X';
	// check to see if we are working with the diagnostic data
   	if (*dialog==TRUE) 
	{
		// check the user's diagnostic count. if the user is at zero
		// we supply the user with all test data
    	if (ServerThreads[*iIterator].diagCount[*jIterator]== 0) {
           type='X';
        }
   }
   else
   {
	   // if we are working with the test data, we check that too
	   // to see if the user's test count is zero, if so, supply
	   // the user with all data within the test buffer
       if (ServerThreads[*iIterator].testCount[*jIterator]== 0) {
            type='X';
        }
   }
    // create Test Command
    sprintf(buffer,"00000TSTDA%c%c%c%c",type,0x0a,0x0a,0x0a);
}


/************************************************************************************
*
*	singleNonUpdate
*
*	Updates user queue
*      
*	Arguments:
*
*		short dialog - diagnostic flag 
*		void *fp - function pointer
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void updateUserQueues(short dialog,void *fp(void*,void*,void*,void*))
{
	
    int iIterator = 0,jIterator=0; // i and j iterators
	// a twenty five character buffer for who knows what
	// do you know? I sure don't!
    char buffer[25];

	// iterator through all open threads
    for (; iIterator <= openThreads; iIterator++ ) {

		// test to see if the current thread has already viewed their test
		// data. If not, continue
        if ( (ServerThreads[iIterator].threadStatus&TESTVIEW) != TESTVIEW ) {
            continue;
        }
		// next, iterator through all connections within each thread
        for(; jIterator< CONNECTIONSPERTHREAD; jIterator++)
        {
			// obtain the file descriptor for this connection
          	int fd = ServerThreads[iIterator].threadFD[jIterator];
			// if the file descriptor equals 0, then it has not been initialized
			// or is disabled
          	if (fd==0x0000) {
              	continue;
          	}
			
			// execute the function pointer to determine what should be done
			// when the user queue is updated
          	fp(&dialog,&iIterator,&jIterator,buffer);

			// lock the thread mutex, to prevent the queue from being updated
          	pthread_mutex_lock(threadHandler);               

			// add the buffer to the queue. The buffer should be populated through
			// the fp callback. Why I did it this way? I have no clue. I wanted
			// to see if I could still remember how to do callbacks/function pointers
          	addElementToQueue(buffer,strlen(buffer),jIterator,TCP,iIterator,fd,ServerThreads[iIterator].messageQueue);
         
			// unlock the mutex, to release this thread's queue
			pthread_mutex_unlock(threadHandler);
        }
    }
	
	// finally, iterate through each open thread, and if the thread status
	// indicates the thread is within TESTVIEW mode, signal the thread to do
	// its THANG ( ie, to check the queue )
	for (iIterator=0; iIterator <= openThreads; iIterator++ )
    	if ( (ServerThreads[iIterator].threadStatus&TESTVIEW) == TESTVIEW ) {
            pthread_cond_signal(ServerThreads[iIterator].threadNotifier);
    }
    

}

/************************************************************************************
*
*	updateDiagnosticTestDataQueue
*
*	Updates diagnostic data
*      
*	Arguments:
*
*		char *sstring - command with which to update the last diagnostic data string
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void updateDiagnosticTestDataQueue(char *sstring)
{
    if (isBurnout==TRUE)
    {
        return;
    }
	// lock the thread mutex
    pthread_mutex_lock(threadHandler);
		
	// copy the diagnostic data to the structure
    strcpy(diagnosticData[DIAGCOUNT-1].testData,sstring);
    
	// unlock the mutex
    pthread_mutex_unlock(threadHandler);
	// UpdateUserQueue, to notify the user
    updateUserQueues(TRUE,(void *)singleUpdate);
}

void updateStatus(char *sstring, short type)
{
    if (type == TEST)
        updateTestDataStatus(sstring);
    else if (type==DIAG)
        updateDiagnosticDataStatus(sstring);
}

/************************************************************************************
*
*	updateDiagnosticDataStatus
*
*	Updates the previous diagnostic data status
*      
*	Arguments:
*
*		char *sstring - command with which to update the last test status string
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void updateDiagnosticDataStatus(char *sstring)
{
	// lock the mutex to prevent the test data
	// from being updated by another thread
    pthread_mutex_lock(threadHandler);

    if (isBurnout)
    {
        return;
    }

	// copy the status into the test structure
    strcpy(diagnosticData[DIAGCOUNT-1].status,sstring);
	// release the mutex
    pthread_mutex_unlock(threadHandler);
    // update the user queue
    updateUserQueues(TRUE,(void *)singleUpdate);
}


/************************************************************************************
*
*	updateTestDataStatus
*
*	Updates test data status
*      
*	Arguments:
*
*		char *sstring - command with which to update the last test status string
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void updateTestDataStatus(char *sstring)
{
	// lock the mutex to prevent the test data
	// from being updated by another thread
    pthread_mutex_lock(threadHandler);
	
	// copy the status into the test structure
    if (!strncmp(sstring,FAILEDSTRING,strlen(FAILEDSTRING)))
    {
        referenceBoardInfo->passing=FALSE;
    }
    else
    {
        if (isBurnout)
        {
            if (TESTCOUNT-1 >= 0)
                TESTCOUNT--;
            pthread_mutex_unlock(threadHandler);
            return;
        }
    }

    strcpy(testData[TESTCOUNT-1].status,sstring);
	// release the mutex
    pthread_mutex_unlock(threadHandler);
    // update the user queue
     updateUserQueues(FALSE,(void *)singleNonUpdate);
    //updateUserQueues(FALSE,(void *)singleUpdate);
}


/************************************************************************************
*
*	updateTestDataQueue
*
*	Updates test data
*      
*	Arguments:
*
*		char *sstring - command with which to update the last test data string
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void updateTestDataQueue(char *sstring)
{
	// lock the mutex
    pthread_mutex_lock(threadHandler);
	// free the test data
    memset(testData[TESTCOUNT-1].testData,0,sizeof(testData[TESTCOUNT-1].testData));
	// re-allocate memory for the test data
    
	// copy the test data
    strcpy(testData[TESTCOUNT-1].testData,sstring);
    
	// release the mutex
    pthread_mutex_unlock(threadHandler);
    // update the queue
    updateUserQueues(FALSE,(void*)singleUpdate);
}

/************************************************************************************
*
*	queueTestData
*
*	Places string at the end of the test data buffer list
*      
*	Arguments:
*
*		char *sstring - command to push onto test data buffer
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void queueTestData(char *sstring)
{

    // lock thread mutex
    pthread_mutex_lock(threadHandler);
    TESTCOUNT++;
    // reallocate size of testData so we can increase the size
    // of our buffer
    testData = (test*)realloc(testData,TESTCOUNT*sizeof(test));
    // create memory for buffer
    // copy test data into buffer 
    strncpy(testData[TESTCOUNT-1].testData,sstring,sizeof(testData[TESTCOUNT-1].testData)-1);
    // set the status of the current test to NULL
    memset(testData[TESTCOUNT-1].status,0x00,sizeof(testData[TESTCOUNT-1].status));
    sprintf(testData[TESTCOUNT-1].status,"\r\n");

    // unlock thread mutex
    pthread_mutex_unlock(threadHandler);

    

    if (testData[TESTCOUNT-1].testData[strlen(testData[TESTCOUNT-1].testData)-1] == '\n')
    {
        // update the queue, and users
        updateUserQueues(FALSE,(void *)singleNonUpdate);
    }
    
    


}


/************************************************************************************
*
*	queueDiagnosticData
*
*	Places string at the end of the diagnostic data buffer list
*      
*	Arguments:
*
*		char *sstring - command to push onto diagnostic data buffer
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void queueDiagnosticData(char *sstring)
{
    if (isBurnout==TRUE)
    {
        return;
    }
    // lock thread mutex
    pthread_mutex_lock(threadHandler);
    DIAGCOUNT++;
    // reallocate size of testData so we can increase the size
    // of our buffer
    diagnosticData = (test*)realloc(diagnosticData,DIAGCOUNT*sizeof(test));
    // create memory for buffer
    
    strncpy(diagnosticData[DIAGCOUNT-1].testData,sstring,sizeof(diagnosticData[DIAGCOUNT-1].testData)-1);
    
    // set the status of the current test to NULL
    memset(diagnosticData[DIAGCOUNT-1].status,0x00,sizeof(diagnosticData[DIAGCOUNT-1].status));
    sprintf(diagnosticData[DIAGCOUNT-1].status,"\r\n");
    
    // unlock thread mutex


    pthread_mutex_unlock(threadHandler);
    
    if (diagnosticData[DIAGCOUNT-1].testData[strlen(diagnosticData[DIAGCOUNT-1].testData)-1]=='\n')
    {
        updateUserQueues(TRUE,(void *)singleNonUpdate);
    }
    

}


/************************************************************************************
*
*	answerQuestion
*
*	Poses question to the user
*      
*	Arguments:
*
*		char *qstion - Question to place in buffer
*		char *type - Type of question to pose to the user
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void answerQuestion(char *qstion, const char *type)
{

    // lock mutex, so we have exclusive access to *question
    pthread_mutex_lock(threadHandler);

    strncpy(questionType,type,( strlen(type) < 5 ? strlen(type) : 5 ) );

    

    // allocate memory for question
    question = (char*)malloc( ( strlen( qstion )+1) * sizeof(char) );
	if (question == NULL)
		exit(1); // not enough memory

    // copy qstion into question
    sprintf(question,"%s",qstion);

    
    

    // iterate through each open thread and signal the threads
    // so they can begin sending data

    pthread_mutex_unlock(threadHandler);
    int iIterator = 0,jIterator=0;
    for (; iIterator <= openThreads; iIterator++ ) {
        for (; jIterator <= CONNECTIONSPERTHREAD; jIterator++)
        {
            ServerThreads[iIterator].connectionStatus[jIterator]&=(QUESTIONPAUSE^0xffff);
        }
        pthread_cond_signal(ServerThreads[iIterator].threadNotifier);
    }
    
    // unlock mutex, so we release this and other threads that 
    // may want to access question
    
    
}

/************************************************************************************
*
*	userAnswerQuestion
*
*	Sends question data to the client
*      
*	Arguments:
*
*		int thread - thread at which to send data
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void userAnswerQuestion(int thread)
{
    
	// allocate enough memory for question
    
    char *qst = (char*)malloc( ( strlen(question)+20+45) * sizeof(char));
	
	if (qst == NULL)
		exit(1); // not enough memory
    
    char length[6] = "";
    
    //put the length of the array into the protocol format
    sprintf(length,"%X",strlen(question)+5);
    
    // expand length to five characters
    prefixExpand(length,5);

    int i=0;
    char type[6];
    char postFix[45];
    memset(postFix,0x00,sizeof(postFix));
    
    // poll through each connection for this thread
    for (i=0; i < CONNECTIONSPERTHREAD; i++) 
    {
		// if the file desciptor is zero, skip connection
        
        if (ServerThreads[thread].threadFD[i] == 0x0000) {
            
            continue;
        }
        
        if ((ServerThreads[thread].connectionStatus[i]&QUESTIONPAUSE) == QUESTIONPAUSE)
        {
            
            continue;
        }

        if (repairLock != 0)
        {
            if (repairLock != ServerThreads[thread].threadIP[i])
            {
                if (repairCounter == -1)
                {
                    repairCounter = 0;
                    repairTimer(2);
                }
                continue;
            }
            
        }
        // copy 4 characters of the question type to type
        strncpy(type,questionType,4);
        if (!strcmp(questionType,"USRIN")) {
                    type[4] = questionType[4];
                    type[5] = 0x00;    
        }
        // if SELA, we are dealing with a message box to show
        else if (!strncmp(questionType,"SELA",4)) {
            
			// place the user access level just after SELA
            sprintf(type,"SELA%i",databaseGetUserAccessLevel( ServerThreads[thread].macAddress[i] ) );
        }
        else if (!strncmp(questionType,"RETRYFAIL",4))
        {
            strcpy(type,"YESNO");
            strcpy(postFix,"|RETRY|FAIL");
            sprintf(length,"%X",strlen(question)+5+strlen(postFix));
    
            // expand length to five characters
            prefixExpand(length,5);
        }
        else if (!strncmp(questionType,"YESNO",4))
        {
            type[4] = questionType[4];
            type[5] = 0x00;    
            strcpy(postFix,"|YES|NO");
            sprintf(length,"%X",strlen(question)+5+strlen(postFix));

            // expand length to five characters
            prefixExpand(length,5);

        }
        else
            type[5] = 0x00; // null terminate type
    

    // create question to send to user    
    sprintf(qst,"%c%c%c%c%cMSGBOX%s%s%s",length[0],length[1],length[2],length[3],length[4],type,question,postFix);

	// set length equal to the string length of qst	
    int len=strlen(qst);
    
    
        // make sure someone didn't already answer the question
        if (question==NULL)
		{
			free(qst);
            return;
		}
		
                      

         // send data to client
         
         if (find(qst,question)==ERROR)
         {
             return;
         }
         
         if (sendall(ServerThreads[thread].threadFD[i], qst, &len) == -1) 
         {
         }
         else
         {
               // don't pause the question as we're not sure the user received
               // the data. Nonetheless, this will be taken care by the user's
               // client
//             ServerThreads[thread].connectionStatus[i]|=QUESTIONPAUSE;
         }

         
         

    
    } // end for loop
    // release memory
    free(qst);

}

/************************************************************************************
*
*	storeCurrentTestDataInDatabase
*
*	Stores temporary data into database
*      
*	Arguments:
*
*		
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void storeCurrentTestDataInDatabase()
{
    // insert all temporary data
    databaseInsertTemporaryTestData(referenceBoardInfo,TESTCOUNT,testData,DIAGCOUNT,diagnosticData,0,1);
}


/************************************************************************************
*
*	notifyUserToCloseMessageBox
*
*	Notifies each thread and each connection to close any visible message box
*      
*	Arguments:
*
*		
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void notifyUserToCloseMessageBox()
{
	// lock mutex, so we have exclusive access to 'question'
    pthread_mutex_lock(threadHandler);
	/* if question is not null, make it null
	 * then set it to null, therefore
	 * no message box will be displayed to the user
	 * The reasoning is that when a thread's signaled to begin
	 * checking the queue, it also checks the question buffer.
	 * If a thread has not been notified yet, we make question NULL
	 * to prevent any further threads from sending the question
	 * to the user
	 */
    if (question != NULL) {
        free(question);
        question=NULL;
    }
    else
    {
        pthread_mutex_unlock(threadHandler);
        return;
    }
        

	// now, we begin killing the message boxes for those who have
	// already entered the process of answering a question
    // unlock the mutex
    pthread_mutex_unlock(threadHandler);
	// close message box command
    char closeMessage[17]= "00005MSGBOXCLOSE";
    
    closeMessage[17] = 0x00;
	
	// set len equal to string length of closeMessage
    int len = strlen(closeMessage);
	
    int i=0,j=0; // our iterators
    
    // traverse all open threads
    for (; j <= openThreads; j++) 
    {

		// and, traverse each connection
        for (i=0; i < CONNECTIONSPERTHREAD; i++) 
        {
         
             // send data to client
             if (ServerThreads[j].threadFD[i] != 0x0000) 
             {
             
                 sendall(ServerThreads[j].threadFD[i], closeMessage, &len);

                 ServerThreads[j].connectionStatus[i]&=(QUESTIONPAUSE^0xffff);
             }

        }

    }
	// signal each thread that we have something for it..whether it will like
	// what we have remains to be seen
    int iIterator = 0;
    for (; iIterator <= openThreads; iIterator++ ) {
        pthread_cond_signal(ServerThreads[iIterator].threadNotifier);
    }

	// get queue ID for the question queue
    key_t mykey = TEST_EXECUTE_QUESTION_QUEUE;
    int userQid=ipcGetKey(mykey);
    IPCPACKET userResponse={};
        
       
    snprintf(userResponse.data.response,BUF_LEN,"");
        
    userResponse.mtype=1;
    userResponse.data.releaseListener=FALSE;
	// send a message to the question queue so each active user will be notified
	// to close his/her message box
    int value = ipcSendMessage(userQid,(struct IPCPACKET*)&userResponse,sizeof(IPCPACKET));

}

/************************************************************************************
*
*	saveUnPlannedTestData
*
*	Saves test data from unplanned restarts into the database
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
void saveUnPlannedTestData()
{
    // get attempt for test data
    int tempAttempt = getAttemptForSavedTestData(&referenceBoardInfo->boardInfo.bootMacNumber,FALSE);
    // save current test attempt
    int currentAttempt = referenceBoardInfo->attempt;
    referenceBoardInfo->attempt=tempAttempt;

    
    int myTestCount=0;
    // tell the database function that yes, we want
    // test data only
    test * myTestData =  databaseGetTestData(TRUE,&myTestCount,&referenceBoardInfo->boardInfo.bootMacNumber,FALSE); // get only planned test data

    // save the printout
    databaseSaveTestPrintout(referenceBoardInfo,myTestData,myTestCount,NULL,0);

	// restore the test attempt

    referenceBoardInfo->attempt=currentAttempt;
}

/************************************************************************************
*
*	getPreviousTestData
*
*	Obtains previous test data, most likely following a system restart
*      
*	Arguments:
*
*		
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void getPreviousTestData()
{
	// lock the mutex
	pthread_mutex_lock(threadHandler);
    // tell the database function that yes, we want
    // test data only
    testData =  databaseGetTestData(TRUE,&TESTCOUNT,&referenceBoardInfo->boardInfo.bootMacNumber,TRUE); // get only planned test data
    
	// now, we want diagnostic data
    diagnosticData =  databaseGetTestData(FALSE,&DIAGCOUNT,&referenceBoardInfo->boardInfo.bootMacNumber,TRUE); // get only planned test data
    
	// unlock the mutex, as we are finished with testData and diagnosticData
    pthread_mutex_unlock(threadHandler);
}


/************************************************************************************
*
*	saveTestPrintout
*
*	Saves the test printout to the database
*      
*	Arguments:
*
*		PTESTBOARDINFO boardInfo - Board data and characteristics
*       PTESTPARAMETERS test - test data 
*
*	Return Value:
*
*		Status of saving the test printouts
*
*************************************************************************************/
short saveTestPrintout(PBOARDSTATE boardInfo,PTESTPARAMETERS test)
{
    sleep(2);

    if (databaseSaveTestPrintout(boardInfo,testData,TESTCOUNT,testExecutionRows,EXECUTIONROWCOUNT) == TRUE)
        return TRUE;
    else
        return FALSE;
    
}

/************************************************************************************
*
*	setListenerRequiredVersion
*
*	Sets the required client version to use this version of Test Dispatcher
*      
*	Arguments:
*
*		float version - Version
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void setListenerRequiredVersion(float version)
{
    serverVersion = version;
}

/************************************************************************************
*
*	logFinalTestData
*
*	Logs test data after a test is executed
*      
*	Arguments:
*
*		char *executionString - Test data to be logged
*       float version - Version of test
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void logFinalTestData(char *executionString,int status, float version)
{
    EXECUTIONROWCOUNT++;
    // reallocate size of testData so we can increase the size
    // of our buffer
    testExecutionRows = (test*)realloc(testExecutionRows,EXECUTIONROWCOUNT*sizeof(test));
    // create memory for buffer
    // copy test data into buffer 
    char sstring [10];

    memset(testExecutionRows[EXECUTIONROWCOUNT-1].testData,0x00,sizeof(testExecutionRows[EXECUTIONROWCOUNT-1].testData));
    strncpy(testExecutionRows[EXECUTIONROWCOUNT-1].testData,executionString,sizeof(testExecutionRows[EXECUTIONROWCOUNT-1].testData));
    snprintf(sstring,10,"%i",status);
    
    int i=0,cnt = LINE_BUF-1;;
    for (i=strlen(testExecutionRows[EXECUTIONROWCOUNT-1].testData); i < cnt; i++)
    {
        testExecutionRows[EXECUTIONROWCOUNT-1].testData[i] = 0x20;
    }
    
    testExecutionRows[EXECUTIONROWCOUNT-1].testData[i]=0x00;
    snprintf(testExecutionRows[EXECUTIONROWCOUNT-1].status,sizeof(testExecutionRows[EXECUTIONROWCOUNT-1].status),"%.2f",version);
}


/************************************************************************************
*
*	beginTesting
*
*	Loads all Test data into boardInfo
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
void beginTesting(PBOARDSTATE boardStatusAndState,char *Tester,ulong *serial, char *revision)
{

    // get the processor speed
    getProcessorSpeed(boardStatusAndState->processor,sizeof(boardStatusAndState->processor));

    
    // if the Boot Mac number is empty, attempt to fill it
    if (boardStatusAndState->boardInfo.bootMacNumber == 0)
    {
        // pull mac from either a file, database, or ask the user
        
        long mac = 0;
        databaseGetBootMacNumber(boardStatusAndState,&mac);
        if (boardStatusAndState->boardInfo.bootMacNumber==0)
        {
            // get the boot MAC number
            getBootMacNumber(&boardStatusAndState->boardInfo.bootMacNumber);
        }
    }
    


    


   setBootMacNumber(&boardStatusAndState->boardInfo.bootMacNumber);


    

    /* safeguard to wait for default_message to finish processing
     * normally, I'd go for a mutex, but there is no need in this case
     * since processing the UDP message should take milliseconds.
     * Locking a mutex often takes several milliseconds ( depends on implementation )
     */ 
    
   while(handleDefaultMessage==NOTLOCKED);

   while(handleDefaultMessage!=FALSE);

   

    


   if (strlen(boardStatusAndState->boardInfo.tester)==0)
    {
        Tester != NULL ? strcpy(boardStatusAndState->boardInfo.tester,Tester)
        : getTestersInitialsFromUser(boardStatusAndState->boardInfo.tester);
    }

    if (boardStatusAndState->boardInfo.serialNumber==0)
      boardStatusAndState->boardInfo.serialNumber = ( *serial != 0 ?  *serial : getSerialNumberFromUser());

    

    if (boardStatusAndState->boardInfo.finishedGoodRev=='0')
        *revision != '0' ? *revision : getBomRevisionFromUser(&boardStatusAndState->boardInfo.finishedGoodRev);

    setTestersInitials(boardStatusAndState->boardInfo.tester);

    setBomRevision(&boardStatusAndState->boardInfo.finishedGoodRev);

    setSerialNumber(&boardStatusAndState->boardInfo.serialNumber);

    setFinishedGoodNumber(&boardStatusAndState->boardInfo.finishedGoodNumber);

    

 while(1)
    {
        if (boardStatusAndState->boardInfo.finishedGoodNumber!= 0 && !isFinishedGoodNumber(&boardStatusAndState->boardInfo.finishedGoodNumber))
        {
            
            if (askRetryFailQuestion("%lu is not a valid finished good number?",boardStatusAndState->boardInfo.finishedGoodNumber) == FAIL)
            {
                
                testPrint("Invalid finished Good Number");
                failedMessage();
                return;

            }
            else
            {
                // force user to enter Finished Good Number
                boardStatusAndState->boardInfo.finishedGoodNumber = getFgNumberFromUser();
                continue;
            }
                
            boardStatusAndState->boardInfo.finishedGoodNumber=0;
        }
        else if (boardStatusAndState->boardInfo.finishedGoodNumber == 0)
        {
            if (boardStatusAndState->boardInfo.finishedGoodNumber != 0)
                    break;
            boardStatusAndState->boardInfo.finishedGoodNumber = getFgNumberFromUser();
            
            continue;
        }
        else
            break;
    }

    if ( databaseGetBoardName(boardStatusAndState->boardInfo.finishedGoodNumber, boardStatusAndState->boardInfo.boardName) == FALSE)
    {
        getBoardNameFromUser(boardStatusAndState->boardInfo.boardName);
    }

    setBoardName(boardStatusAndState->boardInfo.boardName);

    initBoardInfoFile();

    updateBoardInfoFile();

    
    // update the database with 
    DBUpdateBoardInfo(boardStatusAndState);

    
    char answer[255];

    // save unplanned test data IMMEDIATELY
    saveUnPlannedTestData();


    // getBoardAttempt obtains the attempt we are working with
    // and thesequence at which we will begin
    ulong secondsToExecute=0; // number of seconds to execute our test
    boardStatusAndState->attempt = getBoardAttempt(boardStatusAndState->boardInfo.finishedGoodNumber,boardStatusAndState->boardInfo.serialNumber,&boardStatusAndState->accessLevel,&boardStatusAndState->currentSequence,&boardStatusAndState->testType,&secondsToExecute);
    
    

    int numTests = 0, i=0; 
    
    // get the number of tests within this FinishedGoodNumber and test attempt
    databaseGetNumberOfTests(boardStatusAndState->boardInfo.finishedGoodNumber,boardStatusAndState->attempt,boardStatusAndState->testType, &numTests);
    

    TESTPARAMETERS test;
    test.FGNumber=boardStatusAndState->boardInfo.finishedGoodNumber;
    test.attempt=boardStatusAndState->attempt;

    test.Sequence = boardStatusAndState->currentSequence;

    saveTestConfig(TESTCONFIG,&boardStatusAndState->boardInfo.finishedGoodNumber,(short*)&boardStatusAndState->attempt);

    boardStatusAndState->loaded=TRUE;


    
    char testString[BUF_LEN];

    EXECUTIONROWCOUNT=0;
    testExecutionRows=NULL;

    

    //testExecutionRows=(test*)malloc(1 * sizeof(test) );
    // if our test sequence is greater than or equal to zero, this means
    // that we need to retrieve previous test and diagnostic data
    if (test.Sequence>= 0)
    {
        getPreviousTestData();
    }


    if (boardStatusAndState->testType==FUNCTIONAL)
    {
        printBoardHeader(boardStatusAndState);
    }
    else
    {

        printDateAndTimeString();
        testPrint("");
        customMessage("\n");
        testPrint("Performing Burn-In test");
        customMessage("");
        if (secondsToExecute == ((ulong)(-1)) )
            linePrint("Burn-In test will execute indefinitely\n");
        else
        {
            int days=0,hours=0,minutes=0;
            long composeSecondsToExecute=secondsToExecute;
            days = composeSecondsToExecute/(24*60*60);
            composeSecondsToExecute-=days*(24*60*60);
            hours = composeSecondsToExecute/(60*60);
            composeSecondsToExecute-=hours*(60*60);
            minutes = composeSecondsToExecute/60;
            linePrint("Burn-In test will execute for %i day(s) %lu hour(s) and %lu minute(s)\n",days,hours,minutes);
        }
        linePrint("--------\n");
        linePrint("Only test failures will be printed!\n");
        linePrint("--------\n");
        setBurnoutTest();
    }
    

    ushort status=0;
    //loop through calling the tests 
    int testCount = 0;

    
    if (boardStatusAndState->testType==FUNCTIONAL)
        testCount = functionalTest(boardStatusAndState,&test,numTests);
    else
        burninTest(boardStatusAndState,&test,numTests,secondsToExecute);
      
   
    free(testExecutionRows);


}

/************************************************************************************
*
*	burninTest
*
*	Sets a variable, which lets the server know we're in burn-in mode
*      
*	Arguments:
*
*		PTESTBOARDINFO boardInfo - board information
*       TESTPARAMETERS *test - Test Variable
*       int numTests - Maximum number of tests that need to be executed
*       ulong secondsToExecute - Number of seconds to execute this test
*
*	Return Value:
*
*		void
*
*************************************************************************************/

void burninTest(PBOARDSTATE boardStatusAndState,TESTPARAMETERS *test, int numTests, ulong secondsToExecute)
{

    short i=0;
    // allocate memory for driver string
    char **driverString;
    driverString = (char**)malloc(numTests*sizeof(char*));
    char **testString;
    // allocate memory for test execution string
    testString = (char**)malloc(numTests*sizeof(char*));

    ulong secondsExitTesting = time(NULL) + secondsToExecute;

    if (secondsToExecute == ((ulong)(-1)) )
    {
        secondsExitTesting=-1;
    }
    int counter=0;
    // tell the console that we are caching tests
    consolePrint("Caching tests...");

    /* When we cache tests, we read the database for all tests
     * Since connecting and reading data from the tables is our 
     * bottleneck, we can execute more of our tests during this
     * time period by simply caching the data into a buffer,
     * then excuting the strings from the buffer. In addition,
     * ODBC drivers have been known to cause some problems, so
     * this method should eliminate the problems associated with ODBC
     * by limiting the number of SQL calls
     */
    for (i=0; i < numTests; i++)
    {
        // set the currentSequence in the board and test structures
        boardStatusAndState->currentSequence=test->Sequence=i;
        driverString[i] = (char*)malloc(BUF_LEN);
        testString[i] = (char*)malloc(BUF_LEN);
        if (!databaseGetTest(test,boardStatusAndState->testType))
            continue;

        if (strlen(test->Test) == 0)
        {
            continue;
        }

        
        sprintf(testString[counter],"/tests/");
        if (test->Type[0]=='1')
            strcat(testString[counter],"experimental");
        else if (test->Type[0]=='2')
        {
            strcat(testString[counter],"validation");
        } else
            strcat(testString[counter],"production");

        // if the driver field is not empty, we attempt to load
        // a device driver
        if (strlen(test->Driver) > 0)
        {
            sprintf(driverString[counter],"%s",test->Driver);
            
        }
        else
            memset(driverString[counter],0x00,30);

        
        sprintf(testString[counter],"%s/%s %s",testString[counter],test->Test,test->TestParam);

        counter++;
    }

    consolePrint("Finished caching\n");

    char commandLineDriverString[BUF_LEN];
    for (i=0; i < counter; i++)
    {
        // compute the percent complete. 
        boardStatusAndState->percentComplete=(int)(  (double)( (double)(time(NULL)-(secondsExitTesting-secondsToExecute))/(double)secondsToExecute )*100  );

        // load any driver
        if (strlen(driverString[i]) > 0)
        {
            sprintf(commandLineDriverString,"insmod %s",driverString[i]);
            system(commandLineDriverString);
        }
        
        system(testString[i]);

        // unload the driver
        if (strlen(driverString[i]) > 0)
        {
            sprintf(commandLineDriverString,"rmmod %s",driverString[i]);
            system(commandLineDriverString);
        }

        if (time(NULL) > secondsExitTesting)
        {
            consolePrint("Timeout\n");
            break;
        }
        else if (i == (counter-1))
        {
            // restart the test. Set to negative one, so the counter
            // increments i to 0
      
            i=-1;
        }
            
            

    }//end for 
    printDateAndTimeString();
    testPrint("");
    customMessage("\n");
    linePrint("Test Finished\n");
    // free the memory associated with our tests
    for (i=0; i < counter; i++)
    {
        free(driverString[i]);
        free(testString[i]);
    }
    free(driverString);
    free(testString);
    boardStatusAndState->percentComplete=100;
    
    
    if (i>=numTests)
    {
        databaseRemoveTemporaryTestData(&boardStatusAndState->boardInfo.bootMacNumber);
    }
}

char **getDriverArray(char *driverData, int *size)
{
    char **driverList=NULL;
        char * pch;
      pch = strtok (driverData,";");
      *size=0;
      while (pch != NULL)
      {
          (*size)++;
        driverList = (char**)realloc(driverList,*size);
        removeTrailingWhiteSpace(pch);
        driverList[*size-1] = (char*)malloc(strlen(pch));
        strcpy(driverList[*size-1],pch);
        pch = strtok (NULL, ";");
      }
      return driverList;
}

void destroyDriverArray(char **array,int *size)
{
    short i=0;
    for (; i < *size; i++)
    {
        free(array[i]);

    }
    free(array);
}
////////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         ushort functionalTest(PBOARDSTATE boardStatusAndState,TESTPARAMETERS *test, int numTests)
 */
///////////////////////////////////////////////////////////////////////////////
ushort functionalTest(PBOARDSTATE boardStatusAndState,TESTPARAMETERS *test, int numTests)
{
    short i=0,j=0;
    char testString[BUF_LEN];
    int status=0;
    databaseInsertTemporaryTestData(boardStatusAndState,TESTCOUNT,testData,DIAGCOUNT,diagnosticData,0,0);

    

    TESTPARAMETERS **cachedTests = (TESTPARAMETERS**)malloc(sizeof(TESTPARAMETERS)*(numTests));

    int oldSequence = test->Sequence+1;
    consolePrint("starting at %i\n",oldSequence);
    for (i=test->Sequence+1; i < numTests; i++)
    {
        test->Sequence=i;

        // obtain the test data associated with this test sequence
        if (!databaseGetTest(test,boardStatusAndState->testType))
        {
            continue;
        }

        if (strlen(test->Test) == 0)
        {
            continue;
        }
        // simply cache the test by copying the data over
        cachedTests[j] = (TESTPARAMETERS*)malloc(sizeof(TESTPARAMETERS));
        cachedTests[j]->attempt = test->attempt;
        cachedTests[j]->BurnIn = test->BurnIn;
        strcpy(cachedTests[j]->Driver,test->Driver);
        strcpy(cachedTests[j]->DriverParam,test->DriverParam);
        cachedTests[j]->FGNumber = test->FGNumber;
        cachedTests[j]->Production = test->Production;
        cachedTests[j]->Sequence = test->Sequence;
        strcpy(cachedTests[j]->Test,test->Test);

        cachedTests[j]->TestNumber = test->TestNumber;
        strcpy(cachedTests[j]->TestParam,test->TestParam);
        strcpy(cachedTests[j]->Type,test->Type);
        cachedTests[j]->activeSequence = test->activeSequence;
        j++;
    }


    short cacheSize=j;

    // let repair choose the tests they wish to execute
    
    if ( getRepairTestLevel(cachedTests,cacheSize) )
    {
        numTests=0;

        for (i=0; i < cacheSize; i++)
        {
            if (cachedTests[i]->activeSequence!=FALSE)
                numTests++;
        }

    } 
    
    // the following line is the old way of doing it
    //for (i=test->Sequence+1; i < numTests; i++) 
    for (i=0; i < cacheSize; i++)
    {
        if (cachedTests[i]->activeSequence==FALSE)
            continue;
        
        // compute the percent complete. 
        boardStatusAndState->percentComplete=(int)(  (double)( (double)i/(double)numTests )*100  );
        
        // set the currentSequence in the board and test structures
        boardStatusAndState->currentSequence=cachedTests[i]->Sequence=i;

              

        // if the driver field is not empty, we attempt to load
        // a device driver

        /* update:
         * drivers, and associated parameters will split by semicolons ';'
         */

        int driverSize=0,driverParamSize=0;
        char **driverArray =NULL,**driverParamArray=NULL;
        if ( strlen(cachedTests[i]->Driver) > 0)
        {
        
            driverArray = getDriverArray(cachedTests[i]->Driver,&driverSize);
            driverParamArray = getDriverArray(cachedTests[i]->DriverParam,&driverParamSize);

            if (driverSize != driverParamSize)
            {
                diagnosticPrint("Driver/Parameter sizes don't match");
                failedMessage();
            }
            else
            {
                for (j=0; j < driverSize; j++)
                {
                    sprintf(testString,"insmod %s %s",driverArray[j],driverParamArray[j]);
                    int ret = system(testString);
                    if (ret != 0)
                    {
                        diagnosticPrint("Loading module %s failed",driverArray[j]);
                        failedMessage();
                    }
    
                }
            }
        }
        

         sprintf(testString,"/tests/");
        if (cachedTests[i]->Type[0]=='1')
            strcat(testString,"experimental");
        else if (cachedTests[i]->Type[0]=='2')
        {
            strcat(testString,"validation");
        } else
            strcat(testString,"production");
        

        sprintf(testString,"%s/%s %s",testString,cachedTests[i]->Test,cachedTests[i]->TestParam);

        status = system(testString);
        
        if (cachedTests[i]->Type[0]=='1')
            strcpy(testString,"experimental -");
        else if (cachedTests[i]->Type[0]=='2')
        {
            strcpy(testString,"validation   -");
        } else
            strcpy(testString,"production   -");

        status >>= 8;
        sprintf(testString,"%s  %i  - %s %s",testString,status,cachedTests[i]->Test,cachedTests[i]->TestParam);

        // we should be finished with this test, so we log test data
        // which will be used later for sneaky sneaky purposes
        logFinalTestData(testString,status,getTestVersion());

        databaseInsertTemporaryTestData(boardStatusAndState,TESTCOUNT,testData,DIAGCOUNT,diagnosticData,0,0);

        // remove any device drivers, if they were loaded
        for (j=driverSize-1; j >=0 ; j--)
        {
            sprintf(testString,"rmmod %s",driverArray[j]);
            int ret = system(testString);

        }

        
        



        if (driverSize > 0)
        {
            destroyDriverArray(driverArray,&driverSize);
            destroyDriverArray(driverParamArray,&driverParamSize);
        }
        
        
       

    }//end for 


    for (i=0; i < cacheSize; i++)
    {
        free(cachedTests[i]);
    }
    
    free(cachedTests);

    // at this point, we may or may not be finished testing. If all tests
    // completed, the percentage will be 100%
    boardStatusAndState->percentComplete=(int)(  (double)( (double)i/(double)numTests )*100  );
    if (i>=numTests)
    {
        saveTestPrintout(boardStatusAndState,test);
        databaseRemoveTemporaryTestData( &boardStatusAndState->boardInfo.bootMacNumber);
    }


    consolePrint("Testing complete\n");

    return (ushort)i;

}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void setBurnoutTest()
 */ 
/////////////////////////////////////////////////////////////////////////
void setBurnoutTest()
{
    isBurnout=TRUE;
}

