///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDCommandHandler.c
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
#include "TDCommandHandler.h"
#include "TDServer.h"
#include "../CommonLibrary/IPCFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
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
#include "encryption/mymd5.h"
#include "encryption/rijndael_encryption.h"




/*! \file
	\brief Definitions for the functions involved with analyzing and responding
    to packets send to and received from the Test Dispatcher
*/

// *TODO* some of these functions were copied from michael lindsey's server.c,
// as such need to be cleaned up a bit since server.c was a test...
// but they work, so that's all that matters right now


static void getMD5String (char *string,char buffer[33])
{
	if (!string)
		return;
 	MD5_CTX context;
  	unsigned char digest[16];
  	unsigned int len = strlen (string);

  	MD5Init (&context);
  	MD5Update (&context, string, len);
  	MD5Final (digest, &context);
	
	sprintf(buffer,"%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x",
	digest[0],digest[1],digest[2],digest[3],digest[4],digest[5],digest[6],digest[7],
	digest[8],digest[9],digest[10],digest[11],digest[12],digest[13],digest[14],digest[15]);
}

/************************************************************************************
*
*	userRespondToQuestion
*
*	Function which handles user's response to the question
*      
*	Arguments:
*
*       char* data -- command
*
*	Return Value:
*
*		TRUE/FALSE/DEFER
*
*************************************************************************************/
short userRespondToQuestion(char *data)
{
    repairCounter = 0;

    // lock mutex so we'll have exclusive access to the question
    pthread_mutex_lock(threadHandler);
    
    // okay, analyze our packet
    if (!question)
    {

        pthread_mutex_unlock(threadHandler);
        return FALSE;
    }
    data+=11;

    char *q = data;
    // allocate enough memory in myQuestion for the data
    char *myQuestion = (char*)malloc( strlen(data)*sizeof(char));
    int i=0;
    /* now, we traverse the data packet. This allows us to copy
     * the question from the response and compare it to our question
     * to make sure they are equal, and user is answering the current
     * question
     */
    while (*q)
    {
        myQuestion[i] = *q;
        i++;
        q++;
        if (*q == '|')
        { // search for the pipe symbol -- |
            q++;
            break;
        }
    }
    myQuestion[i] = 0x00; // end with null terminated character

    
    if (!strcmp(myQuestion,question))
    {

        // if the question from the packet and our question are equal
        // we send a message to our question queue saying that we have
        // a response
        key_t mykey = TEST_EXECUTE_QUESTION_QUEUE;
        int userQid=ipcGetKey(mykey);
        IPCPACKET userResponse={};
        
        if (!strncmp(q,"defer",5))
        {
            
            pthread_mutex_unlock(threadHandler);
            return DEFER;
        }

        free(question); // deallocate memory for our question

        question=NULL; // set our question to NULL

        // incomingPacket.data.response = (char*)malloc(10*sizeof(char));
        snprintf(userResponse.data.response,BUF_LEN,"%s",q);

        userResponse.mtype=1;
        userResponse.data.releaseListener=FALSE; // prevent queue from closing
        // send response to question queue
        int value = ipcSendMessage(userQid,(struct IPCPACKET*)&userResponse,sizeof(IPCPACKET));

        pthread_mutex_unlock(threadHandler);

        return TRUE;

    }
    // release the mutex
    pthread_mutex_unlock(threadHandler);
    return FALSE;
}

void requestToClearData(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char type,int new_fd)
{
    char sendBuffer[15];
    memset(sendBuffer,0x00,15);
    char length[11];
    sprintf(length,"%X",2);
    prefixExpand(length,5);
    strncat(sendBuffer,length,5);
    

    if (type == '1')
        {
            strcat(sendBuffer,"TSTCLR");
        }
        if (type == '2')
        {
            strcat(sendBuffer,"DIACLR");
        }
        int len = strlen(sendBuffer);
        sendBuffer[len++]=0x0a;
        sendBuffer[len++]=0x0a;
        sendBuffer[len++]=0x0a;

        if (sendall(new_fd, sendBuffer, &len) == -1)
        {
            consolePrint("error at command handler 304");
            perror("sendall");
            return;
        }
        

}


/************************************************************************************
*
*	send_test_clear
*
*	Tells client to clear all of their test data
*      
*	Arguments:
*
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*       int new_fd - file descriptor       
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void send_test_clear(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,int new_fd)
{
    
    char buffer[] = {'T','S','T','C','L','R',0x0d,0x0a,0x0a,0x0a,0};
    int len = sizeof(buffer);
    sendall(new_fd, buffer, &len);

}

/************************************************************************************
*
*	user_authenticate
*
*	Attempts to authenticate a user via a UDP message
*      
*	Arguments:
*
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*       uint thread - user's thread
*       uint fd - file descriptor  
*       char * data - data from user
*       char *myReturn - return buffer     
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void user_authenticate(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,uint thread, uint fd,char *data ,char *myReturn)
{
    short loginStatus=0;
    char *encryptedPassword=NULL;
    // data should be username|password. If it is not, then we have a problem
    if (!data)
    {
        goto authenticateReturnStatus;
    }

    char *username=NULL,*password=NULL;
    if (strlen(data) <= 6)
    {
        goto authenticateReturnStatus;
    }
    char *tempData = data;
    short iterator=0;
    short iteratorStart=0;
    if ( !strncmp(data,"USRLOG",6))
    {
        tempData+=6;
        iteratorStart=6;
    }

    

    while(*tempData)
    {
        if (*tempData == '|')
            break;
        iterator++;
        tempData++;
    }
    username = (char*)malloc(iterator+1);
    strncpy(username,data+iteratorStart,iterator); // copy the username into username
    username[iterator] = 0x00;

    data+=iterator+1+iteratorStart;
    iteratorStart=iterator=0;
    char size[25]="";
    tempData=data;
    while(*tempData)
    {
        if (*tempData == '|')
            break;
        iterator++;
        tempData++;
    }

    
    strncpy(size,data+iteratorStart,iterator); // copy the username into username

    data+=iterator+1+iteratorStart;



    //data; // should be the password

    char keyMaker[33]="",key[33]="";

    sprintf(keyMaker,"V%.2f",serverVersion);

    getMD5String(keyMaker,key);


    createKey(key);

    char *buffer = (char*)malloc(atoi(size)+1);

    encryptedPassword=data;

    short decryptSize = decryptText(data,atoi(size),buffer);


    password = buffer;

    loginStatus = databaseIsValidUsernamePassword(username,password);

    short userAccessLevel=PRODUCTION_ACCESS_LEVEL;
    if ( ( userAccessLevel = databaseGetUserAccessLevelFromUserName( username ) ) == PRODUCTION_ACCESS_LEVEL )
    {
        loginStatus=0;
    }


authenticateReturnStatus:

    if (username)
    {
        free(username);
    }

    if (password)
    {
        free(password);
    }
    

    sprintf(myReturn,"00001USRLOG%i%i%s",loginStatus,userAccessLevel,encryptedPassword);
    

}

/************************************************************************************
*
*	send_test_data
*
*	Function which handles user's response to the question
*      
*	Arguments:
*
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*       char type  - type of data to send user
*       int *startLocation - pointer to structure which indicates where to begin sending test data 
*		uint end - end of test data structure
*		int new_fd - connection's file descriptor
*		char *bcBuffer - data buffer
*		test *array - array which contains test data 
*		char *myReturn - return buffer
*
*
*	Return Value:
*
*		short - TRUE or FALSE
*
*************************************************************************************/
short send_test_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char type,int *startLocation,uint end, int new_fd, char *bcBuffer,test *array ,char *myReturn)
{


       // buffer for file data
    unsigned char fileData[2000]="";
    char msg[2100] = "";    //array for length + destination + data
    char buff[512]= ""; //receiving array
    short i =0;

    int size = 0;


    if (*startLocation>=end)
    {
        return ERROR;
    }

    // lock main thread mutex. This allows us to have exclusive 
    // access to array
    pthread_mutex_lock(threadHandler);

    // obtain the size of all values in the array, which we will
    // send to the user
    int amount = end-*startLocation;

    size = amount*(MAX_TEST_LENGTH+(LINE_BUF-MAX_TEST_LENGTH)+2)+amount*11+amount;

    // buffer pointer
    char *buffer= NULL;

    
    // dynamically allocate enough memory in buffer 
    buffer = (char*)malloc((size)*sizeof(char));

    if (buffer == NULL)
        exit(1);

    memset(buffer,0x00,size);

    
    unsigned int loc = 0;
    int j=0;
    // iterate through all of the test data
    char *status=NULL;
    int statusLength = (LINE_BUF-MAX_TEST_LENGTH)+2;
    int testLength = MAX_TEST_LENGTH+2;
    loc=0;
    for (i =*startLocation; i < end; i++,(*startLocation)++)
    {
        
        
        
        // first, place the type before the test data
        // this will tell the client if the data is test
        // printout data, or diagnostic data
        if (strlen(array[i].testData) == 0)
        {
            continue;
        }


        
            status = (char*)malloc(statusLength+3);
            
            memset(status,0x00,statusLength+3);
//            if (strlen(array[i].status) > 0)
                status[0] = '|';
                            
                status[1] = 0x00;
                strcat(status,array[i].status);
        
        
//        loc+=strlen(array[i].testData);


        char *sendBuffer = (char*)malloc(strlen(status) + strlen(array[i].testData) + 14);
        if (sendBuffer==NULL)
        {
            exit(1);
        }
        memset(sendBuffer,0x00,strlen(status) + strlen(array[i].testData) + 14);
        // static array which will hold the size of the buffer we will send the user
        char length[10]="";
    
        // print size into length array
        if (i == *startLocation)
        {
            sprintf(length,"%X",amount);
        }
        else
            sprintf(length,"%X",statusLength + testLength + 11);
    
        // expand this array to five characters with zeros prepended
        prefixExpand(length,5);
        strncat(sendBuffer,length,5);
        if (type == '1')
        {
            strcat(sendBuffer,"TSTDAT");
        }
        if (type == '2')
        {
            strcat(sendBuffer,"DIADAT");
        }

        if (type == '3')
        {
            
            strcat(sendBuffer,"TSTUPD");
        }
        if (type == '4')
        {
            strcat(sendBuffer,"DIAUPD");
        }
        if (array[i].testData[ strlen(array[i].testData)-1 ] == '\n')
        {
            array[i].testData[ strlen(array[i].testData)-1 ] = 0x00;
        }
        strcat(sendBuffer,array[i].testData);
        strcat(sendBuffer,status);
        loc += strlen(sendBuffer);// compute size
        strcat(buffer,sendBuffer);
         
    
        
        // free our memory
        free(sendBuffer);
        free(status);
       
        
    }
    // now, we create a command and total length
    char totalLength[10];
    sprintf(totalLength,"%X",loc);
    prefixExpand(totalLength,5);
    // move the buffer over a bit
    memmove(buffer+11,buffer,strlen(buffer));
    for (i=0; i < 5; i++)
    {
        buffer[i] = totalLength[i];
    }
    buffer[i++] = 'T';
    buffer[i++] = 'S';
    buffer[i++] = 'T';
    buffer[i++] = 'D';
    buffer[i++] = 'A';
    buffer[i++] = 'T';
    
    int len = loc+11;
          
        // send all of the data to the provided file descriptor
        if (sendall(new_fd, buffer, &len) == -1)
        {
            pthread_mutex_unlock(threadHandler);
            // free buffer, as an error has occurred
            free(buffer);
            return ERROR;
        }
    // unlock the mutex as we are finished with the test data structure
    // this only prevents data from being added and possibly corrupted
    // by the multiple reads ( since we are using pointers for the TESTCOUNT )

    pthread_mutex_unlock(threadHandler);

    // assign the length of buffer to len
    

    
    /*
    char endTransmission[3]="";
    endTransmission[0] = 0x0a;
    endTransmission[1] = 0x0a;
    endTransmission[2] = 0x0a;
    len = 3;
    if (sendall(new_fd, endTransmission,&len ) == -1)
    {
        return;
    }
    */
     // set the value of the startLocation pointer to i                        
    
    free(buffer); // free buffer
    return 0;
}


/************************************************************************************
*
*	send_filesystem_data
*
*	Depending on the command in bcBuffer, will either return a list of files or
*	data from a specific file
*      
*	Arguments:
*
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*		int new_fd - connection's file descriptor
*		char *bcBuffer - data buffer
*		char *myReturn - return buffer
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void send_filesystem_data(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int new_fd, char *bcBuffer, char *myReturn)
{
    // test the command in bcBuffer. If the command ever changes
    if (memcmp(bcBuffer, "00000LFILES", 11) ==0 )
        send_file_information(their_addr,bc_addr,myinaddr,new_fd,bcBuffer,myReturn);
    else
        send_file(their_addr,bc_addr,myinaddr,new_fd,bcBuffer,myReturn);
}


/************************************************************************************
*
*	send_file_information
*
*	Returns a list of files within /var/log
*      
*	Arguments:
*
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*		int new_fd - connection's file descriptor
*		char *bcBuffer - data buffer
*		char *myReturn - return buffer
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void send_file_information(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int new_fd, char *bcBuffer, char *myReturn)
{
    char fileList[800]="", length[6]="00000", message[810]="";

    addFileNames(fileList,1);

    unsigned short listLen = strlen(fileList);

    //put the length of the array into the protocol format
    sprintf(length,"%X",listLen);
    prefixExpand(length,5);

    short i=0;
    //copy length into message
    for (i=0;i<5;i++)
        message[i] = length[i]; 

    unsigned char dest[6]="LFILES"; //put destination LFILES for "list files"
    for (i=0;i<6;i++)
        message[5+i] = dest[i]; 

    //copy the data to the message
    for (i=0; i<strlen(fileList);i++)
        message[11+i] = fileList[i];


    //send the information to the client
    int len = strlen(message);
    if (sendall(new_fd, message, &len) == -1)
    {
    }



}

/************************************************************************************
*
*	send_file
*
*	Sends file data through tcp socket
*      
*	Arguments:
*
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*		int new_fd - connection's file descriptor
*		char *bcBuffer - data buffer
*		char *myReturn - return buffer
*
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void send_file(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int new_fd, char *bcBuffer, char *myReturn)
{


    char *readBuffer = bcBuffer;

  

    unsigned char fileData[2000]="";
    //char msg[2100] = "";    //array for length + destination + data
    char buff[512]= ""; //receiving array

    char file[30]="";
    char fileName[40] = "/var/log/";
    
    short i=0;
    for (i=0;i< strlen(readBuffer) - 11;i++)
        file[i] = readBuffer[11+i];  //copy the file name without the protocol header
    short startFile = 9;
    if (file[0] == '/')
    {
        memset(fileName,0x00,40);
        startFile=0;
    }
    
    for (i=0;i<strlen(file);i++) //get the filename and add it to the "/var/log/" prefix
    {
        fileName[startFile+i] = file[i];
    }
    consolePrint("Opening file %s\n",fileName);

    unsigned int dataLen = getFileByteCount(fileName);  //get byte count

    // warn the user that we cannot read the file. This could indicate that a file
    // no longer exists


    int dataCompare = dataLen; //used in sending to compare read chars VS chars in file

    char datLenArray[10] = "00000";
    memset(datLenArray,'0',9);
    datLenArray[9] = 0x00;

    sprintf(datLenArray,"%X",dataLen);

    if (dataLen==-1)
    {
            /*
        sprintf(fileData,"File %s no longer exists!",fileName);
        char *sendBuffer = (char*)malloc(strlen(fileData)+15);
        sprintf(datLenArray,"%X",strlen(fileData));
        prefixExpand(datLenArray,5);
        sprintf(sendBuffer,"%sBFILES%s",datLenArray,fileData);
        int rd=strlen(sendBuffer)-1;
        sendBuffer[rd] = 0x0a;
        sendBuffer[rd++] = 0x0a;
        sendBuffer[rd++] = 0x0a;
        if (sendall(new_fd, sendBuffer,&rd ) == -1)
            {
                consolePrint("error on 546");
                free(sendBuffer);
                perror("sendall");
                return;
            }
        free(sendBuffer);
        */
        sprintf(fileData,"INFOFile %s no longer exists!",fileName);
        char *sendBuffer = (char*)malloc(strlen(fileData)+15);
        sprintf(datLenArray,"%X",strlen(fileData));
        prefixExpand(datLenArray,5);
        sprintf(sendBuffer,"%sMSGBOX%s",datLenArray,fileData);
        int rd=strlen(sendBuffer)-1;

        sendBuffer[rd] = 0x0a;
        sendBuffer[rd++] = 0x0a;
        sendBuffer[rd++] = 0x0a;
        if (sendall(new_fd, sendBuffer,&rd ) == -1)
            {
                consolePrint("error on 546");
                free(sendBuffer);
                perror("sendall");
                return;
            }


        free(sendBuffer);
        
        return;
    }

    

    dataLen+=3;

    //put buffer size in an array in HEX notation
    prefixExpand(datLenArray,5);

    dataLen = dataCompare; //dataLen is 0 from the above loop. Give it back it's correct value

                   


    FILE * pFile;
    char * buffer;

    pFile = fopen ( fileName , "rb"  );

    if (!pFile)
    {
        consolePrint("file gone\n");
        return;
    }

    unsigned char firstBuffer = 1; //flag for putting protcol info in first 'send'
    int j=0;
    // continue while we haven't reached the end of file
    char binaryFlag = 0;
    char *binaryFlagName = "board.info";
    if (find(fileName,binaryFlagName) >= 0)
    {
        binaryFlag=1;
    }
    if (!feof(pFile))
    {

        // read size of fileData into fileData buffer

        fseek (pFile , 0 , SEEK_END);
        int lSize = ftell (pFile);
        rewind (pFile);
        char *sendBuffer = (char*)malloc(lSize+11+4);
        if (!sendBuffer)
        {
            return;
        }
        if (lSize == 0)
        {
            return;
        }

        
        
        
        int rd = fread (sendBuffer,1,lSize,pFile);
        if (binaryFlag)
        {
            short z=0;
            for (; z < rd; z++)
                sendBuffer[z]+=1;
        }
        sendBuffer[rd] = 0x0a;
        sendBuffer[rd++] = 0x0a;
        sendBuffer[rd++] = 0x0a;
        
        //char *msg = (char*)malloc(
        memmove(sendBuffer+11,sendBuffer,rd);
        for (i=0;i<5;i++)  //put length at beginning of the msg array
        sendBuffer[i] = datLenArray[i];

        unsigned char dest[6]="BFILES"; //put destination 
        for (i=0;i<6;i++)
            sendBuffer[5+i] = dest[i];  
        rd+=11;
            if (sendall(new_fd, sendBuffer,&rd ) == -1)
            {
                consolePrint("error on 546");
                perror("sendall");
                return;
            }
            free(sendBuffer);

    }
    fclose (pFile); // close the file and release the resources
    


}

/************************************************************************************
*
*	default_broadcast
*
*	This is the default broadcoast which occurs when the board is present within
*   the user's local board list
*      
*	Arguments:
*		PBOARDSTATE board - pointer to current board's information
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*		int new_fd - connection's file descriptor
*		char *bcBuffer - data buffer
*		char *myReturn - return buffer
*
*
*	Return Value:
*
*		short success - TRUE/FALSE/ERROR
*
*************************************************************************************/
short default_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
{
    // if the pointer to our board info is empty, exit
    if (board==NULL)
    {
        return FALSE;
    }
    // if our board hasn't been loaded yet, do not provide
    // information to the client
    if ( board->loaded==FALSE ) //|| board->loaded==TEMP)
        return FALSE;


    // if our buffer is null or empty, return
    if (bcBuffer == NULL || strlen(bcBuffer)==0)
    {
        return FALSE;
    }
    
    handleDefaultMessage=TRUE;

    char ipAddress[16]=""; // array for our ip address
    char Serial[9]=""; // array for our serial number
    long mySerial = 0,myFg=0;
    char FGNumber[10]=""; // array for our FG Number
    char FGRev=' '; // array for our FG Revision

    char initials[TESTER_NAME_LENGTH]; // array for tester's initials

    memset(initials,0x00,TESTER_NAME_LENGTH);
    memset(FGNumber,0x00,10);
    memset(ipAddress,0x00,16);
    memset(Serial,0x00,9);
    short i=0;

    // traverse the buffer, attempt to locate our IP address
    for (; i < 15 && *bcBuffer; i++ )
    {
        if (*bcBuffer=='|')
        {
            bcBuffer++;
            break;
        }

        ipAddress[i]=*bcBuffer;
        bcBuffer++;
    }
    ipAddress[i]='\0';

    // check our ip address. make sure it is not empty
    // and ensure it following the proper ip format
    if (strlen(ipAddress) == 0 || inet_addr(ipAddress)==-1)
    {
        return ERROR;
    }

    // now, attempt to locate the Serial Number which is located before
    // the pound sign (#)
    for (i=0; i < 8 && *bcBuffer; i++ )
    {
        if (*bcBuffer=='#')
        {
            bcBuffer++;
            break;
        }

        Serial[i]=*bcBuffer;
        bcBuffer++;
    }
    // null terminate our seial number
    Serial[i] = 0x00;
    if (*bcBuffer=='#')
        bcBuffer++;


    // check to see if the Serial Number found is valid, and that
    // we need a serial number
    if (atol(Serial) > 0 && board->boardInfo.serialNumber == 0)
    {

        // if so, assign our board the serial number provided by
        // the user
        board->boardInfo.serialNumber = atol(Serial);
        setSerialNumber(&board->boardInfo.serialNumber);
        // notify the user to close his/her message box
        // if they are in the processes of providing this information
        notifyUserToCloseMessageBox();
    }

    // the next data we can pull from our command buffer is
    // the FGNumber
    for (i=0; i < 9 && *bcBuffer; i++ )
    {
        if (*bcBuffer=='|')
        {
            bcBuffer++;
            break;
        }

        FGNumber[i]=*bcBuffer;
        bcBuffer++;
    }
    FGNumber[i] = 0x00;
    if (*bcBuffer=='|')
        bcBuffer++;

    // most likely, we will not need the FinishedGoodNumber, but
    // if we do, then assign it to the local board structure
    
    if (atol(FGNumber) > 0 && board->boardInfo.finishedGoodNumber == 0)
    {

        board->boardInfo.finishedGoodNumber = atol(FGNumber);

        setFinishedGoodNumber(&board->boardInfo.finishedGoodNumber);
        // notify the user to close his/her message box
        // if they are in the processes of providing this information
        notifyUserToCloseMessageBox();
    }

    // the next character should be the Finished Good Revision
    FGRev = *bcBuffer;

    bcBuffer+=2;


    // if FGRev is not 0, and we need a FinishedGoodRevision, 
    // assign it to our local board's finished good revision
    if (FGRev!='0' && board->boardInfo.finishedGoodRev == '0')
    {


        board->boardInfo.finishedGoodRev = FGRev;
        setBomRevision(&board->boardInfo.finishedGoodRev);
        // notify the user to close his/her message box
        // if they are in the processes of providing this information
        notifyUserToCloseMessageBox();
    }
    int j=0;
    // finally, the Initials will be available following a series
    // of Zeros, which can be used later for other data
    for (i=0; *bcBuffer; i++ )
    {
        if (*bcBuffer!='0')
        {

            initials[j]=*bcBuffer;
            j++;
        }

        bcBuffer++;
    }
    initials[j] = 0x00;

    // assign the initials only if we need it
    if (strlen(initials) > 0 && strlen(board->boardInfo.tester) == 0)
    {
        strcpy(board->boardInfo.tester,initials);
        setTestersInitials(board->boardInfo.tester);
        notifyUserToCloseMessageBox();

    }
    else if (strlen(initials) ==0 && strlen(board->boardInfo.tester) == 0)
    {
        memset(board->boardInfo.tester,0x00,TESTER_NAME_LENGTH);
    }


    bc_addr->sin_family = AF_INET;     // host byte order
    bc_addr->sin_port = htons(BROADCAST_PORT); // short, network byte order

    //take char array & change format so it can be assigned to s_addr
    myinaddr->s_addr = inet_addr(ipAddress);

    bc_addr->sin_addr = *myinaddr; 
    memset(&(bc_addr->sin_zero), '\0', 8); // zero the rest of the struct

    char serial[8];
    char buffer[BUF_LEN];
    // place the board's serial number into an array and expand it to 
    // eight characters if we need to
    sprintf(serial,"%lu",board->boardInfo.serialNumber);
    prefixExpand(serial,8);
    
    // the percent complete will originate from the board's current status
    unsigned short percent = board->percentComplete;
    char pcnt[3]; 
    memset(pcnt,0x00,3);
    snprintf(pcnt,3,"%X",percent); // convert the percentage to HEX

    memset(buffer,0x00,BUF_LEN);
    prefixExpand(pcnt,2);
    pcnt[2]=0x00;
    // create buffer comma
    char status[7];
    if (board->passing==TRUE)
    {
        strcpy(status,"Passing");
    }                            
    else
        strcpy(status,"Failing");

    snprintf(buffer,BUF_LEN,"%lu|%c|%s%s|%s|%s|%.2f",board->boardInfo.finishedGoodNumber,board->boardInfo.finishedGoodRev,serial,board->boardInfo.tester,status,pcnt,serverVersion);
    
    char length[10]="";


    //put the length of the array into the protocol format
    sprintf(length,"%X",strlen(buffer));
    //printf("oh yeah\n");
    prefixExpand(length,5);

    /* copy the buffer and the command into myReturn. 
     * We use snprintf, in case our command exceeds the size of 
     * myReturn, which at this point is 1024...I really don't
     * like statically sized array
     */
    snprintf(myReturn,1024,"%sDFAULT%s",length,buffer);

    // everything should be fine at this point
    handleDefaultMessage=FALSE;
    return TRUE;

}

/************************************************************************************
*
*	initial_broadcast
*
*	Handles the intial broadcast message which notifies the user that the board is 
*	being tested. If the board info is null, we don't yet notify the user that Test 
*	Dispatcher is active.
*      
*	Arguments:
*		PBOARDSTATE board - pointer to current board's information
*       socketAddress *their_addr - user's socket structure
*       socketAddress *bc_addr - broadcast socket structure
*       socketAddress *myinaddr - tcp socket structure
*		int new_fd - connection's file descriptor
*		char *bcBuffer - data buffer
*		char *myReturn - return buffer
*
*
*	Return Value:
*
*		short success - TRUE/FALSE/ERROR
*
*************************************************************************************/

////////////////////////////////////////////////////////////////////////
/* 
 * @fn    short initial_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
 */ 
////////////////////////////////////////////////////////////////////////
short initial_broadcast(PBOARDSTATE board,socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr, int fd, char *bcBuffer, char *myReturn)
{

    // board info is null, so exit
    if (board==NULL)
    {
        return FALSE;
    }
    // the board data is not loaded, so we must again exit
    if ( board->loaded==FALSE )
        return FALSE;

    // crate structure for my local IP address
    char destIPAddr[20] = "";
    int i,j;

    char MacAddress[18] = "";

    for (j=0;j< (strlen(bcBuffer)-(34));j++)
        destIPAddr[j] = bcBuffer[17+j]; //client IP located in broadcast string after "TBS_IPCLIENT_APP "

    destIPAddr[j]='\0';

    for (j=0;(j+27)< strlen(bcBuffer) && j < 17;j++)
        MacAddress[j] = bcBuffer[27+j]; //client IP located in broadcast string after "TBS_IPCLIENT_APP "

    MacAddress[j]='\0';

    //get mac at eth0. if it's all zeros, then get eth1.
    //the buffer.ifr_name to that eth and run the ioctl function once

    struct ifreq buffer;
    memset(&buffer, 0x00, sizeof(buffer));
    strcpy(buffer.ifr_name, "eth0");
    ioctl(fd, SIOCGIFHWADDR, &buffer);

    for ( j = 0; j < 6; j++ )    //if MAC returned is all 0's, try eth1 instead
    {
        if ((unsigned char)buffer.ifr_hwaddr.sa_data[j] != 0)
            break;
        if (j==5)
        {
            strcpy(buffer.ifr_name, "eth1");
            ioctl(fd, SIOCGIFHWADDR, &buffer);
        }
    }

    //tranfer mac from buffer to the broadcast response array
    unsigned short k =0; //index of next open place in myReturn
    char test[3]="";
    for ( j = 0; j < 6; j++ )
    {
        sprintf(test, "%.2X",(unsigned char)buffer.ifr_hwaddr.sa_data[j]);
        myReturn[k] = test[0];
        myReturn[k+1] = test[1];
        if (k<15) myReturn[k+2] = ':';
        else myReturn[k+2] = '|';       //seperator after entire MAC is in buffer
        k=k+3;
    }
    char acc[2];
    sprintf(acc,"%i",databaseGetUserAccessLevel(MacAddress));
    myReturn[k]=acc[0];
    k++;
    myReturn[k]='|';
    k++;
    //add serial number to the return [Global variable]
    int l =k;
    sprintf(mySerial,"%lu",board->boardInfo.serialNumber);
    prefixExpand(mySerial,8);
    for (; k < (l+8);k++)
        myReturn[k] = mySerial[k-l];

    myReturn[k] = '|'; //add seperator. this comes after the MAC|Serial
    k++; //move to next place mac|serial|

    memcpy(myReturn+k, streamPort, 5); //copy in the streamPort array
    k+=5; 
    myReturn[k] = '_'; //add underscore before files


    //add list of interesting files from var/log to the return message
    //they will be seperated by |
    addFileNames(myReturn,0); //the second parameter is 0 for "Don't tack on the file sizes"

    bc_addr->sin_family = AF_INET;     // host byte order
    bc_addr->sin_port = htons(BROADCAST_RETURN_PORT); // short, network byte order

    //take char array & change format so it can be assigned to s_addr
    myinaddr->s_addr = inet_addr(destIPAddr);

    setMacLookup(inet_lnaof( their_addr->sin_addr ),MacAddress);

    bc_addr->sin_addr = *myinaddr; 
    memset(&(bc_addr->sin_zero), '\0', 8); // zero the rest of the struct

    //respond to broadcast with string 


    //clear buffer
    return TRUE;



}


/************************************************************************************
*
*	getCommandDecision
*
*	Analyzes the provided command and returns an integer value based on what the type
*	of command received
*      
*	Arguments:
*		char *data - command
*
*
*	Return Value:
*
*		uint - type of data received
*
*************************************************************************************/
uint getCommandDecision(char *data)
{
    // if the first three characters are TBS, this is the initial broadcast
    if (!memcmp(data,"TBS",3) )
    {
        return TBS_BROADCAST;
    }
    if (!memcmp(data,"TBS_IPCLIENT_APP",16) )
    {
        return TBS_BROADCAST;
    }
    else if (!memcmp(data,"USR",3) )
    {
        return TBS_AUTHENTICATE_USER;
    } else
    {

        char testBuffer[6]=""; // create array
        int i=0;

        // grab the six characters which represent the type of command with which
        // we are working
        for (i=0; i<6;i++)
            testBuffer[i] = data[5+i]; //read destination protocol 

        if (memcmp(testBuffer, "LFILES",6) == 0 || memcmp(testBuffer, "BFILES",6) == 0)
        {
            return TBS_TCP_GET_FILE;
        } else if (memcmp(testBuffer,"BYEBYE",6) == 0)
        {
            return TBS_EXIT;
        } else if (memcmp(testBuffer,"TSTDAX",6) == 0)
        {
            return TBS_TCP_GET_TEST_UPDATE_DATA;
        } else if (memcmp(testBuffer,"SINGLX",6) == 0)
        {
            return TBS_TCP_GET_LAST_TEST_UPDATE_DATA;
        } else if (memcmp(testBuffer,"RESPON",6) == 0)
        {
            return TBS_TCP_GET_QUESTION_RESPONSE;
        } else if (memcmp(testBuffer, "TSTDAT",6) == 0)
        {
            return TBS_TCP_GET_TEST_DATA;
        } else if (memcmp(testBuffer,"LCKBRD",6) == 0)
        {
            return TBS_TCP_REPAIR_LOCK_BOARD;
        } else
            return UNKNOWN;
    }

}




/**********************************************************
*	getFileByteCount
*		Opens a file, reads in each char in order to count
*		the number of chars in the file, and returns the count.
*
*	Arguments:
*		char myFile: array holding the file name to open
*
*	Returns:
*		int : the char count or negative on failure
*
************************************************************/

int getFileByteCount(char myFile[])
{

    int fileByteCount= -1;
    FILE * pFile;
    char c;

    // open a file
    pFile = fopen(myFile,"r");
    // if the file pointer is null
    // then we don't have an active file
    if (pFile != NULL)
    {
        // seek to the end of the file
        fseek(pFile,0,SEEK_END);
        // use ftell to tell what location
        // we are at
        fileByteCount = ftell(pFile);
        // close the file
        fclose(pFile);

    }

    return fileByteCount;

}

/**********************************************************
*	checkEmpty
*		This function is used to see if a file is empty.
*		It functions the same as getFileByteCount, but stops
*		after reading 10 chars so as not to take a very long 
*		time to check a large file
*
*	Arguments:
*		char myFile: array holding the file name to open
*
*	Returns:
*		int : the char count or a negative on failure
*
************************************************************/

int checkEmpty(char myFile[])
{
    // changed to just return getFileByteCount
    /*
     * fread executes read(), and places the data
     * into a large buffer. It's just as easy to seek to the end, but
     * to be honest, you aren't gaining anything by running fread here
     * and reading only the first ten characters as the entire file
     * has already been loaded into an internal buffer
     */
    return getFileByteCount(myFile);

}

/**********************************************************
*	addFileNames
*		When the app receives a broadcast to respond to, the
*		response is an array containing the MAC address, serial 
*		number and then  a list of the nonempty files in 
*		/var/log sperated by |. The client app parses the 
*		array and  displays the file names for the user. 
*		The array is a jumble of information, but it all needs to 
*		be returned to the client after a broadcast
*
*	Arguments:
*
*		char myArray: array that will be returned to the client
*		int addSize : 1 if list sizes along with file names
*
*	Returns:
*		none
*
************************************************************/

void addFileNames(char myArray[], int addSize)
{
    // directory variable ( from direntry)
    DIR *dp;
    struct dirent *ep;
    char temp[20], fileSize[12];
    int i;
    short k = strlen(myArray);
    char filePath[40] = "/var/log/";

    dp = opendir ("/var/log/");
    // traverse the directory if while dp is not NULL
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            memset(temp,0x00, sizeof(temp));
            memset(fileSize,0x00, sizeof(fileSize));

            memcpy(temp, ep->d_name, ep->d_reclen); //copy file name to array
            for (i=0; i<ep->d_reclen; i++)
                filePath[9+i] = temp[i];
            // only list the file if it is not empty, and is not either the previous
            // or current directory markers
            if (getFileByteCount(filePath) > 0 && ep->d_type == 0x08 && strcmp(ep->d_name,".") && strcmp(ep->d_name,"..") )
            {
                // copy the temporary name into myARray
                k = strlen(myArray);
                for (i=0; i < strlen(temp) ; i++)
                    myArray[k+i] = temp[i];

                if (addSize == 1 && (memcmp(temp, "lastlog", strlen(temp)) != 0) && (memcmp(temp, "wtmp", strlen(temp)) != 0)) //add the size of each file to the end of the name : name#2313
                {
                    myArray[strlen(myArray)] = '#'; //add # after the name
                    int bc = getFileByteCount(filePath);

                    sprintf(fileSize, "%i",bc);

                    k = strlen(myArray);
                    for (i=0; i < strlen(fileSize) ; i++)    //add file size to the array
                        myArray[k+i] = fileSize[i];
                }
                myArray[strlen(myArray)] = '|'; //add seperator between file names
            }

            memset(filePath,0x00, sizeof(filePath));
            memcpy(filePath, "/var/log/",9);

        }
        (void) closedir (dp);
    }


    return;
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void catchTimer(int sig)
 */ 
////////////////////////////////////////////////////////////////////////
void catchTimer(int sig)
{
    if (++repairCounter == 5)
    {
        repairCounter=-1;
        repairLock=0;
        consolePrint("lock released\n");
    }
    else
    {
        repairTimer(2);
    }
        
    signal(sig,(void*)catchTimer);
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int repairTimer(unsigned int secondsToWait)
 */ 
////////////////////////////////////////////////////////////////////////
unsigned int repairTimer(unsigned int secondsToWait)
{
    struct itimerval old, new;
    new.it_interval.tv_usec = 0;
    new.it_interval.tv_sec=0;
    new.it_value.tv_usec=0;
    new.it_value.tv_sec=(long int)secondsToWait;
    if (setitimer(ITIMER_REAL,&new,&old) < 0)
        return 0;
    else
    {

        return old.it_value.tv_sec;
    }
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short requestToLockBoard(socketAddress *their_addr, socketAddress *bc_addr, inaddress *myinaddr,char *data)
 */ 
////////////////////////////////////////////////////////////////////////
short requestToLockBoard(ulong ip, char *data)
{
    data +=11;
    if (data[0] == '1' && repairLock == 0)
    {
        repairLock = ip;
    }
    else if (data[0] == 0 && repairLock == ip)
    {
        repairLock = 0;
    }

    signal(SIGALRM,catchTimer);

    //repairTimer(2);

    
    consolePrint("Lock board function; repairLock owner is  %ul; ip is %ul\n",repairLock,ip);

    return TRUE;
    
}


