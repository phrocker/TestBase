///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDSignalHandler.c
 *
 *  @brief      Test Execution suite -- Signal Handler
 *
 *              Copyright (C) 2006 @n@n
 *              Handles signals which may be received from the Test Dispatcher
  *             This includes exiting and release memory, and signals from the 
 *              keyboard such as Ctrl-C
 *
 *              **** Note: Nothing in this file needs to be called directly. It should not
 *              be included in your source code. ***
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
#include <signal.h>

#include "TDThreadHandler.h"
#include "TDSignalHandler.h"



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int handle_signals(int signal) 
 */
///////////////////////////////////////////////////////////////////////////////
int handle_signals(int signal)
{
    switch(signal)
    {
    case SIGTERM:
        consolePrint("Received a signal to force termination...\n");
    case SIGINT:
    case SIGQUIT:
        databaseDisconnect();
    default:
        TdShutDownSequence();
        break;
    };

    exit(0);
}


