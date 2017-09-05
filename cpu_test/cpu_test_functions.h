
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       cpu_test_functions.h
 *
 *  @brief      Function Definitions / Documentation of cpu_test_functions.c
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


#ifndef PASS
#define PASS    1
#endif  

#ifndef FAIL
#define FAIL    0
#endif  



/////////////////////////////////////////////////////////////////////////////
/** 
 *  @stuct      CPU_INFO
 *  
 *  @property   <b>unsigned long</b> cpu_speed    
 *                  -CPU Speed in MHz
 *  @property   <b>char</b> cpu_id_string[64]     
 *                  -Storage of CPU Identification String
 *  @property   <b>unsigned</b> long l2_cache     
 *                  -L2 Cache size in KB
 *  @property   <b>unsigned</b> long l3_cache     
 *                  -L3 Cache size in KB
 *  @property   <b>unsigned</b> long apic_id      
 *                  -CPU Local APIC ID
 *  @property   <b>unsigned</b> long test_speed   
 *                  -CPU Speed in MHz to test against
 *  @property   <b>unsigned</b> long test_l2      
 *                  -Size of L2 Cache in KB to test against
 *  @property   <b>unsigned</b> long test_l3      
 *                  -Size of L3 Cache in KB to test against
 *
 *
 *  @brief      A structure that holds all test data for the CPU Test
 *
 *              This structure is a definition of the CPU it represents.  The
 *              information held is both calculted and the values to test
 *              against. This structure should be instantiated for each cpu 
 *              individually.
 *
 */
//////////////////////////////////////////////////////////////////////////////
typedef struct
{
    // Information Gathered from the Board
    //
    unsigned long cpu_speed;    //  CPU Speed in MHz
    char cpu_id_string[64];     //  Storage of CPU Identification String
    unsigned long l2_cache;     //  L2 Cache size in KB
    unsigned long l3_cache;     //  L3 Cache size in KB
    unsigned long apic_id;      //  CPU Local APIC ID


    // Information Specifed by the command line
    //
    unsigned long test_speed;   //  CPU Speed in MHz to test against
    unsigned long test_l2;      //  Size of L2 Cache in KB to test against
    unsigned long test_l3;      //  Size of L3 Cache in KB to test against

}CPU_INFO, *PCPU_INFO;

///////////////////////////////////////////////////////////////////////////
/**
 *     @fn         void get_cpu_info(PCPU_INFO cpu_info_pointer);
 *
 *     @arg         <b>PCPU_INFO</b> cpu_info_pointer@n
 *                      - A pointer to a CPU_INFO structure
 * 
 *     @brief       This function fills a CPU_INFO structure with data.
 *
 *
 */
///////////////////////////////////////////////////////////////////////////
void get_cpu_structure(PCPU_INFO cpu_info_pointer);

//////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void print_cpu_structure(PCPU_INFO cpu_info_pointer, unsigned int size)
 *
 *  @arg        <b>PCPU_IFNO</b> cpu_info_pointer
 *                  - A pointer to the stucture you wish to print
 *
 *
 *  @brief      This function dumps the contents of the available cpu structrues
 *              to the screen.
 *
 *              This function takes the pointer to your structure and prints out
 *              critical data in an easy to read format.  Useful for debug.
 *
 */
//////////////////////////////////////////////////////////////////////////////
void print_cpu_structure(PCPU_INFO cpu_info_pointer);

//////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int test_cpu_speed(PCPU_INFO cpu_info_pointer);
 *
 *  @arg        <b>PCPU_IFNO</b> cpu_info_pointer
 *                  - A pointer to the stucture you wish to verify speed
 *
 *  @return     Returns an integer specifiying PASS(1) or FAIL(0)
 *
 *  @brief      This function verifies the CPU speed if entered at test start
 *              or prompts the user for interaction if no speed was given
 *
 *              This function is to be executed after a valid CPU_INFO structure
 *              has been instantiated and loaded by the get_cpu_structure() function.
 *              When executed, this function compares the profiled speed with the
 *              desired test speed passed at cputest runtime (If no value is given,
 *              the test assumes a speed of 0).  Upon matching the test speed and
 *              the profiled speed, the function returns a 1; otherwise, a 0 is
 *              returned
 *
 */
///////////////////////////////////////////////////////////////////////////////
int test_cpu_speed(PCPU_INFO cpu_info_pointer, int cpu_number);

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int test_l2_cache(PCPU_INFO cpu_info_pointer);
 *  
 *  @brief      This function verifies the L2 Cache size
 *
 *  @return     PASS(1) or FAIL(0)
 *
 *              This function is to be executed after a valid CPU_INFO structure
 *              has been instantiated and loaded by the get_cpu_structure() function.
 *              When executed, this function compares the profiled cache size with 
 *              the desired size passed to cputest at runtime (If no value is 
 *              specified, the test size is assumed 0).  Upon matching the 
 *              test size and the profiled size, the function returns a 1. A 0 is 
 *              returned for no match.
 */
/////////////////////////////////////////////////////////////////////////////
int test_l2_cache(PCPU_INFO cpu_info_pointer, int cpu_number);

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int test_l3_cache(PCPU_INFO cpu_info_pointer, int cpu_number);
 *
 *  @brief      This function verifies the L3 Cache size.
 *
 *  @return     PASS(1) or FAIL(0)
 *
 *              This function is to be executed after a valid CPU_INFO structure
 *              has been instantiated and loaded by the get_cpu_structure() function.
 *              When executed, this function compares the profiled cache size with 
 *              the desired size passed to cputest at runtime (If no value is 
 *              specified, the test size is assumed 0).  Upon matching the 
 *              test size and the profiled size, the function returns a 1. A 0 is 
 *              returned for no match.
 */
/////////////////////////////////////////////////////////////////////////////
int test_l3_cache(PCPU_INFO cpu_info_pointer, int cpu_number);


/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int test_cpu_count(int cpu_count, int htt_enabled);
 *
 *  @brief      Verify that there are the proper number of CPUs installed
 */
/////////////////////////////////////////////////////////////////////////////
int test_cpu_count(int cpu_count, int htt_enabled);
