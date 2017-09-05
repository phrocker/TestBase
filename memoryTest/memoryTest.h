#ifndef MEMORYTEST_H
#define MEMORYTEST_H

///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       memoryTest.h
 *
 *  @brief      Drivers for the page allocation kernel module
 *
 *              Copyright (C) 2006 @n@n
 *              These drivers allow one to allocate, read/write, and free memory
 *              aligned to pages
 *
 *		Utilizes page_allocator to allocate as many pages as the system will allow
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


unsigned long testMemory(short memType,unsigned int *failures, unsigned short debug);


unsigned long burninMemTest(unsigned int passes, unsigned short debug);

#endif
