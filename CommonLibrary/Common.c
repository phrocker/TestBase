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
	\brief This file holds definitions for the declarations in Common.h
	\n See Common.h for details about the methods in this file. 
*/
#include <signal.h>
#include "Common.h"
#include "Debug.h"



/************************************************************************************
*
*	setTestVersion
*
*	Sets the version of the current test by creating the version message queue and
*   adding a packet to it which will be read by getTestVersion
*      
*	Arguments:
*
*      unsigned float version
*
*	Return Value:
*
*      sets the version of the current test
*
*************************************************************************************/

void setTestVersion(float version)
{
    
    IPCPACKET queryPacket = {};

    // tell IPC listener that we're sending
    // a emssage
    queryPacket.mtype=SET_VERSION;

    // of course, set releaseListener to false, so the listener
    // doesn't misinterperet our packet
    queryPacket.data.releaseListener=FALSE;

    // place question into info
    snprintf(queryPacket.data.info,BUF_LEN,"%.2f",version);

    // set our response packet as NULL
    memset(queryPacket.data.response,0x00,BUF_LEN);
    

    // create key baesd on QUESTION queue
    key_t mykey = TEST_EXECUTE_QUESTION_QUEUE;

    if (ipcMakeKey(&mykey) == ERROR)
        return;

    
    mykey = TEST_VERSION_QUEUE;


    int userQid=0;

    // open queue

    if ( ipcOpenQueue(mykey, &userQid) == ERROR)
        return;

    // send question packet
    ipcSendMessage(userQid,&queryPacket,sizeof(IPCPACKET));

    // let's go ahead and attach a seg-fault signal handler

    //signal(SIGSEGV, (void*)handle_segfault);
    
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = handle_segfault;
    action.sa_flags = SA_SIGINFO;
    if(sigaction(SIGSEGV, &action, NULL) < 0) {
        perror("sigaction");
    }
    
}

/************************************************************************************
*
*	getTestVersion
*
*	Reads the version message queue and returns the version of the current test
*   if it was set
*      
*	Arguments:
*
*      void
*
*	Return Value:
*
*		float version of the current test
*
*************************************************************************************/
float getTestVersion()
{
    key_t mykey = TEST_VERSION_QUEUE;

    IPCPACKET versionPacket = {};

    // get Queue ID for question queue
    int userQid=ipcGetKey(mykey);

    // in case of error, return nothing

    if (userQid == ERROR)
    {
        return 0;
    }

    // read QUESTION queue and block until a message is present
    
    int err = ipcReadMessage(userQid, 0, (struct IPCPACKET*)&versionPacket,sizeof(IPCPACKET), TRUE); //block until we get one 

    if (err==ERROR)
    {
        return;
    }
    // remove question queue in case another program wishes to ask
    // a question
    
    ipcRemoveQueue(userQid);
    
    return atof( versionPacket.data.info );
}

/************************************************************************************
*
*	haltTest
*
*	Halts all execution of testing
*      
*	Arguments:
*
*      char *errorMessage - Error message to be displayed
*      int exitStatus - Exit status to send testDispatcher
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void haltTest(char *errorMessage,int exitStatus)
{
    testPrint(errorMessage);
    exit(exitStatus);
}

/************************************************************************************
*
*	noMemoryHalt
*
*	Halts on memory allocation error
*      
*	Arguments:
*
*      void
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void noMemoryHalt()
{
    consolePrint("Could not allocate memory! System Error, Halting!\n");
    haltTest("Could not allocate memory! System Error, Halting!",255);
}


/************************************************************************************
*
*	getConfigFileData
*
*	Returns configuration file data based on the testname
*      
*	Arguments:
*
*      char *configFile - Path to configuration file
*      char *testName - test name
*
*	Return Value:
*
*		configuration data
*
*************************************************************************************/

char *getConfigFileData(char *configFile,char *testName)
{
    char *configurationData = NULL;
    char *searchBeginString = (char*)malloc(strlen(testName)+3);
    if (searchBeginString == NULL)
         {
             noMemoryHalt();
         }
    sprintf(searchBeginString,"<%s>",testName);
    char *searchEndString = (char*)malloc(strlen(testName)+4);
    if (searchEndString == NULL)
         {
             noMemoryHalt();
         }
    sprintf(searchEndString,"</%s>",testName);
    FILE *configFilePointer = fopen(configFile,"rt");
    if (configFilePointer!=NULL)
    {

         fseek (configFilePointer, 0, SEEK_END);
         long size=ftell (configFilePointer);
         fseek (configFilePointer, 0, SEEK_SET);
         char *readBuffer = (char*)malloc(size);
         if (readBuffer == NULL)
         {
             noMemoryHalt();
         }
         int bytesRead = fread(readBuffer,1,size,configFilePointer);
         char *pBuffer = readBuffer;
         while(*pBuffer)
         {
             if (!strncmp(pBuffer,searchBeginString,strlen(searchBeginString)) )
             {
                 // pBuffer now contains our data, and then some 
                 pBuffer +=strlen(searchBeginString);
                 char *pPointerBuffer = pBuffer;
                 if (*pBuffer == 0x0a || *pBuffer == 0x0d)
                 {
                     pBuffer++;
                 }
                 while(*pPointerBuffer)
                 {
                      if (!strncmp(pPointerBuffer,searchEndString,strlen(searchEndString)))
                      {
                          configurationData = (char*)malloc((strlen(pBuffer)-strlen(pPointerBuffer))+1);
                          
                          strncpy(configurationData,pBuffer,strlen(pBuffer)-strlen(pPointerBuffer));
                          if (configurationData[ strlen(configurationData)-1] == 0x0a ||
                              configurationData[ strlen(configurationData)-1] == 0x0a
                              )
                          {
                            configurationData[ strlen(configurationData)-1]=0x00;
                          }
                          break;
                      }
                      pPointerBuffer++;

                 }
                 break;
             }
             pBuffer++;
         }

         free(readBuffer);
         fclose(configFilePointer);
    }

    free(searchBeginString);
    free(searchEndString);
    return configurationData;
}

void displayUsage(void *argtable[], char *testName, double testVersion)
{
    consolePrint("\n./%s ",testName);
    arg_print_syntaxv(stdout,argtable,"\n");
    consolePrint("\n%s %s\n",testName, TBSCOPYRIGHT);
    consolePrint("\nProgram Usage\n");
    arg_print_glossary(stdout,argtable,"  %-25s %s\n");
    consolePrint("Test Version: %.2f\n",testVersion);
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));        
    exit(1);
    return 1;
}


