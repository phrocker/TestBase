///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TimeFunctions.c
 *
 *  @brief      Functions which may provide simple time functionality such
 *              as getting the current number of seconds or microseconds
 *
 *              Copyright (C) 2006 @n@n
 *              This will not be included in Common.h, since this library will
 *              not always be needed. This library will be expanded as needed  
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

#include "TimeFunctions.h"
#include <time.h>
#include <sys/time.h>

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         unsigned long getMicroSeconds() 
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned long getMicroSeconds()
{
    struct timeval timerValue;
    struct timezone timeZone;
    struct tm *tm;
    
    gettimeofday(&timerValue,&timeZone);
    // get the current number of seconds past
    // the minute. This will also provide uSeconds
    
    tm = localtime (&timerValue.tv_sec);
    
    return timerValue.tv_usec;
}

