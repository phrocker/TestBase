///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       StringFunctions.h
 *
 *  @brief      Header file for all string functions used in the Common Library
 *
 *              Copyright (C) 2006 @n@n
 *              
 *  
 *  @author     Marc Parisi
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

/*! \file
	\brief Prototypes for all string functions
*/

#ifndef STRING_FUNCTIONS_H

#define STRING_FUNCTIONS_H 1

#include <stdlib.h>

/*! \fn char void removeTrailingWhiteSpace(char *str)
    \brief Remove any trailing white space on str
    \param str
    \Return void
*/
void removeTrailingWhiteSpace(char *str);

/*! \fn void makeLowerCase(char* str)
    \brief This function converts all the upper case characters in a string to lower case characters
	\param str Pointer to a Buffer whose characters will be altered'
*/
void makeLowerCase(char *str);

/*! \fn void makeLowerCase(char* str)
    \brief This function converts all the lower case characters in a string to upper case characters
	\param str Pointer to a Buffer whose characters will be altered'
*/
void makeUpperCase(char *str);

/*! \fn void prefixExpand(char* str)
    \brief This function converts all the lower case characters in a string to upper case characters
    \param str Pointer to a Buffer whose characters will be altered'
*/
void prefixExpand(char *,int);

/*! \fn int find(char* T,char* fT)
    \brief String search function
    \param T Pointer to string we will search
    \param fT Pointer to search String
    \Return Returns found location or -1
*/
int find(char* T,char* fT);

/*! \fn char *getDayOfWeek(unsigned short day)
    \brief Returns string representation of the current day of the week
    \Return Returns day of week
*/
char *getDayOfWeek(unsigned short day);

    /*! \fn char *getMonth(unsigned short month)
    \brief Returns string representation of the current month
    \Return Returns month
*/

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn        char *getMonth(unsigned short month);
 * 
 *  @arg        <b>unsigned short</b> month
 *                  - month number
 *
 *  @return     string representation of the current month
 *
 *  @brief      Returns a string representation of the current month. Begins at 0
 *              
 *
 */
///////////////////////////////////////////////////////////////////////////////
char *getMonth(unsigned short month);



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void fillRandomArray(char *array,unsigned int size)
 *
 *  @arg    <b>char</b> *array
 *          - Array to be populated with random values
 *
 *  @arg    <b>unsigned int</b> size
 *          - size of of array
 *
 *  @brief  Populates array with random characters from /dev/urandom 
 *
 *  @note   Call this function if you need to populate an array, of which
 *          you know the size, with random chracters.  
 *  
 *  @Warning  These characters are generated with a pseudorandom number
 *            generator, and consequently, should not be considered completely
 *            random, though the driver does its best. /dev/random is considered
 *            quite random, however, as its entropy pool ( a topic in random
 *            number generation that refers to a channel of data with a high
 *            degree of entropy ) reaches its capacity, it will halt. /dev/urandom,
 *            will return as many bytes as desired, but as its entropy pool
 *            is emptied, then the result of the device driver will become less
 *            random and will be only "cryptographically" strong. Consequently,
 *            fillRandomArray should be sufficient for most purposes, barring
 *            security applications.
 */ 
/////////////////////////////////////////////////////////////////////////
void fillRandomArray(char *array,unsigned int size);

#endif
