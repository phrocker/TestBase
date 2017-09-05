/***************************************************************************
 *                                                                         *
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
	\brief This file holds definitions for the declarations in DBFunc.h
	\n See DBFunc.h for details about the methods in this file. 
*/
 
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "DBFunc.h"
#include "../Prompt.h"
#include "../Common.h"
#include "DBResultFunctions.h"


/*! \var uchar ConnectionStatus
	\brief variable containing the current database connection status
*/
 /*! \var uchar usingMSSQL
	\brief flag denoting that the database being used is Microsoft SQL Server
*/
 /*! \var uchar LongText
	\brief flag used in databaseExecSQL to denote a field being retrieve is of type 'Long Text'
*/

/*! \var uchar TICK
	\brief character which will be used to surround text in SQL statement.
*/
uchar ConnectionStatus = NOT_CONNECTED;
uchar usingMSSQL = FALSE; //variable to determine if we are using MSSQL or not
uchar LongText = FALSE; 	//flag used in data fetching to handle long text
uchar TICK='`'; // may be different between versions/type of SQL Server


 /*! \var SQLHENV SqlEnvironment
	\brief SQL handle for the Environment
*/
 /*! \var long	SqlResult
	\brief variable used to store return values of ODBC functions
*/
 /*! \var SQLHDBC SqlConnectHandle
	\brief SQL Handle for the connection to the database
*/
 /*! \var SQLHSTMT 	SqlStatementHandle
	\brief SQL Handle for the statement run on the database
*/
 
//Global variables used for databse actions
SQLHENV		SqlEnvironment;     // Handle ODBC environment
long		SqlResult;     		// result of functions
SQLHDBC		SqlConnectHandle;   // Handle connection
SQLHSTMT 	SqlStatementHandle; // Handle for a statement

 /*upper level database functions*/

/************************************************************************************ 
*	databaseIsValidUsernamePassword
*	
*   Returns TRUE or FALSE, depending on whether or not the username/password combo
*   is valid
*
*	Arguments:
*		char *username
*		char *password
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
short databaseIsValidUsernamePassword(char *username, char *password)
{

    char Query[BUF_LEN];

    snprintf(Query,BUF_LEN,"select count(user) from mysql.user where user='%s' and password=PASSWORD('%s')",username,password);

     Result_Set results; // create result set
	 //call DB Select function to get out value back
	 char result = (char)databaseExecSQL(Query, &results, 20);
     int foundUsers =0;
	 if(result == TRUE){
         // if we have a succssful query, and results in
         // the result set, copy the board name into BoardName
         
         if ( results.rows > 0 ) {

             foundUsers = atoi( getResultData(&results,0,0) );
         }
         // of course, free our dynamically allocated memory, if any
         freeResultData(&results);   
         // return success
	 }	 
     else
     {
         // uh oh, the result was not TRUE, so an error probably occurred
         databaseErrors();
     }

     return foundUsers;

}

/************************************************************************************ 
*	databaseGetUserAccessLevelFromUserName
*	
*   Returns the user's access level
*
*	Arguments:
*		char *username
*
*	Return Value:
*		user access level
*
************************************************************************************/
short databaseGetUserAccessLevelFromUserName(char *username)
{
   
    // if the mac address is null, or its length is zero
    // we assume the user has production level access only
    if (username == NULL) {
        return PRODUCTION_ACCESS_LEVEL;
    }
    else
    {
        if (strlen(username) == 0)
            return PRODUCTION_ACCESS_LEVEL;
    }

    char Query[BUF_LEN];

    //Test_Software
    // create our SQL query from the above buffer
    sprintf(Query,"SELECT access FROM software.macAuthentication WHERE user='%s'",username);

    int ret = PRODUCTION_ACCESS_LEVEL;
    Result_Set results;
    //call DB Select function to get out value back
    char result = (char)databaseExecSQL(Query, &results, 20);
    if(result == TRUE){
    
        // if we have results, set ret to integer representation
        // of our return value
        if ( results.rows > 0 ) {
            
            ret = atoi( results.resultData[0][0] );
        }

        // fre our data
        freeResultData(&results);
    
    }	 	 	 
    
    // return the user's access level
    return ret;

}

/************************************************************************************ 
*	databaseGetBoardName
*	
*	Attempts to locate the board name for the input FinishedGoodNumber
*
*	Arguments:
*		unsigned long FinishedGoodNumber
*		char *BoardName -  buffer in which the board name will be placed
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
short databaseGetBoardName(ulong FinishedGoodNumber, char *BoardName)
{
     char Query[BUF_LEN]; // static buffer for query

     sprintf(Query,"SELECT BoardName FROM fgnumbers where FGNumber=%lu",FinishedGoodNumber);


     Result_Set results; // create result set
	 //call DB Select function to get out value back
	 char result = (char)databaseExecSQL(Query, &results, 20);

	 if(result == TRUE){
         // if we have a succssful query, and results in
         // the result set, copy the board name into BoardName
         if ( results.rows > 0 ) {

             strcpy(BoardName,getResultData(&results,0,0));
         }
         // of course, free our dynamically allocated memory, if any
         freeResultData(&results);   
         // return success
         return TRUE;
	 }	 
     else
     {
         // uh oh, the result was not TRUE, so an error probably occurred
         databaseErrors();
         return FALSE;
     }
     
}

/************************************************************************************ 
*	databaseIsValidFinishedGoodNumber
*	
*	Attempts to locate the finished good number within the database. Returns 0 if
*   the finished good number was not found, 1 otherwise
*
*	Arguments:
*		ulong &FinishedGoodNumber - board's finished good number
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
short databaseIsValidFinishedGoodNumber(ulong *FinishedGoodNumber)
{
    char Query[BUF_LEN];

    sprintf(Query,"SELECT count(fgNumber) FROM fgnumbers WHERE fgNumber=%lu",*FinishedGoodNumber);

    Result_Set results;
        //call DB Select function to get out value back
    char result = (char)databaseExecSQL(Query, &results, 20);
    short ret = FALSE;
    if(result == TRUE)
    {
        
        if (atoi(getResultData(&results,0,0)) > 0)
        {
            ret = TRUE;
        }
        freeResultData(&results);
    }

    return ret;
}



/************************************************************************************ 
*	databaseGetFinishedGoodRev
*	
*	This function uses lower level database functions to retrieve a Finished Good
*	Rev from the database. [the BOM REV]
*
*	Arguments:
*		unsigned long SeachValue - FinishedGoodNumber to Query DB with
*		unsinged char* ReturnValue - reference to variable for return value storage
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
int databaseGetFinishedGoodRev(ulong SearchValue, uchar* ReturnValue)
{

	 char Query[BUF_LEN];

     sprintf(Query,"SELECT BOMRev FROM boardinfo WHERE FinishedGoodNumber=%lu",SearchValue);
     
     Result_Set results;
     	 //call DB Select function to get out value back
	 char result = (char)databaseExecSQL(Query, &results, 20);
	 if(result == TRUE){

         
         *ReturnValue = *results.resultData[0][0];
         

         freeResultData(&results);
		 //*ReturnValue = (uchar) ReturnBuffer[0];
		 return TRUE; 
	 }	 	 	 
     else

        
	 return FALSE;
 }

 /************************************************************************************ 
*	databaseGetFinishedGoodNumber
*	
*	This function uses lower level database functions to retrieve a Finished Good
*	Number from the database.
*
*	Arguments:
*		unsigned long SeachValue - index to query the database with
*		unsinged long* ReturnValue - reference to variable for return value storage
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/

int databaseGetFinishedGoodNumber(ulong SearchValue, ulong *ReturnValue)
{
	//since most queries use the FGN as the search index, i'm not sure yet how or if
	//i need to write this funciton
	
	 return FALSE;
}

/************************************************************************************ 
*	databaseGetSerialNumber
*	
*	This function uses lower level database functions to retrieve a Serial Number
*	from the database.
*
*	Arguments:
*		unsigned long SeachValue - FinishedGoodNumber to Query DB with
*		unsinged long* ReturnValue - reference to variable for return value storage
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
 int databaseGetSerialNumber(ulong SearchValue, ulong* ReturnValue){

	 char Query[BUF_LEN];
 
     sprintf(Query,"SELECT SerialNumber FROM boardinfo WHERE FinishedGoodNumber=%lu limit 0,1",SearchValue);

     Result_Set results;
     
     
	 char result = (char)databaseExecSQL(Query, &results, BUF_LEN); //ReturnBuffer);
	 if(result == TRUE){
         
		 // at this point, we should have a single result from the database. set the returnvalue
         // equal to the long value of that result
        *ReturnValue = (ulong) atol(results.resultData[0][0]);
         // free resultset
         freeResultData(&results);
		 return TRUE; 
	 }	 	 

	 return FALSE;
 }

/************************************************************************************ 
*	databaseGetBootMacNumber
*	
*	This function uses lower level database functions to retrieve a Boot Mac
*	Number from the database.
*
*	Arguments:
*		unsigned long SeachValue - FinishedGoodNumber to Query DB with
*		unsinged long* ReturnValue - reference to variable for return value storage
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
 int databaseGetBootMacNumber(PBOARDSTATE info, ulong* ReturnValue){

	 //we are passed in the Finished Good Number, but no table in the Db has the MAC and FGN together, 
	 //So we get the Serial for the FGN and then get the MAC from the Serial in table: MACs
	 ulong Serial;

     
     if(databaseGetSerialNumber(info->boardInfo.finishedGoodNumber, &Serial) != TRUE) // error getting associated Serial
		 return FALSE; 

     
	 //use Serial to find the MAC
	 
	 char Query[BUF_LEN];

     // formulate Query
     sprintf(Query,"SELECT distinct MACAddress from MACs where serialnumber=%lu and testSequence=%i",Serial,info->attempt-1);

     Result_Set results;
	 //call DB Select function to get out value back
     
	 char result = (char)databaseExecSQL(Query, &results, BUF_LEN);
     
	 if(result == TRUE){

		 // at this point, we should have a single result from the database. set the returnvalue
         // equal to the long value of that result
		 *ReturnValue = (ulong) atol(results.resultData[0][0]);
         // free result set

         freeResultData(&results);

		 return TRUE; 
	 }

	 return FALSE;
 }


 //Functions to retrieve entries from the `Tests` Table

/************************************************************************************ 
*	databaseGetFirstTest
*	
*	This function uses lower level database functions to retrieve the first `Tests` entry
*	for a FGNumber in the Database. The FGNumber in the struct must be set as the search 
*	parameter.
*
*	Arguments:
*		*TESTPARAMETERS tp - pointer to a struct to hold all returned data
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/
 int databaseGetFirstTest(PTESTPARAMETERS tp){

	 //just set sequence value and call databaseGetTest 
     
	 tp->Sequence =0; 
	 
	 return  databaseGetTest(tp,1); 
 }

/************************************************************************************ 
*	databaseGetTest
*	
*	This function uses lower level database functions to retrieve the next `Tests` entry
*	for a FGNumber in the Database. The FGNumber and Sequence in the struct must be set 
*	as the search parameters. The Sequence Number supplied is the test returned, and Numbering
*	begins at Zero. So if you want the second test, Sequence=1;
*
*	Arguments:
*		*TESTPARAMETERS tp - pointer to a struct to hold all returned data
*
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/

 int databaseGetTest(PTESTPARAMETERS tp, short testType){

     // initially, this function grabbed all tests, but there is no
     // point in doing that, since we let the client choose the test type
     char *type = ( testType == 1 ? "production" : "burnin" );

     //char *Query = (char*)malloc(BUF_LEN);
     char Query[BUF_LEN]="";

     sprintf(Query,"select test,testParam,driver,DriverParam,testNumber,production,burnin,type from tests where fgNumber=%lu and Attempt=%i and sequence=%i and %s='1'",tp->FGNumber,tp->attempt,tp->Sequence,type);

     Result_Set results;

     char result = databaseExecSQL(Query,&results ,BUF_LEN);
     //free(Query);
     if(result == TRUE){ //query table for actual data

          // copy the test data into the test structure
          strcpy(tp->Test, getResultData(&results,0,0));
          strcpy(tp->TestParam, getResultData(&results,0,1));
          strcpy(tp->Driver, getResultData(&results,0,2));
          strcpy(tp->DriverParam, getResultData(&results,0,3));

          // convert the string values from the result set
          // to a long value 
          tp->TestNumber = atol(getResultData(&results,0,4));
          tp->Production = atol(getResultData(&results,0,5));
          tp->BurnIn = atol(getResultData(&results,0,6));
          strcpy(tp->Type, getResultData(&results,0,7));
          tp->activeSequence = TRUE;

          // don't forget to free the result set
          
          freeResultData(&results);
			
       
	 }//end if TRUE
	 else
     {
            return FALSE;
     }
      

	 return TRUE;
 }


/************************************************************************************ 
*	databaseGetNumberOfTests
*	
*	This function uses lower level database functions to retrieve all the Sequence Numbers 
*	and determine the highest value in order to return the # of tests for that FGN.
*
*	Arguments:
*		unsigned long FGNumber - number to search against
*       unsigned int attempt - Test attempt
*       short testType - Type of test
*		unsigned int Sequence - pointer to an int to set as the number of tests
*       
*	Return Value:
*		TRUE or FALSE depending on successful query of the database
*
************************************************************************************/

 int databaseGetNumberOfTests(unsigned long FGNumber,unsigned int attempt,short testType, unsigned int *Sequence){
     
     char Query[BUF_LEN];


     char *type = ( testType == 1 ? "production" : "burnin" );
     sprintf(Query,"Select max(Sequence) from tests where fgnumber=%lu and Attempt=%i and %s = '1' ORDER BY Sequence DESC",FGNumber,attempt,type);

	 //call DB Select function to get out value back
     Result_Set results;
	 char result = (char)databaseExecSQL(Query, &results, BUF_LEN);
	 if(result == TRUE){

		 /*
          * The returned value should be the string representation of
          * the highest test sequence with that Finished Good Number
          * thus, we send that value to atoi and use that as our highest
          * test Sequence
          */   

         if (strncmp(results.resultData[0][0],"NULL",4))
         {
             *Sequence = (unsigned int)( atoi(results.resultData[0][0]) +1); //plus one accounts for Zero indexing
         }
         else
             *Sequence=0;
         

         // free result set
         freeResultData(&results);
		 return TRUE; 
	 }

     freeResultData(&results);
	 return FALSE;
 }


/************************************************************************************ 
*	databaseRemoveTemporaryTestData
*	
*	Removes all temporary data located in testingData that is associated with FinishedGoodNumber
*	and the corresponding attempt
*
*	Arguments:
*		*FinishedGoodNumber - Finished Good Number for the current board
*		*attempt - Attempt for the current board
*
*	Return Value:
*		void
*
************************************************************************************/
void databaseRemoveTemporaryTestData(ulong *bootMac)
{

	// crate Query within query buffer, then execute query with a NULL dataset since
	// nothing is returned
    char Query[BUF_LEN];
    sprintf(Query,"delete from testingData where bootMAC=%lu",*bootMac);  
    databaseExecSQL(Query, NULL, BUF_LEN);

}

/************************************************************************************ 
*	databaseRemoveIllegalCharacter
*	
*	Saves test data as a test printout in the database
*
*	Arguments:
*		char charToRemove - character we must locate, and remove
*		char *stringToSearch - String we will search
*       char *newBuffer - Buffer in which to place new string
*       int bufLen - length of newBuffer
*
*	Return Value:
*       void
*
************************************************************************************/
void databaseRemoveIllegalCharacter(char charToRemove,char *stringToSearch, char* newBuffer, int bufLen)
{
    
    short i=0, j=0;
    // set all null characters
    memset(newBuffer,0x00,bufLen);
    // traverse stringToSearch
    for (; i < strlen(stringToSearch); i++)
    {
        // if the character is found, append a \
        // to newBuffer
        if (stringToSearch[i] == charToRemove)
        {
            newBuffer[j++] = '\\';
            if (j>=bufLen)
            {
                return;
            }
        }
        // append characters
        newBuffer[j++]=stringToSearch[i];
        if (j>=bufLen)
        {
            return;
        }
    }
}

/************************************************************************************ 
*	databaseSaveTestPrintout
*	
*	Saves test data as a test printout in the database
*
*	Arguments:
*		PBOARDSTATE boardInfo - Current test board data
*		test *testData - Test data structure which will compose the test printout
*		uint testCount - Number of test data items
*       test *executionData - Execution data from individual tests
*       executionCount - Count of execution data
*
*	Return Value:
*		TRUE/FALSE/ERROR
*
************************************************************************************/
short databaseSaveTestPrintout(PBOARDSTATE boardInfo,test *testData,uint testCount,test *executionData,uint executionCount)
{
    
    if (testCount == 0)
    {
        return TRUE;
    }

    DBUpdateBoardInfo(boardInfo);

    int i=0;

    char Query[BUF_LEN*2];

    snprintf(Query,BUF_LEN*2,"select count(attemptSequence) from %cTest History Query%c where FinishedGoodNumber=%lu and SerialNumber = %lu",TICK,TICK,boardInfo->boardInfo.finishedGoodNumber,boardInfo->boardInfo.serialNumber);

    Result_Set results;
    int res = databaseExecSQL(Query, &results, strlen(Query));
    if (res != TRUE )
    {
        databaseErrors();
        return res;
    }
    int newAttemptSequence=0;
    if (results.rows > 0)
    {
        newAttemptSequence = atoi(getResultData(&results,0,0))+1;
    }

    freeResultData(&results);


    char TestDataBuffer[LINE_BUF+2]="";
    char insertBuffer[BUF_LEN+LINE_BUF]="";
    // copy test data into our test data buffer
    
    strcpy(TestDataBuffer,testData[0].testData);
    // concatenate status into our test data buffer
    strcat(TestDataBuffer,testData[0].status);

    // ensure we remove all apostrophes from TestDataBuffer
    databaseRemoveIllegalCharacter('\'',TestDataBuffer,insertBuffer,sizeof(insertBuffer));

    snprintf(Query,BUF_LEN*2,"insert into %cTest History Query%c (SerialNumber,TestSequence,attemptSequence,FinishedGoodNumber,BoardName,BOMRev,BIOSRev,Processor,Tester,`date/time`,TestResults)values(%lu,%i,%i,%lu,'%s','%c','B','%s','%s',now(),'%s')",TICK,TICK,boardInfo->boardInfo.serialNumber,boardInfo->attempt-1,newAttemptSequence,boardInfo->boardInfo.finishedGoodNumber,boardInfo->boardInfo.boardName,boardInfo->boardInfo.finishedGoodRev,boardInfo->processor,boardInfo->boardInfo.tester,insertBuffer);
    


    res = databaseExecSQL(Query, NULL, strlen(Query));
    if (res != TRUE )
    {
        databaseErrors();
        return res;
    }
    int j=0;
    
    for (i=1; i < testCount; i++)
    {       

        strcpy(TestDataBuffer,testData[i].testData);
        strcat(TestDataBuffer,testData[i].status);
        // ensure we remove all apostrophes from TestDataBuffer
        databaseRemoveIllegalCharacter('\'',TestDataBuffer,insertBuffer,sizeof(insertBuffer));
        sprintf(Query,"update %cTest History Query%c set TestResults=CONCAT(TestResults,'%s') where TestSequence=%i and SerialNumber=%lu and FinishedGoodNumber=%lu and attemptSequence=%i",TICK,TICK,insertBuffer,boardInfo->attempt-1,boardInfo->boardInfo.serialNumber,boardInfo->boardInfo.finishedGoodNumber,newAttemptSequence);
        res = databaseExecSQL(Query, NULL, strlen(Query));
        if (res != TRUE )
        {

            databaseErrors();
            return res;
        }
    }
//test *executionData,uint executionCount
    char line[LINE_BUF+((LINE_BUF-MAX_TEST_LENGTH)+1)]="";
    //testBoundedColumnBring
    for (i=0; i < executionCount; i++)
    {
        snprintf(line,LINE_BUF+((LINE_BUF-MAX_TEST_LENGTH)),"%s%s",executionData[i].testData,executionData[i].status);
        sprintf(Query,"insert into TestExecutionData (attemptSequence,data,attempt,SerialNumber,FinishedGoodNumber) values (%i,'%s',%i,%lu,%lu)",newAttemptSequence,line,boardInfo->attempt-1,boardInfo->boardInfo.serialNumber,boardInfo->boardInfo.finishedGoodNumber);
        res = databaseExecSQL(Query, NULL, strlen(Query));
        if (res != TRUE )
        {
            databaseErrors();
            return res;
        }
    }

    return TRUE;
}

/************************************************************************************ 
*	getAttemptForSavedTestData
*	
*	Obtains the attempt for any saved temporary test data in the database. If a test
*   restarts, we will want to know the test's attempt so we can save it
*
*	Arguments:
*		ulong *bootMac - boot card mac address
*       short type - test type ( whether it is planned or not )
*
*	Return Value:
*		test attempt
*
************************************************************************************/

short getAttemptForSavedTestData(ulong *bootMac, short type)
{
    char Query[BUF_LEN];
	// create query. isTestData helps us determine if we are goign to pull test data
	// or diagnostic data. It only matters in the SQL query. After this point, we need not
	// be concerned with what type of test data this is
    sprintf(Query,"select attempt from testingData where bootMAC=%lu and planned =%i",*bootMac,type);

	// create data set
    Result_Set results;
	// set test structure pointer to NULL
    test *data = NULL;
    int r=0; // r is our TESTCOUNT
	// execute query and grab the result of the query
    int res = databaseExecSQL(Query, &results, BUF_LEN);

    short attempt = -1;
    if (res==TRUE)  // if our query is TRUE
    {
        attempt = atoi ( getResultData(&results,0,0) );

        freeResultData(&results);
    }

    return attempt;
}


/************************************************************************************ 
*	databaseGetTestData
*	
*	Creates a test data structure and returns data that may already be in the database
*
*	Arguments:
*		short isTestData - flag to indicate if we are dealing with test data
*		int *counter - pointer to test counter
*		ulong *FinishedGoodNumber - Pointer to finished good number
*		short *attempt - pointer to attempt
*
*	Return Value:
*		test Test - data Structure
*
************************************************************************************/
test *databaseGetTestData(short isTestData,int *counter, ulong *bootMac, short type)
{

    char Query[BUF_LEN];
	// create query. isTestData helps us determine if we are goign to pull test data
	// or diagnostic data. It only matters in the SQL query. After this point, we need not
	// be concerned with what type of test data this is
    sprintf(Query,"select data,passData from testingData where bootMAC=%lu and planned =%i",*bootMac,type);

	// create data set
    Result_Set results;
	// set test structure pointer to NULL
    test *data = NULL;
    int r=0; // r is our TESTCOUNT
	// execute query and grab the result of the query
    int res = databaseExecSQL(Query, &results, BUF_LEN);

    if (res==TRUE)  // if our query is TRUE
    {
        
        int i=0;
		// temporary pointers to make assigning our data easier
		// go ahead and set them to NULL
        char *mydata=NULL,*status=NULL;
        for (i =0; i < results.rows; i++) 
		{
			// increment our test count
            r++;
			// if status is NULL, set status to an empty string
            if (status == NULL) {
                status="";
            }
			// populate the test data and test status
            mydata = getResultData(&results,i,0);
            status = getResultData(&results,i,1);
            // reallocate size of testData so we can increase the size
            // of our buffer
            data = (test*)realloc(data,r*sizeof(test));
            // create memory for buffer
            

            // copy test data into buffer 
            strncpy(data[r-1].testData,mydata,sizeof(data[r-1].testData));

            // set the status of the current test to NULL
            
        
        
            // copy test status into buffer 
            if (status != NULL && strncmp(status,"(null)",6)  && strncmp(status,"NULL",4) && strlen(status) > 0)
            {
                //data[r-1].status=(char*) malloc( (strlen(status)+1) * sizeof(char));
				strncpy(data[r-1].status,status,sizeof(data[r-1].status));
            }                   
            else
            {
                memset(data[r-1].status,0x00,sizeof(data[r-1].status));
                data[r-1].status[ sizeof(data[r-1].status)-2 ] = 0x0a;
                data[r-1].status[ sizeof(data[r-1].status)-2 ] = 0x00;
            }

        }
		// free our result set
        freeResultData(&results);
    }
	// set the value of the memory pointer to by counter
	// to r
    *counter=r;

	// now, return our test data structure
    return data;


}

/************************************************************************************ 
*	databaseGetConfigurationData
*	
*	Returns configuration data for specified revision
*
*	Arguments:
*		ulong *fgNumber - finished good number
*		short *attempt - test revision
*
*	Return Value:
*		configuration data
*
************************************************************************************/
char *databaseGetConfigurationData(ulong *fgNumber, short *attempt)
{
    char *configData = NULL;
    char Query[BUF_LEN]; // query buffer
    // create query to obtain the required access level
    sprintf(Query,"select configData from testConfigData where fgNumber=%lu and revision=%lu",*fgNumber,*attempt);

    // create result set
    Result_Set results;
    // execute query and get return value
    int ret =  databaseExecSQL(Query, &results, BUF_LEN);
    if (ret == TRUE)
    { 
        // set the value of type to the integer representation of 
        // the result
        char *data  = getResultData(&results,0,0);

        configData = (char*)malloc(strlen(data)+1);
        strcpy(configData,data);

        // free the result set
        freeResultData(&results);
        
    }

    return configData;
}

/************************************************************************************ 
*	databaseGetTypeForAttempt
*	
*	Obtains the required access level for this test attempt
*
*	Arguments:
*		ulong FinishedGoodNumber 
*		short FinishedGoodNumber
*		short *type Pointer to variable which will hold the required access level
*
*	Return Value:
*		TRUE or FALSE depending on status of queries
*
************************************************************************************/
 short databaseGetTypeForAttempt(ulong FinishedGoodNumber, short attempt, short *type)
 {
    char Query[BUF_LEN]; // query buffer
    // create query to obtain the required access level
    sprintf(Query,"select type from fgnumbers where FGNumber=%lu and attempt=%i",FinishedGoodNumber,attempt);

    // create result set
    Result_Set results;
    // execute query and get return value
    int ret =  databaseExecSQL(Query, &results, BUF_LEN);
    if (ret == TRUE)
    { 
        // set the value of type to the integer representation of 
        // the result
        *type  = atoi ( getResultData(&results,0,0) );

        // free the result set
        freeResultData(&results);
        
    }

    // return the status of our query as it will be the litmus
    // test for this entire function
    return ret;
 }


/************************************************************************************ 
*	databaseGetSequenceForAttempt
*	
*	Gets the most recently executed test sequence that corresponds to FinishedGoodNumber
*	and the attempt
*
*	Arguments:
*		*FinishedGoodNumber - Finished Good Number for the current board
*		*attempt - Attempt for the current board
*		*sequence - pointer to sequence integer
*
*	Return Value:
*		void
*
************************************************************************************/
void databaseGetSequenceForAttempt(ulong *FinishedGoodNumber, short *attempt, short *sequence)
{
    char Query[BUF_LEN];
	// create query
	sprintf(Query,"select max(currentSequence) from testingData where FinishedGoodNumber=%i and Attempt =%i",*FinishedGoodNumber,*attempt);
    
	Result_Set results; // our normal result set
	int ret = databaseExecSQL(Query, &results, BUF_LEN); // execute our query

	// if the query was a success...
	if ( ret == TRUE)
	{

        // if the field is null, that means there is no current sequence
        if (strncmp(getResultData(&results,0,0),"NULL",4)  )
        {
            // dereference sequence and assign the integer representatoin
            // of the result to sequence
            *sequence = atoi( getResultData(&results,0,0) );
        }
		
		freeResultData(&results);
	}
}


/************************************************************************************ 
*	databaseGetUserAccessLevel
*	
*	Attempts to retrieve the associated MAC address's user access level
*
*	Arguments:
*		char *macAddress 
*
*	Return Value:
*		The user's access level
*
************************************************************************************/
int databaseGetUserAccessLevel(char *macAddress)
{
    // if the mac address is null, or its length is zero
    // we assume the user has production level access only
    if (macAddress == NULL || strlen(macAddress) == 0) {
        return PRODUCTION_ACCESS_LEVEL;
    }


    char Query[BUF_LEN];

    // create our SQL query from the above buffer
        //Test_Software
    sprintf(Query,"SELECT access FROM software.macAuthentication WHERE macAddress='%s'",macAddress);

    int ret = PRODUCTION_ACCESS_LEVEL;
    Result_Set results;
    //call DB Select function to get out value back
    char result = (char)databaseExecSQL(Query, &results, 20);
    if(result == TRUE){
    
        // if we have results, set ret to integer representation
        // of our return value
        if ( results.rows > 0 ) {
            
            ret = atoi( results.resultData[0][0] );
        }

        // fre our data
        freeResultData(&results);
    
    }	 	 	 
    
    // return the user's access level
    return ret;
}




/************************************************************************************ 
*	databaseInsertTemporaryTestData
*	
*	Inserts temporary data into `testData`, which will allow testing to continue the
*   sequence of test events following a restart. 
*
*	Arguments:
*		PTESTBOARDINFO struct containing the common board info
*       uint counter for test data
*		test struct containing the test data
*       uint counter for diagnostic data data
*		test struct containing the diagnostic data
*	
*	Return Value:
*		TRUE or FALSE depending on successful action of the database
*
************************************************************************************/
short databaseInsertTemporaryTestData(PBOARDSTATE info,ushort TESTCOUNT, test *testData,ushort DIAGCOUNT, test *diagnosticData, ushort start, short PLANNED)
{

    
    // Go ahead and remove all temporary data which may be associated
    // with this FGNumber and test attempt
    short att = info->attempt-1;
    // remove all data ONLY if we are at the beginning
    databaseRemoveTemporaryTestData(&info->boardInfo.bootMacNumber);
    


    char Query[BUF_LEN*2]; // create query buffer

    short i = start; 

    // character which will be used to hold the result
    // of each SQL query
    char res=0x00;

    // iterate through the testData
    for (; i < TESTCOUNT; i++) {
        
        // create sql query
        char d[2555]="";
        strcpy(d,testData[i].testData);
        strcpy(d,testData[i].status);
        sprintf(Query,"insert into testingData (planned,bootMAC,FinishedGoodNumber,Attempt,currentSequence,data,passData,type) values(%i,%lu,%i,%i,%i,'%s','%s',%i)",PLANNED,info->boardInfo.bootMacNumber,info->boardInfo.finishedGoodNumber,info->attempt-1,info->currentSequence,testData[i].testData,testData[i].status,info->accessLevel);
        // execute sql query, and dump any error information
        // if it exists
        res = databaseExecSQL(Query,NULL ,BUF_LEN); 
		if(res != TRUE){
            databaseErrors();
			return FALSE;
		}
    }

    // iterator through all diagnostic data
    for (i=start; i < DIAGCOUNT; i++) {
        
        // create sql query
        sprintf(Query,"insert into testingData (planned,bootMAC,FinishedGoodNumber,Attempt,currentSequence,data,passData,testData,type) values(%i,%lu,%i,%i,%i,'%s','%s',0,%i)",PLANNED,info->boardInfo.bootMacNumber,info->boardInfo.finishedGoodNumber,info->attempt,info->currentSequence,diagnosticData[i].testData,diagnosticData[i].status,info->accessLevel);
        res = databaseExecSQL(Query,NULL ,BUF_LEN); 
        // execute sql query, and dump any error information
        // if it exists
		if(res != TRUE){
            databaseErrors();
			return FALSE;
		}
    }
    
    // return success
    return TRUE;

}


/************************************************************************************ 
*	databaseGetBoardAttemptList
*	
*   Creates a list of attempts associated with FinishedGoodNumber. The list will
*   contain all attempts and the access level required to execute this test attempt
*
*	Arguments:
*		unsigned long FinishedGoodNumber 
*       char *buffer Buffer in which to place attempt list
*	
*	Return Value:
*		TRUE or FALSE depending on successful action of the database
*
************************************************************************************/
short databaseGetBoardAttemptList(ulong *bootMac,char *buffer)
{
    // if our buffer is null, we need to return FALSE
    if (buffer==NULL) {
        return FALSE;
    }

    char Query[BUF_LEN*2]; // create buffer for query

    // this query is used to search previously executed tests
    // which may be present to allow the user to continue a previous 
    // test attempt
    sprintf(Query,"select distinct testingData.attempt,testingData.type from testingData,fgnumbers where testingData.FinishedGoodnumber=%lu and testingData.planned=1 order by attempt",*bootMac);

    Result_Set results; //  result set

    int res = databaseExecSQL(Query, &results, BUF_LEN);
    int i=0;
    if(res != TRUE){
        // if we query didn't execute properly, printout any debug/error data
        databaseErrors();
        
    }
    else
    {
        
    
        
        for (i=0; i < results.rows; i++) {
            // the format of the attempts and access levels is
            // Attempt:AccessLevel -- yeah, this isn't the best way, but it works
            strcat(buffer,getResultData(&results,i,0));
            strcat(buffer,":");
            strcat(buffer,getResultData(&results,i,1));
            // now, if we're not on the last row, we place a @ at the end
            // of the string since we attempt/access level combos are split
            // by @s
            if (i!=results.rows-1) {
                strcat(buffer,"@");    
            }
        }
    }

    // The previously executed tests are split apart from the list of all
    // test attempts by an equal sign (=)
    strcat(buffer,"=");    

    // now, execute the query to obtain all test attempts for FinishedGoodNumber
    sprintf(Query,"select attempt,type from fgnumbers where fgNumber=%lu order by attempt",*bootMac);

    // execute the query
    res = databaseExecSQL(Query, &results, BUF_LEN);
    if(res != TRUE){
        // if an error occurs this time, we print any error/debug data, and return
        // FALSE
        databaseErrors();
        return FALSE;
    }
    // the same format applies for this listing as above
    for (i=0; i < results.rows; i++) {

        strcat(buffer,getResultData(&results,i,0));
        strcat(buffer,":");
        strcat(buffer,getResultData(&results,i,1));
        if (i!= results.rows-1) {
            strcat(buffer,"@");    
        }
    }
    // free our result set data
    freeResultData(&results);

    /* okay, now a quick overview/example of how the attempt list will be formed and parsed
     * previous test attempts, ie. a test which may still be in progress will be first in the list
     * then, all test attempts associated with the FinishedGoodNumber will come. The in progress
     * tests are separated from all attempts by an equal sign. Each test attempt is paired with 
     * the access level which is required to execute the attempt. The test attempt and access level
     * are separated by a colon. The pairs are separated by @ symbols. Thus, you may have something
     * like this: 5:0=4:2@3:0. This should be read as
     * Attempt 5 (V1.4) requires access level 0 to continue
     * Attempt 4 (V1.3) requires access level 2 to begin
     * Attempt 3 (V1.2) requires access level 0 to begin
     */
    return TRUE;


}

/************************************************************************************ 
*	DBUpdateBoardInfo
*	
*   Updates current board information
*
*	Arguments:
*		PBOARDSTATE info - Board buffer
*	
*	Return Value:
*		TRUE or FALSE depending on successful action of the database
*
************************************************************************************/

int DBUpdateBoardInfo(PBOARDSTATE info){
    char Query[BUF_LEN]; 
    // delete current  board information
    sprintf(Query,"delete from boardinfo where FinishedGoodNumber=%lu",info->boardInfo.finishedGoodNumber);

    Result_Set results;
    int res = databaseExecSQL(Query, &results, BUF_LEN);

    if(res != TRUE){
        databaseErrors();
        return FALSE;
    }

    // now, insert board information into the database
    sprintf(Query,"insert into boardinfo (FinishedGoodNumber,attempt,SerialNumber,BoardName,BOMRev,Processor) values(%lu,%i,%lu,'%s','%c','%s')",info->boardInfo.finishedGoodNumber,info->attempt-1,info->boardInfo.serialNumber,info->boardInfo.boardName,info->boardInfo.finishedGoodRev,info->processor);

    res = databaseExecSQL(Query, &results, BUF_LEN);
    if(res != TRUE){
        databaseErrors();
        return FALSE;
    }

    // delete MAC information
    sprintf(Query,"Delete from MACs where SerialNumber=%lu and testSequence=%i",info->boardInfo.serialNumber,info->attempt-1);
    res = databaseExecSQL(Query, &results, BUF_LEN);
    if(res != TRUE){
        databaseErrors();
        return FALSE;
    }
    char timeBuffer[BUF_LEN]="";
    if ( databaseMakeTime(timeBuffer) == FALSE)
        return FALSE;
    sprintf(Query,"insert into MACs (DTStamp,testSequence,SerialNumber,MacStatus,macAddress) values('%s',%i,%lu,'used',%lu)",timeBuffer,info->attempt-1,info->boardInfo.serialNumber,info->boardInfo.bootMacNumber);

    res = databaseExecSQL(Query, &results, BUF_LEN);
    if(res != TRUE){
        databaseErrors();
        return FALSE;
    }




    return TRUE;
}


//Generic DB Functions written to utilize ODBC calls

/************************************************************************************ 
*	databaseExecSQL
*	
*	This function does all the ODBC dirty work (other than connect and disconnect) for
*	running an SQL statement on the database. It process the results and appends
*	them all to the ReturnBuffer. If the statement was a select, data will be
*   placed in userResultSet
*
*	NOTE: A NULL value is returned as "NULL"
*
*	Arguments:
*		char * SqlQuery - string to use as the SQL statment
*		Result_Set * userResultSet - Pointer in which results will be placed
*
*	Return Value:
*		TRUE or FALSE depending on successful query and errors
*
************************************************************************************/
 int databaseExecSQL(char* SqlQuery, Result_Set *userResultSet, unsigned int RetBufLen){	
	
	unsigned short	SqlColCount;
    SQLINTEGER  SqlRowCount;

	short i; 
	char FetchBuffer[RET_BUF_LEN] = ""; /* buffer for a column's value*/
	SQLINTEGER charCount;			/*count of chars returned by SQLGetData*/


	if(ConnectionStatus != CONNECTED){		//Make sure we're connected before trying to query
		if(databaseConnect(dsn,uid,pwd) == FALSE){			//If we still can't connect Inform User and return FALSE
            consolePrint("Cannot connect to database!\n");
			return FALSE;
		}
	}

	//Now allocate a handle for the query statement 
	SqlResult = SQLAllocHandle(SQL_HANDLE_STMT, SqlConnectHandle, &SqlStatementHandle);
    
	if (databaseResultFail(SqlResult))
	{
        databaseErrors();
		databaseDisconnect(); 
		return FALSE;
	}
    
	// Run Query once without preparation
	SqlResult = SQLExecDirect(SqlStatementHandle, (uchar *) SqlQuery, SQL_NTS);
	if(databaseResultFail(SqlResult)){	//error in execution
		//could get the error code from SQLError
        databaseErrors();
		databaseDisconnect();
		return FALSE;
	}

	// Find Column Count
	SqlResult = SQLNumResultCols(SqlStatementHandle,&SqlColCount);
	if(databaseResultFail(SqlResult)){ 	//error getting count
		databaseErrors();
		databaseDisconnect();
		return FALSE;
	}

	// Find Row Count
	SqlResult = SQLRowCount(SqlStatementHandle,&SqlRowCount);
	if(databaseResultFail(SqlResult)){ 	//error getting count
		databaseErrors();
		databaseDisconnect();
		return FALSE;
	}

	//if we are not performing a Select, we need to quit here
	memcpy(FetchBuffer, SqlQuery, 6); 
	makeLowerCase(FetchBuffer); 

	if(memcmp(FetchBuffer,"select",6) != 0){

        SQLFreeHandle(SQL_HANDLE_STMT, SqlStatementHandle);
        return TRUE;        
	}

	//if we get no results for a Select a special return value is given
	if(SqlRowCount == 0)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, SqlStatementHandle);
        return ZERO_RESULTS;
    }
		

    
    // Cycle through the results creating a return string
    userResultSet->rows = SqlRowCount;
    userResultSet->columns = SqlColCount;

    
    createResultDataSet( userResultSet );
    

    unsigned int rowCounter=0;
    
	while(1){

		SqlResult = SQLFetch(SqlStatementHandle);  

		//if there's no data left, leave loop
		if(SqlResult == SQL_NO_DATA || SqlResult == SQL_NO_DATA_FOUND)
			break; 

		if(SqlResult == SQL_ERROR){ // report any errors that may occur
			databaseErrors(); 
            SQLFreeHandle(SQL_HANDLE_STMT, SqlStatementHandle);
			return FALSE;
		}

		// get resulting data.fetch each column as a char and let the calling function
		// deal with any conversions needed


		// get value for each column for this row in the result set
        
		for(i=1; i <= SqlColCount; i++){

			memset(FetchBuffer, 0x00, RET_BUF_LEN); //clear the buffer
			charCount =0; 

			//get the col's value as a char
			SqlResult = SQLGetData (SqlStatementHandle, i, SQL_CHAR, FetchBuffer, RET_BUF_LEN, &charCount);
			if (databaseResultFail(SqlResult)){
				databaseErrors();
                SQLFreeHandle(SQL_HANDLE_STMT, SqlStatementHandle);
				return FALSE;
			}

			if(SqlResult == SQL_SUCCESS_WITH_INFO){ 

				//we are most likely selecting LONGTEXT, so we have to treat it uniquely
				
				SQLCHAR       SqlState[6],Msg[SQL_MAX_MESSAGE_LENGTH];
				SQLINTEGER    NativeError;
				SQLSMALLINT    MsgLen;
				
				SQLGetDiagRec(SQL_HANDLE_STMT, SqlStatementHandle, 1, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen); 
                if(memcmp(SqlState, "01004", 5) == 0){ 
					//we have a truncation warning: we know we need to keep 
					// pulling from that column until we get a SQL_SUCCESS
					LongText = TRUE; //set the flag
					i--; //decrease i so that we query the same column again next time
				}
			}

			if(SqlResult == SQL_SUCCESS && LongText)
				LongText = FALSE; //we reset this flag because we are done with getting the LONGTEXT
				
			//append this value to the return buffer
			// a pipe seperates column values
			
			if(charCount == SQL_NULL_DATA){ //we've got a NULL value
				//Check if the buffer's going to fail on us
			
                userResultSet->resultData[rowCounter][i-1] = (char*)malloc(5*sizeof(char*));
                // marker 585
                if (userResultSet->resultData[rowCounter][i-1]==NULL)
                {
                    fprintf(stderr,"Error allocating memory at marker 585");
                    exit(1);
                }
                sprintf(userResultSet->resultData[rowCounter][i-1],"NULL");
			}
			else{ //otherwise, add on the column's value and the pipe

                
                if (userResultSet->resultData[rowCounter][i-1]==NULL) {
                    userResultSet->resultData[rowCounter][i-1] = (char*)malloc((charCount+1)*sizeof(char*));
                    // market 592
                    if (userResultSet->resultData[rowCounter][i-1]==NULL)
                    {
                        fprintf(stderr,"Error allocating memory at marker 592");
                        exit(1);
                    }
                    strcpy(userResultSet->resultData[rowCounter][i-1],FetchBuffer);
                   
                }
                else
                {

                    // if our current data pointer is not NULL, that means we are working with a 
                    // LONGTEXT column, and such should reallocate the pointer to account for this
                    userResultSet->resultData[rowCounter][i-1] = (char*)realloc(userResultSet->resultData[rowCounter][i-1],strlen(userResultSet->resultData[rowCounter][i-1])+(charCount+1)*sizeof(char));
                    // marker 5A1
                    if (userResultSet->resultData[rowCounter][i-1]==NULL)
                    {
                        fprintf(stderr,"Error allocating memory at marker 5A1");
                        exit(1);
                    }
                    userResultSet->resultData[rowCounter][i-1] = strcat(userResultSet->resultData[rowCounter][i-1],FetchBuffer);
                    // need to reallocate memory
                }

			}

		}//end for
        
        rowCounter++;

	}//end while 

	// initially, this wasn't here. Valgrind helped demonsrate
    // that nearly all of our memory leaks originated from
    // this file
    SQLFreeHandle(SQL_HANDLE_STMT, SqlStatementHandle);
    // If we didn't error out anywhere, return true	
	return TRUE;
 }


/************************************************************************************ 
*	databaseConnect
*	
*	databaseConnect attempts to create a connection to the SQL server using the provided
*   information
*
*
*	Arguments:
*		uchar *DBDsn -- character represenation of the database type
*       uchar *DBUser -- Username used to connect to DB Server
*       uchar *DBPassword -- password for username
*
*	Return Value:
*		TRUE or FALSE depending on connection attempt
*
************************************************************************************/
 int databaseConnect(uchar* DBDsn, uchar* DBUser, uchar* DBPassword){

	//if we are already connected, return true
	if(ConnectionStatus == CONNECTED)
		return TRUE;

    
	// 1. allocate Environment handle and register version 
	SqlResult=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&SqlEnvironment);
	if (databaseResultFail(SqlResult))
	{
    
            databaseErrors(); // report errors
			return FALSE;
	}
    
	SqlResult=SQLSetEnvAttr(SqlEnvironment, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
	if (databaseResultFail(SqlResult))
	{
        databaseErrors(); // report errors		
        SQLFreeHandle(SQL_HANDLE_ENV, SqlEnvironment);
        return FALSE;
	}
    
	// 2. allocate connection handle, set timeout
	SqlResult = SQLAllocHandle(SQL_HANDLE_DBC, SqlEnvironment, &SqlConnectHandle); 
	if (databaseResultFail(SqlResult))
	{
        databaseErrors(); // report errors
        SQLFreeHandle(SQL_HANDLE_ENV, SqlEnvironment);
        return FALSE;
	}
    
	SQLSetConnectAttr(SqlConnectHandle, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
    
	// 3. Connect to the datasource
	SqlResult = SQLConnect(SqlConnectHandle, (SQLCHAR*) DBDsn, strlen(DBDsn), 
										(SQLCHAR*) DBUser, strlen(DBUser), (SQLCHAR*) DBPassword, strlen(DBPassword));
     					
	if (databaseResultFail(SqlResult))
	{
        databaseErrors(); // report errors
		SQLFreeHandle(SQL_HANDLE_DBC, SqlConnectHandle);
        SQLFreeHandle(SQL_HANDLE_ENV, SqlEnvironment);
        return FALSE;
	}

	//determine what kind of DBMS we are working with
	char dbms[BUF_LEN];
	memset(dbms, 0x00, BUF_LEN); 
	SQLSMALLINT RetLen=0; 

	SqlResult = SQLGetInfo(SqlConnectHandle,SQL_DBMS_NAME,dbms, BUF_LEN, &RetLen); 
	if(databaseResultFail(SqlResult)){
        databaseErrors(); // report errors
		
	}

    if(memcmp(dbms, "Microsoft", 9) == 0)	//we are using MSSQL
    {
    
		usingMSSQL = TRUE; 
        TICK='['; // set the tick for the DBMS type
    }
	else
    {
        TICK='`'; // set the tick for the DBMS type
		usingMSSQL = FALSE;
    }
	
	ConnectionStatus = 	CONNECTED;		//update the connection status
	
	return TRUE; 
}

 /************************************************************************************ 
 *	databaseDisconnect
 *	This function disconnects the global variables from the currently used database
 *	If the program is not currently connected to a database, it also returns TRUE.
 *	If any Memory freeing funciton fails, FALSE is returned.
 *
 *	Arguments: NONE
 *		
 *	Returns:
 *	TRUE on successful disconnection or nothing to disconnect from. FALSE otherwise
 ************************************************************************************/

int databaseDisconnect(){

	if(ConnectionStatus == NOT_CONNECTED)
		return TRUE;

	SqlResult = SQLDisconnect(SqlConnectHandle);
	if(databaseResultFail(SqlResult))		//error
	{
		databaseErrors(); // report errors
		return FALSE;
	}

	SqlResult = SQLFreeHandle(SQL_HANDLE_DBC, SqlConnectHandle);
    if(databaseResultFail(SqlResult))		//error
	{
		databaseErrors(); // report errors
		return FALSE;
	}

	SqlResult = SQLFreeHandle(SQL_HANDLE_ENV, SqlEnvironment); 
	if(databaseResultFail(SqlResult)) 	//error
	{
		databaseErrors(); // report errors
		return FALSE;
	}
	
	ConnectionStatus = NOT_CONNECTED;
	
	return TRUE;
}

 /************************************************************************************ 
 *	databaseResultFail
 *	This function is simply for easier coding in the above functions. It tests the result
 *	variable and if it is not Successful, TRUE is returned. [YES there's an error].
 *	Otherwise, FALSE is returned.
 *	
 *	Arguments:
 *	long Result - return value from another function that we'll be testing for failure
 *
 *	Returns:
 *	TRUE on Error, False otherwise
 ************************************************************************************/
int databaseResultFail(long Result){
	if(Result != SQL_SUCCESS && SqlResult != SQL_SUCCESS_WITH_INFO){

		databaseErrors(); // report errors
		return TRUE;
	}
	else
		return FALSE;


}

 /************************************************************************************ 
 *	databaseErrors
 *	This function is called by databaseResultFail to output specific reasons the attempt failed
 *	
 *	Arguments:
 *	none
 *
 *	Returns:
 *	TRUE - since we already know errors occurred
 ************************************************************************************/

int databaseErrors(){

  uchar buf[250];
  uchar sqlstate[15];
  uchar PrintBuffer[500];

  /*
   *  Get statement errors
   */
  while (SQLError (SqlEnvironment, SqlConnectHandle, SqlStatementHandle, sqlstate, NULL,buf, sizeof (buf), NULL) == SQL_SUCCESS){

      consolePrint("%s, SQLSTATE=%s\n",buf,sqlstate);
	  
  }

  /*
   *  Get connection errors
   */
  while (SQLError (SqlEnvironment, SqlConnectHandle, SQL_NULL_HSTMT, sqlstate, NULL,buf, sizeof (buf), NULL) == SQL_SUCCESS){
        consolePrint("%s, SQLSTATE=%s\n",buf,sqlstate);
  }

  /*
   *  Get environmental errors
   */
  while (SQLError (SqlEnvironment, SQL_NULL_HDBC, SQL_NULL_HSTMT, sqlstate, NULL, buf, sizeof (buf), NULL) == SQL_SUCCESS){
        consolePrint("%s, SQLSTATE=%s\n",buf,sqlstate);

  }

  return TRUE;
}


/***********************************************************************
*	databaseMakeTime
*	This function creates a time string used when inserting values into 
*	the MACs table in the database.
*	It is necessary because the database column is a VARCHAR type.
*	Form: 'mm/dd/yyyy hh:mm:ss AM' 
*	
*	Arguments: 
*		char* to buffer for storing the return value.
*		IT SHOULD BE PASSED IN INITIALIZED WITH ALL 0x00
*	Return Value:
*		char * with time string it it
*		
***********************************************************************/
int databaseMakeTime(char* myTime){

	//build time string for dtstamp in MACs
	time_t rawtime;
	time(&rawtime);
	struct tm *timeStruct;
	timeStruct = gmtime(&rawtime); 
	int pm =0;
	char buffer[8]; 

	sprintf(buffer, "%i", (timeStruct->tm_mon + 1)); 
	if(strlen(buffer)==1)	//pad with a 0 if under 10
	{
		buffer[1] = buffer[0];
		buffer[0] = '0';
		buffer[2] = '\0';
	}

	//add the month to our resulting string with a / after it
	strcat(myTime, buffer); 
	strcat(myTime, "/"); 

	sprintf(buffer, "%i", (timeStruct->tm_mday)); 
	if(strlen(buffer)==1)	//pad with a 0 if under 10
	{
		buffer[1] = buffer[0];
		buffer[0] = '0';
		buffer[2] = '\0';
	}

	//add the dat to our resulting string with a / after it
	strcat(myTime, buffer); 
	strcat(myTime, "/"); 

	sprintf(buffer, "%i", (timeStruct->tm_year + 1900)); 
	strcat(myTime, buffer); 
	strcat(myTime, " "); 

	int myHour = timeStruct->tm_hour -5; //hour is 0-23. +1 for 1-24 and -6 for UTC

	if(myHour >12)
	{
		myHour = myHour-12;
		pm = 1; 
	}

	sprintf(buffer, "%i", myHour); 
	strcat(myTime, buffer); 
	strcat(myTime, ":"); 

	sprintf(buffer, "%i", (timeStruct->tm_min)); 
	if(strlen(buffer)==1)	//pad with a 0 if minutes under 10
	{
		buffer[1] = buffer[0];
		buffer[0] = '0';
	}
	strcat(myTime, buffer); 
	strcat(myTime, ":"); 

	sprintf(buffer, "%i", (timeStruct->tm_sec)); 
	if(strlen(buffer)==1)	//pad with a 0 if under 10
	{
		buffer[1] = buffer[0];
		buffer[0] = '0';
	}
	strcat(myTime, buffer); 

	if(pm==1)
		strcat(myTime, " PM");
	else
		strcat(myTime, " AM");

	return TRUE;
}


