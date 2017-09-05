#ifndef DEFINITIONS_H
#define DEFINITIONS_H 1

/*! \file
	\brief Global definitions
*/

#define Byte unsigned char
#define Word unsigned short
#define DWord unsigned long

#define UByte char
#define UWord short
#define UDoubleWord long

#define BUF_LEN					300
#define MID_BUF					100
#define SMALL_BUF				50

#define LINE_LENGTH 80
#define LINE_BUF LINE_LENGTH + 1
#define STATUS_COLUMN 39


#define TBSCOPYRIGHT "Copyright (c) 2006 Diversified Technology"


typedef struct 
{
    unsigned short marker;
    // deprecated in second revision
    unsigned int lowAddr;
    unsigned int highAddr;
    void *ptr;

} memPointer;



/*! \def BUF_LEN
    \brief Preprocessor value for a buffer size
*/
/*! \def MID_BUF
    \brief Preprocessor value for a buffer size
*/
/*! \def SMALL_BUF
    \brief Preprocessor value for a buffer size
*/

/*! \def LINE_LENGTH
    \brief Preprocessor value for 80 character buffer
*/
/*! \def LINE_BUF
    \brief Preprocessor value for 81 character buffer
*/
/*! \def STATUS_COLUMN
    \brief Preprocessor value for a buffer size
*/

#define FAILEDSTRING "FAILED"
#define PASSEDSTRING "Passed"
#define EMPTYSTRING " "

/*! \def FAILEDSTRING
    \brief Preprocessor value for the failed test string
*/

/*! \def PASSEDSTRING
    \brief Preprocessor value for the passed test string
*/

/*! \def EMPTYSTRING
    \brief Preprocessor value for an empty string
*/


#define TEMP 2

#ifndef TRUE
#define TRUE 1
#define PASS TRUE
#define YES TRUE
#define RETRY YES
#endif

#ifndef FALSE
#define FALSE 0
#define NO FALSE
#define FAIL NO
#endif

#define ERROR -1


/*! \def TEMP
    \brief Preprocessor value for 2
*/

/*! \def TRUE
    \brief Preprocessor value for 1
*/
/*! \def FALSE
    \brief Preprocessor value for 0
*/

/*! \def ERROR
    \brief Preprocessor value for -1
*/

#ifndef ulong
#define ulong unsigned long
#define uint unsigned int
#define uchar unsigned char
#define ushort unsigned short
#endif

typedef struct 
{
	ulong	Eax;
	ulong	Ebx;
	ulong	Ecx;
	ulong	Edx;
} BigRegsType;

/*! \def ulong
    \brief Preprocessor value for \a unsigned \a long
*/
/*! \def uchar
    \brief Preprocessor value for \a unsigned \a char
*/
/*! \def ushort
    \brief Preprocessor value for \a unsigned \a short
*/

#ifndef LINE_BUFFER
#define LINE_BUFFER             80	
#endif

/*! \def LINE_BUFFER
    \brief Preprocessor value for a buffer size
*/

#define MAX_TEST_LENGTH 39

/*! \def MAX_TEST_LENGTH
    \brief Preprocessor value for maximum test row length
*/


#define CONSOLE_MODE 0
#define REMOTE_CONSOLE_MODE 1

/*! \def CONSOLE_MODE
    \brief Preprocessor value to signify console only mode
*/
/*! \def REMOTE_CONSOLE_MODE
    \brief Preprocessor value to signify remote console mode
*/

#ifndef CURRENT_TEST_MODE
	#define CURRENT_TEST_MODE captureCurrentTestMode()
#endif

/*! \def CURRENT_TEST_MODE
    \brief Definition of macro which executes captureCurrentTestMode()
    
*/

typedef struct sockaddr_in socketAddress;
typedef struct in_addr inaddress;



/*! \struct test /CommonLib/definitions.h
   \brief Data structure used to house test data
 
*/
typedef struct
{

    char status[(LINE_BUF-MAX_TEST_LENGTH)+1];
    char testData[LINE_BUF];

} test;


// repair can choose which tests they wish to execute
#define REPAIR_ACCESS_LEVEL 2
#define PRODUCTION_ACCESS_LEVEL 3

/*! \def PRODUCTION_ACCESS_LEVEL
    \brief Access level at which production runs
    
*/

#define TESTCONFIG "/var/log/testConfig"
#define BOARDINFOFILE  "/var/log/board.info"

#endif
