/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

 /*! \file
	\brief Definitions for those functions related to user interaction
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
#include <termios.h>
#include "Prompt.h"
#include "Common.h"

#include "IPCFunctions.h"
#include "StringFunctions.h"


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       int promptUser(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen)
 */
///////////////////////////////////////////////////////////////////////////////
int promptUser(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen)
{
    return createUserPrompt(QuestionBuffer,AnswerBuffer,AnswerLen,QUESTION,NULL);

}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       int createUserPrompt(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen, int extendedType, char **validInputList)
 */
///////////////////////////////////////////////////////////////////////////////
int createUserPrompt(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen, int extendedType, char **validInputList)
{

    // failure is not an option
	int success = FALSE;	//Assume Failure 
    
	char* test = NULL;

    // execute MACRO for CURRENT_TEST_MODE
    short validInput=FALSE;
    
    switch(CURRENT_TEST_MODE)
    {
	    case REMOTE_CONSOLE_MODE:		//We're in test mode, so question needs to go to the Viewer

            /*Send question to viewer here*/
            {
            
                // create packet for IPC message
                IPCPACKET queryPacket = {};

                // tell IPC listener that we're sending
                // a emssage
                queryPacket.mtype=(QUESTION|extendedType);

                // of course, set releaseListener to false, so the listener
                // doesn't misinterperet our packet
                queryPacket.data.releaseListener=FALSE;

                // place question into info
                snprintf(queryPacket.data.info,BUF_LEN,"%s",QuestionBuffer);

                // set our response packet as NULL
                memset(queryPacket.data.response,0x00,BUF_LEN);
                

                // create another empty packet. Though not really necessary
                // this helps prevent confusion
                IPCPACKET userResponse = {};

                // create key baesd on QUESTION queue
                key_t mykey = TEST_EXECUTE_QUESTION_QUEUE;

                ipcMakeKey(&mykey); 

                
                mykey = TEST_EXECUTE_QUESTION_QUEUE;


                int userQid=0;

                // open queue

                ipcOpenQueue(mykey, &userQid);



                mykey = TEST_DISPATCHER_PROJECT_ID;

                /*
                 * we call ipcGetKey to obtain the Queue ID for
                 * the question listener. this will allow us to 
                 * receive messages for the QUESTION queue
                 */
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

                // place user response into AnswerBuffer ( pointer to user's answer buffer )
                snprintf(AnswerBuffer,AnswerLen,"%s",userResponse.data.response);

                
                success=TRUE;

                
            }

            break;
	    case 0:		//if not in test mode, always output to the screen
	    default:
            {
            
                //default print to screen
                
                char appendQuestion[25]="";
    
                if (extendedType == YESNO)
                {
                    strcat(appendQuestion,"(Yes/No) ");
                }
                else if (extendedType == RETRYFAIL)
                {
                    strcat(appendQuestion,"(Retry/Fail) ");
                }
        		consolePrint("%s %s", QuestionBuffer,appendQuestion); 

                BeginQuestion:
                if (extendedType==QUESTION)
                {
                    test = fgets(AnswerBuffer, AnswerLen, stdin); //read the user's input
                }
                else
                {
                
                    AnswerLen=1;
                    AnswerBuffer[0] =  (char)linuxGetChar(); // added to allow user to enter single letter
    
                    if (AnswerBuffer[0] != EOF)
                    {
                        success = TRUE;
                    }
    
                    fflush(stdin);
                    
                    AnswerBuffer[1] = 0; // added to allow user to enter single letter

                    if (validInputList != NULL)
                    {
                        short listIterator=0;
                        for (; listIterator < sizeof(validInputList); listIterator++)
                        {
                            if (  ((validInputList[listIterator])[0]&0xCF) == (AnswerBuffer[0]&0xCF))
                            {
                                validInput=TRUE;
                                break;
                            }
                        }
                    }
    
                    if (!validInput)
                        goto BeginQuestion; 

                    printf("%c\n",AnswerBuffer[0]);

                }
                // remove the excess newline
                if (AnswerBuffer[strlen(AnswerBuffer)-1] <= 0x0d)
                {
                    AnswerBuffer[strlen(AnswerBuffer)-1]=0x00;
                }
                

                

        		if(test)
        			success = TRUE;

            }
		break;
	}

	return success;
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       int testPrint(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
int testPrint(char *Buffer,...)
{

    // exit on null BUFFER
	if (!Buffer) {
        return FALSE;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);

	// justify the print procedure
    boundedPrint(Buffer,LINE_LENGTH,ap,MAX_TEST_LENGTH,IPCREGULARPRINT);

	

}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       void centerPrint(unsigned short lineLength, char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
void centerPrint(unsigned short lineLength, char *Buffer,...)
{

    // exit on null BUFFER
    
	if (!Buffer) {
        return;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);

    char buffer[LINE_BUF];

    vsnprintf(buffer,LINE_LENGTH,Buffer,ap);
    int amount = strlen(buffer);
    int amountToMove = (( (lineLength)-strlen(buffer))/2);
    memmove(buffer+amountToMove,buffer,amount);
    int i=0;
    for(; i < amountToMove; i++)
        buffer[i]=0x20;
    buffer[amountToMove+amount]=0x00;
    
    // now, call a more general form of the print
    generalPrint(buffer,IPCREGULARPRINT);
    customMessage(" ");


}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       void linePrint(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
void linePrint(char *Buffer,...)
{

    // exit on null BUFFER
    
	if (!Buffer) {
        return;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);

    char buffer[LINE_BUF];

    vsnprintf(buffer,LINE_BUF,Buffer,ap);

	// now, call a more general form of the print
    generalPrint(buffer,IPCREGULARPRINT);

	

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       void diagnosticLinePrint(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
void diagnosticLinePrint(char *Buffer,...)
{

    // exit on null BUFFER
    
	if (!Buffer) {
        return;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);

    char buffer[LINE_BUF];

    vsnprintf(buffer,LINE_BUF,Buffer,ap);

	// now, call a more general form of the print
    generalPrint(buffer,IPCDIAGNOSTICPRINT);

	

}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void generalPrint(char *Buffer,short type)
 */
///////////////////////////////////////////////////////////////////////////////
void generalPrint(char *Buffer,short type)
{
    switch(CURRENT_TEST_MODE){
    	case REMOTE_CONSOLE_MODE:		//We're in test mode, so question needs to go to the Viewer
            
            {
                // print to the remote console, based on type
                printToIPC(Buffer,type);
            
            }

        case CONSOLE_MODE:		//always output to the screen
    	default:

                // print to the console using an argument list
                
                if (type == IPCREGULARPRINTUPDATE)
                {
                    printDirectlyToConsoleNoFlush(Buffer);
                    printDirectlyToConsole("\r");
                }
                else
                    printDirectlyToConsole(Buffer);
            

    	break;
	}
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        short testBoundedColumnBring(char *bufferInsertTo,uint bufferSize, char *externalBuffer,va_list arguments,uint maxLength)
 */
///////////////////////////////////////////////////////////////////////////////
short testBoundedColumnBring(char *bufferInsertTo,uint bufferSize, char *externalBuffer,va_list arguments,uint maxLength)
{
    // if our array size is too small, return the size and exit this function
    
    if (maxLength > bufferSize) {
        return bufferSize;
    }
    
    // use vsnprint to safely print externalBuffer and arguments into bufferInsertTo
    // in addition, get the size that the buffer would have been

    /*
     * vsnprintf accepts a buffer, an argument list, AND the size of the
     * buffer. This is a security measure to prevent buffer overflows
     * which would occur if the size of the buffer and argument list 
     * exceeds the size of bufferInsertTo
     */
    int actualSize = vsnprintf(bufferInsertTo,bufferSize-1,externalBuffer,arguments);
    short bufferLength=strlen(bufferInsertTo);
    if (actualSize > maxLength || actualSize == -1 || bufferLength > maxLength) {
        return actualSize;
    }

    
    short i =0;

    char *newline = "\n";
    if (find(bufferInsertTo,newline) == ERROR)
    {
        for (i=strlen(bufferInsertTo); i < maxLength; i++) {
        bufferInsertTo[i] = 0x20; // add space
        
        }
    }
    //bufferInsertTo[maxLength-1]=0x00;


    return 0;

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void boundedPrint(char *Buffer,uint bufferLength, va_list arguments,uint maxSize, short type)
 */
///////////////////////////////////////////////////////////////////////////////
void boundedPrint(char *Buffer,uint bufferLength, va_list arguments,uint maxSize, short type)
{

	// buffer to which we place line
    char localBuffer[BUF_LEN]="";

	// set all characters to NULL
    memset(localBuffer,0x00,BUF_LEN);

	// justfiy the text
    if ( testBoundedColumnBring(localBuffer,bufferLength,Buffer,arguments,maxSize) > 0 )
    {
           consolePrint("Text exceeds bounds");
           return;
    }

	// execute generalPrint
    generalPrint(localBuffer,type);
    
    
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void printToIPC(char *Buffer,short type)
 */
///////////////////////////////////////////////////////////////////////////////
void printToIPC(char *Buffer,short type)
{
    // we should push what we have onto the test data stack...
    // 
    // create IPC 'packet' 
    IPCPACKET queryPacket = {};
    
    queryPacket.data.releaseListener=FALSE;

    strcpy(queryPacket.data.info,Buffer);

    // we won't need a response, so set the response pointer to NULL
    memset(queryPacket.data.response,0x00,BUF_LEN);

    // the packet will be going from the server to the client
    queryPacket.mtype=(IPCSERVERTOCLIENT|type);

    // send a message to local to kill IPC Listening
 
    ipcSendMessage(captureIpcQid(),&queryPacket,sizeof(IPCPACKET));
 
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        int diagnosticPrint(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
int diagnosticPrint(char *Buffer,...)
{

    // exit on null BUFFER
	if (!Buffer) {
        return FALSE;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);

    boundedPrint(Buffer,LINE_LENGTH,ap,MAX_TEST_LENGTH,IPCDIAGNOSTICPRINT);

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn       void debugPrint(int output_debug_info, char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
void debugPrint(int output_debug_info, char *Buffer,...)
{
    if (output_debug_info == TRUE)
    {
        // formulate argument list
        // and set ap to beginning element
        va_list ap;

        va_start(ap, Buffer);

        char buffer[LINE_BUF];

        vsnprintf(buffer,LINE_BUF,Buffer,ap);

        // Appending Debug to all lines from this function for easy searching
        consolePrint("DEBUG:\t");
        //boundedPrint(Buffer,LINE_LENGTH,ap,MAX_TEST_LENGTH,IPCDIAGNOSTICPRINT);
        diagnosticLinePrint(Buffer);
    }
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void printVAConsole(char *Buffer,va_list arguments)
 */
///////////////////////////////////////////////////////////////////////////////
void printVAConsole(char *Buffer,va_list arguments)
{

    char localBuffer[BUF_LEN]="";

    vsnprintf(localBuffer,BUF_LEN,Buffer,arguments);

    printDirectlyToConsole(localBuffer);

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void printDirectlyToConsoleNoFlush(char *Buffer)
 */
///////////////////////////////////////////////////////////////////////////////
void printDirectlyToConsoleNoFlush(char *Buffer)
{

    printf(Buffer);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void printDirectlyToConsole(char *Buffer)
 */
///////////////////////////////////////////////////////////////////////////////
void printDirectlyToConsole(char *Buffer)
{

    printf(Buffer);
    /*
     * REMEMBER TO FLUSH STDOUT. This prevents the current
     * thread from proceeding before all data is flushed
     * to the creen
     */
    fflush(stdout);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void failedMessage()
 */
///////////////////////////////////////////////////////////////////////////////
void failedMessage()
{
    // send the pre-determined failure string
    // to customMessage
    customMessage(FAILEDSTRING);
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void passedMessage()
 */
///////////////////////////////////////////////////////////////////////////////
void passedMessage()
{
    // send the pre-determined pass string
    // to customMessage
    customMessage(PASSEDSTRING);
    
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void customMessage(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
void customMessage(char *Buffer,...)
{
    // exit if our buffer is null
    if (!Buffer) {
        return;
    }

    // create argument list 
    va_list arguments;

    va_start(arguments, Buffer);

    // copy Buffer into buffer, along with a carriage return and newline
    char *buffer = (char*)malloc(strlen(Buffer)+3);

    //sprintf(buffer,"%s%c%c",Buffer,0x0a,0xd);
    sprintf(buffer,"%s\r\n",Buffer);
    // call bounded print to print this line per the specified
    // values
    
    boundedPrint(buffer,(LINE_BUF-MAX_TEST_LENGTH),arguments,strlen(buffer),IPCREGULARPRINTSTATUS);

    // free buffer...why did I dynamically allocate it in the first place?
    free(buffer);



}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void consolePrint(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
void consolePrint(char *Buffer,...)
{

    // if buffer is empty, leave
	if (!Buffer) {
        return;
    }

    // formulate argument list. set ap
    // to the first element of the argument list
    va_list ap;

    va_start(ap, Buffer);


    // call our brother in arms, printVAConsole
    printVAConsole(Buffer,ap);
}



///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        int diagnosticPrintUpdate(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
int diagnosticPrintUpdate(char *Buffer,...)
{

    // exit on null BUFFER
    if (!Buffer) {
        return FALSE;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);

    // call bounded print to print this line per the specified
    // values


    char newBuffer[BUF_LEN]="";

    testBoundedColumnBring(newBuffer,BUF_LEN,Buffer,ap,MAX_TEST_LENGTH);

    printToIPC(newBuffer,IPCDIAGNOSTICPRINTUPDATE);

	

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         int testPrintUpdate(char *Buffer,...)
 */
///////////////////////////////////////////////////////////////////////////////
int testPrintUpdate(char *Buffer,...)
{

    // exit on null BUFFER
	if (!Buffer) {
        return FALSE;
    }


    // formulate argument list
    // and set ap to beginning element
    va_list ap;

    va_start(ap, Buffer);


    // call bounded print to print this line per the specified
    // values
    boundedPrint(Buffer,LINE_LENGTH,ap,MAX_TEST_LENGTH,IPCREGULARPRINTUPDATE);

	

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         short askRetryFailQuestion(char *Question,...)
 */
///////////////////////////////////////////////////////////////////////////////
short askRetryFailQuestion(char *Question,...)
{

    va_list ap;

    va_start(ap, Question);

    char Buffer[strlen(Question)+BUF_LEN];

    vsnprintf(Buffer,sizeof(Buffer),Question,ap);

    char answerBuffer[255];
    // sleeping for one second prevents questions from running into each other
    // in a client's buffer.
    if (CURRENT_TEST_MODE == REMOTE_CONSOLE_MODE)
    {
        sleep(1);
    }
    // pose question the user
    
    char *validInput[4]= {"retry","r","fail","f"};
    if (createUserPrompt(Buffer,answerBuffer,254,RETRYFAIL,validInput)==TRUE)
    {
        // the response should either be yes or no
        makeLowerCase(answerBuffer);
        
        if (!strncmp(answerBuffer,"retry",5) || !strncmp(answerBuffer,"r",1))
        {
            return RETRY;
        }
        else
            return FAIL;
                    
    }
    else
        return FAIL;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         short askYesNoQuestion(char *Question,...)
 */
///////////////////////////////////////////////////////////////////////////////
short askYesNoQuestion(char *Question,...)
{
    va_list ap;

    va_start(ap, Question);

    char Buffer[strlen(Question)+BUF_LEN];

    vsnprintf(Buffer,sizeof(Buffer),Question,ap);

    char answerBuffer[255];
    // sleeping for one second prevents questions from running into each other
    // in a client's buffer.
    if (CURRENT_TEST_MODE == REMOTE_CONSOLE_MODE)
    {
        sleep(1);
    }
    
    // pose question the user
    //askUserQuestion:
    char *validInput[4]= {"yes","y","no","n"};
    if (createUserPrompt(Buffer,answerBuffer,254,YESNO,validInput)==TRUE)
    {
        // the response should either be yes or no
        makeLowerCase(answerBuffer);
        
        if (!strncmp(answerBuffer,"yes",3) || !strncmp(answerBuffer,"y",1))
        {
            return YES;
        }
        else
            return NO;
            
    }
    else
        return NO;
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         int linuxGetChar( )
 */
///////////////////////////////////////////////////////////////////////////////
int linuxGetChar( ) {
    // terminal i/o structure
    struct termios oldt,newt;
    // returned character
    int ch=0;
    // directs oldt to standard input
    // and gets the current attribute set
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    // enable echoing and canonical names. 
    // canonical names allow EOL KILL, etc
    newt.c_lflag &= ~( ICANON | ECHO );
    // sets the attributes for standard input
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    // uses getchar to actually get the character
    // we must, of course, ignore escape sequences
    while(ch < 33 || ch > 126)
    {
        if (ch == 27) // we have an escape character
        {
            fflush(stdin);
            ch = getchar();
            if (ch == 91) // we have an escape character
            {
                fflush(stdin);
                ch = getchar();
            }
        }
        
        ch = getchar();

    }
    // resets the attribute set for standard input
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}
