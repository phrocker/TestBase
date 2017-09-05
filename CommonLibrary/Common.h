///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       Common.h
 *
 *  @brief      This file holds declarations for the functions in Common.c
 *
 *              Copyright (C) 2006 @n@n
 *              This file holds declarations for functions in Common.c and 
 *              define statements for values used throughout the project
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

#ifndef COMMON_H
#define COMMON_H 1




#include "environ.h"

#include  "definitions.h"

#include "StringFunctions.h"

#include "Prompt.h"

#include "OperatingSystemFunctions.h"

#include "printHeader.h"

#include "IPCFunctions.h"

/*! \fn void setTestVersion(unsigned int major, unsigned int minor )
    \brief Allows individual tests to set their current test version
    \param version Version number
    \return void
*/
void setTestVersion(float version);


/*! \fn float getTestVersion()
    \brief Returns the version of the current running test
    \return Current test version
*/
float getTestVersion();

/*! \fn void haltTest(char *errorMessage,int exitStatus)
    \brief Halts all testing and execution
    \return void
*/
void haltTest(char *errorMessage,int exitStatus);

/*! \fn void noMemoryHalt()
    \brief Halts when no memory is available
    \return void
*/
void noMemoryHalt();

/*! \fn char *getConfigFileData(char *configFile,char *testName)
    \brief Returns contents of the configuration for the specified testName
    \param configFile path to onfiguration file
    \param testName name of test
    \return configuration data
*/
char *getConfigFileData(char *configFile,char *testName);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         short displayUsage(void *argtable[], char *testName, double testVersion)
 * 
 *  @arg        <b>void</b> *argtable[]
 *                  - argument table buffer

 *  @arg        <b>char</b> *testName
 *                  - name of the test being executed
 * 
 *  @arg        <b>double</b> testVersion
 *                  - Version of the test being executed
 *
 *  @brief      Prints out a list of arguments accepted by the test being executed
 *              
 *              Uses argtable2 for its structure
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
void displayUsage(void *argtable[], char *testName, double testVersion);
#endif //endif for _COMMON



