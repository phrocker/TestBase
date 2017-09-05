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
// all functions with ellipses ( ... ), take multiple arguments, all of which
// function similarly to printf
/*! \file
	\brief User prompt function prototypes
*/
#ifndef PROMPT_H
#define PROMPT_H       1

#include <stdarg.h>

#define NAME_LENGTH		100

#include "definitions.h"

/*! \def NAME_LENGTH
	\brief Preprocessor value for a buffer size
*/


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int createUserPrompt(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen, int extendedType,char **validInputList);
 * 
 *  @arg        <b>char</b> *QuestionBuffer
 *                  - buffer that contains the question
 *
 *  @arg        <b>char</b> *AnswerBuffer
 *                  - buffer that contains the question
 *
 *  @arg        <b>int</b> AnswerLen
 *                  - maximum length of AnswerBuffer
 *
 *  @arg        <b>int</b> extendedType
 *                  - type of question to be posed to the user
 *
 *  @return     result of whether or not question was successfully answered
 *
 *  @brief      createUserPrompt is the hub for all questions. It should not be called
 *              directly. Instead call, promptUser(...) for a text based question, 
 *              askYesNoQuestion(...) for a yes/no question, or askRetry/FailQuestion(...)
 *              for a question that asks the user to retry or fail on a sequence.
 *
 *              If TestDispatcher is running, then tests are executed in remote test mode.
 *              In this case, the question is sent through a question message queue and 
 *              awaits a response on that same queue. 
 *
 *              If TestDispatcher is not running, then the text goes directly to the console,
 *              awaiting the response from standard input.
 *
 */
///////////////////////////////////////////////////////////////////////////////
int createUserPrompt(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen, int extendedType,char **validInputList);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn        int promptUser(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen);
 * 
 *  @arg        <b>char</b> *QuestionBuffer
 *                  - buffer that contains the question
 *
 *  @arg        <b>char</b> *AnswerBuffer
 *                  - buffer that contains the question
 *
 *  @arg        <b>int</b> AnswerLen
 *                  - maximum length of AnswerBuffer
 *
 *
 *  @return     result of createUserPrompt
 *
 *  @brief      calls createUserPrompt to pose a question to the user. 
 *
 */
///////////////////////////////////////////////////////////////////////////////
int promptUser(char *QuestionBuffer, char *AnswerBuffer, int AnswerLen);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void testPrint(char *Buffer,...);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed as diagnostic data
 *
 *  @brief      Calls generalPrint to print test data to either the console
 *              or remote client. In the remote client. Test data will is text
 *              to be displayed on a test printout, if running in remote test
 *              mode, which occurs when a test is executed through Test Dispatcher.
 *              In console mode, the text is printed directly to the terminal.
 *
 *  @note       Must be followed by the result. This occurs by calling either:
 *               1: customMessage(EMPTYSTRING);
 *               2: passedMessage();
 *               3: failedMessage();
 *
 *  @note       If testPrint is not followed by one of the three messages
 *              above, then you must append a newline at the end of the string
 *              otherwise, you will not need a newline
 *
 */
///////////////////////////////////////////////////////////////////////////////
int testPrint(char *Buffer,...);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void diagnosticPrint(char *Buffer,...);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed as diagnostic data
 *
 *  @brief      Calls generalPrint to print diagnostic to either the console
 *              or remote client. In the remote client, diagnostic data will 
 *              display under "Detailed view". Diagnostic data should be used
 *              to provide testers with debug information to help diagnose
 *              problems with executing tests, or to provide debug info about
 *              a malfunctioning test.
 *
 *  @note       Must be followed by the result. This occurs by calling either:
 *               1: customMessage(EMPTYSTRING);
 *               2: passedMessage();
 *               3: failedMessage();
 *  
 *  @note       If diagnosticPrint is not followed by one of the three messages
 *              above, then you must append a newline at the end of the string
 *              otherwise, you will not need a newline
 *
 */
///////////////////////////////////////////////////////////////////////////////
int diagnosticPrint(char *Buffer,...);

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         void debugPrint(int output_debug_info, char *Buffer,...)
 *
 *  @arg        <b>int</b> output_debug_info
 *                  - Value to switch this output on or off.  Must pass TRUE 
 *                    for this function to output anything.
 *
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed as diagnostic data
 *
 *  @brief      Calls diagnosticLinePrint to print to either the console
 *              or remote client. In the remote client, diagnostic data will 
 *              display under "Detailed view". Diagnostic data should be used
 *              to provide testers with debug information to help diagnose
 *              problems with executing tests, or to provide debug info about
 *              a malfunctioning test.Prints an entire line
 *
 */
///////////////////////////////////////////////////////////////////////////////
void debugPrint(int output_debug_info, char *Buffer,...);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void consolePrint(char *Buffer,...);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - Buffer to be printed
 *
 *  @arg        <b>char</b> *Buffer
 *                  - Buffer to be printed
 *
 *  @brief      Uses Buffer and arguments to create a local buffer which is
 *              sent to printDirectlyToConsole, where it is sent to STDOUT
 *
 *              
 *
 */
///////////////////////////////////////////////////////////////////////////////
void printVAConsole(char *Buffer,va_list arguments);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void consolePrint(char *Buffer,...);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - Buffer to be printed

 *
 *
 *  @brief      calls printVAConsole to print the Buffer directly to the console. printVAConsole
 *              uses an argument list to print directly to the console
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
void consolePrint(char *Buffer,...);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short testBoundedColumnBring(char *bufferInsertTo,uint bufferSize, char *externalBuffer,va_list arguments,uint maxLength);
 * 
 *  @arg        <b>char</b> *BufferInsertTo
 *                  - final buffer location for data in externalBuffer
 *
 *  @arg        <b>char</b> bufferSize
 *                  - length of bufferInsertTo
 *
 *  @arg        <b>va_list</b> *externalBuffer
 *                  - list of arguments that will replace their respective identifiers
 *                    in Buffer
 *
 *  @arg        <b>uint</b> maxSize
 *                  - maximum printed size
 *
 *  @arg        <b>short</b> type
 *                  - type to be printed
 *
 *  @return     Returns teh test of whether or not the externalBuffer string fits  within maxLength
 *
 *  @brief      boundedPrint sets a maximum size for the Buffer to be displayed.
 *              boundedPrint simply cuts off Buffer then sends the result to generalPrint
 *
 *  @note       Please see general print for for further explanation of type
 *
 */
///////////////////////////////////////////////////////////////////////////////
short testBoundedColumnBring(char *bufferInsertTo,uint bufferSize, char *externalBuffer,va_list arguments,uint maxLength);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void printDirectlyToConsole(char *Buffer);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed to the screen
 *
 *  @brief      calls printf to print Buffer to the terminal
 *               
 *
 */
///////////////////////////////////////////////////////////////////////////////
void printDirectlyToConsole(char *Buffer);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void printToIPC(char *Buffer,short type);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed
 *
 *  @arg        <b>short</b> type
 *                  - type of output to be sent to the user
 *
 *  @brief      printToIPC send a message to the listening message queue that
 *              will receive the message, then send it to all clients.
 *        
 *
 */
///////////////////////////////////////////////////////////////////////////////
void printToIPC(char *Buffer,short type);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void boundedPrint(char *Buffer,uint bufferLength, va_list arguments,uint maxSize, short type);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed
 *
 *  @arg        <b>uint</b> bufferLength
 *                  - length of Buffer
 *
 *  @arg        <b>va_list</b> arguments
 *                  - list of arguments that will replace their respective identifiers
 *                    in Buffer
 *
 *  @arg        <b>uint</b> maxSize
 *                  - maximum printed size
 *
 *  @arg        <b>short</b> type
 *                  - type to be printed
 *
 *  @brief      boundedPrint sets a maximum size for the Buffer to be displayed.
 *              boundedPrint simply cuts off Buffer then sends the result to generalPrint
 *
 *  @note       Please see general print for for further explanation of type
 *
 */
///////////////////////////////////////////////////////////////////////////////
void boundedPrint(char *Buffer,uint bufferLength, va_list arguments,uint maxSize, short type);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void generalPrint(char *Buffer,short type);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed
 *
 *  @arg        <b>short</b> type
 *                  - type of output to be sent to the user
 *
 *  @brief      General print functions as a general printing mechanism. If
 *              the text is executing through TestDispatcher, then the sysm
 *              is in remote test mode, forcing all messages to be forwarded
 *              to any listening clients. Otherwise, printDirectlyToConsole is
 *              called to print the text directly to the terminal.
 *
 *  @note       type includes the following values:
 *               QUESTION - Question to be sent solely to the user
 *               YESNO    - Yes/No question to be posed to the user
 *               RETRY/FAIL - Retry/fail qeustion to be posed to the user              
 *
 */
///////////////////////////////////////////////////////////////////////////////
void generalPrint(char *Buffer,short type);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void centerPrint(unsigned short lineLength, char *Buffer,...);
 * 
 *  @arg        <b>unsigned short</b> lineLength
 *                  - size of line, used to help center data in the buffer
 *
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed
 *
 *  @brief      Calls generalPrint to print Buffer to the screen; however, 
 *              centerPrint uses spaces to center the Buffer within the 
 *              length specified by lineLength.
 *
 */
///////////////////////////////////////////////////////////////////////////////
void centerPrint(unsigned short lineLength, char *Buffer,...);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void diagnosticLinePrint(char *Buffer,...);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed and displayed as diagnostic data
 *
 *  @brief      Calls generalPrint to print diagnostic to either the console
 *              or remote client. In the remote client, diagnostic data will 
 *              display under "Detailed view". Diagnostic data should be used
 *              to provide testers with debug information to help diagnose
 *              problems with executing tests, or to provide debug info about
 *              a malfunctioning test.Prints an entire line
 *
 */
///////////////////////////////////////////////////////////////////////////////
void diagnosticLinePrint(char *Buffer,...);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void linePrint(char *Buffer,...);
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed to the screen
 *
 *  @brief      Calls generalPrint to print an 80 character string to the screen
 *              or the remote console
 *               
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
void linePrint(char *Buffer,...);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void customMessage();
 * 
 *  @arg        <b>char</b> *Buffer
 *                  - buffer to be printed alongside the test result
 *
 *  @brief      Prints Buffer alongside a test. CustomMessage can be referred
 *              to as the 'result' column of an executing test. Calls boundedPrint
 *              to actually print the result
 *               
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
void customMessage(char *Buffer,...);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void passedMessage();
 * 
 *  @brief      Calls customMessage to print passed as the result of an executing
 *               test
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
void passedMessage();


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void failedMessage();
 * 
 *  @brief      Calls customMessage to print FAILED as the result of an executing
 *               test
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
void failedMessage();


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short askRetryFailQuestion(char *Question,...);
 * 
 *  @return     Answer from user. Is either RETRY or FAIL
 *
 *  @arg        <b>char</b> *Question
 *                  - Question to prompt the user
 *
 *  @brief      Asks the user a retry or fail question. Calls createUserPrompt(...)
 *              to prompt the user. If this function is called from a test running
 *              under TestDispatcher, then a message queue will be enabled, 
 *              forcing the question to go to through TestDispatcher to 
 *              a remote client. Otherwise, the question will be sent to 
 *              the command line  (Retry/Fail) to Question
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
short askRetryFailQuestion(char *Question,...);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short askYesNoQuestion(char *Question,...);
 * 
 *  @return     Answer from user. Is either YES or NO
 *
 *  @arg        <b>char</b> *Question
 *                  - Question to prompt the user
 *
 *  @brief      Asks the user a yes or no question. Calls createUserPrompt(...)
 *              to prompt the user. If this function is called from a test running
 *              under TestDispatcher, then a message queue will be enabled, 
 *              forcing the question to go to through TestDispatcher to 
 *              a remote client. Otherwise, the question will be sent to 
 *              the command line
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
short askYesNoQuestion(char *Question,...);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int linuxGetChar( )
 *
 *  @return     Returns the character entered in std input 
 *
 *  @brief      'Gets' a character from standard input
 *
 *
 *  @note       Gets a character from standard input. This works by first using
 *              the get and set attribute functions on standard input to enable
 *              echoing and canonical names. A character is retrieved ( in blocking
 *              I/O), and then returned
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////

int linuxGetChar( );

void printDirectlyToConsoleNoFlush(char *Buffer);

#endif
