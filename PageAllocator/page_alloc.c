///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       page_alloc
 *
 *  @brief      Source file for the page allocator
 *
 *              Copyright (C) 2006 @n@n
 *		Page allocator is a mechanism to safely allocate pages in low and high
 *		memory. Page allocator ensures that, no matter what, the minimum amount
 *		of memory is available to the Linux Kernel. 
 *
 *		The system requires that a certain amount of free memory is reserved
 *		for the kernel and running programs. This memory will never be allocated
 *		Note, however, if a kmalloc occurs and attempts to allocate memory while
 *		this kernel module is working, it may fail since kmalloc allocates memory
 *		in a contiguous space.
 *
 *		The page allocator arose from the memory test. Memory cannot be allocated
 *		freely in user space until none is available, as the Out of Memory Killer
 *		will be engaged, as it will be if the reserved pool is not met. To avoid 
 *		this, a kernel module to watch the number of free pages is needed. In
 *		addition if memory is wantonly malloced in user space, the OOM killer may
 *		not be triggered deterministically, since by design, the OOM killer uses
 *		a non-deterministic heuristic to gauge when an OOM situation has occured,
 *		meaning that the reserved pool may fluctuate intermittently, thus the reasoning
 *		behind using a kernel module to constantly monitor the free and minimum
 *		page amounts. 
 *
 *		To allocate low memory, alloc_pages is called with the GFP_USER flag to
 *		allocate low memory, and keep the page table entries within low memory. 
 *		A pointer is used to keep track of these pages so the pages can be freed
 *		whenever a free is triggered or the module exits. A simple array keeps track
 *		of low memory.
 *	
 *		To allocate high memory, however, pages must be allocated in low memory to 
 *		keep track of the page pointers to the pages in high memory since the pointers,
 *		at 4 bytes each ( on a 32 bit sys ), can take up 500 MB alone, on a 64 GB system.
 *		Since the kernel can only allocate 128 megs at a time, this is not feasible. Allocating
 *		then freeing pages in high mem is not feasible either since the system may simply reallocate
 *		freed pages later. This may not be a problem in most situations, but it would not fulfill
 *		the requirements of the memory test. 
 *		
 *		Each low mem page can keep approximately PAGE_SIZE/POINTERSIZE 
 *		(1024 b on 32 bit sys w/ 4096 b pages ) pointers to high page. This lowers the 
 *		required kernel virtual space to 4 megs for a 64 GB sys. The high mem pages will be accessed
 *		by reading the page pointers from low mem, then kmapping the page into kernel space.
 *		kmap is relatively slow, but it's alternative, kmap_atomic, does not resolve cache coherency
 *		and is, as the function name states, atomic.  The remaining functionality this modules behavior 
 *		is inherent from generic procfs modules.
 *
 *      New addition: spin locks to ensure that multiple read/writes maintain coherency
 *
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
#include <linux/version.h>
#include <linux/config.h>
#include <linux/fs.h>
#include <linux/types.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include <linux/mmzone.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/highmem.h>
#include <linux/sched.h>
#include <linux/interrupt.h>



#define MODULE_VERS "1.0"
MODULE_AUTHOR("Marc Parisi");
MODULE_LICENSE("GPL");

#include "page_alloc.h"



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         short verify_memory_range(int zone, unsigned int page_number,unsigned int sector, int page_file_number);
 */
////////////////////////////////////////////////////////////////////////
short verify_memory_range(int zone, unsigned int page_number,unsigned int sector, int page_file_number)
{
    // this was changed slightly, now, both memory regions
    // are represented in allocated_pages the same way
    if (page_number+sector >= 
    allocated_pages[zone][page_file_number])
    {
        return FALSE;
    }
    else
        return TRUE;
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         struct page* get_mem_page(int zone,unsigned int page_file_number,unsigned long page_number,unsigned long sector)
 */
////////////////////////////////////////////////////////////////////////
struct page* get_mem_page(int zone,
			unsigned int page_file_number,
			unsigned long page_number,		
			unsigned long sector)
{
	unsigned long arrayLocation = 0;
	switch(zone)
	{
    case HIGH_MEM_ZONE:
            // center the location within our block page pages in low mem
			arrayLocation = (page_number+sector)/(PAGES_PER_PAGE);
			break;
		case LOW_MEM_ZONE:
        default:
            arrayLocation = page_number+sector;
			break;
	};
    // return the pointer to the requested page
	return pages[zone][page_file_number][arrayLocation];

}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn    unsigned int page_action(int zone,unsigned int page,unsigned int sector,struct page *pagePointer,void *data,unsigned long size,short action);
 */
////////////////////////////////////////////////////////////////////////
unsigned int page_action(int zone,
			unsigned int page,
			unsigned int sector,
			struct page *pagePointer,
			void *data,
			unsigned long size,
			short action)
{
    void *address=NULL;
	unsigned int len = 0;
	if (pagePointer == NULL)
		return len;

    spin_lock( get_lock(zone) );
	if (zone==HIGH_MEM_ZONE)
	{
		if ( !(pagePointer = get_high_page(pagePointer,page,sector)) )
        {
            spin_unlock( get_lock(zone) );
            return len;
        }
			
			
	}
    // map our page pointer
	address = kmap(pagePointer);

	if (address != NULL)
	{
		len=size;

		(action == WRITE_PAGE)  ? memcpy(address,data,size) 
					: memcpy(data,(char*)address,size);

        // unmap the page pointer
		kunmap(pagePointer);
	}
    spin_unlock( get_lock(zone) );
	return len;
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn    struct page *allocate_pages(int zone, unsigned int *flags, unsigned long *count, unsigned long *max)
 */
////////////////////////////////////////////////////////////////////////
struct page *allocate_pages(int zone, unsigned int *flags, unsigned long *count, unsigned long *max)
{
    // allocate single page
    spin_lock( get_lock(zone) );
	struct page *pagePointer = alloc_pages(*flags,0);
    
	struct page *tempPage = NULL;
	if (!pagePointer)
    {
        printk(KERN_INFO "couldn't allocate page\n");
		return pagePointer;
    }
	if ( zone == HIGH_MEM_ZONE)
	{	
		unsigned int i=0;
        // map the first page, and cast it as an integer
        // effectively accessing the page as a series of integers
		int *address = kmap(pagePointer);
		if (address != NULL)
		{
			// set all to zero
			memset(address,0,PAGE_SIZE);
            // allocate the subsequent pages, and place the pointer into
            // the first page ( which should be in low memory, but can also
            // be in high memory)
			for (; i < (PAGES_PER_PAGE) && *count < *max; i++,*count=*count+1)
			{
				tempPage = alloc_pages(*flags|__GFP_HIGHMEM,0);
				// save the address for our high memory page
                if (tempPage == NULL)
                {
                    *count = *count-1;
                    break;
                }
                address[i] = (int)tempPage;
				
			}
            // unmap the initial page
			kunmap(pagePointer);
		}
		else
		{
			// since we couldn't map the address, go ahead and 
			// free the page we allocated, and terminate our current process
            spin_unlock( get_lock(zone) );
			__free_pages(pagePointer,0);
			return NULL;
		}
	
	}
	
    spin_unlock( get_lock(zone) );
    return pagePointer;
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn    struct page *get_high_page(struct page *lowPage, unsigned int page_number,unsigned int sector)
 */
////////////////////////////////////////////////////////////////////////
struct page *get_high_page(struct page *lowPage, 
			unsigned int page_number,
			unsigned int sector)
{
	struct page *highPage = NULL;
	unsigned long loc =(page_number+sector)/(PAGES_PER_PAGE);
	int *tempAddr=NULL;

    // revision -- the following shouldn't need to be an atomic map
    // since it does not reside in high memory
    // tempAddr = kmap_atomic(highPage,KM_USER0);


    tempAddr = kmap(lowPage);


	if (tempAddr == NULL)
		goto exitRoutine;
    // adjust location so that we are dealing with elements of an 
    // array, instead individual pages
	loc= ((page_number+sector)-(loc*(PAGES_PER_PAGE)));
	
	if (loc*POINTER_SIZE >= PAGE_SIZE)
		goto exitRoutine;
    /*
     tempAddr[loc] contains the pointer to the high memory page
     therefore, we should copy the value within tempAddr[loc] into
     the highPage, but not as a value as memcpy dereferences the first pointer,
     but rather as the actual address of highPage, so we can change the location
     to which it points
     */
	memcpy(&highPage,&tempAddr[loc],POINTER_SIZE);
    
	exitRoutine:
	// not needed any more 
	// kunmap_atomic(highPage,KM_USER0);
    kmap(lowPage);
	return highPage;

}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn    void free_high_pages(struct page *lowPage)
 */
////////////////////////////////////////////////////////////////////////
void free_high_pages(struct page *lowPage)
{
	short i=0;
	struct page *highPage = NULL;
	int *tempAddr = kmap(lowPage);
	// traverse the array of allocated high pages
	for (; i < (PAGES_PER_PAGE); i++)
	{
		if (tempAddr[i] == 0)
			break;
        memcpy(&highPage,&tempAddr[i],POINTER_SIZE);
        if (highPage != NULL)
        {
            
            __free_pages(highPage,0);
        }
		
	}
    //printk("\n");
    kunmap(lowPage);
	

}

// create global page reader and writer structures
// in addition to the page free mechanism so calling
// programs can free memory
page_reader_structure pageReader;
page_writer_structure pageWriter;
page_liberator	      pageFreeMechanism;



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         char check_scheduler() 
 */
////////////////////////////////////////////////////////////////////////
char check_scheduler(void) 
{
    // the following code was copied from vm regress
    if (in_interrupt() || !current) return 0;

    // if the program needs to be rescheduled, then
    // we should delay it so we can continue our current task
	if (need_resched()) 
    {
		__set_current_state(TASK_RUNNING);
        // recall the sechedule
		schedule();
		return 1;
	}

	return 0;
}



int zone_high,zone_normal;





////////////////////////////////////////////////////////////////////////
/** 
 *  @fn   static int proc_read_min_pages(char *page,char **start, off_t offset,int count, int *eof, void *data)
 */
////////////////////////////////////////////////////////////////////////
static int proc_read_min_pages(char *page,char **start, off_t offset,
				int count, int *eof, void *data)
{
	// obtain the zone id
	int zone_id,len;
	unsigned long       flags;
	pg_data_t *pgdat=NULL;
	struct zone	  *zone=NULL;


	zone_id = *(int*)data;
	len = -EFAULT;

	pgdat = get_zones();
	
	if (pgdat) zone = &pgdat->node_zones[zone_id]; 
	if (!zone)
	{
		printk(KERN_INFO "ERROR: Could not find zone\n");
	}
	else
	{
		// lock the zone id with a spin lock, so 
		// no other process can gain access to this proc function
		spin_lock_irqsave(&zone->lock, flags);
		len = sprintf(page, "%lu\n",zone->pages_min); // subtract one for new line
		spin_unlock_irqrestore(&zone->lock, flags);
	}
	return len;
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn   static int proc_read_free_pages(char *page,char **start, off_t offset,int count, int *eof, void *data)
 */
////////////////////////////////////////////////////////////////////////
static int proc_read_free_pages(char *page,char **start, off_t offset,
				int count, int *eof, void *data)
{
	// obtain the zone id
	int zone_id,len;
	unsigned long       flags;
	pg_data_t *pgdat=NULL;
	struct zone	  *zone=NULL;

	zone_id = *(int*)data;
	len = -EFAULT;

	pgdat = get_zones();

	if (pgdat) zone = &pgdat->node_zones[zone_id]; 
	if (!zone)
	{
		printk(KERN_INFO "ERROR: Could not find zone\n");
	}
	else
	{
		// lock the zone id with a spin lock, so 
		// no other process can gain access to this proc function
		spin_lock_irqsave(&zone->lock, flags);
		len = sprintf(page, "%lu\n",zone->free_pages); // subtract one for new line
		spin_unlock_irqrestore(&zone->lock, flags);
	}
	return len;
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn  static int proc_allocate_pages(char *page,char **start, off_t offset,int count, int *eof, void *data)
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocate_pages(char *page,char **start, off_t offset,
				int count, int *eof, void *data)
{
	int len = -EFAULT; // set to negative value to start with
	int zone_id = *(int*)data; // get the zon eid
	short zone_current=0,i=0,size=0;
    unsigned int cnt=0;
	char *localBuffer=NULL,extraBuffer[64];
	sprintf(extraBuffer,"%u",-1);
	size = strlen(extraBuffer);
	if (zone_id == ZONE_HIGHMEM)
		zone_current = HIGH_MEM_ZONE;
	else
		zone_current = LOW_MEM_ZONE;

	for (i=0; i < MAX_PAGE_BLOCKS; i++)
	{
		if (allocated_pages[zone_current][i] != 0)
		{
			cnt++;
		}
	}
	// allocate memory to print out 
	localBuffer = (char*)vmalloc( ( (size+4)*cnt)+1 );
	memset(localBuffer,0,((size+4)*cnt)+1);

	// if we could not allocate memory, leave
	if (!localBuffer)
		return len;
	for (i=0; i < MAX_PAGE_BLOCKS; i++)
	{
		if (allocated_pages[zone_current][i] != 0)
		{
			// get the count of pages
			cnt = allocated_pages[zone_current][i];
			sprintf(extraBuffer,"%i:%u\n",i,cnt);
			strcat(localBuffer,extraBuffer);
		}
	}

	len = sprintf(page,"%s",localBuffer);
	vfree(localBuffer);
	return len;

}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn  static int proc_allocate_pages_write(struct file *file,const char *buffer,unsigned long count, void *data)
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocate_pages_write(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data)
{
	// create variables
	int zone_id = 0,len=-EFAULT;
	char nameBuffer[256];
	unsigned int sched_count=0;
    unsigned long i = 0;
	unsigned long flags,pagesToAllocate,pageStructure=0,freePages=0,arraySize=0,high_mem_count=0;
	static struct proc_dir_entry *tempEntry=NULL;
	pg_data_t *pgdat = get_zones();
	struct zone	  *zone=NULL;
	// normalize gfp flags to GFP_USER
	unsigned int gfpFlags=GFP_USER| __GFP_NOWARN;
	char localBuffer[32];
	short zone_current=0,mem_current=0;

	// conver incoming data to the zone identifier
	zone_id = *(int*)data;

	// if the user requested high memory
	// tell the os that we would like to work within the high
	// memory zone ( i.e. above 896 MB)
	if (zone_id == ZONE_HIGHMEM)
	{
		zone_current = HIGH_MEM_ZONE;
//		gfpFlags|=__GFP_HIGHMEM;
	}
	else
		zone_current = LOW_MEM_ZONE;
	
	
	// copy the user's buffer into our buffer, which is in kernel
	// memory
	if(copy_from_user(&localBuffer, buffer, count))
               goto exitRoutine;
	// convert the string to a long integer
	pagesToAllocate=(unsigned long)simple_strtol((const char*)localBuffer,(char **)(localBuffer + sizeof(localBuffer)),(unsigned int)10);

	// if the user requested zero pages, or an error occurred in conversion, simply exit
    	if (pagesToAllocate == 0)
    	{
	        goto exitRoutine;
    	}	
	// get the zone data
	if (pgdat) zone = &pgdat->node_zones[zone_id]; 
	
	if (!zone)
	{
		printk(KERN_INFO "ERROR: Could not find zone\n");
		goto exitRoutine;
	}

	spin_lock_irqsave(&zone->lock, flags);
	// ensure the number of requested pages is within the zone's bounds
	if (pagesToAllocate > (zone->free_pages-zone->pages_min) ||
	    pagesToAllocate <= 0)
	{	
		// exit if no pages exist
		if ((zone->free_pages-(zone->pages_min+1)) <= 0)
		{
			spin_unlock_irqrestore(&zone->lock, flags);
			goto exitRoutine;
		}
		// adjust pagesToAllocate the largest possible size
		pagesToAllocate =  zone->free_pages-(zone->pages_min+1);
	}

	// freePages is not the number of free zone pages, but rather the minimum
	// number of free pages allowed within that zone
	freePages = zone->pages_min;
	
	spin_unlock_irqrestore(&zone->lock, flags);
	// compute the size for an additional page structure
	pageStructure = (pagesToAllocate * sizeof(struct page *)) / PAGE_SIZE + 1;
	// if we are at the limit of memory, then we should remove the memory required for a single
	// page structure so that we can successfully retain the pointers to our memory pages
	if (pagesToAllocate >= (zone->free_pages-(zone->pages_min+1)-pageStructure))
	{
		if (pageStructure >= pagesToAllocate )
		{
			goto exitRoutine;
		}
		pagesToAllocate -= pageStructure;
	}
	
	// locate the first available block
	for (i=0; i < MAX_PAGE_BLOCKS; i++)
	{
		if (allocated_pages[zone_current][i] == 0)
		{
			mem_current = i;
			break;
		}
	}
	if (i == MAX_PAGE_BLOCKS)
	{
		printk(KERN_INFO "All memory allocated\n");
		goto exitRoutine;
	}
	// allocate the structure using virtual malloc, which allocates memory for the kernel
	// on non-contiguous pages. This, of course, can cause fragmentation, but improves the chance that
	// memory will be allocated for our process
	arraySize = (zone_current==HIGH_MEM_ZONE) ? (pagesToAllocate/(PAGES_PER_PAGE)): pagesToAllocate;

    	if (arraySize == 0 && zone_current==HIGH_MEM_ZONE)
    	{
	        arraySize = 1;
    	}	

	pages[zone_current][mem_current] = vmalloc((arraySize+1) * sizeof(struct page **));
	if (pages[zone_current][mem_current]== NULL)
	{
		goto exitRoutine;
	}
	// set all of the memory to zero
	memset(pages[zone_current][mem_current], 0, arraySize*sizeof(struct page **));	
	// preemption is no longer needed. it's handled by the check_resched below
	//preempt_disable();
	high_mem_count = 0;
	for (i=0; i < arraySize; i++)
	{
		if (zone->free_pages <= freePages+1)
		{
			i--;
			break;
		}
		// this is quite important. What we're doing here is forcing the scheduler
		// not to put this process into blocked/io queue ( which is unlikely anyway )
		// or more importantly, the end of the ready queue. This stops the scheduler
		// when my time slice expires
		check_resched(sched_count);

		// allocate our page
        //struct page *allocate_pages(int zone, unsigned int *flags, unsigned long *count, unsigned long *max)
		pages[zone_current][mem_current][i] = allocate_pages(zone_current,
						&gfpFlags,
						&high_mem_count,&pagesToAllocate);

		// if our page is null, then we cannot allocate any additional
		// memory
		if (pages[zone_current][mem_current][i] == NULL)
		{
			 printk(KERN_INFO "could not allocate page %lu\n",i--);
			 break;
		}

	}

	if (zone_current == HIGH_MEM_ZONE)
	{
	
		i=high_mem_count;
	}
		if (i <= 0)
	{
			goto exitRoutine;
	}
	else if (i == (unsigned long)-1)
	{
		goto exitRoutine;
	}
	//preempt_enable();
	sprintf(nameBuffer,"%i_read",mem_current);
	allocated_pages[zone_current][mem_current] = i;
	
	if (zone_current==1)
	{
		tempEntry = create_proc_entry(nameBuffer, 0644, allocated_high_pages);
	}
	else
	{
		tempEntry = create_proc_entry(nameBuffer, 0644, allocated_low_pages);
	}
	if (tempEntry != NULL) // perform a sanity check, just in case
	{
		len = mem_current;
		tempEntry->data = data; //&page_reader_teller[zone_current][mem_current];
		tempEntry->write_proc = proc_allocated_pages_read;
		tempEntry->owner = THIS_MODULE;

	}

	sprintf(nameBuffer,"%i_write",mem_current);
	allocated_pages[zone_current][mem_current] = i;
	// create a file for our memory entry
	if (zone_current==1)
	{
		tempEntry = create_proc_entry(nameBuffer, 0644, allocated_high_pages);
	}
	else
	{
		tempEntry = create_proc_entry(nameBuffer, 0644, allocated_low_pages);
	}
	if (tempEntry != NULL) // perform a sanity check, just in case
	{
		len = mem_current;
		tempEntry->data = data; // &page_reader_teller[zone_current][mem_current];
	//	tempEntry->read_proc = proc_allocated_pages_read;
		tempEntry->write_proc = proc_allocated_pages_write;
		tempEntry->owner = THIS_MODULE;

	}



	exitRoutine:
	return len;
}
////////////////////////////////////////////////////////////////////////
/** 
 *  @fn        static int proc_allocated_pages_read(struct file *file,const char *buffer,unsigned long count, void *data)
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocated_pages_read(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data)
{
	short zone_current;
	struct page *pagePointer=NULL;
	unsigned int zone_id;
    // set length to zero
	int len = 0;

	zone_id = *(int*)data;

    // copy the user read structure into the local
    // static structure
	if(copy_from_user(&pageReader, buffer, count))
	{
		return len;
	}

    // set the current zone
	if (zone_id == ZONE_HIGHMEM)
		zone_current = HIGH_MEM_ZONE;
	else
		zone_current = LOW_MEM_ZONE;

    // verify the memory region
	if (!verify_memory_range(zone_current,
				pageReader.page_number,
				0,
				pageReader.page_file_number))
		return len;

    // grab the local memory page pointer
	pagePointer = get_mem_page(zone_current,
				pageReader.page_file_number,
				pageReader.page_number,
				0);

    // perform the specified action on the current page

    if (pagePointer == NULL)
    {
        return len;
    }

	len = page_action(zone_current,
			pageReader.page_number,
			0,
			pagePointer,
			pageReader.data,
			pageReader.data_size,
			READ_PAGE);
    
	return len;
		
}

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn        spinlock_t *get_lock(short memType)
 */
////////////////////////////////////////////////////////////////////////
spinlock_t *get_lock(short memType)
{
    switch(memType)
    {

    case HIGH_MEM_ZONE:
        return &high_mem_lock;
    case LOW_MEM_ZONE:
    default:
        return &low_mem_lock;
    };
    return NULL;
}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn        void destroy_fs(unsigned int page_file_number, struct proc_dir_entry *entry)
 */
////////////////////////////////////////////////////////////////////////

void destroy_fs(unsigned int page_file_number, struct proc_dir_entry *entry)
{
	char privateBuffer[256];
	sprintf(privateBuffer,"%i_read",page_file_number);
	remove_proc_entry(privateBuffer, entry);
	sprintf(privateBuffer,"%i_write",page_file_number);
	remove_proc_entry(privateBuffer,entry);	
}



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn        static int let_my_pages_go(struct file *file,const char *buffer,unsigned long count, void *data)
 */
////////////////////////////////////////////////////////////////////////
static int let_my_pages_go(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data)
{
	// when marc was in kernel land, let his memory gooooo
	unsigned int i=0,k=0;
	short zone_current=0;
	unsigned int zone_id=0,freeCnt = 0;

	int len = 0;

	zone_id = *(int*)data;
	
	// copy the user's page free mechanism to kernel space
	if(copy_from_user(&pageFreeMechanism, buffer, count))
		goto exitRoutine;


	zone_current = pageFreeMechanism.mem_type;
	// traverse all blocks within this zone
    
	for (i=0; i < MAX_PAGE_BLOCKS; i++)
	{
	// the number of pages to free
        freeCnt = allocated_pages[zone_current][i];

	// if we are dealing with high memory, the number of pages to free HERE
	// should be divided by the number of PAGES_PER_PAGE, and the rest will be 
	// freed in free_high_mem
        if (zone_current == HIGH_MEM_ZONE)
        {
            if ( allocated_pages[zone_current][i] > 0 &&  allocated_pages[zone_current][i] < PAGES_PER_PAGE)
                {
                    freeCnt = 1;
                }
                else
                    freeCnt/=PAGES_PER_PAGE;
        }
	
	if (freeCnt > 0 && pageFreeMechanism.block[i] == 1)
	{
        spin_lock( get_lock(zone_current) );
        if (zone_current==HIGH_MEM_ZONE)
		{
			destroy_fs(i,allocated_high_pages);
		}	
		else
		{
			destroy_fs(i,allocated_low_pages);
		}
		
		for (k=0; k < freeCnt; k++,len++)
		{
	
			if (zone_current==HIGH_MEM_ZONE)
			{
				free_high_pages(pages[zone_current][i][k]);
			}	

            
			__free_pages(pages[zone_current][i][k],0);

		}
		// set the number of allocated pages to zero, as all
		// pages should be free by now
		allocated_pages[zone_current][i]=0;
		
		vfree(pages[zone_current][i]);	
		pages[zone_current][i] = NULL;
        spin_unlock( get_lock(zone_current) );
	}

	}
	exitRoutine:
    
	return len;

}


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         static int proc_allocated_pages_write(struct file *file,const char *buffer,unsigned long count,void *data);
 */
////////////////////////////////////////////////////////////////////////
static int proc_allocated_pages_write(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data)
{
	unsigned int i;
	short zone_current;
	struct page *pagePointer=NULL;
	unsigned int zone_id,sched_count=0;
    // initialize length to zero bytes
	int len = 0;

	zone_id = *(int*)data;
	
    // copy the user page writer structe into kernel memory
	if(copy_from_user(&pageWriter, buffer, count))
		return len;

	if (zone_id == ZONE_HIGHMEM)
		zone_current = HIGH_MEM_ZONE;
	else
		zone_current = LOW_MEM_ZONE;

	
	for (i=0; i < pageWriter.pages_span; i++)
	{
        // check the scheduler to ensure that we will not be preempted;
        // however, we should not disable preemption entirely.
		check_resched(sched_count);

        // verify that the page number + i is within the valid range
		if (!verify_memory_range(zone_current,
					pageWriter.page_number,
					i,
					pageWriter.page_file_number))
			break;

        // get the current memory page pointer
		pagePointer = get_mem_page(zone_current,
					pageWriter.page_file_number,
					pageWriter.page_number,
					i);

        if (pagePointer == NULL)
        {
            break;
        }

        // perform a write on the page
        len += page_action(zone_current,
				pageWriter.page_number,
				i,
				pagePointer,
				pageWriter.data,
				pageWriter.data_size,
				WRITE_PAGE);
                
	
	}

	return len;
}


pg_data_t *get_zones(void) 
{
	struct page *pageNode= NULL;
	struct zone  *zoneList = NULL;

	// allocate page node for kernel memory
	pageNode = alloc_pages_node(0, GFP_KERNEL, 0);
	if (!pageNode)
		return NULL;
	// get the zone list for the node
	zoneList = page_zone(pageNode);
	if (!zoneList)
		return NULL;
	// free the page
    
	__free_pages(pageNode, 0);
	return zoneList->zone_pgdat;
}


static int __init init_page_alloc(void)
{
	short i=0;
	int rv = 0;
	printk(KERN_INFO "%s %s initialized\n",
               MODULE_NAME, MODULE_VERS);

    // initialize the allocated page structures
    // and nullify each element of the page controller
	for (i= 0; i < MAX_PAGE_BLOCKS; i++)
	{
		pages[0][i] = NULL;
		allocated_pages[0][i] = 0;
	}

	for (i= 0; i < MAX_PAGE_BLOCKS; i++)
	{
		pages[1][i] = NULL;
		allocated_pages[1][i] = 0;
	}
	zone_high = ZONE_HIGHMEM;
	zone_normal = ZONE_NORMAL;

    main_dir = proc_mkdir(MODULE_NAME, NULL);
    if(main_dir == NULL) {
            rv = -ENOMEM;
            goto finished;
    }

	
        
        main_dir->owner = THIS_MODULE;

	high_mem_dir = proc_mkdir(HIGH_MEM_DIR,main_dir);
	if (high_mem_dir == NULL)
	{
		rv = -ENOMEM;
		goto removeModuleOnly;
	}
       
	high_mem_dir->owner = THIS_MODULE;

	high_mem_pages_free = create_proc_entry(FREE_PAGES, 0644, high_mem_dir);
        if(	high_mem_pages_free == NULL) {
                rv = -ENOMEM;
                goto removeHighMemDir;
        }

       	high_mem_pages_free->data = &zone_high;
	high_mem_pages_free->read_proc = proc_read_free_pages;
	high_mem_pages_free->owner = THIS_MODULE;


	high_mem_pages_min = create_proc_entry(MIN_PAGES, 0644, high_mem_dir);
        if(	high_mem_pages_min == NULL) {
                rv = -ENOMEM;
                goto removeHighFreePages;
        }

	high_mem_pages_min->data = &zone_high;
	high_mem_pages_min->read_proc = proc_read_min_pages;
	high_mem_pages_min->owner = THIS_MODULE;

	
	/* NOW WE DO LOWMEM */

	low_mem_dir = proc_mkdir(LOW_MEM_DIR,main_dir);
	if (low_mem_dir == NULL)
	{
		rv = -ENOMEM;
		goto removeHighPagesMin;
	}
        
	low_mem_dir->owner = THIS_MODULE;

	low_mem_pages_free = create_proc_entry(FREE_PAGES, 0644, low_mem_dir);
        if(	low_mem_pages_free == NULL) {
                rv = -ENOMEM;
                goto removeLowMemDir;
        }

       	low_mem_pages_free->data = &zone_normal;
	low_mem_pages_free->read_proc = proc_read_free_pages;
	low_mem_pages_free->owner = THIS_MODULE;


	low_mem_pages_min = create_proc_entry(MIN_PAGES, 0644, low_mem_dir);
        if(	low_mem_pages_min == NULL) {
                rv = -ENOMEM;
                goto removeLowFreePages;
        }


	low_mem_pages_min->data = &zone_normal;
	low_mem_pages_min->read_proc = proc_read_min_pages;
	low_mem_pages_min->owner = THIS_MODULE;




	allocate_high_pages = create_proc_entry(PAGE_ALLOCATOR_NAME, 0644, high_mem_dir);
        if(allocate_high_pages == NULL) {
                rv = -ENOMEM;
                goto removeLowMinPages;
        } 
	
	allocate_high_pages->data = &zone_high;
	// read will return a list of page blocks and their sizes
	allocate_high_pages->read_proc = proc_allocate_pages;
	allocate_high_pages->write_proc = proc_allocate_pages_write;
	allocate_high_pages->owner = THIS_MODULE;



       	allocated_high_pages = proc_mkdir(ALLOCATED_PAGE_DIR, high_mem_dir);
        if(allocated_high_pages == NULL) {
                rv = -ENOMEM;
                goto removeHighAllocatePages;
        } 

	allocated_high_pages->owner = THIS_MODULE;


	allocate_low_pages = create_proc_entry(PAGE_ALLOCATOR_NAME, 0644, low_mem_dir);
        if(allocate_low_pages == NULL) {
                rv = -ENOMEM;
                goto removeHighAllocatedPages;
        } 
	
	allocate_low_pages->data = &zone_normal;
	// read will return a list of page blocks and their sizes
	allocate_low_pages->read_proc = proc_allocate_pages;
	allocate_low_pages->write_proc = proc_allocate_pages_write;
	allocate_low_pages->owner = THIS_MODULE;



	allocated_low_pages = proc_mkdir(ALLOCATED_PAGE_DIR, low_mem_dir);
        if(allocated_low_pages == NULL) {
                rv = -ENOMEM;
                goto removeLowAllocatePages;
        } 

	allocated_low_pages->owner = THIS_MODULE;

    // the free page structure allows the calling program to free
    // pages
	page_liberator_file = create_proc_entry(FREE_PAGES, 0644, main_dir);
        if(page_liberator_file == NULL) {
                rv = -ENOMEM;
                goto removeLowAllocatedPages;
        } 
	
	page_liberator_file->data = &zone_normal;
	page_liberator_file->write_proc = let_my_pages_go;
	page_liberator_file->owner = THIS_MODULE;

	
    spin_lock_init(&low_mem_lock);
    spin_lock_init(&high_mem_lock);
    printk(KERN_INFO "%s %s inita\n",MODULE_NAME, MODULE_VERS);
        return 0;

removeLowAllocatedPages:
	remove_proc_entry(ALLOCATED_PAGE_DIR,  low_mem_dir);
removeLowAllocatePages:
	remove_proc_entry(PAGE_ALLOCATOR_NAME,  low_mem_dir);
removeHighAllocatedPages:
	remove_proc_entry(ALLOCATED_PAGE_DIR,  high_mem_dir);
removeHighAllocatePages:
        remove_proc_entry(PAGE_ALLOCATOR_NAME,  high_mem_dir);
removeLowMinPages:
        remove_proc_entry(MIN_PAGES, low_mem_dir);
removeLowFreePages:
        remove_proc_entry(FREE_PAGES, low_mem_dir);
removeLowMemDir:
        remove_proc_entry(LOW_MEM_DIR, main_dir);
removeHighPagesMin:
        remove_proc_entry(MIN_PAGES, high_mem_dir);	
removeHighFreePages:
        remove_proc_entry(FREE_PAGES, high_mem_dir);
removeHighMemDir:
        remove_proc_entry(HIGH_MEM_DIR, main_dir);
removeModuleOnly:                           
        remove_proc_entry(MODULE_NAME, NULL);
finished:
        return rv;
}


static void __exit cleanup_page_alloc(void)
{
	short i=0,j=0;
	unsigned int k=0;
    unsigned int freeCnt = 0;

    // traverse the two zones ( low and high mem )
    // then de-allocate currently allocated pages if any exist
	for (i= 0; i < 2; i++)
	{
		for (j=0; j < MAX_PAGE_BLOCKS; j++)
		{
            freeCnt = allocated_pages[i][j];
            if (i == HIGH_MEM_ZONE)
            {
                // check if pages are allocated in high mem
                if ( allocated_pages[i][j] > 0 &&  allocated_pages[i][j] < PAGES_PER_PAGE)
                {
                    freeCnt = 1;
                }
                else
                    freeCnt/=PAGES_PER_PAGE; 
                

            }

			if (freeCnt > 0)
			{
                // destroy the file system entries
                if (i==HIGH_MEM_ZONE)
					{
						destroy_fs(j,allocated_high_pages);
					}	
					else
					{
						destroy_fs(j,allocated_low_pages);
					}
				for (k=0; k < freeCnt; k++)
				{

                    // if dealing with high memory, then we
                    // must remove those pages as well
					if (i==HIGH_MEM_ZONE)
					{
						free_high_pages(pages[i][j][k]);
					}	
                    
					__free_pages(pages[i][j][k],0);
                    pages[i][j][k] = NULL;


				}
				vfree(pages[i][j]);
				pages[i][j] = NULL;

			}

		}
	}


        // remove the remaining proc entries

        remove_proc_entry(MIN_PAGES, low_mem_dir);
        
        remove_proc_entry(FREE_PAGES, low_mem_dir);
        
        remove_proc_entry(MIN_PAGES, high_mem_dir);
        
        remove_proc_entry(PAGE_ALLOCATOR_NAME, high_mem_dir);
        
        remove_proc_entry(PAGE_ALLOCATOR_NAME, low_mem_dir);
    
        remove_proc_entry(ALLOCATED_PAGE_DIR, high_mem_dir);
        
        remove_proc_entry(ALLOCATED_PAGE_DIR, low_mem_dir);
    
        remove_proc_entry(FREE_PAGES, high_mem_dir);
        
        remove_proc_entry(LOW_MEM_DIR, main_dir);
        
        remove_proc_entry(HIGH_MEM_DIR, main_dir);
        
        remove_proc_entry(FREE_PAGES, main_dir);
        
        remove_proc_entry(MODULE_NAME, NULL);

        printk(KERN_INFO "%s %s removed\n",
               MODULE_NAME, MODULE_VERS);
}


module_init(init_page_alloc);
module_exit(cleanup_page_alloc);

