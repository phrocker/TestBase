///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       BoardInfo.h
 *
 *  @brief      This file holds declarations for the functions in BoardInfo.c
 *
 *              Copyright (C) 2006 @n@n
 *              Function prototypes and structures for the board, including 
 *              the test information
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

#ifndef _BOARD_INFO
#define _BOARD_INFO 1

#include "definitions.h"
#define NAME_LENGTH             100
#define BOARD_NAME_LENGTH       30
#define PROCESSOR_SPEED_LENGTH  10
#define TESTER_NAME_LENGTH      5
#define TEST_RESULTS_LENGTH		100000
/* Board Info action codes */

#define ACTION_BOARD_NAME                   0x00000001
#define ACTION_FINISHED_GOOD_NUMBER         0x00000002
#define ACTION_FINISHED_GOOD_REV            0x00000003
#define ACTION_SERIAL_NUMBER                0x00000004
#define ACTION_BOOT_MAC                     0x00000005
#define ACTION_TESTER_NAME                  0x00000006


/*! \def NAME_LENGTH
    \brief Preprocessor value for a buffer size
*/
/*! \def BOARD_NAME_LENGTH
    \brief Preprocessor value for a buffer size
*/
/*! \def TESTER_NAME_LENGTH
    \brief Preprocessor value for a buffer size
*/
/*! \def TEST_RESULTS_LENGTH
    \brief Preprocessor value for a buffer size
*/




/*! \struct TESTBOARDINFO BoardInfo.h 
   \brief This struct holds the common board info
 
	The variables in this struct are the most commonly needed pieces of 
	info about a board. PTESTBOARDINFO is a pointer to this structure. 
	Additional board info is located in the TESTRESULTS struct.
*/


typedef struct {

    
    
	/*! \var char boardName[BOARD_NAME_LENGTH]
		\brief bName of the Board
	*/

	/*! \var char tester[TESTER_NAME_LENGTH]
		\brief Tester's initials
	*/

	/*! \var unsigned long finishedGoodNumber
		\brief Finished Good Number
	*/

	/*! \var unsigned char finishedGoodRev
		\brief Finished Good Rev, this is the BOMRev in the database
	*/

	/*! \var unsigned long serialNumber
		\brief Serial Number
	*/

	/*! \var unsigned long BootMacNumber
		\brief MAC Addresses used to boot the board
	*/

       

    char            boardName[BOARD_NAME_LENGTH];
    char            tester[TESTER_NAME_LENGTH];
    unsigned long   finishedGoodNumber;
    unsigned char   finishedGoodRev;
    unsigned long   serialNumber;
    unsigned long   bootMacNumber;
    
    

} TESTBOARDINFO, *PTESTBOARDINFO;

/*! \var typedef TESTBOARDINFO *PTESTBOARDINFO
    \brief A pointer TypeDef for the TESTBOARDINFO struct
*/

typedef struct
{
    /*! \var  short loaded
		\brief flag used to signify if the board data has been loaded
	*/

    /*! \var short accessLevel
		\brief Access level of the current test being run on this board
	*/

    /*! \var short currentSequence
		\brief current location within test sequence
	*/

    /*! \var int attempt
		\brief current attempt for this board
	*/

    /*! \var processor[PROCESSOR_SPEED_LENGTH]
		\brief Processor speed. i.e. '1.5 GHz'
	*/

    /*! \var unsigned short percentComplete
		\brief percentage of completed tests
	*/
    short           loaded;
    short           testType;
    short           accessLevel;
    short           currentSequence;
    int             attempt;
    short           passing;
    char            processor[PROCESSOR_SPEED_LENGTH];
    unsigned short  percentComplete;
    TESTBOARDINFO   boardInfo;

} BOARDSTATE,*PBOARDSTATE; 



/*! \struct TESTRESULTS BoardInfo.h 
   \brief additional board info including test results
 
	The variables in this struct are a continuation of
	the TESTBOARDINFO. This struct is used when inserting or updating
	test results in the database.
*/

typedef struct {
	
	/*! \var BIOSRev[BOARD_NAME_LENGTH]
		\brief buffer for storing the BIOS Rev
	*/

	

	/*! \var TestResults[TEST_RESULTS_LENGTH]
		\brief extremely large buffer for storing the test results that will be placed into the database
	*/

	/*! \var short TestSequence
		\brief variable for the test sequence value
	*/

    char            BIOSRev[BOARD_NAME_LENGTH];
	
    char            TestResults[TEST_RESULTS_LENGTH];
    short           TestSequence; 

} TESTRESULTS, *PTESTRESULTS;

/*! \var typedef TESTRESULTS *PTESTRESULTS
    \brief A pointer TypeDef for the TESTRESULTS struct
*/


/*! \fn short getBoardName(char *)
    \brief Gets the board name
    \param buffer pointer to board name buffer
	\return TRUE or FALSE
*/

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBoardName(char *)
 *
 *  @returns the status of the operations
 *
 *
 *  @arg    <b>char</b> *buffer
 *          - buffer in which to place the board name
 *
 *  @brief  Obtains the board name by calling getBoardInfo and accessing
 *          ACTION_BOARD_NAME
 * 
 *          
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short getBoardName(char *);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBootMacNumber(ulong *)
 *
 *  @returns the status of the operations
 *
 *
 *  @arg    <b>ulong</b> *buffer
 *          - buffer in which to place the boot mac  number
 *
 *  @brief  Obtains the board name by calling getBoardInfo and accessing
 *          ACTION_BOOT_MAC
 * 
 *  
 *  @warning The MAC is in base 10 and excludes the TBS prefix        
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short getBootMacNumber(ulong *);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBomRevision(char *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>ulong</b> *fg
 *          - buffer in which to place the boot finished good number
 *
 *  @brief  Obtains the board name by calling getBoardInfo and accessing
 *          ACTION_FINISHED_GOOD_NUMBER
 * 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short getFinishedGoodNumber(ulong *);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getBomRevision(char *)
 *
 *  @returns the status of the operations
 *
 *
 *  @arg    <b>char</b> *bomRevision
 *          - buffer in which to place the BOM revision
 *
 *  @brief  Obtains the board name by calling getBoardInfo and accessing
 *          ACTION_FINISHED_GOOD_REV
 * 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short getBomRevision(char *);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getSerialNumber(ulong *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>ulong</b> *serial
 *          - buffer in which to place the boot finished good number
 *
 *  @brief  Obtains the board name by calling getBoardInfo and accessing
 *          ACTION_SERIAL_NUMBER
 * 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short getSerialNumber(ulong *);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short getTestersInitials(char *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>char</b> *init
 *          - buffer in which to place the tester's initials
 *
 *  @brief  Obtains the board name by calling getBoardInfo and accessing
 *          ACTION_TESTER_NAME
 * 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short getTestersInitials(char *);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBoardName(char *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>char</b> *buffer
 *          - pointer to the board name
 *
 *  @brief  Creates a temporary TESTBOARDINFO structure so the board name
 *          can be copied into it, then the board information is updated
 *      
 *          updateBoardInfo is called with ACTION_BOARD_NAME as the directive
 *          argument, so the board info file and structure can be updated 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short setBoardName(char *);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBootMacNumber(ulong *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>ulong</b> *buffer
 *          - pointer to the boot mac number
 *
 *  @brief  Creates a temporary TESTBOARDINFO structure so the boot mac
 *          can be copied into it, then the board information is updated
 *      
 *          updateBoardInfo is called with ACTION_BOOT_MAC as the directive
 *          argument, so the board info file and structure can be updated 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short setBootMacNumber(ulong *);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setFinishedGoodNumber(ulong *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>ulong</b> *buffer
 *          - pointer to the finished good number
 *
 *  @brief  Creates a temporary TESTBOARDINFO structure so the FG number
 *          can be copied into it, then the board information is updated
 *      
 *          updateBoardInfo is called with ACTION_FINISHED_GOOD_NUMBER as the directive
 *          argument, so the board info file and structure can be updated 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short setFinishedGoodNumber(ulong *);

///////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBomRevision(char *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>char</b> *buffer
 *          - pointer to the bom revision
 *
 *  @brief  Creates a temporary TESTBOARDINFO structure so the BOM Rev
 *          can be copied into it, then the board information is updated
 *      
 *          updateBoardInfo is called with ACTION_FINISHED_GOOD_REV as the directive
 *          argument, so the board info file and structure can be updated 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short setBomRevision(char *);

///////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setBomRevision(ulong *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>ulong</b> *buffer
 *          - pointer to the serial number
 *
 *  @brief  Creates a temporary TESTBOARDINFO structure so the serial
 *          can be copied into it, then the board information is updated
 *      
 *          updateBoardInfo is called with ACTION_SERIAL_NUMBER as the directive
 *          argument, so the board info file and structure can be updated 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short setSerialNumber(ulong *);



///////////////////////////////////////////////////////////////////////
/** 
 *  @fn     short setTestersInitials(char *)
 *
 *  @returns the status of the operations
 *
 *  @arg    <b>char</b> *buffer
 *          - pointer to the tester's initials
 *
 *  @brief  Creates a temporary TESTBOARDINFO structure so the initals
 *          can be copied into it, then the board information is updated
 *      
 *          updateBoardInfo is called with ACTION_TESTER_NAME as the directive
 *          argument, so the board info file and structure can be updated 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
short setTestersInitials(char *);




/*! \fn void getProcessorSpeed(char *processorBuffer, int sizeOfProcessorBuffer)
    \brief Obtains the current processor speed
    \param buffer in which to place processor speed
    \param maximum size of processorBuffer
	\return void
*/

void getProcessorSpeed(char *processorBuffer, int sizeOfProcessorBuffer);

/*! \fn int getBoardInfo(int actionCode, void *data)
    \brief Gets the board info in a manner defined by \a actionCode
    \param actionCode to determine the action
    \param data Data
	\return TRUE or FALSE
*/
short getBoardInfo(int actionCode, void *data);

short getAuto(int actionCode, void *data);

/*! \fn short initBoardInfoFile(void)
    \brief  This function will initialize board info file to all zeros   
	\return TRUE or FALSE
*/
short initBoardInfoFile(void);

/*! \fn int getBoardInfoFromFile(int actionCode, void *data)
    \brief This function will read in the board.info file and then update the board information structure according to the desired action request
    \param actionCode Get the board information for the desired action code
    \param data Data pointer
	\return TRUE or FALSE
*/
short getBoardInfoFromFile(int actionCode, void *data);

/*! \fn short askForBoardInfo(int actionCode)
    \brief This function will prompt the user for specific board information.  
	If the information is correct it will update the global structure and update the board information file.

    \param actionCode Get the board information for the desired action code
	\return TRUE or FALSE
*/
short askForBoardInfo(int actionCode, void *data);

/*! \fn int updateBoardInfo(int actionCode, PTESTBOARDINFO pInfo, int UpdateFileFlag)
    \brief This function will update specific data in the global board information structure
    \param actionCode Get the board information for the desired action code
	\param pInfo A pointer to the temp TESTBOARDINFO structure
	\param UpdateFileFlag TRUE or FLASE on updating the board information file
	\return TRUE or FALSE
*/
int updateBoardInfo(int actionCode, PTESTBOARDINFO pInfo, int UpdateFileFlag);


/*! \fn short updateBoardName(char *BoardNameBuffer)
    \brief This function will update board name in the global board information structure  
    \param BoardNameBuffer A buffer holding the board name
	\return TRUE or FALSE
*/
short updateBoardName(char *BoardNameBuffer);

/*! \fn void updateBoardInfoFile(void)
    \brief This function will update data in the global board information file 
*/
void updateBoardInfoFile(void);

	




/*! \fn short isFinishedGoodNumber(ulong &FinishedGoodNumber)
    \brief Attempt to validate the finishedGoodNumber through the database
    \param FinishedGoodNumber
    \param SerialNumber
	\return TRUE/FALSE

*/
short isFinishedGoodNumber(unsigned long *FinishedGoodNumber);



/*! \fn short copyBoardInfoToData(int actionCode,PTESTBOARDINFO info,void *data)
    \brief Copys the board information into the data pointer
    \param actionCode
    \param info - board information
    \param data - data pointer
	\return status of our copy

*/
short copyBoardInfoToData(int actionCode,PTESTBOARDINFO info,void *data);




/***********************************************************************************
                                Update Functions:
    The following functions update the global board information structure and
    the board file
************************************************************************************/

/*! \fn short updateBoardName(char*BoardNameBuffer)
    \brief Updates the global board information structure with this board name
    \param BoardNameBuffer - board name
	\return status of the update

*/
short updateBoardName(char *BoardNameBuffer);

/*! \fn short updateFGNumber(ulong *finishedGoodNumber)
    \brief Updates the global board information structure with this finished good number
    \param finishedGoodNumber
	\return status of the update

*/
short updateFGNumber(ulong *finishedGoodNumber);

/*! \fn short updateBOMRev(char finishedGoodRev)
    \brief Updates the global board information structure with this BOM Revision
    \param finishedGoodRev - BOM Revision
	\return status of the update

*/
short updateBOMRev(char finishedGoodRev);


/*! \fn short updateSerialNumber(ulong *serialNumber)
    \brief Updates the global board information structure with this serial number
    \param serialNumber
	\return status of the update

*/
short updateSerialNumber(ulong *serialNumber);


/*! \fn short updateBootMacNumber(ulong *bootMacNumber)
    \brief Updates the global board information structure with this boot MAC number
    \param bootMacNumber
	\return status of the update

*/
short updateBootMacNumber(ulong *bootMacNumber);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short updateTesterInitials(char *tester)
 *  
 *  @arg        <b>char</b> *tester
 *               tester's initials
 *
 *  @return     status of the update
 *
 *  @brief      Updates the tester's initials in the global test array by
 *              copying the value of tester into BoardInfo.tester
 *
 */
////////////////////////////////////////////////////////////////////////////
short updateTesterInitials(char *tester);

/***********************************************************************************
                                Prompt Functions:
    The following functions prompt the user for the specific information
************************************************************************************/

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         ulong getFgNumberFromUser(void)
 *
 *  @return     The finished good number
 *
 *  @brief      Obtains the finished Good number from the user by prompting the user,
 *              using the common library
 *
 */
////////////////////////////////////////////////////////////////////////////
ulong getFgNumberFromUser(void);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         ulong getSerialNumberFromUser(void)
 *
 *  @return     The serial number
 *
 *  @brief      Obtains the serial number from the user by prompting the user,
 *              using the common library
 *
 */
////////////////////////////////////////////////////////////////////////////
ulong getSerialNumberFromUser(void);


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         ulong getBootMacNumberFromUser(void)
 *
 *  @return     The boot mac number
 *
 *  @brief      Obtains the boot mac from the user by prompting the user,
 *              using the common library
 *
 */
////////////////////////////////////////////////////////////////////////////
ulong getBootMacNumberFromUser(void);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void getTestersInitialsFromUser(char *buffer)
 *
 *  @arg        <b>char</b> *buffer
 *               buffer in which to place the tester's initials
 *
 *
 *  @brief      Obtains the tester's initials from the user by prompting the user,
 *              using the common library
 *
 */
////////////////////////////////////////////////////////////////////////////
void getTestersInitialsFromUser(char *buffer);


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void getBomRevisionFromUser(char *buffer)
 *
 *  @arg        <b>char</b> *buffer
 *               buffer in which to place the BOM Revision
 *
 *
 *  @brief      Obtains the BOM Revision from the user by prompting the user,
 *              using the common library
 *
 */
////////////////////////////////////////////////////////////////////////////
void getBomRevisionFromUser(char *buffer);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void getBoardNameFromUser(char *buffer)
 *
 *  @arg        <b>char</b> *buffer
 *               buffer 
 *
 *
 *  @brief      Obtains the board name from the user by prompting the user,
 *              using the common library
 *
 */
////////////////////////////////////////////////////////////////////////////
void getBoardNameFromUser(char *buffer);

/***********************************************************************************
                                Verification Functions:
    These functions are used to verify the sanctity of an input value
************************************************************************************/

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short isValidSerialNumber(char *serial)
 *
 *  @arg        <b>char</b> *fg
 *               Board's finished good number
 *
 *  @return     PASS/FAIL depending on whether or not the serial number
 *              is valid
 *
 *  @brief      Validates the serial number; makes sure that it is eight 
 *              characters
 *
 */
////////////////////////////////////////////////////////////////////////////
short isValidSerialNumber(char *serial);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short isValidFinishedGoodNumber(char *fg)
 *
 *  @arg        <b>char</b> *fg
 *               Board's finished good number
 *
 *  @return     PASS/FAIL depending on whether or not the finished good number
 *              is valid
 *
 *  @brief      Validates the Finished Good number; makes sure it's nine
 *              characterss
 *
 */
////////////////////////////////////////////////////////////////////////////
short isValidFinishedGoodNumber(char *fg);



////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short isValidTesterInitials(char *initials)
 *
 *  @arg        <b>char</b> *initials
 *               tester's initials
 *
 *  @return     PASS/FAIL depending on whether or not the initials are valid
 *
 *  @brief      Validates the tester's initials by testing the length to make
 *              sure that the length is less than four and greater than zero
 *              characters
 *
 */
////////////////////////////////////////////////////////////////////////////
short isValidTesterInitials(char *initials);


////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short getBoardAttempt(ulong FinishedGoodNumber, ulong SerialNumber, short *type, short *sequence, short *testType,ulong *secondsToExecute);
 *
 *  @arg        <b>ulong</b> FinishedGoodNumber
 *               board's finished good number
 *
 *  @arg        <b>short</b> *type
 *               test attempt type. This will be used to determine the testing campaign
 *
 *  @arg        <b>short</b> *sequence
 *               sequence number in the list of attempts
 *
 *  @arg        <b>short</b> *testType
 *               type of test being executed
 *
 *  @arg        <b>ulong</b> *secondsToExecute
 *               number of seconds to execute the test
 *.
 *  @return     Current test attempt
 *
 *  @brief      Obtains the current type and sequence for the user's desired test attempt. 
 *
 *              A list of available attempts is sent to all listening users, at which time
 *              a user may choose which attempt to execute
 *
 */
////////////////////////////////////////////////////////////////////////////
short getBoardAttempt(ulong FinishedGoodNumber, ulong SerialNumber, short *type, short *sequence, short *testType,ulong *secondsToExecute);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void saveTestConfig(char *locationToSave, ulong *fgNumber, short *attempt)
 *
 *  @arg        <b>char</b> *locationToSave
 *               file location to save test configuration file
 *
 *  @arg        <b>ulong</b> *fgnumber
 *               the board's finished good number
 *
 *  @arg        <b>short</b> *attempt
 *               test attempt
 *.
 *  @brief      Saves the test configuration file. Included may be configuration
 *              data that the engineers may wish to use 
 *
 */
////////////////////////////////////////////////////////////////////////////
void saveTestConfig(char *locationToSave, ulong *fgNumber, short *attempt);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void initializeBoardAndState(PBOARDSTATE boardStatusAndState)
 *
 *  @arg        <b>PBOARDSTATE</b> boardStatusAndState
 *               board status variable 
 *.
 *  @brief      Initializes and zeros all values in the boardStatusAndState 
 *              variable
 *
 *              PBOARDSTATE is a pointer, and consequently, boardStatusandState
 *              must be allocated or sent by reference so it can be zerod
 */
////////////////////////////////////////////////////////////////////////////
void initializeBoardAndState(PBOARDSTATE boardStatusAndState);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short getRepairTestLevel(TESTPARAMETERS **cachedTests,int cacheSize)
 *
 *  @return     Returns the TRUE if the user responded to the question
 *.
 *  @brief      This function verifies that the CPUID command is supported.
 *
 *              This function checks to see if the cpu under test acually 
 *              Supports the CPUID function by attempting to set the ID flag
 *              in the EFLAGS register.  If you can set the flag, then 
 *              the CPUID command is supported  and returns a 0(false) or 1(true).
 *
 *  @warning    <b><i>This command must be run BEFORE any other CPUID request!
 *              Since the CPUID command is inline-assembly, there is no error
 *              checking with it. If CPUID is not supported and you issue a CPUID 
 *              request,  --!! YOUR PROGRAM WILL CRASH !!--</i></b>
 */
////////////////////////////////////////////////////////////////////////////
//short getRepairTestLevel(TESTPARAMETERS**,int);


#define MAXSERIAL   8
#define MAXFGNUM    9

/*! \def MAXSERIAL
    \brief Maximum serial number length
*/
/*! \def MAXFGNUM
    \brief Maximum length for a finished good number
*/
#endif
