#ifndef PAGE_ALLOCATOR_H
#define PAGE_ALLOCATOR_H

///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       pageAllocator.h
 *
 *  @brief      Drivers for the page allocation kernel module
 *
 *              Copyright (C) 2006 @n@n
 *              These drivers allow one to allocate, read/write, and free memory
 *              aligned to pages
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


// current size of a page in memory
#define PAGE_SIZE ((unsigned long)(getpagesize()))


// this needs to be defined carefully. This must be defined
// here because pageallocator may be used for a memory test, meaning
// that we should never take any more memory away from the system
#define MAX_LENGTH_QUAD_WORD 21

// length of a block to read
#define BLOCK_READER_LENGTH ( (MAX_LENGTH_QUAD_WORD+4)*MAX_PAGE_BLOCKS)+1


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     int allocate_pages(unsigned int pages,short memType)
 *
 *  @arg    <b>unsigned int</b> @pages 
 *          - Number of pages we request to allocate
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @return Number of pages that were actually allocated
 *
 *  @brief  Asks the page allocator to allocate the number
 *          of requested pages
 *
 *          Writes to the page allocator file, which requests 
 *          the number of desired pages to be allocated.
 *          
 *  @note   The number of allocated pages may differ greatly with
 *          your request. This depends heavily on current memory
 *          allocations, which may change very quickly
 *          
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
int allocate_pages(unsigned int,short);

// Allocates the number of bytes requested
#define allocate_bytes(bytes,type) allocate_pages((bytes/PAGE_SIZE) + (bytes%PAGE_SIZE > 0 ? 1 : 0),type)

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int read_page(short memType,char *data,short block,unsigned int page,unsigned int repeat);
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @arg    <b>char</b> @*data
 *          - pointer block in which we should place our data
 *
 *  @arg    <b>short</b> @block
 *          - block from which we request our read
 *
 *  @arg    <b>unsigned int</b> @page
 *          - page number
 *
 *  @arg    <b>unsigned int</b> @repeat
 *          - repeat variable - currently unimplemented 
 *
 *  @return The result of the read
 *
 *  @brief  Reads data from the page allocation block.
 *
 *          In order to read a page from memory, we must access the specified
 *          block file which contains the pointer to our page in memory.
 *          To access this block, we use a page reader structure that
 *          gives us the opportunity to read any page in memory that we allocate
 *          
 *  @note   repeat is currently unimplemented
 *          
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int read_page(short memType,char *data,short block,unsigned int page,unsigned int repeat);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int write_to_page(short memType,char *block_data,unsigned int block_size,short block,unsigned int page,unsigned int repeat);
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @arg    <b>char</b> @*data
 *          - pointer block in which we should place our data
 *
 *  @arg    <b>short</b> @block
 *          - block from which we request our read
 *
 *  @arg    <b>unsigned int</b> @page
 *          - page number
 *
 *  @arg    <b>unsigned int</b> @repeat
 *          - number of times to repeat this write
 *
 *  @return The result of the write
 *
 *  @brief  Writes data from the page allocation block.
 *
 *          In order to write a page to memory, we must access the specified
 *          block file which contains the pointer to our page in memory.
 *          To access this block, we use a page writer structure that
 *          gives us the opportunity to write to page in memory that we allocate
 *      
 *          The page variable is the page we wish to begin writing to. Specifying
 *          a value in repeat greater than zero will write to subsequent allocated
 *          pages
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int write_to_page(short memType,char *block_data,unsigned int block_size,short block,unsigned int page,unsigned int repeat);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void free_memory(short memType, char *block)
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @arg    <b>char</b> @*block
 *          - pointer block in which we should place our data
 *
 *  @arg    <b>short</b> @block
 *          - block from which we request our read
 *
 *  @arg    <b>unsigned int</b> @page
 *          - page number
 *
 *  @arg    <b>unsigned int</b> @repeat
 *          - number of times to repeat this write
 *
 *  @return The result of the write
 *
 *  @brief  Writes data from the page allocation block.
 *
 *          In order to write a page to memory, we must access the specified
 *          block file which contains the pointer to our page in memory.
 *          To access this block, we use a page writer structure that
 *          gives us the opportunity to write to page in memory that we allocate
 *      
 *          The page variable is the page we wish to begin writing to. Specifying
 *          a value in repeat greater than zero will write to subsequent allocated
 *          pages
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void free_memory(short memType, char *block);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int open_alloc_proc(short memType,char *procName,unsigned int flag)
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @arg    <b>char</b> @*procName
 *          - name of the proc file
 *
 *  @arg    <b>unsigned int</b> @flag
 *          - flags for opening the proc file
 *
 *  @return The file descriptor
 *
 *  @brief  Opens the specified proc file and returns the associated file descriptor
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int open_alloc_proc(short memType,char *procName,unsigned int flag);

// closes the file descriptor
#define close_alloc_proc(fd) close(fd)


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int free_pages(short memType)
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *
 *  @return The number of free pages for the system
 *
 *  @brief  Returns the number of available pages for the memory zone
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int free_pages(short memType);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int reserved_pages(short memType)
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *
 *  @return The number of reserved pages for the system
 *
 *  @brief  Returns the number of pages reserved for the system kernel.
 *          if this number is breached, the system risks entering an OOM
 *          situation, first enabling kswapd, followed by OOM-Killer
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int reserved_pages(short memType);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void free_memory(short memType, char *block)
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @arg    <b>char</b> @block
 *          - list of blocks to free
 *
 *  @brief  Traverses block as an array and frees the pages which correspond
 *          to the block elements that are set to 1
 *      
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void free_memory(short memType, char *block);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int block_size(short memType,short block)
 *
 *  @arg    <b>short</b> @memType
 *          - memory zone, either low or high mem
 *
 *  @arg    <b>short</b> @block
 *          - block from which to obtain the size
 *
 * @return  Number of bytes in block
 *
 *  @brief  Returns the block size in bytes
 *      
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int block_size(short memType,short block);

#endif


