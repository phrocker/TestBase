///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       memoryTest.c
 *
 *  @brief      Drivers for the page allocation kernel module
 *
 *              Copyright (C) 2006 @n@n
 *	            Utilizes page_allocator to allocate as many pages as the system will allow
 *		        Allocates all low memory then frees it, afterwhich the same is done with
 *		        all available high memory
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
#include "pageAllocator.h"
#include "page_allocator_defs.h"
#include "argtable2.h"
#include "../CommonLibrary/Common.h"
#include "memoryTest.h"
#include <signal.h>



#define MYVERSION 0.02

int handle_signals(int signal);

int main(int argc, char *argv[])
{
    struct arg_lit *help,*debug;
    struct arg_int *passes;
    unsigned int memoryPasses=0;
    struct arg_end *end;

    unsigned short local_debug=FALSE;

    setTestVersion(MYVERSION);


    // create argument table
     void *argtable[] = {
         passes      = arg_int0("p","passes","[# passes]","Number of passes on each page."),
         arg_rem(NULL,"If not set, only one pass will be performed"),
         arg_rem(NULL,"If set to -1, memory will be tested indefinitely"),
         arg_rem(NULL,""),
         debug = arg_lit0("d","debug","Displays debug information."),
         help        = arg_lit0("h","help","Displays usage information"),
         end         = arg_end(20)
    };
    
    // check argtable to make sure it's not null
    if (arg_nullcheck(argtable) != 0)
    {
        consolePrint("ERROR! Insufficient memory\n");
        exit(1);
    }

    // parse arguments
    arg_parse(argc,argv,argtable);

    if (help->count > 0)
    {
        displayUsage(argtable,"memoryTest",MYVERSION);
    }

    if (debug->count > 0)
    {
        local_debug=TRUE;
    }

    if (passes->count > 0)
    {
        // since memoryPasses is unsigned, if passes is negative
        // memoryPasses will be a max unsigned val, ( 4 Billion + on 32 bit sys ).
        // therefore, we will have a seemingly infinite number of tests
        memoryPasses = passes->ival[0];
        consolePrint("Performing %u passes and allocations\n",memoryPasses);
        
    }


	unsigned int failures=0;
    signal(SIGINT, (void*)handle_signals);
	signal(SIGQUIT, (void*)handle_signals);
    signal(SIGHUP,(void*)handle_signals);
    signal(SIGTERM,(void*)handle_signals);

    if (memoryPasses > 0)
    {
        unsigned int i=0;
        
        short j=0;
        testPrint("Beginning memory tests\n");

        failures = burninMemTest(memoryPasses,local_debug);
        
        testPrintUpdate("Burnin Memory Test");
         if (failures > 0)
            failedMessage();
        else
            passedMessage();
        
    }
    else
    {
   
    	unsigned long bytesTested =testMemory(LOW_MEM,&failures,local_debug);
    	int lowMemMB = (bytesTested/(1024*1024))+128;
        //diagnosticPrint("%i MB of low mem allocated\n",lowMemMB);
    	bytesTested =testMemory(HIGH_MEM,&failures,local_debug);
        
    	int highMemMB = (bytesTested/(1024*1024));
    
        //diagnosticPrint("%i MB of high mem allocated\n",highMemMB);
    	unsigned int totalMem = highMemMB + lowMemMB;
        char *size = "MB";
    	if (totalMem>1000)
        {
            size = "GB";
    		totalMem /= 1000;
        }
    
    	testPrint("%lu %s Memory Test",totalMem,size);
        if (failures > 0)
            failedMessage();
        else
            passedMessage();
    }

    
	return 0;

}

unsigned long burninMemTest(unsigned int passes, unsigned short debug)
{
    char j=0,test[PAGE_SIZE], data[PAGE_SIZE];
    char testChar = 'a';
	unsigned long totalSize=0;
    unsigned int  i=0;
    unsigned long failures=0;
    int previousP = 0;
    char buffer[256];
    char t[256];
    int d=35;
    int bars =0;
	memset(data,'X',PAGE_SIZE);

    memset(buffer,0,256);
    for (j=0; j< d; j++)
     {
         strcat(buffer,"-");
     }
    sprintf(t," %i%%",previousP);
    
    strcat(buffer,t);
    testPrintUpdate(buffer);  
    for (i=0; i < passes; i++)
        {
             if ( (((float)(i+1)/(float)passes)*100 ) != previousP)
             {
                 testMemory(LOW_MEM,&failures,debug);
                 testMemory(HIGH_MEM,&failures,debug);
                 previousP = ((float)(i+1)/(float)passes)*100;
                 bars = ((float)(i+1)/(float)passes)*d;
                 memset(buffer,0,256);
                 for (j=0; j< bars; j++)
                 {
                     strcat(buffer,"|");
                 }
                 for (j=0; j< d-bars; j++)
                 {
                     strcat(buffer,"-");
                 }
                 sprintf(t," %i%%",previousP);
                 
                 strcat(buffer,t);
                 testPrintUpdate(buffer);   
             }

             

        }
return failures;

}

unsigned long testMemory(short memType,unsigned int *failures, unsigned short debug)
{
    if (available_pages(memType) == 0)
    {
        return 0;
    }
	char j,test[PAGE_SIZE], data[PAGE_SIZE];
    // if we are dealing with high memory, we split the memory
    // between 
	short maxCount = (memType == HIGH_MEM) ? 2 : 31; // should be 2 for high mem
    short dividor =  (memType == HIGH_MEM) ? 1 : 31,nomem = FALSE;
    //available_pages(memType)/dividor
	unsigned int pages_to_allocate= available_pages(memType)/dividor,pagesAllocated=0,block,wrote;
	char testChar = 'a';
	unsigned long totalSize=0;
    unsigned int  i=0;
	memset(data,0,PAGE_SIZE);
	for (j=0; j < maxCount; j++)
	{
        if (available_pages(memType) == 0)
        {
            nomem = TRUE;
            break;
        }
        
		memset(test,testChar++,PAGE_SIZE);
		block = allocate_pages(pages_to_allocate,memType);
        
		pagesAllocated = block_size(memType,block);
        
		totalSize+=(pagesAllocated*PAGE_SIZE);
        
		wrote = write_to_page(memType,test,PAGE_SIZE,block,0,pagesAllocated);
        debugPrint(debug,"Wrote %u bytes, across %u pages\n",wrote,pagesAllocated);
        int read=0;
		for (i=0; i < pagesAllocated; i++)
		{
			read = read_page(memType,data,block,i,0);
            
			if (memcmp(data,test,sizeof(test)))
            {
                *failures=*failures+1;
                debugPrint(debug,"Blocks to not match\n");
            }
				
			
		}

	}
    
    if (nomem  == FALSE)
    {
    	memset(test,testChar++,PAGE_SIZE);
        pages_to_allocate  = available_pages(memType);
    	block = allocate_pages(pages_to_allocate,memType);
        
    	pagesAllocated = block_size(memType,block);
        debugPrint(debug,"%u Pages Allocated; %u requested\n",pagesAllocated,pages_to_allocate);
    	totalSize+=(pagesAllocated*PAGE_SIZE);
    	wrote = write_to_page(memType,test,PAGE_SIZE,block,0,pagesAllocated);
    	
    	for (i=0; i < pagesAllocated; i++)
    	{
    		int read = read_page(memType,data,block,i,0);
    	
    		if (memcmp(data,test,sizeof(test)))
    			failures++;
    		
    	}
        j++;
    }
    else
    {
        j--;
    }
    

    char blocks[MAX_PAGE_BLOCKS];
	for (i=0; i < j; i++)	
	{
		blocks[i] = 1;
	}
	free_memory(memType,blocks);
    
	return totalSize;
}


int handle_signals(int signal)
{
    char blocks[MAX_PAGE_BLOCKS];
    short i;
	for (i=0; i < MAX_PAGE_BLOCKS; i++)	
	{
		blocks[i] = 1;
	}
	free_memory(LOW_MEM,blocks);
    free_memory(HIGH_MEM,blocks);
    consolePrint("\n");
    exit(0);
}

