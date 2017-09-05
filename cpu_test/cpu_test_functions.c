
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       cpu_test_functions.c
 *
 *  @brief      Test functions to support the cputest execution
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

// local includes
#include "cpu_test_functions.h"


/////////////////////////////////////////////////////////////////////////////
/*
 *  typedef struct
 *  {
 *      // Information Gathered from the Board
 *      //
 *      unsigned long cpu_speed;    //  CPU Speed in MHz
 *      char cpu_id_string[64];     //  Storage of CPU Identification String
 *      unsigned long l2_cache;     //  L2 Cache size in KB
 *      unsigned long l3_cache;     //  L3 Cache size in KB
 *      unsigned long apic_id;      //  CPU Local APIC ID  
 *
 *      // Information Specifed by the command line
 *      //
 *      unsigned long test_speed;   //  CPU Speed in MHz to test against
 *      unsigned long test_l2;      //  Size of L2 Cache in KB to test against
 *      unsigned long test_l3;      //  Size of L3 Cache in KB to test against
 *
 *  }CPU_INFO, *CPU_INFO;
 *
 */
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void get_cpu_structure(PCPU_INFO cpu_info_pointer)
 */
//////////////////////////////////////////////////////////////////////////////
void get_cpu_structure(PCPU_INFO cpu_info_pointer)
{
    iopl(3);

    //  Load detected settings
    //////////////////////////////////////////////////////////////////////
    cpu_info_pointer->cpu_speed = get_cpu_speed();

    /** @todo      Why do I have to store the results of get_l2_cache(); 
     *             in an intermediate variable?? 
     */
    unsigned long l2_cache  = get_l2_cache();
    unsigned long l3_cache  = get_l3_cache();
    cpu_info_pointer->l2_cache = l2_cache;
    cpu_info_pointer->l3_cache = l3_cache;

    cpu_info_pointer->apic_id   = get_local_apic_id();


    //must pass a pointer to the string you want to load
    char cpu_id_string[64];

    //set a base value of 0x20 to ease in parsing
    memset(cpu_id_string,0x20,sizeof(cpu_id_string));
    get_cpu_id_string(cpu_id_string,sizeof(cpu_id_string));

    strcpy(cpu_info_pointer->cpu_id_string,cpu_id_string);
}


//////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void print_cpu_structure(PCPU_INFO cpu_info_pointer, unsigned int size)
 */
//////////////////////////////////////////////////////////////////////////////
void print_cpu_structure(PCPU_INFO cpu_info_pointer)
{
    printf("CPU Name: \t%s\n",cpu_info_pointer->cpu_id_string);
    printf("CPU Speed: \t%u MHz\n",cpu_info_pointer->cpu_speed);
    printf("APIC ID: \t%u\n",cpu_info_pointer->apic_id);
    printf("L2 Cache: \t%u KB\n",cpu_info_pointer->l2_cache);
    printf("L3 Cache: \t%u KB\n",cpu_info_pointer->l3_cache);

 }

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int test_cpu_speed(PCPU_INFO cpu_info_pointer);
 */
/////////////////////////////////////////////////////////////////////////////
int set_cpu_test_speed(PCPU_INFO cpu_info_pointer, unsigned long cpu_speed)
{
    cpu_info_pointer->test_speed = cpu_speed;
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int set_l2_cache_test_size(PCPU_INFO cpu_info_pointer, unsigned long l2_cache_size)
 */
/////////////////////////////////////////////////////////////////////////////
int set_l2_test_size(PCPU_INFO cpu_info_pointer, unsigned long l2_cache_size)
{
    cpu_info_pointer->test_l2 = l2_cache_size;
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int set_l3_cache_test_size(PCPU_INFO cpu_info_pointer, unsigned long l3_cache_size)
 */
/////////////////////////////////////////////////////////////////////////////
int set_l3_test_size(PCPU_INFO cpu_info_pointer, unsigned long l3_cache_size)
{
    cpu_info_pointer->test_l3 = l3_cache_size;
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int test_cpu_speed(PCPU_INFO cpu_info_pointer, int cpu_number);
 */
/////////////////////////////////////////////////////////////////////////////
int test_cpu_speed(PCPU_INFO cpu_info_pointer, int cpu_number)
{

    if (cpu_info_pointer->test_speed == 0)     //no cpu speed  given at runtime; requires user interaction
    {
        char question_buffer[32];
        memset(question_buffer,0x00,sizeof(question_buffer));

        
        // Loading my question buffercus with a formatted string:
        // the non-spacing between string and speed is due to the cpu_id_string having a space at the end. 
        sprintf(question_buffer,"%s%uMHz?", cpu_info_pointer->cpu_id_string, cpu_info_pointer->cpu_speed);

        if (askYesNoQuestion(&question_buffer))
        {
            //testPrint("CPU %u: Speed Test",cpu_number);
            return PASS;
        }
        else
        {
            testPrint("CPU %u: Speed Test",cpu_number);
            return FAIL;
        }
    }
    else if (cpu_info_pointer->test_speed == cpu_info_pointer->cpu_speed)
    {
        //testPrint("CPU %u: Speed Test",cpu_number);
        return PASS;
    }
    else    //cpu must be bad
    {
        testPrint("CPU %u: Speed Test",cpu_number);
        return FAIL;
    }

}


/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int test_l2_cache(PCPU_INFO cpu_info_pointer, int cpu_number);
 */
/////////////////////////////////////////////////////////////////////////////
int test_l2_cache(PCPU_INFO cpu_info_pointer, int cpu_number)
{
    if (cpu_info_pointer->test_l2 == cpu_info_pointer->l2_cache)    // Cache size passed at runtime
    {
        //testPrint("CPU %u: L2 Cache Test", cpu_number);
        return PASS;
    }
    else if (cpu_info_pointer->test_l2 == 0)                        // No cache size given prompt
    {
        char question_buffer[32];
        memset(question_buffer,0x00,sizeof(question_buffer));

        //Loading my question buffercus with a formatted string
        sprintf(question_buffer, "L2 Cache Size %u?", cpu_info_pointer->l2_cache);

        if (askYesNoQuestion(&question_buffer))
        {
            //testPrint("CPU %u: L2 Cache Test", cpu_number);
            return PASS;
        }
        else
        {
            testPrint("CPU %u: L2 Cache Test", cpu_number);
            return FAIL;
        }

    }
    else    //wrong cache size
    {
        testPrint("CPU %u: L2 Cache Test", cpu_number);
        return FAIL;
    }
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int test_l3_cache(PCPU_INFO cpu_info_pointer, int cpu_number);
 */
/////////////////////////////////////////////////////////////////////////////
int test_l3_cache(PCPU_INFO cpu_info_pointer, int cpu_number)
{
    if (cpu_info_pointer->test_l3 == cpu_info_pointer->l3_cache)  // Cache size passed at runtime
    {
        //testPrint("CPU %u: L3 Cache Test",cpu_number);
        return PASS;
    }
    else if (cpu_info_pointer->test_l3 == 0)
    {
        char question_buffer[32];
        memset(question_buffer,0x00,sizeof(question_buffer));

        //Loading my question buffercus with a formatted string
        sprintf(question_buffer, "L3 Cache Size %u?", cpu_info_pointer->l3_cache);

        if (askYesNoQuestion(&question_buffer))
        {
            //testPrint("CPU %u: L3 Cache Test",cpu_number);
            return PASS;
        }
        else
        {
            testPrint("CPU %u: L3 Cache Test",cpu_number);
            return FAIL;
        }
    }
    else    //wrong cache size
    {
        testPrint("CPU %u: L3 Cache Test", cpu_number);
        return FAIL;
    }
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int test_cpu_count(int cpu_count, int htt_enabled);
 */
/////////////////////////////////////////////////////////////////////////////
int test_cpu_count(int cpu_count, int htt_enabled)
{
    unsigned int number_of_cpus = get_processor_count();

    if(htt_enabled)
    {
        if(number_of_cpus == (cpu_count*2))
            return PASS;
        else
            return FAIL;
    }
    else
    {
        if(number_of_cpus == cpu_count)
            return PASS;
        else
            return FAIL;
    }

}
