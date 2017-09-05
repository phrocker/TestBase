
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       cpu_test.c
 *
 *  @brief      Functional Test of Intel CPUs
 *
 *  @return     an integer specifying PASS(1) or FAIL(0)
 *
 *              Copyright (C) 2006 @n@n
 *              This program is intended to be a full functional test of 
 *              Intel Processors.  There are heavy dependcies on the cpuid
 *              section of the TBS Common Library, so be sure you know what
 *              you're doing with this low-level interfaces.
 *  
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
 
#include "argtable2.h"
#include "cpu_test_functions.h"

#include "../CommonLibrary/cpuid.h"
#include "../CommonLibrary/Debug.h"
#include "../CommonLibrary/printHeader.h"
#include "../CommonLibrary/Common.h"


//  Linux File Control, Error, and Standard in/out headers 
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MYVERSION 0.01

// define the usage function
void usage(void *argtable[]);

int main(int argc, char *argv[])
{
    // Trigger values
    unsigned int Verbose     = 0;
    unsigned int SkipL2      = 0;
    unsigned int SkipL3      = 0;
    unsigned int htt_enabled = 0;
    

    //values that we want to be tested
    unsigned long test_speed        = 0;
    unsigned long test_l2_size      = 0;
    unsigned long test_l3_size      = 0;
    unsigned long test_cpu_number   = 0;

    struct arg_lit *debug,*help,*skipL2,*skipL3,*htt;
    struct arg_int *L2Size,*L3Size,*speed, *cpus;
    struct arg_str *retryarg;
    struct arg_end *end;
    
	setTestVersion(MYVERSION);

    // create argument table
     void *argtable[] = {
         speed       = arg_int0("s","speed","[speed]","Specify cpu speed in MHz."),
         L2Size      = arg_int0(NULL,"L2","[L2 size]","Specify L2 cache size in KB."),
         L3Size      = arg_int0(NULL,"L3","[L3 size]","Specify L3 cache size in KB."),
         skipL2      = arg_lit0(NULL,"SKIP_L2,skip_l2","Skip verifiying L2 Cache."),
         skipL3      = arg_lit0(NULL,"SKIP_L3,skip_l3","Skip verifiying L3 Cache."),
         debug       = arg_lit0(NULL,"debug","Displays debug information."),
         cpus        = arg_int0("c","cpus","[cpus]","Specify total CPUs."),
         htt         = arg_lit0(NULL,"htt","Enable HyperThreading support (Requred of HTT enabled boards)"),
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
        usage(argtable);
    }
    if(skipL2->count > 0)
    {
 		SkipL2 = 1;
    }
    if(skipL3->count > 0)
    {
        SkipL3 = 1;
    }
    if(speed->count > 0)
    {
        test_speed = speed->ival[0];
    }
    if(L2Size->count >0)
    {   
        test_l2_size = L2Size->ival[0];
    }
    if(L3Size->count > 0)
    {
        test_l3_size = L3Size->ival[0];
    }
    if(cpus->count > 0)
    {
        test_cpu_number = cpus->ival[0];
    }
    if(htt->count > 0)
    {
        htt_enabled = 1;
    }
    if (debug->count > 0)
        Verbose=1;

    

    //senteniel value
    int i,j;

    int success=PASS;

    // Construct the CPU affinity mask so that we don't have global variables
    unsigned long affinity_mask[32];
    for (i=0; i<32; i++) 
    {
        affinity_mask[i] = AFFINITY_MASK << i;
    }

    unsigned int number_of_cpus = get_processor_count();
    
    // Just some Forking variables
    int new_pid;
    int child_status;

    // Affinity error Checking
    int affinity_result;
    int affinity_mask_length = sizeof(affinity_mask[0]);



    // An array of structures... one for each processor.
    CPU_INFO cpu[number_of_cpus];


    char info_message[64];
    memset(info_message,0x00,sizeof(info_message));

    //informational message specifying delay expected.
    sprintf(info_message,"Executing CPU/Cache Tests: %us delay",(number_of_cpus*3));
    testPrint(info_message);
    customMessage(EMPTYSTRING);

    //populate CPU Structure
    for (i=0; i < number_of_cpus; i++)
    {
        get_cpu_structure(&cpu[i]);
    }

    for (i=0; i < number_of_cpus; i++)
    {
        new_pid = fork();
        waitpid(0,&child_status,0);     //wait for and ensure that my child is actually the one exiting...

        if(new_pid == 0) //child process
        {
            if ((affinity_result = sched_setaffinity(new_pid,affinity_mask_length,&affinity_mask[i])) == 0) 
            {

                //load in test values
                set_cpu_test_speed(&cpu[i], test_speed);
                set_l2_test_size(&cpu[i], test_l2_size);
                set_l3_test_size(&cpu[i], test_l3_size);


                if(test_cpu_speed(&cpu[i],i))
                {   
                    //  I moved cpu speed test identifier into the cpu test to hide it in case
                    //  where the user is prompted for the speed.
                    //passedMessage();
                }
                else
                {
                    success=FAIL;
                    failedMessage();
                }


                //L2 Cache check
                if (!SkipL2)
                {
                    if (test_l2_cache(&cpu[i],i)) 
                    {
                        //  I moved L2 size test identifier into the cpu test to hide it in case
                        //  where the user is prompted for the speed.
                        //passedMessage();
                    }
                    else
                    {
                        success=FAIL;
                        failedMessage();
                    }
                }

                //L3 Cache check
                if (!SkipL3)
                {
                    if (test_l3_cache(&cpu[i],i))
                    {                     
                        //  I moved L2 size test identifier into the cpu test to hide it in case
                        //  where the user is prompted for the speed.
                        //passedMessage();
                    }
                    else
                    {
                        success=FAIL;
                        failedMessage();
                    }
                }

                //specify our return status
                if(success)
                {
                    // alter child return status in such a way that a failed test
                    // is detectable without having to use pipes
                    return 0;
                }
                else
                {
                    // alter child return status in such a way that a failed test
                    // is detectable without having to use pipes
                    exit (-1);
                }
               
            }
            else
            {
                perror("sched_setaffinity");
            }
        }

        // any failure is detected
        if (child_status!=0)
        {
            success=FAIL;
        }

    }

    //Pretty up test printout but leave debug data going to console
    if (success)
    {
        testPrint("%u CPU Speed Test(s):",number_of_cpus);
        passedMessage();
        testPrint("%u CPU L2 Cache Test(s):",number_of_cpus);
        passedMessage();
        testPrint("%u CPU L3 Cache Test(s):",number_of_cpus);
        passedMessage();

    }

    if(test_cpu_count(test_cpu_number,htt_enabled))
    {
        testPrint("CPU Count Test: ");
        passedMessage();
        
    }
    else
    {
        testPrint("CPU Count Test: ");
        failedMessage();
        success = FAIL;
    }

    if(success)
        return PASS;
    else
        return FAIL;
}


/////////////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void usage(void *argtable[]);
 *
 *  @brief      Nice output for test use help
 */
////////////////////////////////////////////////////////////////////////////////////
void usage(void *argtable[])
{
	testPrint("CPU Test");
	failedMessage();
	consolePrint("\n%s, Version %.2f\n", TBSCOPYRIGHT);    
	consolePrint("Identified the processor and verifies speed and cache size.\n");
    arg_print_glossary(stdout,argtable,"  %-25s %s\n");
    consolePrint("Test Version: %.2f\n",MYVERSION);
	exit(1);
}
