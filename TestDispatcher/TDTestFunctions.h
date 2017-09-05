///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDTestFunctions.h
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
#ifndef TDTESTFUNCTIONS_H
#define TDTESTFUNCTIONS_H 1

#include "../CommonLibrary/definitions.h"
#include "../CommonLibrary/BoardInfo.h"
#include "../CommonLibrary/Database/DBFunc.h"

#define FUNCTIONAL 1
#define BURNIN 0
#define TEST 1
#define DIAG 2

test *testExecutionRows;

unsigned int EXECUTIONROWCOUNT;

/*! \fn void updateUserQueues(short dialog,void *fp(void*,void*,void*,void*) )
    \brief Updates user's queue with local command
	\param dialog  value to determine if we are dealing with diagnostic data
	\param fp  function pointer/callback
	\return void
*/

void updateUserQueues(short dialog,void *fp(void*,void*,void*,void*) );

/*! \fn void updateStatus(char *sstring, short type)
    \brief Updates the previously set test data's status
	\param sstring data with which to update queue
    \param type The type (diagnostic or test ) of test data
	\return void
*/
void updateStatus(char *sstring, short type);

/*! \fn void updateDiagnosticDataStatus(char *sstring)
    \brief Updates previous diagnostic status
	\param sstring - data with which to update queue
	\return void
*/
void updateDiagnosticDataStatus(char *sstring);

/*! \fn void updateTestDataStatus(char *sstring)
    \brief Updates previous test status
	\param sstring - data with which to update queue
	\return void
*/
void updateTestDataStatus(char *sstring);

/*! \fn void updateTestDataStatus(char *sstring)
    \brief Updates test previously added test data on queue
	\param sstring - data with which to update queue
	\return void
*/
void updateTestDataQueue(char *sstring);


/*! \fn void updateDiagnosticTestDataQueue(char *sstring)
    \brief Updates diagnostic data previously added diagnostic data on queue
	\param sstring - data with which to update queue
	\return void
*/
void updateDiagnosticTestDataQueue(char *sstring);


/*! \fn void singleNonUpdate(short *dialog, int *iIterator, int *jIterator, char *buffer)
    \brief Updates user queue with single update data. Does not cause an update( deprecated )
	\param short *dialog - diagnostic flag
	\param iIterator - thread iterator
	\param jIterator - connection iterator
	\param buffer - local buffer
	\return void
*/
void singleNonUpdate(short *dialog, int *iIterator, int *jIterator, char *buffer);


/*! \fn void singleNonUpdate(short *dialog, int *iIterator, int *jIterator, char *buffer)
    \brief Updates user queue with single update data. Causes an update ( deprecated )
	\param short *dialog - diagnostic flag
	\param iIterator - thread iterator
	\param jIterator - connection iterator
	\param buffer - local buffer
	\return void
*/
 
void singleUpdate(short *dialog, int *iIterator, int *jIterator, char *buffer);

/*! \fn void queueTestData(char *sstring)
    \brief Updates test data
	\param string - data with which to update test data
	\return void
*/
void queueTestData(char *string);

/*! \fn void queueDiagnosticData(char *sstring)
    \brief Updates diagnostic data
	\param string - data with which to update test data
	\return void
*/
void queueDiagnosticData(char *string);


/*! \fn void answerQuestion(char *qstion, const char *type)
    \brief Poses question to user
	\param qstion - question to pose to user
	\param type - type of question to pose to user
	\return void
*/
void answerQuestion(char *qstion, const char *type);

/*! \fn void notifyUserToCloseMessageBox()
    \brief Notifies user to close his/her message box
	\return void
*/
void notifyUserToCloseMessageBox();

/*! \fn void userAnswerQuestion(int thread)
    \brief Function which creates question and type
	\return void
*/
void userAnswerQuestion(int thread);

/*! \fn void getPreviousTestData()
    \brief Returns previous test data in test Data buffer
	\return void
*/
void getPreviousTestData();

/*! \fn short saveTestPrintout(PBOARDSTATE boardInfo,PTESTPARAMETERS testData)
    \brief Asks the user if he/she wishes to save test data
	\return PASS/FAIL of this operation
*/
short saveTestPrintout(PBOARDSTATE boardInfo,PTESTPARAMETERS test);


/*! \fn void setListenerRequiredVersion(float version)
    \brief Sets the version which is required to acces this Test Dispatcher
	\return void
*/
void setListenerRequiredVersion(float version);

/*! \fn void logFinalTestData(char *executionString, float version)
    \brief Logs test execution data after a test is run
    \param executionString Full path and parameters used for test
    \param status Return status of the test
    \param version Test version
	\return void
*/
void logFinalTestData(char *executionString,int status, float version);

/*! \fn void beginTesting(PBOARDSTATE,char*,ulong*,char*)
    \brief Loads all test data and begins testing
	\return void
*/
void beginTesting(PBOARDSTATE,char*,ulong*,char*);

/*! \fn void burninTest(PBOARDSTATE boardInfo,TESTPARAMETERS *Test, int numTests, ulong secondsToExecute)
    \brief Executes a Burn-In test
    \param boardInfo - Board information
    \param Test - Test data
    \param numTests - Maximum number of tests we will be executing
    \param secondsToExecute - Number of seconds to execute the burn-In test
	\return void
*/
void burninTest(PBOARDSTATE boardInfo,TESTPARAMETERS *Test, int numTests, ulong secondsToExecute);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     ushort functionalTest(PBOARDSTATE boardInfo,TESTPARAMETERS *Test, int numTests);
 *
 *  @arg    <b>boardInfo</b> @n 
 *          - Structure which contains the current state of the board, including
 *          data about its setup
 *
 *  @arg    <b>Test</b> @n 
 *          - Pointers to structure which contains data about the current tests
 *
 *  @arg    <b>numTests</b> @n 
 *          - Number of tests that will be executed
 *
 *  @brief  Executes a functional test
 * 
 *          Retrieves test data from the database. Caches the tests into an array,
 *          afterwhich the TestDispatcher executes the tests
 *
 *  @note   Repair is sent a dialog request so that they can choose which tests
 *          to execute
 *  
 *          
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
ushort functionalTest(PBOARDSTATE boardInfo,TESTPARAMETERS *Test, int numTests);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void setBurnoutTest()
 *
 *
 *  @brief  Sets the variable to enable burnin in testing
 *         
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void setBurnoutTest();



#endif
