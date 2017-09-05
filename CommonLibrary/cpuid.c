///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       cpuid.c
 *
 *  @brief      This program is intended to be a full featured CPUID Interface
 *
 *              Copyright (C) 2006 @n@n
 *              This program is a fully featured CPUID interface package that
 *              is designed to aid in board/cpu profiling.  It is included in
 *              the TBS Linux Test Platform Common Library and can be accessed
 *              as such.  
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
#include "cpuid.h"


//  Linux File Control, Error, and Standard in/out headers 
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sched.h>



////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void profile_cpu(void);
 */
////////////////////////////////////////////////////////////////////////////
int profile_cpu (void)
{
    iopl(3);
    print_header();
    
    //sentinel value
    int i;

    // affinity variables
    int affinity_result;
    unsigned long affinity_mask[32];

    for (i=0; i < 32; i++)
    {
        affinity_mask[i] = AFFINITY_MASK << i;
    }

    int affinity_mask_write_length = sizeof(affinity_mask[0]);

    // process id variables
    int new_pid;
    int child_status;

   unsigned int cpu_count = get_processor_count();
  
    for (i=0; i < cpu_count; i++) 
    {
        new_pid = fork();
        waitpid(0,&child_status,0);     //wait for and ensure that my child is actually the one exiting...

        if(new_pid == 0) //child process
        {
            if ((affinity_result = sched_setaffinity(new_pid,affinity_mask_write_length,&affinity_mask[i])) == 0) 
            {
                show_cpuid_status();
                return 1;               //force the child to close out properly
            }
            else
            {
                perror("sched_setaffinity");
            }

        }
        
    }
    
    return 0;
}


////////////////////////////////////////////////////////////////////////
/*
 *  @fn    int check_cpuid(void);
 */ 
////////////////////////////////////////////////////////////////////////
int check_for_cpuid(void)
{
    unsigned long eflags_storage = 0x00;
    unsigned long eflags_return = 0x00;

    asm(
        // Code Shamelesly coppied from the web address:
        // http://gcc.gnu.org/onlinedocs/gcc-3.3.6/gnat_ug_unx,Intel_005fCPU-Package-Body.html
            
        "pushfl\n\t"               //push EFLAGS on stack
        "pop %%eax\n\t"            //pop EFLAGS into eax
        "movl %%eax, %%ebx\n\t"    //save EFLAGS content
        "xor $0x200000, %%eax\n\t" //flip ID flag
        "push %%eax\n\t"           //push EFLAGS on stack
        "popfl\n\t"                //load EFLAGS register
        "pushfl\n\t"               //push EFLAGS on stack
        "pop %%ecx\n\t"            //save EFLAGS content
        : "=b"(eflags_storage), "=c"(eflags_return)
        );

    if (eflags_storage != eflags_return) 
    {
        return TRUE;
    }
    else
    {
        // ID bit was unchanged...  therefore, CPUID is NOT Available
        //
        // This is a very old processor, WHY ARE WE USING IT??
        printf("\n\t ***CPUID is not available...  This processor is old as dirt, or we have serious problems*** \n\n");
        return FALSE;
    }
    
}


////////////////////////////////////////////////////////////////////////
/*
 *  @fn    int check_extended_cpuid(void);
 */ 
////////////////////////////////////////////////////////////////////////
int check_for_extended_cpuid(void)
{
    if (check_for_genuine_intel())
    {
        // Send this command to find what all 
        unsigned long extended_cpuid_query = 0x80000000;
    
        // Storage for the Maximum Input value that specifies the highest command that
        // can be sent to this version of CPUID.  Initializing to 0x00 for detection;
        unsigned long extended_cpuid_MIV = 0x00;
    
        // if we have a MIV of at least this value, we can query brand ID string
        unsigned long brand_string_avaiable = 0x80000004;

        asm( "cpuid" : "=a"(extended_cpuid_MIV): "a"(extended_cpuid_query));
    
    
        if(extended_cpuid_MIV == 0x00)
        {
            printf("/n/t ***CPUID command didn't return properly and did not update MIV!!! /n/n");
            return FALSE;
        }
        else if (extended_cpuid_MIV >= brand_string_avaiable)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
        return FALSE;
    
}


////////////////////////////////////////////////////////////////////////
/*
 *  @fn    int check_for_genuine_intel(void);
 */
////////////////////////////////////////////////////////////////////////
int check_for_genuine_intel(void)
{

    if(check_for_cpuid())
    {
    
        // value to place in the eax register on issuing the cpuid command so that we can look for the 
        // "GenuineIntel" string.  Also, we will get the Maximum Input Value for Basic CPUID Information
        // 
        // I should be able to check MIV to see if there's extra info supported by the processor.  If so,
        // I'll need to re-evaluate this program.
        //
        unsigned long cpuid_eax_command = 0x00;
    
        unsigned long cpuid_eax_return = 0x00;
        unsigned long cpuid_ebx_return = 0x00;
        unsigned long cpuid_ecx_return = 0x00;
        unsigned long cpuid_edx_return = 0x00;
    
        unsigned long cpuid_ebx_compare = 0x756E6547;   // " u n e G "  -|
        unsigned long cpuid_ecx_compare = 0x6C65746E;   // " l e t n "   |-> GenuineIntel (ebx, edx, ecx) 
        unsigned long cpuid_edx_compare = 0x49656E69;   // " I e n i "  -|
    
        asm ( "cpuid" : "=a" (cpuid_eax_return), "=b" (cpuid_ebx_return), "=c" (cpuid_ecx_return), "=d" (cpuid_edx_return) : "a" (cpuid_eax_command)); 
    
        if ((cpuid_ebx_return == cpuid_ebx_compare)&&(cpuid_ecx_return == cpuid_ecx_compare)&&(cpuid_edx_return == cpuid_edx_compare))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
        return FALSE;

}

////////////////////////////////////////////////////////////////////////
/*
 *  @fn    int check_for_brand_id(void);
 */ 
////////////////////////////////////////////////////////////////////////
int check_for_brand_id(void)
{
    if(check_for_genuine_intel())
    {
        unsigned long brand_id_cpuid_query = 0x01;  // Send this command to find out it brand id is supported
        unsigned long ebx_output = 0x00;            // Register variables
        unsigned long ebx_bit_mask=0xFF;            // EBX bit mask
    
        asm( "cpuid" : "=b"(ebx_output): "a"(brand_id_cpuid_query));
    
        ebx_output &= ebx_bit_mask; // bi *
    
        if (ebx_output != 0x00) 
        {
            // We found bits in the lower 8 bits of the ebx register... therefore
            // BrandID is supported
            return TRUE;
        }
        else
        {
            //Brand ID is not supported
            return FALSE;
        }
    }
    else
        return FALSE;
}



////////////////////////////////////////////////////////////////////////
/* 
 *  @fn         int check_for_tsc(void);
 */
////////////////////////////////////////////////////////////////////////
int check_for_tsc(void)
{
    if (check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;
        unsigned long edx_output = 0x00;            // Register variables

        asm( "cpuid" : "=d"(edx_output) : "a"(cpuid_query));

        edx_output &= TSC_FLAG;

        if (edx_output != 0x00) 
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

}


///////////////////////////////////////////////////////////////////////
/**
 *  @fn     int check_for_l2_cache(void)
 */
///////////////////////////////////////////////////////////////////////
int check_for_l2_cache(void)
{
    //a return value to ease the checking for L2 Cache
    int return_value = FALSE;

    if(check_for_genuine_intel())
    {
        // create an array to hold all of the characters of the cache
        // table query in one easily addressable way
        unsigned char cache_table[get_cache_query_count()*16];
        memset(cache_table,0x00,sizeof(cache_table));

        if (get_cache_query_count() >=1 ) 
        {
            // build my descriptor table
            get_cache_info(cache_table,get_cache_query_count());

            //sentinel
            int i = 0;

            for (i=0; i < sizeof(cache_table); i++)
            {
                      //ranges of L2 cache descriptors
                if (((cache_table[i] >= 0x39)&&(cache_table[i] <= 0x3E)) ||
                    ((cache_table[i] >= 0x41)&&(cache_table[i] <= 0x45)) ||
                    ((cache_table[i] >= 0x78)&&(cache_table[i] <= 0x7D)) ||
                    ((cache_table[i] >= 0x82)&&(cache_table[i] <= 0x87)) ||
                     (cache_table[i] == 0x7F))
                {
                    return_value = TRUE;
                }
            }
        }
     }
     return return_value;
}

///////////////////////////////////////////////////////////////////////
/**
 *  @fn     int check_for_l3_cache(void)
 */
///////////////////////////////////////////////////////////////////////
int check_for_l3_cache(void)
{
    //a return value to ease the checking for L2 Cache
    int return_value = FALSE;

    if(check_for_genuine_intel())
    {
        // create an array to hold all of the characters of the cache
        // table query in one easily addressable way
        unsigned char cache_table[get_cache_query_count()*16];
        memset(cache_table,0x00,sizeof(cache_table));

        if (get_cache_query_count() >=1 ) 
        {
            // build my descriptor table
            get_cache_info(cache_table,get_cache_query_count());

            //sentinel
            int i = 0;

            for (i=0; i < sizeof(cache_table); i++)
            {
                      //ranges of L2 cache descriptors
                if ((cache_table[i]==0x22) || (cache_table[i]==0x23) || (cache_table[i]==0x25) || 
                    (cache_table[i]==0x29) || (cache_table[i]==0x46) || (cache_table[i]==0x47) || 
                    (cache_table[i]>=0x49)&&(cache_table[i]<=0x4D))  
                {
                    return_value = TRUE;
                }
            }
        }
    }
    return return_value;
}



////////////////////////////////////////////////////////////////////////
/* 
 * @fn    void get_cpu_id_string(char* cpu_id_string);
 */ 
////////////////////////////////////////////////////////////////////////
void get_cpu_id_string(char* cpu_id_string, int length)
{
    unsigned long eax_output = 0x00;
    unsigned long ebx_output = 0x00;
    unsigned long ecx_output = 0x00;
    unsigned long edx_output = 0x00;

    int i;

        if (check_for_genuine_intel())
        {
            if (check_for_extended_cpuid()) 
            {
                // we have enough extended info support to have a processor ID string, so we'll use it
                unsigned long byte_hold[4];
                byte_hold[0]= 0x000000FF;
                byte_hold[1]= 0x0000FF00;
                byte_hold[2]= 0x00FF0000;
                byte_hold[3]= 0xFF000000;
                
                // issue the CPUID 0x80000002 command to get first part of 
                // Brand ID String
                asm("cpuid" : "=a"(eax_output),"=b"(ebx_output),"=c"(ecx_output),"=d"(edx_output): "a"(0x80000002));
                for (i=0; i<4; i++) 
                {
                    // To properly reorder the register-split, little-endian string, we have to 
                    // mask all the bits except the one for the byte we're looking for.   As we 
                    // mask higher bytes, we shift them down so that they load poperly into
                    // the appropriate space.  To speed in the loading, each 4-byte register is
                    // loaded at the same time with increasing offsets of 4 so we can skip 
                    // multiple, nested for loops.
                    cpu_id_string[i]=((eax_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+4]=((ebx_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+8]=((ecx_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+16]=((edx_output & byte_hold[i])>>(i*8));
                    
                }
                
                // issue the CPUID 0x80000003 command to get second part of 
                // Brand ID String
                asm("cpuid" : "=a"(eax_output),"=b"(ebx_output),"=c"(ecx_output),"=d"(edx_output): "a"(0x80000003));
                for (i=0; i<4; i++) 
                {
                    cpu_id_string[i+20]=((eax_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+24]=((ebx_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+28]=((ecx_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+32]=((edx_output & byte_hold[i])>>(i*8));
                }
                
                // issue the CPUID 0x80000004 command to get last part of 
                // Brand ID String
                asm("cpuid" : "=a"(eax_output),"=b"(ebx_output),"=c"(ecx_output),"=d"(edx_output): "a"(0x80000004));
                for (i=0; i<4; i++) 
                {
                    cpu_id_string[i+36]=((eax_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+40]=((ebx_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+44]=((ecx_output & byte_hold[i])>>(i*8));
                    cpu_id_string[i+48]=((edx_output & byte_hold[i])>>(i*8));
                }


                // Stripping out Leading Space Padding
                int first_char,last_char;

                for(i=0; i<length; i++)
                {
                    if ((cpu_id_string[i] != 0x20)&&(cpu_id_string[i] != 0x00))
                    {
                        first_char = i;
                        break;
                    }
                }
                for (i=first_char; i < length; i++) 
                {
                    //stop moving characters at the "CPU" portion of the string
                    if (!((cpu_id_string[i] == 0x43)&&(cpu_id_string[i+1] == 0x50)&&(cpu_id_string[i+2] == 0x55)))
                    {
                        cpu_id_string[i-first_char] = cpu_id_string[i];
                    }
                    //ensure that the last character is always a NULL
                    cpu_id_string[i-first_char+1] = 0x00;

                    last_char = i-first_char+1;
                }
                //padding extra space with nulls
                for (i=last_char; i<length; i++) 
                {
                    cpu_id_string[i]=0x00;
                }


            }
            else if (check_for_brand_id()) 
            {
                // issue the CPUID command 0x01 to get BrandID and CPU Signature
                asm("cpuid" : "=a"(eax_output), "=b"(ebx_output) : "a"(0x01));

                //get the brand index from ebx_output
                ebx_output &= 0xFF;
                
                // begin massive switch-case selection structure
                switch ( ebx_output ) {
                case    INTEL_PENTIUM_III:
                        strcpy(cpu_id_string,"Intel(R) Pentium(R) III Processor");
                        break;
                case    INTEL_PENTIUM_III_XEON:
                        if(eax_output == 0x6B1)
                        {
                            strcpy(cpu_id_string,"Intel(R) Celeron(R) Processor");
                            break;
                        }
                        else
                        {
                            strcpy(cpu_id_string,"Intel(R) Pentium(R) III Xeon Processor");
                            break;
                        }
                        break;
                case    INTEL_PENTIUM_III_PROCESSOR:
                        strcpy(cpu_id_string,"Intel(R) Pentium(R) III Processor");
                        break;
                        break;
                case    MOBILE_INTEL_PENTIUM_III_M:
                        strcpy(cpu_id_string,"Mobile Intel(R) Pentium(R) III Processor-M");
                        break;
                        break;
                case    MOBILE_INTEL_CELERON:
                        strcpy(cpu_id_string,"Mobile Intel(R) Celeron(R) Processor");
                        break;
                        break;
                case    INTEL_PENTIUM_4_GENUINE:
                        if(eax_output >= 0x0F13)
                        {
                            strcpy(cpu_id_string,"Intel(R) Genuine Processor");
                            break;
                        }
                        else
                        {
                            strcpy(cpu_id_string,"Intel(R) Pentium(R) 4 Processor");
                            break;
                        }
                        break;
                case    INTEL_PENTIUM_4:
                        strcpy(cpu_id_string,"Intel(R) Pentium(R) 4 Processor");
                        break;
                case    INTEL_CELERON_PROCESSOR:
                        strcpy(cpu_id_string,"Intel(R) Celeron(R) Processor");
                        break;
                case    INTEL_XEON:
                        if(eax_output < 0x0F13)
                        {
                            strcpy(cpu_id_string,"Intel(R) Xeon MP");
                        }
                        else
                        {
                            strcpy(cpu_id_string,"Intel(R) Xeon(R) Processor");
                        }
                        break;
                case    INTEL_XEON_MP:
                        strcpy(cpu_id_string,"Intel(R) Xeon(R) Processor MP");
                        break;
                case    MOBILE_PENTUM_4_M:
                        if (eax_output < 0x0F13)
                        {
                            strcpy(cpu_id_string,"Intel(R) Xeon(R) Processor");
                        }
                        else
                        {
                            strcpy(cpu_id_string,"Mobile Intel(R) Pentium(R) Processor-M");
                        }
                        break;
                case    MOBILE_INTEL_CELERON_2:        
                        strcpy(cpu_id_string,"Mobile Intel(R) Celeron(R) Processor");
                        break;
                case    MOBILE_GENUINE_INTEL:
                        strcpy(cpu_id_string,"Mobile Genuine Intel(R) Processor");
                        break;
                case    INTEL_CELERON_M:       
                        strcpy(cpu_id_string,"Intel(R) Celeron(R) M Processor");
                        break;
                case    MOBILE_INTEL_CELERON_3:
                        strcpy(cpu_id_string,"Mobile Intel(R) Celeron(R) Processor");
                        break;
                case    INTEL_CELERON_2:       
                        strcpy(cpu_id_string,"Intel(R) Celeron(R) Processor");
                        break;
                case    MOBILE_GENUINE_INTEL_2:
                        strcpy(cpu_id_string,"Mobile Genuine Intel(R) Processor");
                        break;
                case    INTEL_PENTIUM_M_2:     
                        strcpy(cpu_id_string,"Intel(R) Pentium(R) M Processor");
                        break;
                case    MOBILE_INTEL_CELERON_4:
                        strcpy(cpu_id_string,"Mobile Intel(R) Celeron(R) Processor");
                        break;
                default:
                        strcpy(cpu_id_string,"Brand ID Not Found");
                };
            }
            else
            {
                strcpy(cpu_id_string,"No Brand ID or Brand ID string...");
            }
        }
        else
        {
            //nullify the cpuid pointer somehow           
        }
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned long get_cpu_signature(void);
 */
///////////////////////////////////////////////////////////////////////////////
unsigned long get_cpu_signature(void)
{
    unsigned long cpu_signature = 0x00;
    unsigned long cpuid_query   = 0x01;

    if(check_for_genuine_intel())
    {
        asm( "cpuid" : "=a"(cpu_signature) : "a"(cpuid_query));
        return cpu_signature;
    }
    else
    {
        return FALSE;
    }
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned long tsc_cpuid_speed(void);
 */
///////////////////////////////////////////////////////////////////////////////
unsigned long get_cpu_speed(void)
{
    int i;
    int sleep_time = 1;
    int recheck_count = 1;

    unsigned long eax_out1 = 0x00;
    unsigned long eax_out2 = 0x00;
    unsigned long edx_out1 = 0x00;
    unsigned long edx_out2 = 0x00;

    unsigned long long tsc_first  = 0x00;
    unsigned long long tsc_second = 0x00;

    unsigned long raw_speed = 0x00;

    if(check_for_tsc())
    {
        for(i=0; i< recheck_count; i++)
        {
            asm( "rdtsc" : "=a"(eax_out1), "=d"(edx_out1));
            sleep(sleep_time);
            asm( "rdtsc" : "=a"(eax_out2), "=d"(edx_out2));

            unsigned long long tsc_low  = eax_out1; 
            unsigned long long tsc_high = edx_out1; 
            tsc_first = tsc_low + (tsc_high << 32);

            tsc_low  = eax_out2; 
            tsc_high = edx_out2; 
            tsc_second = tsc_low + (tsc_high << 32);

            if (raw_speed < ((tsc_second - tsc_first)/sleep_time))
            {
                raw_speed = ((tsc_second - tsc_first)/sleep_time); 
            }
        }

        sleep(1);

        unsigned long sys_bus_speed = get_sys_bus_speed();
        if (sys_bus_speed == 100)
            return Nearest100(raw_speed/1000000);                            
        else if (sys_bus_speed == 133)
            return Nearest133(raw_speed/1000000);
        else if (sys_bus_speed == 166)
            return Nearest133(raw_speed/1000000);
        else if (sys_bus_speed == 200)
            return Nearest100(raw_speed/1000000);                           
        else if (sys_bus_speed == 267)
            return Nearest100(raw_speed/1000000);
        else
            return raw_speed/1000000;
    }
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         unsigned long get_sys_bus_speed(void);
 */
///////////////////////////////////////////////////////////////////////////////
unsigned long get_sys_bus_speed(void)
{
    if(check_for_genuine_intel())
    {
        unsigned long cpu_signature = get_cpu_signature();
        if (cpu_signature >= 0xF20)  
        {

            unsigned long eax, edx;
            read_msr(0,0x2C,eax,edx);

            //mask off all uneeded bits
            eax &= SYSTEM_BUS_BITMASK;

            //shift down to a level we can easily switch against
            eax = (eax >> 16);

            switch (eax)
            {
            case 0x00:
                if((cpu_signature >= 0x2f0)&&(cpu_signature <= 0x2F))
                {
                    //100Mhz System Bus
                    return 100;                            
                    break; 
                }
                else if (cpu_signature >= 0x3f0) 
                {
                    //267MHz System Bus
                    return 267;
                    break;
                }
            case 0x01:
                //133MHz System Bus
                return 133;
                break;
            case 0x03:
                //166MHz System Bus
                return 166;
                break;
            case 0x04:
                //200MHz
                return 200;
                break;
            default:
                return FALSE;
                break;
            }
        }
        else if (cpu_signature <= 0xF20)  
        {
            unsigned long eax, edx;
            read_msr(0,0x2C,eax,edx);

            //mask off all uneeded bits
            eax &= SYSTEM_BUS_BITMASK;

            //shift down to a level we can easily switch against
            if((eax >> 16) == 0x000)
            {
                return 100;
            }
            else
                return 0;
        
        }

    }
}

//////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long Nearest100(unsigned long InitialFreq);
 */
//////////////////////////////////////////////////////////////////////
unsigned long Nearest100(unsigned long InitialFreq)
{
	InitialFreq += 25;
	InitialFreq /= 50;
	InitialFreq *= 50;
	InitialFreq /= 10; // Truncate to 10's per Intel recommendation
	InitialFreq *= 10;
	return(InitialFreq);
}

//////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long Nearest133(unsigned long InitialFreq);
 */
//////////////////////////////////////////////////////////////////////
unsigned long Nearest133(unsigned long InitialFreq)
{
	InitialFreq *= 3;
	InitialFreq += 200;
	InitialFreq /= 400;
	InitialFreq *= 400;
	InitialFreq /= 3;
	InitialFreq /= 10; // Truncate to 10's per Intel recommendation
	InitialFreq *= 10;
	return(InitialFreq);
}


/////////////////////////////////////////////////////////////////////
/**
 *  @fn     int get_iopl(void);
 */
/////////////////////////////////////////////////////////////////////
int get_iopl(void)
{
    unsigned long eflags_storage = 0x00;

    asm(
        "pushfl\n\t"               //push EFLAGS on stack
        "pop %%eax\n\t"            //pop EFLAGS into eax
        : "=a"(eflags_storage)
       );

    // bitwize AND of bitmask and flags register, 
    // then right shift 12 bits to convert to int
    return ((eflags_storage &= IOPL_FLAG) >> 12);
    
}

//////////////////////////////////////////////////////////////////////
/**
 *  @fn unsigned long get_l2_cache(void);
 */
//////////////////////////////////////////////////////////////////////
unsigned long get_l2_cache(void)
{
    int i=0;

    // Just in case someone calls this directly, we want to ensure
    // that valid data is retunred
    if(check_for_l2_cache())
    {
        
        unsigned char cache_table[get_cache_query_count()*16];
        memset(cache_table,0x00,sizeof(cache_table));

        if (get_cache_query_count() >=1 ) 
        {
            get_cache_info(cache_table,get_cache_query_count());

            for (i=0; i < sizeof(cache_table); i++)
            {

                switch(cache_table[i])
                {
                case 0x39:
                case 0x3B:
                case 0x41:
                case 0x79:
                            return 128;    
                            break;
                case 0x3A:
                            return 192;    
                            break;
                case 0x3C:
                case 0x42:
                case 0x7A:
                case 0x82:
                            return 256;    
                            break;
                case 0x3D:
                            return 384;    
                            break;
                case 0x3E:
                case 0x43:
                case 0x7B:
                case 0x7F:
                case 0x83:
                case 0x86:
                            return 512;    
                            break;
                case 0x44:
                case 0x78:
                case 0x7C:
                case 0x84: 
                case 0x87:
                            return 1024;    
                            break;
                case 0x45:
                case 0x7D:
                case 0x85:
                            return 2048;    
                            break;
                }   
            }
        }
    }
    else
        return 0;
}

//////////////////////////////////////////////////////////////////////
/**
 *  @fn unsigned long get_l3_cache(void);
 */
//////////////////////////////////////////////////////////////////////
unsigned long get_l3_cache(void)
{
    int i;

    // just in case someone calls this directly, we want to be sure there is
    // valid info being returned
    if(check_for_l3_cache())
    {
        unsigned char cache_table[get_cache_query_count()*16];
        memset(cache_table,0x00,sizeof(cache_table));

        if (get_cache_query_count() >=1 ) 
        {
            get_cache_info(cache_table,get_cache_query_count());

            for (i=0; i < sizeof(cache_table); i++)
            {
                switch(cache_table[i])
                {
                case 0x22:
                            return 512;
                            break;
                case 0x23:
                            return 1024;
                            break;
                case 0x25:
                            return 2048;
                            break;
                case 0x4A:
                            return 6144;
                            break;
                case 0x29:
                case 0x46:
                case 0x49:
                            return 4096;
                            break;
                case 0x47:
                case 0x4B:
                            return 8192;
                            break;
                
                case 0x4C:
                            return 12288;
                            break;
                case 0x4D:
                            return 16384;
                            break;

                };
            }
        }
    }
    else
        return 0;

}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn        void read_msr(unsigned int cpu_number, unsigned int msr_location, unsigned long* eax, unsigned long* edx);
 */
///////////////////////////////////////////////////////////////////////////////
void read_msr(unsigned int cpu_number, unsigned int msr_location, unsigned long eax, unsigned long edx)
{

    if( get_iopl() < 3)
    {
        iopl(3);
    }

    // since the MSR's already have a driver loaded, we'll just use it/them
    int file_handle;
    char file_name[11] = "/dev/msr";

    // This program only supports up to 8 processors for the time being..
    if ((cpu_number >=0 ) && (cpu_number <= 7))
    {
        file_name[8] = cpu_number + 0x30;

        if ((file_handle=open(file_name,O_RDONLY)) < 0)
        {
            // write out a nice litte bit of info about the failure
            perror("open");
            exit (1);
        }

        // move to the offset of the msr_location from begining of file(seek_set)
        lseek(file_handle,msr_location,SEEK_SET);
        read(file_handle,&eax,16);
        read(file_handle,&edx,16);

        // tidy up our file operations
        close(file_handle);
    }
    else
    {
        eax = 0xFFFFFFFF;
        edx = 0xFFFFFFFF;
    }
}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn     unsigned int get_cache_query_count(void);
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int get_cache_query_count(void)
{
    int i;
    unsigned long eax = 0x00;

    if (check_for_genuine_intel())
    {
        asm( 
             "cpuid\n\t"
             : "=a"(eax)
             : "a"(0x02)
            );

        //how many times do we have to call the cache_info?
        return (eax & 0xFF);
    }
    else
        return 0;

}


///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         void get_cache_info(unsigned char* cache_table_pointer, unsigned int cpuid_query_count);
 */
///////////////////////////////////////////////////////////////////////////////
void get_cache_info(unsigned char* cache_table_pointer, unsigned int cpuid_query_count)
{
    if (check_for_genuine_intel())
    {
       
        int i,j;
    
        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;
    
        // Since we need to start at the highest byte, and work our way down,
        // we have our bitmasks in opposite direction
        unsigned long bitmask[4];
        bitmask[0]=0xFF000000;
        bitmask[1]=0x00FF0000;
        bitmask[2]=0x0000FF00;
        bitmask[3]=0x000000FF;
    
        unsigned long invalid_data_mask = 0x80000000;
    
        for (i=0;i<cpuid_query_count*16;i+=16) 
        {
            // Table Descriptor Query
            asm( 
                    "cpuid\n\t"
                    : "=a"(eax),"=b"(ebx),"=c"(ecx),"=d"(edx)
                    : "a"(0x02)
               );
    
            //kill al since it's not applicable
            eax &= 0xFFFFFF00;
    
            for (j=0;j<4;j++)
            {
                if (!(eax & invalid_data_mask))
                {
                    //since we multiplied the i value by 16, we will properly load the cache table without
                    //overwriting any previous sections by using the i+<register byte>+<register offset>
                    cache_table_pointer[i+j]   = ((eax & bitmask[j]) >> 24-(j*8)); 
                }
                else 
                    cache_table_pointer[i+j]   = 0x00;
    
                if (!(ebx & invalid_data_mask))
                {
                    cache_table_pointer[i+j+4] = ((ebx & bitmask[j]) >> 24-(j*8));
                }
                else
                    cache_table_pointer[i+j+4] = 0x00;
    
                if (!(ecx & invalid_data_mask))
                {
                    cache_table_pointer[i+j+8] = ((ecx & bitmask[j]) >> 24-(j*8));
                }
                else
                    cache_table_pointer[i+j+8] = 0x00;
    
                if (!(edx & invalid_data_mask))
                {
                    cache_table_pointer[i+j+12]= ((edx & bitmask[j]) >> 24-(j*8));
                }
                else
                    cache_table_pointer[i+j+12]= 0x00;
    
    
            }
    
        }
    }
    else
        memset(cache_table_pointer,0x00,cpuid_query_count);
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long get_local_apic_id(void);
 */
////////////////////////////////////////////////////////////////////////////
unsigned long get_local_apic_id(void)
{
    if(check_for_genuine_intel())
    {

        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

            //ensure that HTT is supported
        if (ebx & 0xFF000000) 
        {
            // the logical cpu count is in bits 25 - 16 in the ebx register after
            // cpuid query 0x02 is executed
            return (ebx & 0xFF000000)>>24;
        }
        else
            return 0;
    }
    else 
        return 0;
}

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long get_logical_processor_count(void);
 */
////////////////////////////////////////////////////////////////////////////
unsigned int get_processor_count(void)
{

    int i;

    unsigned long affinity_mask[32];

    for (i=0; i< 32; i++)
    {
        affinity_mask[i] = AFFINITY_MASK << i;
    }

    unsigned long affinity_result  = 0x00;

    int cpu_count = 0;

    for(i = 0; i< 32; i++)
    {
        
        if((affinity_result = sched_setaffinity(0,sizeof(affinity_mask[i]),&affinity_mask[i]))==0)
        {
            cpu_count++;
        }
    }

    if(cpu_count == 32)
        return 1;
    else
        return cpu_count;
}



int check_for_fpu(void)
{
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & FPU_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;
}
int check_for_vme(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & VME_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;
}
int check_for_de(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & DE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_pse(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PSE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_msr(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & MSR_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_pae(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PAE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}      
int check_for_mce(void)  
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & MCE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}      
int check_for_cx8(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & CX8_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}        
int check_for_apic(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & APIC_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}    
int check_for_sep(void)     
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & SEP_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}  
int check_for_mtrr(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & MTRR_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}  
int check_for_pge(void)    
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PGE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}   
int check_for_mca(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & MCA_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_cmov(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & CMOV_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_pat(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PAT_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

} 
int check_for_pse36(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PSE36_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

} 
int check_for_psnum(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PSNUM_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_clflush(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & CLFLUSH_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_dts(void)     
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & DTS_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}  
int check_for_acpi(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & ACPI_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_mmx(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & MMX_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_fxsr(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & FXSR_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}    
int check_for_sse(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & SSE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_sse2(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & SSE2_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_ss(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & SS_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_htt(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if ((edx & HTT_FLAG)/((edx & SSE2_FLAG)+1))
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_tm(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & TM_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_ia64(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & IA64_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_pbe(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & PBE_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_sse3(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & SSE3_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_monitor(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & MONITOR_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_ds_cpl(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & DS_CPL_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_eist(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & EIST_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}      
int check_for_tm2(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & TM2_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}       
int check_for_cid(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & CID_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}        
int check_for_cx16(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & CX16_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}
int check_for_xtpr(void)
{    
    if(check_for_genuine_intel())
    {
        unsigned long cpuid_query = 0x01;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (ecx & XTPR_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;

}   
int check_for_lahf(void)
{
    if(check_for_genuine_intel() && check_for_extended_cpuid())
    {
        unsigned long cpuid_query = 0x80000001;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & LAHF_FLAG)
        {
            return TRUE;
        }

    }
    return FALSE;
}
int check_for_syscall(void)
{
    if(check_for_genuine_intel() && check_for_extended_cpuid())
    {
        unsigned long cpuid_query = 0x80000001;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & SYSCALL_FLAG)
        {
            return TRUE;
        }

    }
    return FALSE;
}
int check_for_xd(void)
{
    if(check_for_genuine_intel() && check_for_extended_cpuid())
    {
        unsigned long cpuid_query = 0x80000001;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & XD_FLAG)
        {
            return TRUE;
        }

    }
    return FALSE;
}
int check_for_em64t(void)
{
    if(check_for_genuine_intel() && check_for_extended_cpuid())
    {
        unsigned long cpuid_query = 0x80000001;

        unsigned long eax = 0x00;
        unsigned long ebx = 0x00;
        unsigned long ecx = 0x00;
        unsigned long edx = 0x00;

        asm( "cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx): "a"(cpuid_query));

        if (edx & EM64T_FLAG)
        {
            return TRUE;
        }
    }
    return FALSE;
}
 









//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//                              Testing Block                                                   //
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void print_header(void)
{
    printf("\n");
    printf("----------------------------------------------------------------\n\n");
}


void show_cpuid_status(void)
{
    int i;

    printf("Command: \t\tSupport: \tExtra Data:\n");
    printf("-------- \t\t-------- \t----------- \n");

    // geth_local_apic_id check
    unsigned long local_apic_id = get_local_apic_id();
    printf("Local APIC ID: \t\tAvailable \t%i\n",local_apic_id);

    // check_for_cpuid Check
    if(check_for_cpuid())
        printf("CPUID:  \t\tAvailable\n");
    else
        printf("CPUID:  \t\tNOT Available\n");

    // check_forextended_cpuid Check
    if(check_for_extended_cpuid())
        printf("Extended CPUID: \tAvailable\n");
    else
        printf("Extended CPUID: \tNOT Available\n");

    // check_for_brand_id Check
    if(check_for_brand_id())
        printf("Brand ID: \t\tAvailable\n");
    else
        printf("Brand ID: \t\tNOT Available\n");


    //  get_cpu_signature() check
    unsigned long cpu_signature = get_cpu_signature();
    if(cpu_signature)
        printf("CPU Signature:  \tAvailable \t%#010X\n",cpu_signature);
    else
        printf("CPU Signature:  \tNOT Available");


    // get_sys_bus_speed() check
    unsigned long sys_bus_speed = get_sys_bus_speed();
    if(sys_bus_speed)
        printf("System Bus Speed \tAvailable \t%i MHz\n",sys_bus_speed);
    else
        printf("System Bus Speed \tNOT Available \n");


    // check_for_genuine_intel() Check
    if(check_for_genuine_intel())
        printf("Intel Processor: \tAvailable\n");
    else
        printf("Intel Processor: \tNOT Available\n");

    // check_for_tsc Check
    if(check_for_tsc())
        printf("TSC: \t\t\tAvailable\n");
    else
        printf("TSC: \t\t\tNOT Available\n");

    
    // get_cpu_id_string() check
    char cpu_id_string[64];
    memset(cpu_id_string,0x20,sizeof(cpu_id_string));

    get_cpu_id_string(cpu_id_string,sizeof(cpu_id_string));
    if (cpu_id_string != NULL) 
    {
        printf("CPU ID String: \t\tAvailable \t%s\n",cpu_id_string);
    }
    else
    {
        printf("CPU ID String: \t\tNOT Available \n");
    }



    //get_cpu_speed() check
    unsigned long cpu_speed = get_cpu_speed();
    if (cpu_speed) 
        printf("Calculated CPU Speed:  \tAvailable \t%i MHz\n",cpu_speed);
    else
        printf("Calculated CPU Speed:  \tNOT Available\n");

    // L2 Cache Check
    if(check_for_l2_cache())
    {
        unsigned long l2_size = get_l2_cache();    
        printf("L2 Cache: \t\tAvailable \t%u KB\n",l2_size);
    }
    else
        printf("L2 Cache: \t\tNOT Available\n");

    // L3 Cache Check
    if(check_for_l3_cache())
    {
        unsigned long l3_size = get_l3_cache();
        printf("L3 Cache: \t\tAvailable \t%u KB\n",l3_size);
    }
    else
        printf("L3 Cache: \t\tNOT Available\n");


    unsigned long cpu_count = 0;
    cpu_count = get_processor_count();

    if (cpu_count)
        printf("Total CPU Count: \tAvailable \t%i\n",cpu_count);
    else
        printf("Total CPU Count: \tNOT Available\n");




    ////////////////////////// keep this noisy output last ///////////////////////
    // get_cache_info() check
    if(check_for_genuine_intel())
    {
        
        // create an array to hold all of the characters of the cache
        // table query in one easily addressable way
        unsigned char cache_table[get_cache_query_count()*16];
        memset(cache_table,0x00,sizeof(cache_table));

        printf("\n");
        printf("Miscellaneous Data:\n");
        printf("--------------------\n");

        if (get_cache_query_count() >=1 ) 
        {
            get_cache_info(cache_table,get_cache_query_count());

            printf("Cache Desc. Table: \tAvailable \t");

            for(i=0;i<(sizeof(cache_table)/4);i++)
            { 
                printf("%#02X \t",cache_table[i]);
            }
            printf("\n\t\t\t\t\t");
            for(i=(sizeof(cache_table)/4);i<(sizeof(cache_table)/4)*2;i++)
            { 
                printf("%#02X \t",cache_table[i]);
            }
            printf("\n\t\t\t\t\t");
            for(i=(sizeof(cache_table)/4)*2;i<(sizeof(cache_table)/4)*3;i++)
            { 
                printf("%#02X \t",cache_table[i]);
            }
            printf("\n\t\t\t\t\t");
            for(i=(sizeof(cache_table)/4)*3;i<sizeof(cache_table);i++)
            { 
                printf("%#02X \t",cache_table[i]);
            }
            printf("\n\n");
        }
        else
            printf("Cache Descriptor Table: \tNOT Available\n");
    }

    ////// checking for feature Flags
    if(check_for_genuine_intel())
    {
        printf("Feature Flags: \t\tAvailable \t");

        //long clunky checks

        if (check_for_fpu())     { printf("fpu\t");}        else { printf("\t"); }
        if (check_for_vme())     { printf("vme\t"); }       else { printf("\t"); }    
        if (check_for_de())      { printf("de\t"); }        else { printf("\t"); }
        if (check_for_pse())     { printf("pse\t"); }       else { printf("\t"); }
        if (check_for_tsc())     { printf("tsc\t"); }       else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_msr())     { printf("msr\t"); }       else { printf("\t"); }
        if (check_for_pae())     { printf("pae\t"); }       else { printf("\t"); }
        if (check_for_mce())     { printf("mce\t"); }       else { printf("\t"); }
        if (check_for_cx8())     { printf("cx8\t"); }       else { printf("\t"); }
        if (check_for_apic())    { printf("apic\t"); }      else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_sep())     { printf("sep\t"); }       else { printf("\t"); }
        if (check_for_mtrr())    { printf("mtrr\t"); }      else { printf("\t"); }
        if (check_for_pge())     { printf("pge\t"); }       else { printf("\t"); }
        if (check_for_mca())     { printf("mca\t"); }       else { printf("\t"); }
        if (check_for_cmov())    { printf("cmov\t"); }      else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_pat())     { printf("pat\t"); }       else { printf("\t"); }
        if (check_for_pse36())   { printf("pse36\t"); }     else { printf("\t"); }
        if (check_for_psnum())   { printf("psnum\t"); }     else { printf("\t"); }
        if (check_for_clflush()) { printf("clflush\t"); }   else { printf("\t"); }
        if (check_for_dts())     { printf("dts\t"); }       else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_acpi())    { printf("acpi\t"); }      else { printf("\t"); }
        if (check_for_mmx())     { printf("mmx\t"); }       else { printf("\t"); }
        if (check_for_fxsr())    { printf("fxsr\t"); }      else { printf("\t"); }
        if (check_for_sse())     { printf("sse\t"); }       else { printf("\t"); }
        if (check_for_sse2())    { printf("sse2\t"); }      else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_ss())      { printf("ss\t"); }        else { printf("\t"); }
        if (check_for_htt())     { printf("htt\t"); }       else { printf("\t"); }
        if (check_for_tm())      { printf("tm\t"); }        else { printf("\t"); }
        if (check_for_ia64())    { printf("ia64\t"); }      else { printf("\t"); }
        if (check_for_pbe())     { printf("pbe\t"); }       else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_sse3())    { printf("sse\t"); }       else { printf("\t"); }
        if (check_for_monitor()) { printf("monitor\t"); }   else { printf("\t"); }
        if (check_for_ds_cpl())  { printf("ds_cpl\t"); }    else { printf("\t"); }
        if (check_for_eist())    { printf("eist\t"); }      else { printf("\t"); }
        if (check_for_tm2())     { printf("tm2\t"); }       else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_cid())     { printf("cid\t"); }       else { printf("\t"); }
        if (check_for_cx16())    { printf("cx16\t"); }      else { printf("\t"); }
        if (check_for_xtpr())    { printf("xtpr\t"); }      else { printf("\t"); }
        if (check_for_lahf())    { printf("lahf\t"); }    else { printf("\t"); }
        if (check_for_syscall()) { printf("syscall\t"); } else { printf("\t"); }

        printf("\n\t\t\t\t\t");

        if (check_for_xd())      { printf("xd\t"); }      else { printf("\t"); }
        if (check_for_em64t())   { printf("em64t\t"); } 

    }
    else
        printf("Feature Flags: \tNOT Available \n");

    printf("\n\n\n\n");

}
