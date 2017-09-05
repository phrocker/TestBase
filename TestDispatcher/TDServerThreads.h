///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       TDServerThreads.h
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Prototypes for the functions related to the Test
 *              Dispatcher server thread             
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
#ifndef TESTDISPATCHERSERVERTHREADS_H
#define TESTDISPATCHERSERVERTHREADS_H 1

/*! \file
	\brief 

    **** Note: Nothing in this file needs to be called directly. It should not
    be included in your source code. ****

*/

#include "../CommonLibrary/Common.h"

/*! \file
    \brief Function prototypes for Test Dispatcher threading
*/

/*! \fn int initializeServerThread()
    \brief Initializes all server threads and connection threads
	\return void
*/
void initializeServerThread();

/*! \fn int destroyServerThreads()
    \brief Destroys all server threads and connection threads
	\return void
*/
void destroyServerThreads();

/*! \fn int initializeThreadDescriptors()
    \brief Initializes all file descriptors to 0
	\return void
*/
void initializeThreadDescriptors();


/*! \fn uint getThreadForFileDescriptor(ulong,uint fd, char *)
    \brief Locates thread number for provided file descriptor
	\param in_addr - input file descriptor
	\param mac - user's mac address
	\return Thread number
*/
uint getThreadForFileDescriptor(ulong in_addr,uint fd, char *);


/*! \fn short isThreadDescriptor(int,ulong)
    \brief Returns whether or not the provided file descriptor is part
	of the specified file descriptor
	\param threadNumber - input thread number
	\param in_addr - input file descriptor
	
	\return TRUE/FALSE value of search
*/
short isThreadDescriptor(int, ulong);

/*! \fn short pushThreadCommand(short,uint,uint,ulong,char *);
    \brief Attempts to push command onto connection's queue
	\param type - command type
	\param threadNumber - input thread number
	\param fd - file descriptor
	\param in_addr - input file descriptor
	\param command - input command
	\return TRUE/FALSE value of push command
*/
short pushThreadCommand(short,uint,uint,ulong,char *);

/*! \fn void *threadCommandHandler(void *)
    \brief Thread which handles commands for each connection
	\param tthreadNumber - pointer to thread number
	\return void pointer
*/
void *threadCommandHandler(void *);

/*! \fn void  removeThreadListener(uint threadNumber,ulong in_addr)
    \brief Removes listener for specified file descriptor on threadNumber
	\param threadNumber - thread number from which to remove the file descriptor
	\param in_addr - input file descriptor
	\return void pointer
*/
void  removeThreadListener(uint threadNumber,ulong in_addr);

/*! \fn void  removeThreadListener(uint threadNumber,ulong in_addr)
    \brief Attempts to locate mac on the mac address lookup table and returns the associated
	file descriptor, if found
	\param mac - mac address to locate
	\return file descriptor or error, depending on status of search
*/
int getFileDescriptorByMac(char *mac);

/*! \fn unsigned long getIpForThread(uint thread, uint fd)
    \brief Returns the IP address associated with the thread and file descriptor
	\param thread - connection's thread number
    \param fd - file descriptor
	\return IP address, or zero
*/
unsigned long getIpForThread(uint thread, uint fd);

/*! \fn void removeQuestionHold(uint thread, uint fd)
    \brief Removes any question hold/pause which may be associated with this connection
	\param thread - connection's thread number
    \param fd - file descriptor
	\return void
*/
void removeQuestionHold(uint thread, uint fd);

/*! \fn char *getMacForThread(uint thread, uint fd)
    \brief Returns the MAC address associated with this connection
	\param thread - connection's thread number
    \param fd - file descriptor
	\return void
*/
char *getMacForThread(uint thread, uint fd);

#endif

