///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       queue.c
 *
 *  @brief      This file holds declarations for the functions in BoardInfo.c
 *
 *              Copyright (C) 2006 @n@n
 *              Function definitions for basic queue data structure
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


/*! \file
	\brief This file holds definitions for the declarations in BoardInfo.h
	\n See BoardInfo.h for details about the methods in this file. 
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "BoardInfo.h"
#include "Prompt.h"
#include "Database/DBFunc.h"
#include "Common.h"
#include "IPCFunctions.h"

TESTBOARDINFO BoardInfo;
/*! \var BoardInfo
    \brief A global instance of the TESTBOARDINFO struct
*/

/***********************************************************************************
                                Convenience Functions:
    These functions make life a little bit easier for the programmer, in addition to
    improving the look of the code
************************************************************************************/

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBoardName(char *)
 */ 
/////////////////////////////////////////////////////////////////////////
short getBoardName(char *buffer)
{
    return getBoardInfo(ACTION_BOARD_NAME,buffer);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBootMacNumber(ulong *bootMac)
 */ 
/////////////////////////////////////////////////////////////////////////
short getBootMacNumber(ulong *bootMac)
{
    return getBoardInfo(ACTION_BOOT_MAC,bootMac);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getFinishedGoodNumber(ulong *fg)
 */ 
/////////////////////////////////////////////////////////////////////////
short getFinishedGoodNumber(ulong *fg)
{
    return getBoardInfo(ACTION_FINISHED_GOOD_NUMBER,fg);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBomRevision(char *bomRev)
 */ 
/////////////////////////////////////////////////////////////////////////
short getBomRevision(char *bomRev)
{
    return getBoardInfo(ACTION_FINISHED_GOOD_REV,bomRev);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getSerialNumber(ulong *serial)
 */ 
/////////////////////////////////////////////////////////////////////////
short getSerialNumber(ulong *serial)
{
    return getBoardInfo(ACTION_SERIAL_NUMBER,serial);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getTestersInitials(char *buffer)
 */ 
/////////////////////////////////////////////////////////////////////////
short getTestersInitials(char *buffer)
{
    return getBoardInfo(ACTION_TESTER_NAME,buffer);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBoardName(char *buffer)
 */ 
/////////////////////////////////////////////////////////////////////////
short setBoardName(char *buffer)
{
    TESTBOARDINFO temp;
    strcpy(temp.boardName,buffer);
    return updateBoardInfo(ACTION_BOARD_NAME,&temp,TRUE);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBootMacNumber(ulong *bootMac)
 */ 
/////////////////////////////////////////////////////////////////////////
short setBootMacNumber(ulong *bootMac)
{
    TESTBOARDINFO temp;
    temp.bootMacNumber=*bootMac;
    return updateBoardInfo(ACTION_BOOT_MAC,&temp,TRUE);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setFinishedGoodNumber(ulong *fg)
 */ 
/////////////////////////////////////////////////////////////////////////
short setFinishedGoodNumber(ulong *fg)
{
    TESTBOARDINFO temp;
    temp.finishedGoodNumber=*fg;
    return updateBoardInfo(ACTION_FINISHED_GOOD_NUMBER,&temp,TRUE);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBomRevision(char *bomRev)
 */ 
/////////////////////////////////////////////////////////////////////////
short setBomRevision(char *bomRev)
{
    TESTBOARDINFO temp;
    temp.finishedGoodRev=*bomRev;
    return updateBoardInfo(ACTION_FINISHED_GOOD_REV,&temp,TRUE);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setSerialNumber(ulong *serial)
 */ 
/////////////////////////////////////////////////////////////////////////
short setSerialNumber(ulong *serial)
{
    TESTBOARDINFO temp;
    temp.serialNumber=*serial;
    return updateBoardInfo(ACTION_SERIAL_NUMBER,&temp,TRUE);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setTestersInitials(char *buffer)
 */ 
/////////////////////////////////////////////////////////////////////////
short setTestersInitials(char *buffer)
{
    TESTBOARDINFO temp;
    strcpy(temp.tester,buffer);
    return updateBoardInfo(ACTION_TESTER_NAME,&temp,TRUE);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void getProcessorSpeed(char *processorBuffer, int sizeOfProcessorBuffer)
 */ 
/////////////////////////////////////////////////////////////////////////
void getProcessorSpeed(char *processorBuffer, int sizeOfProcessorBuffer)
{
    FILE *cpuInfo = fopen("/proc/cpuinfo","r");
    
    char askUser=TRUE,divisor=0x00;
    float clock=0;
    if (cpuInfo == NULL)
    {
        goto exitFunction;
    }
    char buffer[256]; // buffer should never be longer 
    while(fgets(buffer,sizeof(buffer),cpuInfo))
    {

        if (sscanf (buffer, "cpu %cHz : %f",&divisor, &clock) == 2)
        {
            if (divisor == 'G')
            {
                snprintf(processorBuffer,sizeOfProcessorBuffer,"%.2f GHz",clock);
            }
            else
                if (clock >= 1000)
                {
                    snprintf(processorBuffer,sizeOfProcessorBuffer,"%.2f GHz",(clock/1000.00));
                }
                
            
            askUser = FALSE;
            break;
        }

    }
    fclose(cpuInfo);

    exitFunction:

    if (askUser==TRUE)
    {
        char Answer[BUF_LEN]; 

        promptUser("What is this processor's speed in MHz?", Answer, BUF_LEN); 

        snprintf(processorBuffer,sizeOfProcessorBuffer,"%.2f GHz",(atof(Answer)/1000.00));
    }
 
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short isFinishedGoodNumber(ulong *FinishedGoodNumber)
 */ 
/////////////////////////////////////////////////////////////////////////
short isFinishedGoodNumber(ulong *FinishedGoodNumber)
{
    // time to validate the finished good number
    return databaseIsValidFinishedGoodNumber(FinishedGoodNumber);
}



/************************************************************************************
*
*	getBoardInfo
*
*		This function will update the board info structure by using the following methods.
*           1. Check the Board Info File
*           2. Check the Data Base with the Finished Good Number
*           3. Try to autodetect from the BIOS
*           4. Ask the user       	 
*
*	Arguments:
*
*		int actionCode - Get the board information for the desired action code
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBoardInfo(int actionCode, void *data)
 */ 
/////////////////////////////////////////////////////////////////////////
short getBoardInfo(int actionCode, void *data)
{
    if (!data) return FALSE;

    short returnValue;

    returnValue = TRUE;                    /* Assume an attempt will pass */

    // the following could be merged into a single function with nested
    // if statements, but it is cleaner to seperate them
    if(!getBoardInfoFromFile(actionCode,data))
    {
        
        if(!getAuto(actionCode, data))
        {
        
            if(!askForBoardInfo(actionCode,data))
            {
                returnValue = FALSE;           /* All attempts failed */
            }

        }
    }

    return returnValue;
}

short getAuto(int actionCode, void *data)
{

}

/************************************************************************************
*
*	getBoardInfoFromFile
*
*		This function will read in the board.info file and then update the board
*       information structure according to the desired action request       	 
*
*	Arguments:
*
*		int actionCode - Get the board information for the desired action code
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBoardInfoFromFile(int actionCode, void *data)
 */ 
/////////////////////////////////////////////////////////////////////////
short getBoardInfoFromFile(int actionCode, void *data)
{

    short returnValue;
    int fileDescriptor;
    int fileErrorCode;
    int ReadCount;
    TESTBOARDINFO Info;

    bzero(&Info, sizeof(TESTBOARDINFO));    /* Zero out the board info structure */

    fileDescriptor = open(BOARDINFOFILE, O_RDONLY);  /* Try to open the file */
    if(fileDescriptor != -1)
    {
        ReadCount = read(fileDescriptor, &Info, sizeof(TESTBOARDINFO));
        close(fileDescriptor);                  /* Close the file */

        if(ReadCount == sizeof(TESTBOARDINFO))  /* Make sure the whole file is read */
            returnValue = updateBoardInfo(actionCode,&Info,FALSE);   /* Update the global structure */
    }
    else
    {

        returnValue = FALSE;           /* Return file check error */
        fileErrorCode = errno;          /* Get the file error code */
        if(fileErrorCode == ENOENT)     /* If the file does not exist */
            initBoardInfoFile();    /* Create the file and initialize to zero */
    }

    // even if returnValue is false, we still copy the board information into
    // data. Fail if copyBoardInfoToData fails

    returnValue &= copyBoardInfoToData(actionCode,&Info,data);
            

    return returnValue;
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short copyBoardInfoToData(int actionCode,PTESTBOARDINFO info,void *data)
 */ 
/////////////////////////////////////////////////////////////////////////
short copyBoardInfoToData(int actionCode,PTESTBOARDINFO info,void *data)
{
    // copy the corresponding data into
    short returnVal = TRUE;
    switch( actionCode )
    {
    case ACTION_BOARD_NAME:
        strcpy((char*)data,info->boardName);
        break;
    case ACTION_FINISHED_GOOD_NUMBER:
        *((ulong*)data) = info->finishedGoodNumber;
        break;
    case ACTION_FINISHED_GOOD_REV:
        *((char*)data) = info->finishedGoodRev;
        break;
    case ACTION_SERIAL_NUMBER:
        *((ulong*)data) = info->serialNumber;
        break;
    case ACTION_BOOT_MAC:
        *((ulong*)data) = info->bootMacNumber;
        break;
    case ACTION_TESTER_NAME:
        strcpy((char*)data,info->tester);
        break;
    default:
        returnVal = FALSE;
    };

    return returnVal;
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short initBoardInfoFile(void)
 */ 
/////////////////////////////////////////////////////////////////////////
short initBoardInfoFile(void)
{

    short returnValue;
    returnValue = FALSE;            /* Assume failure */
	TESTBOARDINFO Info;
    
	int fileDescriptor;
    int writeCount;



    bzero(&Info, sizeof(TESTBOARDINFO));    /* Zero out the board info structure */

    /* Try to create the file */
    fileDescriptor = open(BOARDINFOFILE, O_CREAT|O_WRONLY|O_TRUNC, S_IREAD|S_IWRITE );     
    if (fileDescriptor != -1)
    {
       /* Write the board info structure to the file */

       writeCount = write(fileDescriptor, &Info, sizeof(TESTBOARDINFO));
       if(writeCount == sizeof(TESTBOARDINFO))
           returnValue = TRUE;
       close(fileDescriptor);
    }

    return returnValue;
}

/************************************************************************************
*
*	askForBoardInfo
*
*		This function will prompt the user for specific board information.  If the
*       information is correct it will update the global structure and update the 
*       board information file.       	 
*
*	Arguments:
*
*		int actionCode - Get the board information for the desired action code
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short askForBoardInfo(int actionCode, void *data)
 */ 
/////////////////////////////////////////////////////////////////////////
short askForBoardInfo(int actionCode, void *data)
{

    short returnValue=TRUE;

    TESTBOARDINFO info;

    switch(actionCode)          /* Switch on the passed in action code */
    {
    case  ACTION_BOARD_NAME:
        getBoardNameFromUser(info.boardName);
        break;

    case ACTION_FINISHED_GOOD_NUMBER:
        info.finishedGoodNumber = getFgNumberFromUser();
        break;
    case ACTION_FINISHED_GOOD_REV:
        getBomRevisionFromUser(&info.finishedGoodRev);
        break;
    case ACTION_SERIAL_NUMBER:
        info.serialNumber = getSerialNumberFromUser();
        break;
    case ACTION_BOOT_MAC:
        info.bootMacNumber = getBootMacNumberFromUser();
        break;
    case ACTION_TESTER_NAME:
        getTestersInitialsFromUser(info.tester);
        break;
    default:
        returnValue = FALSE;
        break;
    }

    returnValue &= copyBoardInfoToData(actionCode,&info,data);

    return returnValue;
}



/************************************************************************************
*
*	UpdateBoardInfo
*
*		This function will update specific data in the global board information structure  
*
*	Arguments:
*
*		int actionCode - Which specific board information data to update
*
*       PTESTBOARDINFO pInfo - A pointer to the temp TESTBOARDINFO structure
*
*       int UpdateFileFlag - TRUE or FLASE on updating the board information file
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     int updateBoardInfo(int actionCode, PTESTBOARDINFO pInfo, int updateFileFlag)
 */ 
/////////////////////////////////////////////////////////////////////////
int updateBoardInfo(int actionCode, PTESTBOARDINFO pInfo, int updateFileFlag)
{

    short returnValue=TRUE;

    switch(actionCode)          /* Switch on the passed in action code */
    {
    case  ACTION_BOARD_NAME:
        returnValue = updateBoardName(pInfo->boardName); /* Save the file board name into the global structure */
        break;

    case ACTION_FINISHED_GOOD_NUMBER:
        returnValue = updateFGNumber(&pInfo->finishedGoodNumber);
        break;

    case ACTION_FINISHED_GOOD_REV:
        returnValue = updateBOMRev(pInfo->finishedGoodRev);
        break;

    case ACTION_SERIAL_NUMBER:
        returnValue = updateSerialNumber(&pInfo->serialNumber);
        break;

    case ACTION_BOOT_MAC:
        returnValue = updateBootMacNumber(&pInfo->bootMacNumber);
        break;

    case ACTION_TESTER_NAME:
        returnValue = updateTesterInitials(pInfo->tester);
        break;

    default:
        returnValue = FALSE;
    }

    if(returnValue && updateFileFlag)  /* Update the board information file */
        updateBoardInfoFile();      /* Update the file */

    return returnValue;
}

/************************************************************************************
*
*	UpdateBoardInfoFile
*
*		This function will update data in the global board information file  
*
*	Arguments:
*
*		void - No input arguments
*
*	Return Value:
*
*		void - No return value
*
*************************************************************************************/
void updateBoardInfoFile(void)
{
    
    int fileDescriptor;
    int writeCount;

    fileDescriptor = open(BOARDINFOFILE, O_WRONLY);  /* Try to open the file */
    if(fileDescriptor != -1)
    {
        /* Write the global board info structure to the file */

        writeCount = write(fileDescriptor, &BoardInfo, sizeof(TESTBOARDINFO));
        close(fileDescriptor);                  /* Close the file */
    }

}



/***********************************************************************************
                                Update Functions:
    The following functions update the global board information structure and
    the board file
************************************************************************************/

/************************************************************************************
*
*	UpdateBoardName
*
*		This function will update board name in the global board information structure  
*
*	Arguments:
*
*		char *BoardNameBuffer - A buffer holding the board name
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
short updateBoardName(char *BoardNameBuffer)
{
    if (!BoardNameBuffer) return FALSE;
    
    short returnValue=TRUE;

    if (strlen(BoardNameBuffer) > 0)
        strncpy(BoardInfo.boardName,BoardNameBuffer,sizeof(BoardInfo.boardName));
    else
        returnValue=FALSE;

    return returnValue;
}

/************************************************************************************
*
*	updateFGNumber
*
*		Updates the finished good number
*
*	Arguments:
*
*		ulong *finishedGoodNumber - Finished good number
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
short updateFGNumber(ulong *finishedGoodNumber)
{
    short returnValue=TRUE;
    if ( finishedGoodNumber != 0 )
        BoardInfo.finishedGoodNumber=*finishedGoodNumber;
    else
        returnValue=FALSE;
    return returnValue;
}

/************************************************************************************
*
*	updateBOMRev
*
*		Updates the BOM revision
*
*	Arguments:
*
*		ulong *finishedGoodRev - BOM revision
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
short updateBOMRev(char finishedGoodRev)
{
    short returnValue=TRUE;
    // if the input is not zero, then we can use it
    if (finishedGoodRev == '0' || finishedGoodRev == 0x00)
        BoardInfo.finishedGoodRev=finishedGoodRev; 
    else
        returnValue=FALSE;
    return returnValue;
}

/************************************************************************************
*
*	updateSerialNumber
*
*		Updates the serial number
*
*	Arguments:
*
*		ulong *serialNumber - serial number
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
short updateSerialNumber(ulong *serialNumber)
{
    short returnValue=TRUE;
    // only update the global structure if the input is valid
    if( serialNumber != 0 )
        BoardInfo.serialNumber=*serialNumber;
    else
        returnValue=FALSE;
    return returnValue;
}

/************************************************************************************
*
*	updateBootMacNumber
*
*		Updates the boot MAC number
*
*	Arguments:
*
*		ulong *bootMacNumber - boot mac
*
*	Return Value:
*
*		Return TRUE or FALSE 
*
*************************************************************************************/
short updateBootMacNumber(ulong *bootMacNumber)
{
    short returnValue=TRUE;
    if ( bootMacNumber != 0 )
        BoardInfo.bootMacNumber=*bootMacNumber;
    else
        returnValue=FALSE;
    return returnValue;
}


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     short updateTesterInitials(char *tester)
 */
////////////////////////////////////////////////////////////////////////////

short updateTesterInitials(char *tester)
{
    if (!tester) return FALSE;
    
    short returnValue=TRUE;

    if ( strlen(tester)  > 0 )
        strncpy(BoardInfo.tester,tester,sizeof(BoardInfo.tester));
    else
        returnValue=FALSE;

    return returnValue;
}




/***********************************************************************************
                                Prompt Functions:
    The following functions prompt the user for the specific information
************************************************************************************/


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     ulong getFgNumberFromUser(void)
 */
////////////////////////////////////////////////////////////////////////////
ulong getFgNumberFromUser(void)
{
    char Answer[NAME_LENGTH]; 
    // prompt the user
    promptUser("What is this board's Finished Good Number?", Answer, NAME_LENGTH); 

    if (BoardInfo.finishedGoodNumber != 0) // got it from the client
    {
        snprintf(Answer,NAME_LENGTH-1,"%lu",BoardInfo.finishedGoodNumber);
    }
    while(!isValidFinishedGoodNumber(Answer))
    {
        promptUser("Finished Good Number must be nine digits, please enter a valid Finished Good Number", Answer, NAME_LENGTH); 
    }
    
    return atol(Answer);
}




////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     ulong getSerialNumberFromUser(void)
 */
////////////////////////////////////////////////////////////////////////////
ulong getSerialNumberFromUser(void)
{
    char Answer[NAME_LENGTH]; 
    // prompt the user
    promptUser("What is this board's Serial Number?", Answer, NAME_LENGTH); 
    if (BoardInfo.serialNumber != 0) // got it from the client
    {
        snprintf(Answer,NAME_LENGTH-1,"%lu",BoardInfo.serialNumber);
    }
    while(!isValidSerialNumber(Answer))
    {
        promptUser("Serial number must be eight digits, please enter a valid serial number", Answer, NAME_LENGTH); 
    }
    return atol(Answer);
}


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     ulong getBootMacNumberFromUser(void)
 */
////////////////////////////////////////////////////////////////////////////
ulong getBootMacNumberFromUser(void)
{
    char Answer[NAME_LENGTH],buffer[NAME_LENGTH]=""; 
    ulong bootMac;
    // prompt the user
    promptUser("What is this board's Boot MAC Number?", Answer, NAME_LENGTH); 
    
    
    short i=0,j=0;
    for (i=0; i < strlen(Answer); i++) {
        if (Answer[i]!= ':') 
            {
            buffer[j] = Answer[i];
            j++;
        }
        
    }
    buffer[j]=0x00;
    char *BUF=buffer;
   
    if (!strncmp("002013",buffer,6)) {
        BUF+=6;
    }
   
    bootMac = strtoul(BUF,NULL,16); //atol(buffer);    /*create a long value from the answer*/

    return bootMac;

}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void getTestersInitialsFromUser(char *buffer)
 */
////////////////////////////////////////////////////////////////////////////
void getTestersInitialsFromUser(char *buffer)
{
    if (!buffer)
    {
        return;
    }

    char Answer[NAME_LENGTH]; 
    // prompt the user
    promptUser("What are the tester's initials?", Answer, NAME_LENGTH); 
    if (strlen( BoardInfo.tester ) > 0) // means we got it from the client
    {
        strcpy(Answer,BoardInfo.tester);
    }
    while(!isValidTesterInitials(Answer))
    {
        promptUser("Tester's initials be at least one character and at most three characters. Please enter the tester's initials", Answer, NAME_LENGTH); 
    }
    
    strcpy(buffer,Answer);
}


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void getBomRevisionFromUser(char *buffer)
 */
////////////////////////////////////////////////////////////////////////////
void getBomRevisionFromUser(char *buffer)
{
    if (!buffer)
    {
        return;
    }

    char Answer[NAME_LENGTH]; 
    memset(Answer,0x00,NAME_LENGTH);
    // prompt the user
    promptUser("What is the BOM revision?", Answer, NAME_LENGTH); 

    if (BoardInfo.finishedGoodRev != '0' && BoardInfo.finishedGoodRev != 0) // means we got it from the client
    {
        Answer[0] = BoardInfo.finishedGoodRev;
    }
    if (strlen(Answer)!=1)
    {
        promptUser("BOM revision must be a single character, please enter a BOM revision", Answer, NAME_LENGTH); 
    }
    *buffer = Answer[0];
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void getBoardNameFromUser(char *buffer)
 */
////////////////////////////////////////////////////////////////////////////
void getBoardNameFromUser(char *buffer)
{
    if (!buffer)
    {
        return;
    }

    char Answer[BUF_LEN]; 

    promptUser("What is this Board's Name?", Answer, BUF_LEN); 

    strcpy(buffer,Answer);
}

/***********************************************************************************
                            MISCELLANEOUS FUNCTIONS:
************************************************************************************/


/***********************************************************************************
                            Verification FUNCTIONS:
************************************************************************************/


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     short isValidSerialNumber(char *serial)
 */
////////////////////////////////////////////////////////////////////////////
short isValidSerialNumber(char *serial)
{
    if(strlen(serial)!=8 )
    {
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     short isValidFinishedGoodNumber(char *fg)
 */
////////////////////////////////////////////////////////////////////////////
short isValidFinishedGoodNumber(char *fg)
{
    if(strlen(fg)!=9 )
    {
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     short isValidTesterInitials(char *initials)
 */
////////////////////////////////////////////////////////////////////////////
short isValidTesterInitials(char *initials)
{
    while(strlen(initials)>=4 || strlen(initials) == 0 )
    {
        return FALSE;
    }
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     short getBoardAttempt(ulong FinishedGoodNumber, ulong SerialNumber, short *type,short *sequence,short *testType, ulong *secondsToExecute)
 */
////////////////////////////////////////////////////////////////////////////
short getBoardAttempt(ulong FinishedGoodNumber, ulong SerialNumber, short *type,short *sequence,short *testType, ulong *secondsToExecute)
{
    // create buffer for attempt list
    char buffer[BUF_LEN*3]="";

    // get the attempt list string for the associated FGNumber
    if(databaseGetBoardAttemptList(&FinishedGoodNumber, buffer))
    {
        
        IPCPACKET queryPacket = {};

        IPCPACKET userResponse = {};
    
        queryPacket.data.releaseListener=FALSE;

        // copy buffer into packet buffer
        strcpy(queryPacket.data.info,buffer);

        // we won't need a response, so set the response pointer to NULL
        memset(queryPacket.data.response,0x00,BUF_LEN);

        // the packet will be going from the server to the client
        queryPacket.mtype=(IPCSERVERTOCLIENT|IPCGETATTEMPTFROMUSER);

         // create key baesd on QUESTION queue
        key_t mykey = TEST_EXECUTE_QUESTION_QUEUE;

        // create queue for question
        ipcMakeKey(&mykey); 


        
        mykey = TEST_EXECUTE_QUESTION_QUEUE;


        int userQid=0;

        // now, open queue (open sesame...get it? funny, huh?)

        ipcOpenQueue(mykey, &userQid);


        mykey = TEST_DISPATCHER_PROJECT_ID;

        // get Queue ID for question queue
        int qid=ipcGetKey(mykey);

        // send question packet
        ipcSendMessage(qid,&queryPacket,sizeof(IPCPACKET));


        mykey = TEST_EXECUTE_QUESTION_QUEUE;

        // get Queue ID for question queue
        userQid=ipcGetKey(mykey);

        // read QUESTION queue and block until a message is present

        
        int err = ipcReadMessage(userQid, 0, (struct IPCPACKET*)&userResponse,sizeof(IPCPACKET), TRUE); //block until we get one 

        // remove question queue in case another program wishes to ask
        // a question
        ipcRemoveQueue(userQid);

        // create pointer to user's response, so we can find the desired attempt
        char *attemptString = userResponse.data.response;

        int previousType = 0;
        short attempt = 0;
        int localType = 0;
        unsigned long seconds=0;

        sscanf (attemptString, "%i:%i:%i:%lu",&previousType,&localType,&attempt,&seconds);

        *secondsToExecute = seconds;

        if (previousType == 0) {
            // call databaseGetSequenceForAttempt to obtain the last sequence
            // used with this test
            databaseGetSequenceForAttempt(&FinishedGoodNumber,&attempt,sequence);
        }
        

        // call databaseGetTypeForAttempt to get the type of test 
        databaseGetTypeForAttempt(FinishedGoodNumber,attempt,type);

        *testType  = localType;
        
        
        // return attempt
        return attempt;

        
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void saveTestConfig(char *locationToSave, ulong *fgNumber, short *attempt)
 */
////////////////////////////////////////////////////////////////////////////

void saveTestConfig(char *locationToSave, ulong *fgNumber, short *attempt)
{
    // get configuration data for fg number and attempt from the db
    char *configData = databaseGetConfigurationData(fgNumber,attempt);
    // create character array so we can remove the comments from
    // configData
    if ( configData != NULL)
    {
        char *configDataWithoutComments = (char*)malloc(strlen(configData));
        short i=0,j,cnt=0;
        for (; i < strlen(configData); i++)
        {
            if (i+1 < strlen(configData))
            {
                // locate block comments
                if ( configData[i] == '/' && configData[i+1] == '*')
                {
                        // traverse the data file until we find 
                        // the closing block comment
                        for (j=i; j < strlen(configData); j++)
                        {
                            if ( j+1 < strlen(configData))
                            {
                            
                                if ( configData[j] == '*' && configData[j+1] == '/')
                                    break;
                            }
                        }
                        i=j+2;
                }
                // locate the single line comments
                else if ( configData[i] == '/' && configData[i+1] == '/')
                {
                    // traverse until we find the end of the line
                        for (j=i; j < strlen(configData); j++)
                        {
                            if (configData[j] == 0x0a || configData[j] == 0x0d)
                                break;
                        }
                        i=j;
                }
                configDataWithoutComments[cnt++]=configData[i];
            }
            else
            {
                configDataWithoutComments[cnt++]=configData[i];
            }
                
        }
        configDataWithoutComments[cnt-1] = 0x00;
        free(configData); // free the buffer when we are finished with it
        
        configData = configDataWithoutComments;
        // open the file
        FILE *configFile = fopen(locationToSave,"w+");
        if (configFile!=NULL) // make sure we were able to open/create it
        {

            int size = strlen(configData);
            // write the configuration data to the file
            int writeAmount = fwrite(configData,1,size,configFile);
            if ( writeAmount != size)
            {
                consolePrint("Could not write configuration file; wrote %i of %i bytes\n",writeAmount,size);
            }

            // close file
            fclose(configFile);
        }
        free(configData); // free the buffer when we are finished with it
        
    }
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void initializeBoardAndState(PBOARDSTATE boardStatusAndState)
 */
////////////////////////////////////////////////////////////////////////////
void initializeBoardAndState(PBOARDSTATE boardStatusAndState)
{
    boardStatusAndState->percentComplete=0;
    boardStatusAndState->testType=0;
    boardStatusAndState->attempt =0;
    boardStatusAndState->accessLevel=0;
    boardStatusAndState->currentSequence=-1;
    memset(boardStatusAndState->boardInfo.tester,0x00,TESTER_NAME_LENGTH);
    boardStatusAndState->boardInfo.bootMacNumber=0;
    boardStatusAndState->boardInfo.finishedGoodRev='0';
    memset(boardStatusAndState->boardInfo.boardName,0x00,BOARD_NAME_LENGTH);
    boardStatusAndState->loaded=TRUE;
    boardStatusAndState->passing =TRUE;

}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     short getRepairTestLevel(TESTPARAMETERS **cachedTests,int cacheSize);
 */
////////////////////////////////////////////////////////////////////////////
short getRepairTestLevel(TESTPARAMETERS **cachedTests,int cacheSize)
{
    if (cachedTests == NULL || cacheSize ==0)
    {
        return FALSE;
    }

    char buffer[BUF_LEN*3]="";
    short i;
    for (i=0; i < cacheSize; i++)
    {
        if (i > 0)
            strcat(buffer,"@");
        strcat(buffer,cachedTests[i]->Test);
    }
    IPCPACKET queryPacket = {};

    IPCPACKET userResponse = {};

    queryPacket.data.releaseListener=FALSE;

    // copy buffer into packet buffer
    strcpy(queryPacket.data.info,buffer);

    // we won't need a response, so set the response pointer to NULL
    memset(queryPacket.data.response,0x00,BUF_LEN);

    // the packet will be going from the server to the client
    queryPacket.mtype=(IPCSERVERTOCLIENT|IPCGETREPAIRUSERLIST);

     // create key baesd on QUESTION queue
    key_t mykey = TEST_EXECUTE_QUESTION_QUEUE;

    // create queue for question
    ipcMakeKey(&mykey); 


    
    mykey = TEST_EXECUTE_QUESTION_QUEUE;


    int userQid=0;

    // now, open queue (open sesame...get it? funny, huh?)

    ipcOpenQueue(mykey, &userQid);


    mykey = TEST_DISPATCHER_PROJECT_ID;

    // get Queue ID for question queue
    int qid=ipcGetKey(mykey);

    // send question packet
    ipcSendMessage(qid,&queryPacket,sizeof(IPCPACKET));


    mykey = TEST_EXECUTE_QUESTION_QUEUE;

    // get Queue ID for question queue
    userQid=ipcGetKey(mykey);

    // read QUESTION queue and block until a message is present

    
    int err = ipcReadMessage(userQid, 0, (struct IPCPACKET*)&userResponse,sizeof(IPCPACKET), TRUE); //block until we get one 

    // remove question queue in case another program wishes to ask
    // a question
    ipcRemoveQueue(userQid);

    // create pointer to user's response, so we can find the desired attempt
    char *attemptString = userResponse.data.response;

    
   
    char *pch = strtok (attemptString,"@");
    i=0;
    while (pch != NULL)
    {
        if (!strcmp((pch+1),cachedTests[i]->Test))
        {
            if (pch[0] == 0)
                cachedTests[i]->activeSequence=FALSE;
        }
        else
            return FALSE;
        pch = strtok (NULL, "@");
        i++;
    }
        

    
    
    // return attempt
    return TRUE;
}
