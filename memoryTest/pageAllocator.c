///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       pageAllocator.c
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include "pageAllocator.h"
#include "page_allocator_defs.h"





///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         int allocate_pages(unsigned int pages, short memType)
 */
///////////////////////////////////////////////////////////////////////////////
int allocate_pages(unsigned int pages, short memType)
{
	char buffer[64];
	int fd=0,page_block=0;

    // sprintf the number of pages into buffer
    memset(buffer,0,64);
	sprintf(buffer,"%u",pages);
	
    // open the file allocation file
	if ( (fd = open_alloc_proc(memType,PAGE_ALLOCATOR_NAME,O_WRONLY)) > 0 )
	{
        // write to the file allocation file then promptly close it
		page_block = write(fd,buffer,strlen(buffer),0);
		close_alloc_proc(fd);
	}


	return page_block;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned int open_alloc_proc(short memType,char *procName,unsigned int flag)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int open_alloc_proc(short memType,char *procName,unsigned int flag)
{
	char *nameString =NULL,*memTypeName=NULL,buffer[64];
	switch(memType)
	{
		case HIGH_MEM:
			memTypeName = HIGH_MEM_DIR;
			break;
		case LOW_MEM:
			memTypeName = LOW_MEM_DIR;
			break;
		default:
			return -1;
	};
	nameString = (char*)malloc(strlen(memTypeName) + strlen(MODULE_NAME) + strlen(FILE_SYSTEM) + strlen(procName) + 3);
	sprintf(nameString,"%s%s/%s/%s",FILE_SYSTEM,MODULE_NAME,memTypeName,procName);
	return open(nameString,flag);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn		unsigned int free_pages(short memType)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int free_pages(short memType)
{

	char buffer[64];
	memset(buffer,0,64);
	int fd=0,size_read=0;

	if ( (fd = open_alloc_proc(memType,FREE_PAGES,O_RDONLY)) <= 0 )
		return 0;

	size_read = read(fd,buffer,sizeof(buffer),0);
	close_alloc_proc(fd);

	return atol(buffer);


}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned int write_to_page(short memType,char *block_data,unsigned int block_size,short block,unsigned int page,unsigned int repeat)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int write_to_page(short memType,char *block_data,unsigned int block_size,short block,unsigned int page,unsigned int repeat)
{
	int fd = 0,size=0,block_test=0,len=-1;
	page_writer_structure pageWriter;
	char name[strlen(ALLOCATED_PAGE_DIR) +11];
	sprintf(name,"%s/%u_write",ALLOCATED_PAGE_DIR,block);
	if ( (fd = open_alloc_proc(memType,name,O_WRONLY)) > 0 )
	{
		pageWriter.page_number = page;
		pageWriter.page_file_number = block;
		pageWriter.pages_span = repeat;
		pageWriter.data = block_data;
		pageWriter.data_size=block_size;
		len = write(fd,(void*)&pageWriter,sizeof(page_writer_structure),0);
		close_alloc_proc(fd);
		
	}


	return len;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn     unsigned int read_page(short memType,char *data,short block,unsigned int page,unsigned int repeat)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int read_page(short memType,char *data,short block,unsigned int page,unsigned int repeat)
{
	int fd = 0,size=0,block_test=0,len=0;
	page_reader_structure pageReader;
	// static array should be large enough to contain the name of the file
	char name[strlen(ALLOCATED_PAGE_DIR) +11];
	sprintf(name,"%s/%u_read",ALLOCATED_PAGE_DIR,block);
    // open the proc file for read only access
	if ( (fd = open_alloc_proc(memType,name,O_WRONLY)) > 0 )
	{
		pageReader.page_number = page;
		pageReader.page_file_number = block;
		pageReader.pages_span = repeat;
		pageReader.data = data;
		pageReader.data_size=PAGE_SIZE;
        /*
            write a page reader structure to the page read file, allowing us
            to read the specified page from the page file number. The data
            will be placed in pageReader.data. The size will be set to a single
            page so we can read the entire page
         */
		size = write(fd,(void*)&pageReader,sizeof(page_reader_structure),0);
		close_alloc_proc(fd);
		
		
	}
	return size;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         void free_memory(short memType, char *block)
 */
///////////////////////////////////////////////////////////////////////////////
void free_memory(short memType, char *block)
{
	int fd = 0,size=0,block_test=0,len=0;
	short i=0;
	page_liberator pageFree;
	char nameString[strlen(MODULE_NAME) + strlen(FILE_SYSTEM) + strlen(FREE_PAGES) + 3];
	
	sprintf(nameString,"%s%s/%s",FILE_SYSTEM,MODULE_NAME,FREE_PAGES);
	if ( (fd = open(nameString,O_WRONLY)) > 0 )
	{
		pageFree.mem_type = memType;
		for (; i < MAX_PAGE_BLOCKS; i++)
		{
			pageFree.block[i]=block[i];
		}
		size = write(fd,(void*)&pageFree,sizeof(page_liberator),0);
		close_alloc_proc(fd);
		
		
	}
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned int block_size(short memType,short block)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int block_size(short memType,short block)
{
	int fd = 0,size=0,block_test=0;
	char *tok;
	unsigned int block_size = 0;
	char buffer[BLOCK_READER_LENGTH];
	if ( (fd = open_alloc_proc(memType,PAGE_ALLOCATOR_NAME,O_RDONLY)) > 0 )
	{
		memset(buffer,0,BLOCK_READER_LENGTH);
		size = read(fd,buffer,BLOCK_READER_LENGTH,0);
		close(fd);
		tok = (char*)strtok (buffer,"\n");
		while (tok != NULL)
		{
			if ( sscanf (tok,"%i:%u",&block_test,&block_size) == 2)
			{
				if (block_test == block)
					break;
			
			}
			block_size = 0;
		 	tok = (char*)strtok (NULL, "\n");
		 }
	}
	return block_size;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned int reserved_pages(short memType)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int reserved_pages(short memType)
{
	char buffer[64];
	memset(buffer,0,64);
	int fd=0,size_read=0;

	if ( (fd = open_alloc_proc(memType,MIN_PAGES,O_RDONLY)) <= 0 )
		return 0;

	size_read = read(fd,buffer,sizeof(buffer),0);
	close_alloc_proc(fd);

	return atol(buffer);
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned int available_pages(short memType)
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int available_pages(short memType)
{
    unsigned int avail = 0;

    if ( free_pages(memType) >= (reserved_pages(memType)+2) )
        avail = free_pages(memType)-(reserved_pages(memType)+2);
	return avail;
}
