///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       page_alloc.h
 *
 *  @brief      Header file for the page allocator
 *
 *              Copyright (C) 2006 @n@n
 *  
 *  @author     Marc Parisi
 *  @author     mparisi@dtims.com                                                
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
#ifndef DTIMEM_H
#define DTIMEM_H

#include "page_allocator_defs.h"
#include <linux/proc_fs.h>
#include <linux/spinlock.h>


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         short verify_memory_range(int zone, unsigned int page_number,unsigned int sector, int page_file_number);
 *
 *  @arg        <b>int</b> @zone 
 *               - zone that we are currently within
 *
 *  @arg        <b>unsigned int</b> @page_number
 *               - current page number in our global pages array
 *
 *  @arg        <b>unsigned int</b> @sector
 *               - current element within the global page array
 *  
 *  @arg        <b>int</b> @page_file_number
 *               - current block in global page array
 *
 *  @return     PASS/FAIL status of the memory region
 *
 *  @brief      Depending on the memory test, verify_memory_region ensures that
 *              the current page number and sector do not exceed the bounds
 *              of the currently allocated memory in that region
 */
////////////////////////////////////////////////////////////////////////
short verify_memory_range(int zone, unsigned int page_number,unsigned int sector, int page_file_number);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         struct page* get_mem_page(int zone,unsigned int page_file_number,unsigned long page_number,unsigned long sector)
 *
 *  @arg        <b>int</b> @zone 
 *               - zone that we are currently within
 *
 *  @arg        <b>int</b> @page_file_number
 *               - current block in global page array
 *
 *  @arg        <b>unsigned long</b> @page_number
 *               - current page number in our global pages array
 *
 *  @arg        <b>unsigned long</b> @sector
 *               - current element within the global page array
 *
 *  @return     the page pointed to by our array
 *
 *  @brief      Returns the low memory page, specified by the page_number
 *              and sector. Sector is the element with in the array, beginning
 *              at element page_number.
 *
 *  @note       When dealing with high memory, the low pages returned by
 *              this function provides us a list of pointers to high 
 *              memory pages.
 */
////////////////////////////////////////////////////////////////////////
struct page* get_mem_page(int zone,unsigned int page_file_number,unsigned long page_number,unsigned long sector);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         void destroy_fs(unsigned int page_file_number, struct proc_dir_entry *entry)
 *
 *  @arg        <b>int</b> @page_file_number
 *               - current block in global page array
 *
 *  @arg        <b>struct proc_dir_entry</b> @*entry
 *               - proc file entry
 *
 *  @brief      Deletes the proc files for all memory blocks
 *
 */
////////////////////////////////////////////////////////////////////////
void destroy_fs(unsigned int page_file_number, struct proc_dir_entry *entry);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         char check_scheduler();

 *  @return     Value of whether the program should be rescheduled
 *
 *  @brief      Returns 1 if the kernel scheduler scheduled the calling
 *              process to wait. If the scheduler executed, then we simply
 *              set this task back to the running state
 *
 */
////////////////////////////////////////////////////////////////////////
char check_scheduler(void);

#define check_resched(counter) if (check_scheduler() == 1) counter++


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_allocated_pages_read(struct file *file,const char *buffer,unsigned long count,void *data);
 *
 *  @arg        <b>struct file</b> @*file
 *                - input source file
 *
 *  @arg        <b>const char</b> @*buffer
 *                - input source file
 *
 *  @arg        <b>unsigned long</b> @count
 *                - input source file
 *
 *  @arg        <b>void</b> @*data
 *                - input source file
 *
 *  @return     amount of data read
 *
 *  @brief      Performs a read on the specified pages in memory
 *              A page reader structure is copied from user space. The
 *              page number is extracted from that and the data is copied
 *              into it
 *
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocated_pages_read(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_allocated_pages_write(struct file *file,const char *buffer,unsigned long count,void *data);
 *
 *  @arg        <b>struct file</b> @*file
 *                - input source file
 *
 *  @arg        <b>const char</b> @*buffer
 *                - input source file
 *
 *  @arg        <b>unsigned long</b> @count
 *                - input source file
 *
 *  @arg        <b>void</b> @*data
 *                - input source file
 *
 *  @return     amount of data read
 *
 *  @brief      Performs a wite on the specified pages in memory
 *              A page writer structure is copied from user space. The
 *              page number, and data are specified in the structure.The
 *              data is copied to each page. The page span variable indicates
 *              how many pages subsequent to the initial page number are
 *              to be written to
 *
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocated_pages_write(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         unsigned int page_action(int zone,unsigned int page,unsigned int sector,struct page *pagePointer,void *data,unsigned long size,short action);
 *
*  @arg        <b>int</b> @zone 
 *               - zone that we are currently within
 *
 *  @arg        <b>unsigned int</b> @page
 *               - current page number in our global pages array
 *
 *  @arg        <b>unsigned int</b> @sector
 *               - current element within the global page array
 *  
 *  @arg        <b>struct page</b> @*pagePointer
 *               - pointer to low memory page
 *
 *  @arg        <b>char </b> @*data
 *               - data array
 *
 *  @arg        <b>unsigned int </b> @size
 *               - size of data array
 *  
 *  @arg        <b>short</b> @*action
 *               - action to perform
 *
 *
 *  @return     length of resulting action
 *
 *  @brief      Performs either a read or write on the provided page. 
 *              In the case of a write, data is written to the address
 *              specified by pagePointer. During a read, the page specified
 *              by pagePointer is copied into data
 *              
 *  @note       pagePointer is always the low memory page; however, when dealing
 *              with high memory allocation, the high memory page must mapped into
 *              kernel memory so we can address it. Once the high memory page is mapped,
 *              the read/write can proceed
 *
 */
////////////////////////////////////////////////////////////////////////
unsigned int page_action(int zone,
			unsigned int page,
			unsigned int sector,
			struct page *pagePointer,
			void *data,
			unsigned long size,
			short action);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         struct page *allocate_pages(int zone, unsigned int *flags, unsigned long *count, unsigned long *max);
 *
 *  @arg        <b>int</b> @zone 
 *               - zone that we are currently within
 *
 *  @arg        <b>unsigned int</b> @*flags
 *               - flags sent to allocate_pages(...)
 *
 *  @arg        <b>unsigned int</b> @*count
 *               - counter for the number of allocated pages
 *  
 *  @arg        <b>unsigned long</b> @*max
 *               - total number of pages to allocate
 *
 *  @return     resulting page structure
 *
 *  @brief      Allocates pages within either low or high memory.
 *
 *              In both cases, a low memory page is first allocated. When
 *              high memory is allocated, subsequent high memory pages are allocated
 *              and the pointers are placed in the low memory page to be access
 *              later
 *              
 *  @note       Allocates a single low mem page, which, in the case of high mem
 *              can contain pointers to multiple high memory pages
 *
 */
////////////////////////////////////////////////////////////////////////
struct page *allocate_pages(int zone, unsigned int *flags, unsigned long *count, unsigned long *max);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         struct page *get_high_page(struct page *lowPage,unsigned int page_number,unsigned int sector);
 *
 *  @arg        <b>struct page</b> @*lowPage 
 *               - pointer to low memory page
 *
 *  @arg        <b>unsigned int</b> @page_number
 *               - requested page number
 *
 *  @arg        <b>unsigned int</b> @sector
 *               - offset to page number
 *  
 *  @return     resulting page structure
 *
 *  @brief      Obtains the desired high memory page structure by accessing
 *              the page pointer contained in the provided low memory page
 *
 */
////////////////////////////////////////////////////////////////////////
struct page *get_high_page(struct page *lowPage, 
			unsigned int page_number,
			unsigned int sector);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         void free_high_pages(struct page *lowPage);
 *
 *  @arg        <b>struct page</b> @*lowPage 
 *               - pointer to low memory page
 *
 *  @brief      Frees allocated pages in high memory. 
 *
 *              Functions as the reverse of allocate_pages
 *              
 */
////////////////////////////////////////////////////////////////////////
void free_high_pages(struct page *lowPage);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_read_min_pages(char *page,char **start, off_t offset,
 *				int count, int *eof, void *data);
 *
 *  @arg        <b>void</b> *page
 *              - return page
 *  @arg        <b>void</b> *data
 *              - contains a pointer to the current zone
 *
 *  @brief      Prints the minimum number of pages to the page arguments, 
 *              which subsequently goes to the command line
 *              
 *  @note       The arguments not specified here are not used, they are standard
 *              proc arguments
 *
 */
////////////////////////////////////////////////////////////////////////
static int proc_read_min_pages(char *page,char **start, off_t offset,
				int count, int *eof, void *data);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_read_free_pages(char *page,char **start, off_t offset,
 *				int count, int *eof, void *data);
 *
 *  @arg        <b>void</b> *page
 *              - return page
 *  @arg        <b>void</b> *data
 *              - contains a pointer to the current zone
 *
 *  @brief      Prints the number of free pages for the zone specified by data
 *              
 *  @note       The arguments not specified here are not used, they are standard
 *              proc arguments
 *
 */
////////////////////////////////////////////////////////////////////////
static int proc_read_free_pages(char *page,char **start, off_t offset,
				int count, int *eof, void *data);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_allocate_pages(char *page,char **start, off_t offset,int count, int *eof, void *data);
 *
 *  @arg        <b>void</b> *page
 *              - return page
 *  @arg        <b>void</b> *data
 *              - contains a pointer to the current zone
 *
 *  @brief      Prints a list of allocated page blocks and their associated sizes
 *              
 *  @note       The arguments not specified here are not used, they are standard
 *              proc arguments
 *
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocate_pages(char *page,char **start, off_t offset,
				int count, int *eof, void *data);



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_allocate_pages_write(struct file *file,const char *buffer,unsigned long count, void *data);
 *
 *  @arg        <b>void</b> *page
 *              - return page
 * 
 *  @arg	<b>const char</b> @*buffer
 *		- data written to the associated proc file
 *
 *  @arg        <b>void</b> *data
 *              - contains a pointer to the current zone
 *
 *  @brief      Attempts to allocate the desired number of pages
 *		
 *		Casts the input from buffer into an unsigned long. The input should be
 *		a number specifying the number of pages to allocate to the zone in which 
 *		this proc file exists. The requested number of pages may exceed the number
 *		of free pages at that instant, or pages may suddenly be freed, causing the desired
 *		number of allocated pages to be less than those free. In the latter case, pages
 *		can be allocated later. In the former, the number of allocated pages will be limited
 *		to the number of free pages minus the minimum number of pages before OOM killer is
 *		engaged
 *
 *		If allocating low pages, the pages will be allocated using alloc_pages on low mem
 *		and the pages will be tracked in the pages array, which acts as the page controller
 *		as it contains a list of all page pointers. If allocating high memory, the situation
 *		may arise when the number of pointers in the pages array exceeds the virtual kernel
 *		address space ( currently, @ 128 megs ). Therefore, the pointers to the upper memory
 *		pages are kept in low memory pages. These low memory pages are tracked by the pages
 *		array ( listed below ). Therefore, limiting the number of vmallocs. This causes a greater
 *		load to access high memory pages, but allows page allocator to allocate up to 64 GB,
 *		and beyond when the technology permits. 
 *
 *  @note       The arguments not specified here are not used, they are standard
 *              proc arguments
 *
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocate_pages_write(struct file *file,const char *buffer,unsigned long count, void *data);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         spinlock_t *get_lock(short memType);
 *
 *  @arg        <b>short</b> memType
 *              - memory zone
 *
 *  @brief      returns a pointer to the correct spin lock
 *              
 *
 */
////////////////////////////////////////////////////////////////////////
spinlock_t *get_lock(short memType);



pg_data_t *get_zones(void);

/*
 page structure which is the controller for locating
 allocated pages. In the case of low memory, pages will contain
 a pointer to each individual page. when accessing high memory
 pages are allocated in low memory that will contain pointers to the allocated
 pages in upper memory; thus following pages structure will contain references
 to the low memory pages in the case of High memory and Low memory
 */
struct page **pages[2][MAX_PAGE_BLOCKS];

// a count of the number of allocated pages
unsigned int allocated_pages[2][MAX_PAGE_BLOCKS];

spinlock_t low_mem_lock,high_mem_lock;

// proc entries
static struct proc_dir_entry *main_dir,*high_mem_dir,*low_mem_dir, *high_mem_pages_free,*high_mem_pages_min,*low_mem_pages_free,*low_mem_pages_min,*page_liberator_file;
static struct proc_dir_entry *allocate_high_pages=NULL,*allocated_high_pages=NULL;
static struct proc_dir_entry *allocate_low_pages=NULL,*allocated_low_pages=NULL;

#endif
