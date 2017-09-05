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
    \brief This file holds declarations for the functions in DBFunc.c
*/

#ifndef _DBFUNC
    #define _DBFUNC 1

/*! \def _DBFUNC
    \brief ifndef flag for the header
*/

    #include <stdio.h>
    #include <string.h>
    #include <sql.h>
    #include <sqlext.h>
    #include <odbcinst.h>
    #include "../Common.h"
    #include "../BoardInfo.h"
    #include "DBResultFunctions.h"

    #define NOT_CONNECTED 	0
    #define CONNECTED		1
    #define ZERO_RESULTS	-2
    #define BUFFER_OVERFLOW -3
    #define BOARD_EXISTS	-4

    #define EXPERIMENTAL_LEVEL 0
    #define VALIDATION_LEVEL   1

    #define RET_BUF_LEN 3000

    #define __UCHAR_DEFINED__
    #define __ODBC_DEFINED__

/*! \def NOT_CONNECTED
    \brief Flag for database connection state
*/
/*! \def CONNECTED
    \brief Flag for database connection state
*/
/*! \def ZERO_RESULTS
    \brief Return value for databaseExecSQL Function
*/
/*! \def BUFFER_OVERFLOW
    \brief Return value for databaseExecSQL Function
*/
/*! \def BOARD_EXISTS
    \brief Return value for databaseInsertNewBoard Function
*/
/*! \def RET_BUF_LEN
    \brief Preprocessor value for a Buffer size
*/

//test parameters structure : Matches `Tests` Table in DB

/*! \struct TESTPARAMETERS DBFunc.h 
   \brief This struct holds the data associated with a test pulled from the database

    PTESTPARAMETERS is a pointer to this structure. 
*/

typedef struct
{
    /*! \var char Test[MID_BUF]
       \brief buffer for storing the Name of the Test
   */

    /*! \var char TestParam[MID_BUF]
        \brief buffer for storing the Test Parameters
    */
    /*! \var char Driver[MID_BUF]
        \brief buffer for storing the Driver needed for the test
    */
    /*! \var char DriverParam[MID_BUF]
        \brief buffer for storing the Parameters for the driver
    */
    /*! \var char Type[SMALL_BUF]
        \brief buffer for storing the type of test
        This is either 1,2,3 or experimental, validation, production
    */
    /*! \var unsigned long FGNumber
        \brief variable for the Finished Good Number
    */
    /*! \var unsigned short TestNumber
        \brief variable for the Test Number 
    */
    /*! \var unsigned char BurnIn
        \brief variable for the Burn In Flag
    */
    /*! \var unsigned char Production
        \brief variable for the Production flag
    */
    /*! \var unsigned short Sequence
        \brief variable for the Test Sequence number
    */

    /*! \var unsigned short Sequence
        \brief variable for the Test Sequence number
    */

    /*! \var unsigned char activeSequence
        \brief variable which identifies a nonexecuting test
    */

    char            Test[MID_BUF];          // Name of test in Db table
    char            TestParam[MID_BUF];     // Optional parameter to test
    char            Driver[MID_BUF];        // Optional Name of driver to load
    char            DriverParam[MID_BUF];   // Optional Driver Parameter
    char            Type[SMALL_BUF];        // Test Type, 1,2,3 : experimental, validation, production
    short           attempt;
    unsigned long   FGNumber;               // Finihsed Good Number of board
    unsigned short  TestNumber;             // Test number associated with test name
    unsigned char   BurnIn;                 // Flag if it's used in BurnIn
    unsigned char   Production;             // Flag if it's used in Production 
    short  Sequence;                // order the test is run VS other for same FGNumber

    unsigned char   activeSequence;     // tells us whether or not this sequence is active or not

} TESTPARAMETERS, *PTESTPARAMETERS;

/*! \var typedef TESTPARAMETERS *PTESTPARAMETERS
    \brief A pointer TypeDef for the TESTPARAMETERS struct
*/



//upper level database functions

/*! \fn short databaseIsValidUsernamePassword(char *username, char *password)
    \brief Determines whether or not the username and password are valid
    \param username
    \param password
    \return TRUE or FALSE
*/

short databaseIsValidUsernamePassword(char *username, char *password);

/*! \fn int databaseSaveTestPrintout(PBOARDSTATE boardInfo,test *testData,uint testCount)
    \brief Saves the test data as a test printout in `Test History Query
    \param boardInfo Current Board data
    \param testData Test data which will compose the test printout
    \param testcount Count of test data items
    \return TRUE or FALSE
*/

short databaseSaveTestPrintout(PBOARDSTATE boardInfo,test *testData,uint testCount,test *executionData,uint executionCount);

/*! \fn int databaseGetBoardAttemptList(ulong bootMac, char *buffer)
    \brief Creates a list of attempts associated with FinishedGoodNumber and places this 
        list into buffer
    \param FinishedGoodNumber Finished Good Number which will be used to search the database
    \param buffer Buffer in which to place list of attempts
    \return TRUE or FALSE
*/
short databaseGetBoardAttemptList(ulong *bootMac, char *buffer);

/*! \fn void databaseRemoveTemporaryTestData(ulong *FinishedGoodNumber, short *attempt)
    \brief Remove all temporary data associated with the FinishedGoodNumber and attempt
    \param FinishedGoodNumber FG Number for the current board
    \param attempt Current test attempt
    \return void
*/
void databaseRemoveTemporaryTestData(ulong *bootMac);

/*! \fn void databaseRemoveIllegalCharacter(char charToRemove,char *stringToSearch, char* newBuffer, int bufLen)
    \brief Removes all instances of 'chartToRemove' from stringToSearch
    \param charToRemove Character to locate and remove
    \param stringToSearch String to search
    \param newBuffer Buffer in which to place new string
    \param bufLen Maximum length of newBuffer
    \return void
*/

void databaseRemoveIllegalCharacter(char charToRemove,char *stringToSearch, char* newBuffer, int bufLen);
/*! \fn void databaseGetSequenceForAttempt(ulong *FinishedGoodNumber, short *attempt, short *sequence);
    \brief Locates the most recent sequence in the test attempt, if it exists
    \param FinishedGoodNumber FG Number for the current board
    \param attempt Current test attempt
    \param sequence Pointer to sequence integer
    \return void
*/
void databaseGetSequenceForAttempt(ulong *FinishedGoodNumber, short *attempt, short *sequence);

short getAttemptForSavedTestData(ulong *bootMac, short type);

/*! \fn test* databaseGetTestData(short isTestData,int *counter, ulong *FinishedGoodNumber, short *attempt);
    \brief Retrieves all test data for provided FGNumber and test attempt
    \param isTestData Flag to indicate whether we are dealing with test data or diagnostic data
    \param counter Counter pointer for test or diagnostic data counter
    \param FinishedGoodNumber FG Number for the current board
    \param attempt Current test attempt
    \return Test data structure
*/
test *databaseGetTestData(short isTestData,int *counter, ulong *bootMac, short type);


/*! \fn int databaseGetBoardName(ulong FinishedGoodNumber, char *BoardName)
    \brief Populates BoardName with the name of board associated with FinishedGoodNumber
    \param FinishedGoodNumber Finished Good Number which will be used to search the database
    \param BoardName Buffer to place board Name
    \return TRUE or FALSE
*/
short databaseGetBoardName(ulong FinishedGoodNumber, char *BoardName);


/*! \fn int databaseGetFinishedGoodNumber(ulong SearchValue, ulong* ReturnValue)
    \brief This Functions is currently not defined or used
    \warning This Functions is currently not defined or used
*/
int databaseGetFinishedGoodNumber(ulong SearchValue, ulong* ReturnValue);


/*! \fn short databaseIsValidFinishedGoodNumber(ulong &FinishedGoodNumber)
    \brief Returns whether or not the provided finished good number is valid
    by querying the database
    \param FinishedGoodNumber The board's finished good Number
    \return TRUE or FALSE
*/
short databaseIsValidFinishedGoodNumber(ulong *FinishedGoodNumber);

/*! \fn int databaseGetFinishedGoodRev(ulong SearchValue, uchar* ReturnValue)
    \brief This function gets the Finished Good Rev (BOMRev) from the database
    \param SearchValue The value searched with (the Finished Good Number)
    \param ReturnValue Pointer to a buffer used to store the return string
    \return TRUE or FALSE
*/
int databaseGetFinishedGoodRev(ulong SearchValue, uchar* ReturnValue);

/*! \fn int databaseGetSerialNumber(ulong SearchValue, ulong* ReturnValue)
    \brief This function gets the Serial Number from the database
    \param SearchValue The value searched with (the Finished Good Number)
    \param ReturnValue Pointer to an unsigned long value where the Serial can be stored
    \return TRUE or FALSE
*/
int databaseGetSerialNumber(ulong SearchValue, ulong* ReturnValue);


/*! \fn int databaseGetBootMacNumber(PBOARDSTATE info, ulong* ReturnValue)
    \brief This function gets the Boot MAC Address from the database
    \param info test structure
    \param ReturnValue Pointer to an unsigned long value where the MAC can be stored
    \warning The MAC is returned in base 10, and the initial 002013 for TBS is not included
    \return TRUE or FALSE
*/
int databaseGetBootMacNumber(PBOARDSTATE info, ulong* ReturnValue);

/*! \fn int databaseInsertNewBoard(TESTBOARDINFO info)
    \brief This function inserts a new board's info into the database
    \param info The struct containing the basic board info
    \return TRUE or FALSE or BOARD_EXISTS
*/
int databaseInsertNewBoard(TESTBOARDINFO info);

//Functions to retrieve entries from the `Tests` Table

/*! \fn int databaseGetFirstTest(PTESTPARAMETERS tp)
    \brief This function retrieves the first entry in the Tests table for the board identified in the struct
    \param tp The struct whose info is used to find a board's tests in the database
    \return TRUE or FALSE
*/
int databaseGetFirstTest(PTESTPARAMETERS tp); 

/*! \fn int databaseGetTest(PTESTPARAMETERS tp)
    \brief This function retrieves the entry in the Tests table for the board identified in the struct
    \param tp The struct whose info is used to find a board's tests in the database
    \param testType Type of test being performed
    \return TRUE or FALSE

    The test sequence value in the struct determines which test to retrieve. Therefore, if databaseGetFirstTest was just called
    then setting tp.TestSequence to 1 would retrieve the next test, and so on.  
*/
int databaseGetTest(PTESTPARAMETERS tp, short testType); 

/*! \fn int databaseGetNumberOfTests(unsigned long FGNumber,short testType, unsigned int *Sequence)
    \brief This function retrieves the number of tests that are associated with a board in the Tests table.
    \param FGNumber The Finished Good Number whose test count is needed
    \param attempt Test attempt
    \param testType Test type
    \param Sequence Pointer to an INT where the count can be stored
    \return TRUE or FALSE
    \warning The Sequence count in the table begins with Zero.
*/
int databaseGetNumberOfTests(unsigned long FGNumber,unsigned int attempt,short testType, unsigned int *Sequence);


/*! \fn int databaseGetUserAccessLevel(char *macAddress)
    \brief Returns the access level associated with mac address
    \param macAddress MAC address used to locate user's access level
*/
int databaseGetUserAccessLevel(char *macAddress);

/*! \fn char *databaseGetConfigurationData(ulong *fgNumber, short *attempt)
    \brief Obtains configuration data for finished good number and attempt
    \param fgNumber
    \param attempt Attempt within the FinishedGoodNumber
    \return configuration data
*/
char *databaseGetConfigurationData(ulong *fgNumber, short *attempt);

/*! \fn int databaseGetTypeForAttempt(ulong FinishedGoodNumber, short attempt, short *type)
    \brief Obtains the required access level for the test attempt
    \param FinishedGoodNumber
    \param attempt Attempt within the FinishedGoodNumber
    \param *type Pointer to value which will hold the access level 
    \return TRUE or FALSE
*/
short databaseGetTypeForAttempt(ulong FinishedGoodNumber, short attempt, short *type);

/*! \fn int databaseInsertTemporaryTestData(PBOARDSTATE info,ushort TESTCOUNT, test *testData,ushort DIAGCOUNT, test *diagnosticData, ushort start, short PLANNED)
    \brief Inserts test Data into the database, row by row. Useful prior to a reboot
    \param info Pointer to board structure
    \param TESTCOUNT Count of tests within testData
    \param testData Test structure, which contains the test data and status
    \param DIAGCOUNT Count of diagnostic messages within diagnosticData
    \param diagnosticData Test structure, which contains the diagnostic data and any associated status
    \return TRUE or FALSE
*/
short databaseInsertTemporaryTestData(PBOARDSTATE info,ushort TESTCOUNT, test *testData,ushort DIAGCOUNT, test *diagnosticData, ushort start, short PLANNED);


//Generic DB Functions

/*! \fn int databaseExecSQL(char* SqlQuery, char* ReturnBuffer, unsigned int RetBufLen)
    \brief This function carries out the SQL Statement in \a SqlQuery and stores the results in \a ReturnBuffer
    \param SqlQuery Buffer holding a properly formated SQL statement to be executed
    \param ReturnBuffer Buffer that will contain the results of the SQL statement. See Details for it's formatting
    \param RetBufLen INT containing the buffer's size so the function can check for buffer overflow
    \return TRUE or FALSE or ZERO_RESULTS or BUFFER_OVERFLOW

    Resulting Rows are placed into the ReturnBuffer with each column's value followed by a | and the entire row followed by a ~
    \n An example return is shown below. This return contains a single row with all the columns from the Tests Table. \n
    FLOPPYDISK|TEST|DRIVER|DRIVERP|460|1|0|production|~
*/
int databaseExecSQL(char* SqlQuery, Result_Set *userResultSet, unsigned int RetBufLen);

/*! \fn int databaseConnect(uchar* DBDsn, uchar* DBUser, uchar* DBPassword)
    \brief This function carries out the ODBC calls required to connect to a database
    \param DBDsn Pointer to a Null terminated buffer containing the DSN needed for connection
    \param DBUser Pointer to a Null terminated buffer containing the User ID needed for connection
    \param DBPassword Pointer to a Null terminated buffer containing the Password needed for connection
    \return TRUE or FALSE

    On successful completion, this function sets the ConnectionStatus variable to CONNECTED
*/
int databaseConnect(uchar* DBDsn, uchar* DBUser, uchar* DBPassword);

/*! \fn int databaseDisconnect()
    \brief This function carries out the ODBC calls required to disconnect from a database
    \return TRUE or FALSE

    On successful completion, this function sets the ConnectionStatus variable to NOT_CONNECTED
*/
int databaseDisconnect();

/*! \fn int databaseResultFail(long Result)
    \param Result the SQLRETURN value from a previously called function
    \brief This function Checks a SQLRETURN value to see if it reports a failure. If so, databaseErrors is called.
    \return TRUE or FALSE
    \warning A TRUE return from this function means THERE WAS AN ERROR
*/
int databaseResultFail(long Result);

/*! \fn int databaseErrors()
    \brief This function parses through the SQL errors reported after an ODBC function call and calls InformUser to display each one
    \return TRUE 
*/
int databaseErrors();

/*! \fn int databaseMakeTime(char* myTime)
    \brief This function creates a Time/Date String needed to insert values into the MACs table
    \param myTime Pointer to a buffer where the string can be stored
    \return TRUE or FALSE
*/
int databaseMakeTime(char* myTime);

/*! \var char dsn
    \brief variable for the ODBC DSN String 
*/
/*! \var char uid
    \brief variable for the ODBC User ID String 
*/
/*! \var char pwd
    \brief variable for the ODBC Password String 
*/
char dsn[SMALL_BUF], uid[SMALL_BUF], pwd[SMALL_BUF];



#endif 
