///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TestDispatcher.c
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Receives board information from client, obtains test data from
 *              the database server, executes the individual tests, and returns
 *              any test information to the user 
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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "TDThreadHandler.h"
#include "TestDispatcher.h"
#include "../CommonLibrary/BoardInfo.h"
#include "TDServer.h"
#include "TDSignalHandler.h"
#include "argtable2.h"



// The internal version number is used STRICTLY 
// as a way for Test Console to know if its
// version is outdated
#define INTERNAL_VERSION_NUMBER 1.7

int main (int argc, char *argv[])
{

    struct arg_lit *debug,*help;
    struct arg_str *databaseName,*databaseUsername,*databasePassword,*testInt,*bomRev,*fgNumber,*serialNumber;
    struct arg_end *end;


     void *argtable[] = {
        fgNumber = arg_str0(NULL,"fg","[0-9]","Set the finished good number"),

        serialNumber = arg_str0(NULL,"serial","[0-9]","Set the serial number"),

        testInt = arg_str0(NULL,"init","[a-z]","Set the tester's initials"),

        bomRev = arg_str0(NULL,"rev","[a-z]","Set the BOM Revision"),

        databaseName = arg_str0("d","dsn","[a-z]","Set the database name"),

        databaseUsername = arg_str0("u","uid","[a-z]","Set the username used to access the database"),

        databasePassword = arg_str0("p","pwd","[a-z]","Set the password used to access the database"),

        debug = arg_lit0(NULL,"debug","Displays debug information."),

        help = arg_lit0("h","help","Displays usage information"),

        end = arg_end(20),

    };

    if (arg_nullcheck(argtable) != 0)
    {
        noMemoryHalt();
    }

    arg_parse(argc,argv,argtable);

    if (help->count > 0)
    {
        arg_print_syntax(stdout,argtable,"\n");
        arg_print_glossary(stdout,argtable,"  %-25s %s\n");

        goto exit; // stupid goto statements
    }


    char *fgNumberArgument=NULL;
    
    // initialize the first character of
    // our database values to 0x00, which is the null character
    // this allows strlen(..) to see their length as zero
    dsn[0] = uid[0] = pwd[0] = 0x00;

    databaseName->count > 0 ? strcpy(dsn,databaseName->sval[0]) : strcpy(dsn,"mysql");

    //databaseUsername->count > 0 ? strcpy(uid,databaseUsername->sval[0]) : strcpy(uid,"dispatcher_agent");
    databaseUsername->count > 0 ? strcpy(uid,databaseUsername->sval[0]) : strcpy(uid,"root");

    //databasePassword->count > 0 ? strcpy(pwd,databasePassword->sval[0]) : strcpy(pwd,"dti123");
    databasePassword->count > 0 ? strcpy(pwd,databasePassword->sval[0]) : strcpy(pwd,"ergondt8");

    signal(SIGINT, (void*)handle_signals);
	signal(SIGQUIT, (void*)handle_signals);
    signal(SIGHUP,(void*)handle_signals);
    signal(SIGTERM,(void*)handle_signals);
    
    BOARDSTATE boardStatusAndState={};

    if (fgNumber->count > 0)
    {
        if ( isValidFinishedGoodNumber((char *)fgNumber->sval[0]) )
            boardStatusAndState.boardInfo.finishedGoodNumber=atol(fgNumber->sval[0]);
    }

    if (serialNumber->count > 0)
    {
        if ( isValidSerialNumber((char *)serialNumber->sval[0]) )
            boardStatusAndState.boardInfo.serialNumber=atol(serialNumber->sval[0]);
    }

    // default the loaded value for this board to false
    // as obviously, we have not retrieved this board's information
    boardStatusAndState.loaded=FALSE;

    
    // set the current test mode
    setCurrentTestMode(REMOTE_CONSOLE_MODE);


    setListenerRequiredVersion((float)INTERNAL_VERSION_NUMBER);

    boardStatusAndState.boardInfo.serialNumber=000000;
  

    createCpuProfile("/cpuprofile");

    // uncomment for my purposes
    int qid = 0;
    if (debug->count == 0)
    {
        qid = createIpcListenerThread();    
    }
    
    

    // start our server thread
    createServerThread();

    // connect to the database
    databaseConnect(dsn, uid, pwd);


    // wait for our threads to begin running
    waitForThreadRunningStatus();

    char answer[255];

    loadServer(&boardStatusAndState); 

    short doTest=TRUE;

    // initialize our board data to default/empty values

    initializeBoardAndState(&boardStatusAndState);
       
    // tell the dispatcher that we're temporarily
    // allowing clients to access this board
    boardStatusAndState.loaded=TEMP;
    // get our board's FG number
    
   
    
    
    if (doTest)
    {
        boardStatusAndState.loaded=FALSE;
        boardStatusAndState.percentComplete=0;
        boardStatusAndState.testType=0;
        boardStatusAndState.attempt =0;
        boardStatusAndState.accessLevel=0;
        boardStatusAndState.currentSequence=-1;
        boardStatusAndState.loaded=TRUE;
        boardStatusAndState.passing =TRUE;

        char *Tester = (testInt->count > 0 ? (char *)testInt->sval[0] : NULL);
        ulong serialNum=0;
        if (serialNumber->count > 0 && boardStatusAndState.boardInfo.serialNumber == 0)
        {
            if ( isValidSerialNumber((char *)serialNumber->sval[0]) )
                serialNum = atol( serialNumber->sval[0] );
        }
        char revision = (bomRev->count > 0 ? bomRev->sval[0][0] : '0' );
        beginTesting(&boardStatusAndState,Tester,&serialNum,&revision);
    }


    // tell the signal handler that we are quitting, so
    // it can kill the server and IPC listeners
    //raise(SIGQUIT);


    // for now, let's wait indefinitely until the user
    // manually reboots the machine

    while(1)
    {
        /*
         * usleep waits for x micro seconds
         * in this case, we are assigning an
         * unsigned negative one, thus, we
         * are sending usleep FFFF FFFF, which
         * equates to over four billion. Thus
         * we will wait just over 4,000 seconds
         * per call. Note: we can still accept
         * signals, so the program can be killed
         * via signal/ctrl-c
         */
        
        usleep( ( (unsigned long)-1 ) );
    }

exit:
    // free the argument table, which was dynamically allocated above
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return 0; 
}





