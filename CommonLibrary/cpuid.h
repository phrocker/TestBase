///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       cpuid.h
 *
 *  @brief      Holds function declarations and #define directives.
 *
 *              Copyright (C) 2006 @n@n
 *              This program is intended to be a fully featured CPUID interface
 *              package that  is designed to aid in board/cpu profiling.  It 
 *              will be included in the TBS Linux Test Platform Common Library 
 *              and can be accessed when stable and can be accessed as such.  
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


// Give me some boolean like operators
#ifndef TRUE 
#define TRUE 1
#endif
#ifndef FALSE 
#define FALSE 0
#endif
#ifndef NULL
#define NULL ((void*) 0);
#endif

// Flag Bitmasks
#define FPU_FLAG        0X0001 
#define VME_FLAG        0X0002
#define DE_FLAG         0X0004
#define PSE_FLAG        0X0008
#define TSC_FLAG        0X0010
#define MSR_FLAG        0X0020
#define PAE_FLAG        0X0040
#define MCE_FLAG        0X0080
#define CX8_FLAG        0X0100
#define APIC_FLAG       0X0200
#define SEP_FLAG        0X0800
#define MTRR_FLAG       0X1000
#define PGE_FLAG        0X2000
#define MCA_FLAG        0X4000
#define CMOV_FLAG       0X8000
#define PAT_FLAG        0X10000
#define PSE36_FLAG      0X20000
#define PSNUM_FLAG      0X40000
#define CLFLUSH_FLAG    0X80000
#define DTS_FLAG        0X200000
#define ACPI_FLAG       0X400000
#define MMX_FLAG        0X800000
#define FXSR_FLAG       0X1000000
#define SSE_FLAG        0X2000000
#define SSE2_FLAG       0X4000000
#define SS_FLAG         0X8000000
#define HTT_FLAG        0X10000000
#define TM_FLAG         0X20000000
#define IA64_FLAG       0X40000000
#define PBE_FLAG        0X80000000
#define SSE3_FLAG       0X0001
#define MONITOR_FLAG    0X0008
#define DS_CPL_FLAG     0X0010
#define EIST_FLAG       0X0080
#define TM2_FLAG        0X0100
#define CID_FLAG        0X0400
#define CX16_FLAG       0X2000
#define XTPR_FLAG       0X4000
#define LAHF_FLAG       0X00000001
#define SYSCALL_FLAG    0X00000800
#define XD_FLAG         0X00100000
#define EM64T_FLAG      0X20000000
#define IOPL_FLAG       0x3000

// Brand ID Table
#define INTEL_CELERON               0x01
#define INTEL_PENTIUM_III           0x02
#define INTEL_PENTIUM_III_XEON      0x03    // If signature = 0x06B1, then "Intel(R) Celeron"
#define INTEL_PENTIUM_III_PROCESSOR 0x04
#define MOBILE_INTEL_PENTIUM_III_M  0x06
#define MOBILE_INTEL_CELERON        0x07
#define INTEL_PENTIUM_4_GENUINE     0x08    // If signature >=0x0F13, then "Intel(R) Genuine"
#define INTEL_PENTIUM_4             0x09
#define INTEL_CELERON_PROCESSOR     0x0A
#define INTEL_XEON                  0x0B    // If signature < 0x0F13, then "Intel(R) Xeon MP"
#define INTEL_XEON_MP               0x0C
#define MOBILE_PENTUM_4_M           0x0E    // If signature < 0x0F13, then "Intel(R) Xeon"
#define MOBILE_INTEL_CELERON_2      0x0F
#define MOBILE_GENUINE_INTEL        0x11
#define INTEL_CELERON_M             0x12
#define MOBILE_INTEL_CELERON_3      0x13
#define INTEL_CELERON_2             0x14
#define MOBILE_GENUINE_INTEL_2      0x15
#define INTEL_PENTIUM_M_2           0x16
#define MOBILE_INTEL_CELERON_4      0x17

//MSR Result Bitmasks
#define SYSTEM_BUS_BITMASK 0x38000

// CPUID query response masks
#define LOGICAL_CPU_COUNT_BITMASK    0xFF0000




/** @def    affinity_mask[16]
 * 
 *  @brief  CPU affinity masks
 * 
 *  @note   
 *          Currently, we only have 16 CPUs supported as that is the maximum available in our kernels
 */
/*unsigned long affinity_mask[16] = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 
                                   0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800,
                                   0x1000, 0x2000, 0x4000, 0x8000};
*/

#define AFFINITY_MASK               0x01

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_cpuid(void);
 *
 *  @return     An integer that signfies if CPUID exists (1) or does
 *              not exist (0).
 *  @brief      This function verifies that the CPUID command is supported.
 *
 *              This function checks to see if the cpu under test acually 
 *              Supports the CPUID function by attempting to set the ID flag
 *              in the EFLAGS register.  If you can set the flag, then 
 *              the CPUID command is supported  and returns a 0(false) or 1(true).
 *
 *  @warning    <b><i>This command must be run BEFORE any other CPUID request!
 *              Since the CPUID command is inline-assembly, there is no error
 *              checking with it. If CPUID is not supported and you issue a CPUID 
 *              request,  --!! YOUR PROGRAM WILL CRASH !!--</i></b>
 */
////////////////////////////////////////////////////////////////////////////
int check_for_cpuid(void);


////////////////////////////////////////////////////////////////////////
/**
 *  @fn         check_for_extended_cpuid(void);
 *
 *  @return     An integer that signifies if the Extended CPUID
 *              commands for Brand ID String exist (1) or does not exist(0)
 *
 *  @brief      This function verifies the Brand ID String is supported
 * 
 *              This function issuses a CPUID directive with the 
 *              command 0x80000000.  If the return value in EAX is 
 *              0x80000004 or greater return a 1(true) so that your program 
 *              will access the Brand ID String for id.  Else, a 0(false) is 
 *              returned.
 *
 *  @note       If the function returns a 1, you will need to parse the
 *              Brand ID String to ID the processor.  Otherise, you'll
 *              have to revert to using the Brand ID or the CPU
 *              Signature.
 *
 */ 
////////////////////////////////////////////////////////////////////////
int check_for_extended_cpuid(void);

////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_brand_id(void);
 *
 *  @return     An integer that sifnigies if support for a Brand ID
 *              exists (1) or does not exist (0)
 *  
 *  @brief      This function verifies if the Brand ID is supported
 * 
 *              This function issues the CPUID Command 0x01 and checks 
 *              the bits [0..7] in the EBX Register.  If  bits are 
 *              non zero, Brand ID is supported and it can be located in 
 *              the EBX register bits [0..7].
 * 
 *  @note       If they are not zero, Brand ID is NOT supported and you 
 *              must use a combination of Searching through a table of 
 *              processor signatures and the cache descriptors to 
 *              id the processor
 */ 
/////////////////////////////////////////////////////////////////////////
int check_for_brand_id(void);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         int check_for_genuine_intel(void);
 *
 *  @return     An interger that signifies Genuine (1) or not (0)
 *
 *  @brief      This function checks to see if the processor is a Genuine
 *              Intel Processor 
 *
 *              This function issues the inline-assembly directive of CPUID
 *              with the appropriately loaded eax register to recover the 
 *              "GenuineIntel" string (if it exists) and returns the Maximum 
 *              Input Value as a possible way to notify maintainer of a need 
 *              to update this program to support new data that's available 
 *              from the CPUID
 *
 *  @note       Currently, we are only testing Genuine Intel Processors;
 *              however, that is likely to change in the future, and 
 *              since other processors support CPUID, we need to know
 *              who it is we are actually talking to
 */ 
////////////////////////////////////////////////////////////////////////
int check_for_genuine_intel(void);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         int check_for_tsc(void); 
 *
 *  @return     An interger that signifies TSC Support (1) or not (0)
 * 
 *  @brief      This function ensures that TSC is available
 * 
 *              This function issues the CPUID Command 0x00 to be 
 *              sure this is a  Genuine Intel Processor, then 
 *              executes the Command 0x01 to get supported flags
 *              placed into the EDX Register, ensures that bit 4
 *              in the EDX Register (TSC support) is set.
 */
////////////////////////////////////////////////////////////////////////
int check_for_tsc(void);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         int check_for_l2_cache(void);
 *
 *  @return     Returns an integer signifying a valid L2 cache(1) or no
 *              valid L2 Cache(0)
 *
 *  @brief      This function verifies that there is at least 1 valid L2
 *              cache descriptor.
 *
 *              This function executes the CPUID Command 0x02 to load
 *              registers EA,B,C,DX with a table of descriptors for the 
 *              cache and number of times required to step thought the
 *              0x02 command to get a full description. If any of the 
 *              following ranges of values are in the descriptor table,
 *              there is a valid L2 cache:
 *  @code
 *                    0xE9 <= cache_table <= 0x3E   OR
 *                    0x41 <= cache_table <= 0x45   OR
 *                    0x78 <= cache_table <= 0x7D   OR
 *                    0x82 <= cache_table <= 0x87   OR
 *                    cache_table = 0x7F
 *  @endcode
 */
////////////////////////////////////////////////////////////////////////
int check_for_l2_cache(void);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         int check_for_l3_cache(void);
 *
 *  @return     Returns an integer signifying a valid L3 cache(1) or no
 *              valid L3 Cache(0)
 *
 *  @brief      This function verfies there is at least one valid L3
 *              cache descriptor.
 *
 *              This function executes the CPUID Command 0x02 to load
 *              registers EA,B,C,DX with a table of descriptors for the 
 *              cache and number of times required to step thought the
 *              0x02 command to get a full description. If any of the 
 *              following ranges of values are in the descriptor table,
 *              there is a valid L3 cache:
 *  @code
 *                  cache_table = 0x22      OR
 *                  cache_table = 0x23      OR
 *                  cache_table = 0x25      OR
 *                  cache_table = 0x29      OR
 *                  cache_table = 0x46      OR    
 *                  cache_table = 0x47      OR
 *                  0x49 <= cache_table <= 0x4D
 *  @endcode
 */
////////////////////////////////////////////////////////////////////////
int check_for_l3_cache(void);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         unsigned long get_l2_cache(void);
 *
 *  @return     Returns an unsigned long that is the L2 cache size in 
 *              KBytes.
 *
 *  @brief      This function decodes the Cache size from the cache descriptor
 *              table and returns the value in KBytes.
 *
 *              This function verfies that there is a vaild L2 cache; then,
 *              It decodes the value and returns it as an unsigned long integer
 *              in Kilobytes.
 */
////////////////////////////////////////////////////////////////////////
unsigned long get_l2_cache(void);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         unsigned long get_l3_cache(void);
 *
 *  @return     Returns an unsigned long that is the L3 cache size in 
 *              KBytes.
 *
 *  @brief      This function decodes the Cache size from the cache descriptor
 *              table and returns the value in KBytes.
 *
 *              This function verfies that there is a vaild L3 cache; then,
 *              It decodes the value and returns it as an unsigned long integer
 *              in Kilobytes.
 */
////////////////////////////////////////////////////////////////////////
unsigned long get_l3_cache(void);



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void get_cpu_id_string(char* cpu_id_string, int length);
 *
 *  @arg    <b>cpu_id_string</b> @n 
 *          - A pointer to a C string that will be loaded with the results
 *            of the Brand ID String parsing
 *
 *  @arg    <b>length</b> @n 
 *          - An integer specifying the size of the pointer's target
 *
 *  @brief  This function gets the Brand ID String using CPUID
 * 
 *          This function issues the CPUID Commands 0x80000002 - 0x80000004
 *          that retrun the Processor Brand ID String and stores the 
 *          result in the array pointed at by processor_id_string.  
 *
 *  @note   This function should only be executed AFTER you have
 *          verified that CPUID is Supported, and that the CPU
 *          you are testing also supports the Extended CPUID commands
 *          up to and including 0x80000004.  The functions
 *          check_for_cpuid(); and check_for_extended_cpuid();
 *          should both return a TRUE value for the above criteria to
 *          be achieved. 
 *  
 *  @note   The character string referneced by the brand_id_string
 *          pointer should be at least large enought to hold 48
 *          character values {[(4* 4-byte registers)/command] * 3 commands}.
 *
 *  @note   It is strongly suggested that before you pass the pointer above,
 *          that you first use to initialize the array in a known state.
 *
 *  @code 
 *          memset(processor_id_string,0x20,sizeof(processor_id_string); 
 *  @endcode
 *
 *  @note   OR
 *
 *  @code 
 *          memset(processor_id_string,0x00,sizeof(processor_id_string); 
 *  @endcode
 *          
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void get_cpu_id_string(char* cpu_id_string,int length);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         unsigned long get_cpu_signature(void);
 *
 *  @return     Returns an unsigned long integer that is the CPU Signature
 *
 *  @brief      This function gets the CPU Signature
 *
 *              This function ensures that cpuid is available, then issues
 *              the CPUID query 0x01 and stores the value in the EAX register.
 *              This value is the processor signature or 0 for not available.
 */
///////////////////////////////////////////////////////////////////////////////
unsigned long get_cpu_signature(void);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         unsigned long get_cpu_speed(void);
 *
 *  @return     Returns an unsigned long integert value that is the CPU Speed 
 *
 *  @brief      This function calculates the CPU Speed with the TSC
 *
 *              This funtion makes sure that cpuid is avaiable, and that 
 *              the TSC is available.  Then, it calculates the CPU Speed by 
 *              using the TSC CPUID Command twice at the begining and end
 *              of a reference period, subtracting the difference, and dividing
 *              by the reference time.  Then we divide the raw speed by 1000000
 *              to adjust for MHz and then round to the appropriate nearest speed.
 *              A FALSE is returned on failure
 *
 *  @todo       Find out why this sleep(1) call must be placed before
 *              the sys_bus_speed query(line 565)  to have repeatable 
 *              CPU speed tests.  
 *
 *
 */
///////////////////////////////////////////////////////////////////////////////
unsigned long get_cpu_speed(void);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int get_sys_bus_speed(void);
 *
 *  @return     Returns an unsigned integer that is the System Bus Speed
 *
 *  @brief      This function gets the System Bus Speed from the MSR_EBCFREQUENCY_ID 
 *
 *              This function ensures cpuid is available, and then reads the 
 *              appropriate MSR to get the system bus speed
 *
 *  @note       Bus speed can be gotten from individual processors by twiddling the 
 *              CPU Affinity Bitmask
 */
///////////////////////////////////////////////////////////////////////////////
unsigned long get_sys_bus_speed(void);

/////////////////////////////////////////////////////////////////////
/**
 *  @fn         int get_iopl(void);
 *
 *  @return     Returns an integer specifing the I/O Permission Level
 *
 *  @brief      This function gets the iopl level.
 *
 *              This function allows the user to check and see if they
 *              already have the needed IOPL level, saving a system call if 
 *              it's not needed.
 */
/////////////////////////////////////////////////////////////////////
int get_iopl(void);


// ****************************************************************************
/** 
 *  @fn         unsigned long Nearest100(unsigned long InitialFreq)
 *
 *  @arg        <b>unsigned long</b> MhzFreq@n
 *                  - Initial frequency in MHz
 *
 *  @brief      Find the closest frequency in 100MHz to the specified frequency.
 *
 *              Rounding based on this formula: ((MHz + 25) / 50) * 50
 *
 *	@note       ulong truncates during the divide.  This is expected.
 */
// ****************************************************************************
unsigned long Nearest100(unsigned long InitialFreq);

// ****************************************************************************
/** 
 *  @fn         unsigned long Nearest133(unsigned long InitialFreq);
 *
 *  @arg        <b>unsigned long</b> MhzFreq@n
 *                  - Initial frequency in MHz
 *
 *	            Rounding based on this formula: ((((3 * MHz) + 200) / 400) * 400) / 3
 *
 *	@note       ulong truncates during the divide.  This is expected.
 */
// ****************************************************************************
unsigned long Nearest133(unsigned long InitialFreq);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void read_msr(unsigned int cpu_number, unsigned int msr_location, unsigned long eax, unsigned long edx);
 * 
 *  @arg        <b>unsigned int</b> cpu_number
 *                  - an integer that represents the processor (0, 1, 2, etc)
 *  @arg        <b>unsigned int</b> msr_location
 *                  - an intger that is the location of the msr you'd like to read
 *  @arg        <b>unsigned long</b> eax
 *                  - a pointer to an unsigned long to store the eax output from the read
 *  @arg        <b>unsigned long</b> edx
 *                  - a pointer to an unsigned long to store the edx output from the read
 *
 *  @brief      This function reads from the MSR registers at the <c>msr_location</c> location
 *
 *              This function takes in the CPU number, and the msr location to read.  
 *              It then stores the results into the eax and edx register pointers that 
 *              were also passed to it.
 *
 *  @notice     Currently, this msr read only supports up to 8 different msr devices.  This 
 *              also means that you can have a max of 8 total real and logical processors.
 *              If you attempt to read outside this range, the msr read function will report
 *              0xFFFFFFFF in all registers
 *
 */
///////////////////////////////////////////////////////////////////////////////
void read_msr(unsigned int cpu_number, unsigned int msr_location, unsigned long eax, unsigned long edx);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void get_cache_info(unsigned char* cache_table_pointer, unsigned int cpuid_query_count);
 *
 *  @arg        <b>unsigned char</b> *cache_table_pointer@n
 *                  - The C string this pointer references <b>MUST</b> be unsigned.  Otherwise, you get 
 *                    odd output when the character starts with a 0x8x (may also happen other charcters with a 1 MSB as well)
 *
 *  @arg        <b>unsigned int</b> cpuid_query_count@n
 *                  - integer referencing the number of times the CPUID cache descriptor query needs
 *                    to be executed to get a full description of your cache.
 *
 *  @brief      This function returns an array of registers containg the cache table info
 *
 *              This function sends the CPUID query 0x02 as many times as is specified by the cpuid_query_count
 *              to get a full description of the cache available on the processor.  It then loads that description
 *              into an array specfied by cache_table_pointer for easy storage.
 */
///////////////////////////////////////////////////////////////////////////////
void get_cache_info(unsigned char* cache_table_pointer, unsigned int cpuid_query_count);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         unsigned int get_cache_query_count(void);
 *
 *  @return     Returns an integer that is the number of times needed to execute the CPUID Cache query to get a full description
 *
 *  @brief      This function tells you how many times the CPUID Cache query needs to be executed
 *
 *              This function is (as far as I know) only useful with the get_cache_info(); function as
 *              it is a required argument, and anywhere you create the C string that the cache_table_pointer references.
 *              I just thought it would clean up the code a bit.
 *
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int get_cache_query_count(void);

///////////////////////////////////////////////////////////////////////////////
/** 
 *  @fn         unsigned long get_processor_count(void);
 *
 *  @return     Returns an unsigned long value that is the total number of CPUs (logical and physical)
 *
 *  @brief      This determines the number of CPU's seen by the system
 *
 *              This function executes the sched_setaffinity(new_pid,sizeof(affinity_mask[0]),&affinity_mask[i])
 *              to detect what are valid processor locations as the set affinity 
 *              returns an error on mismatched locations.
 */
///////////////////////////////////////////////////////////////////////////////
unsigned int get_processor_count(void);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         unsigned long get_local_apic_id(void);
 *
 *  @return     Returns an unsigned long value that is the local APIC id of the processor
 *
 *  @brief      This function fetches the local APIC ID of the processor you are testing
 *
 *              This function executes CPUID query 0x01.  It then masks off the upper 8
 *              bits of the ebx register and shifts them down 24 bits to get the integer
 *              value that is the local APIC ID.
 */
////////////////////////////////////////////////////////////////////////////
unsigned long get_local_apic_id(void);

////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         unsigned long get_brand_id(void)
 *
 *  @return     An unsigned long integer that is the Brand ID
 *
 *  @brief      This function fetches the Brand ID  for the processor you are testing
 *
 *              This function executes the CPUID query 0x01.  The response in EBX is
 *              masked off with 0xFF and then returned as the Brand ID
 */
////////////////////////////////////////////////////////////////////////////
unsigned long get_brand_id(void);



///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void print_header(void);
 *
 *  @brief      Nice Intro header for show_cpuid_status().
 *
 */
///////////////////////////////////////////////////////////////////////////////
void print_header(void);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         void show_cpuid_status(void);
 *
 *  @brief      Prints out the status of all available functions
 *
 */
///////////////////////////////////////////////////////////////////////////////
void show_cpuid_status(void);


///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_fpu(void);
 *
 *  @brief      Verifies if the FPU Flag is set         
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_fpu(void);         

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_vme(void);        
 *
 *  @brief      Verifies if the VME Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_vme(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_de(void);                 
 *
 *  @brief      Verifies if the DE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_de(void);         

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_pse(void);        
 *
 *  @brief      Verifies if the PSE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_pse(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_msr(void);        
 *
 *  @brief      Verifies if the MSR Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_msr(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_pae(void);        
 *
 *  @brief      Verifies if the PAE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_pae(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_mce(void);        
 *
 *  @brief      Verifies if the MCE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_mce(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_cx8(void);        
 *
 *  @brief      Verifies if the CX8 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_cx8(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_apic(void);       
 *
 *  @brief      Verifies if the APIC Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_apic(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_sep(void);        
 *
 *  @brief      Verifies if the SEP Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_sep(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_mtrr(void);       
 *
 *  @brief      Verifies if the MTRR Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_mtrr(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_pge(void);        
 *
 *  @brief      Verifies if the PGE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_pge(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_mca(void);        
 *
 *  @brief      Verifies if the MCA Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_mca(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_cmov(void);       
 *
 *  @brief      Verifies if the CMOV Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_cmov(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_pat(void);        
 *
 *  @brief      Verifies if the PAT Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_pat(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_pse36(void);      
 *
 *  @brief      Verifies if the PSE36 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_pse36(void);      

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_psnum(void);      
 *
 *  @brief      Verifies if the PSNUM Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_psnum(void);      

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_clflush(void);    
 *
 *  @brief      Verifies if the CLFLUSH Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_clflush(void);    

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_dts(void);        
 *
 *  @brief      Verifies if the DTS Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_dts(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_acpi(void);       
 *
 *  @brief      Verifies if the ACPI Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_acpi(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_mmx(void);        
 *
 *  @brief      Verifies if the MMX Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_mmx(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_fxsr(void);       
 *
 *  @brief      Verifies if the FXSR Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_fxsr(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_sse(void);        
 *
 *  @brief      Verifies if the SSE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_sse(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_sse2(void);       
 *
 *  @brief      Verifies if the SSE2 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_sse2(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_ss(void);         
 *
 *  @brief      Verifies if the SS Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_ss(void);         

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_htt(void);        
 *
 *  @brief      Verifies if the HTT Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_htt(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_tm(void);         
 *
 *  @brief      Verifies if the TM Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_tm(void);         

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_ia64(void);       
 *
 *  @brief      Verifies if the ia64 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_ia64(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_pbe(void);        
 *
 *  @brief      Verifies if the PBE Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_pbe(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_sse3(void);       
 *
 *  @brief      Verifies if the SSE3 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_sse3(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_monitor(void);    
 *
 *  @brief      Verifies if the MONITOR Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_monitor(void);    

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_ds_cpl(void);     
 *
 *  @brief      Verifies if the FDS_CPL Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_ds_cpl(void);     

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_eist(void);       
 *
 *  @brief      Verifies if the EIST Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_eist(void);       

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_tm2(void);        
 *
 *  @brief      Verifies if the TM2 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_tm2(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_cid(void);        
 *
 *  @brief      Verifies if the CID Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_cid(void);        

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_cx16(void);
 *
 *  @brief      Verifies if the CX16 Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_cx16(void);

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_xtpr(void);
 *
 *  @brief      Verifies if the XTPR Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_xtpr(void);     

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_lahf(void); 
 *
 *  @brief      Verifies if the LAHF Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_lahf(void); 

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_syscall(void);
 *
 *  @brief      Verifies if the SYSCALL Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_syscall(void); 

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_xd(void); 
 *
 *  @brief      Verifies if the XD Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_xd(void);   

///////////////////////////////////////////////////////////////////////////////
/**
 *  @fn         int check_for_em64t(void);
 *
 *  @brief      Verifies if the EM64T Flag is set         
 *
 */
///////////////////////////////////////////////////////////////////////////////
int check_for_em64t(void); 





//Define documentation get so long, I just decided to move it

/** @def    TRUE  
 *  @brief  Making a more boolean 1 appearance in code 
 *  
 *  @def    FALSE
 *  @brief  Making a more boolean 0 appearance in code 
 * 
 *  @def    NULL
 *  @brief  Assigning a proper NULL value if none exists 
 *  
 *  @def    FPU_FLAG        
 *  @brief  FPU support bitmask flag for CPUID command
 *  
 *  @def    VME_FLAG       
 *  @brief  VME support bitmask flag for CPUID command
 *  
 *  @def    DE_FLAG         
 *  @brief  DE support bitmask flag for CPUID command
 *  
 *  @def    PSE_FLAG        
 *  @brief  PSE support bitmask flag for CPUID command
 *  
 *  @def    TSC_FLAG        
 *  @brief  TSC support bitmask flag for CPUID command
 *  
 *  @def    MSR_FLAG        
 *  @brief  MSR support bitmask flag for CPUID command
 *  
 *  @def    PAE_FLAG        
 *  @brief  PAE support bitmask flag for CPUID command
 *  
 *  @def    MCE_FLAG        
 *  @brief  MCE support bitmask flag for CPUID command
 *  
 *  @def    CX8_FLAG        
 *  @brief  CX8 support bitmask flag for CPUID command
 *  
 *  @def    APIC_FLAG       
 *  @brief  APIC support bitmask flag for CPUID command
 *  
 *  @def    SEP_FLAG        
 *  @brief  SEP support bitmask flag for CPUID command
 *  
 *  @def    MTRR_FLAG       
 *  @brief  MTRR support bitmask flag for CPUID command
 *  
 *  @def    PGE_FLAG        
 *  @brief  PGE support bitmask flag for CPUID command
 *  
 *  @def    MCA_FLAG        
 *  @brief  MCA support bitmask flag for CPUID command
 *  
 *  @def    CMOV_FLAG       
 *  @brief  CMOV support bitmask flag for CPUID command
 *  
 *  @def    PAT_FLAG        
 *  @brief  PAT support bitmask flag for CPUID command
 *  
 *  @def    PSE36_FLAG      
 *  @brief  PSE36 support bitmask flag for CPUID command
 *  
 *  @def    PSNUM_FLAG      
 *  @brief  PSNUM support bitmask flag for CPUID command
 *  
 *  @def    CLFLUSH_FLAG    
 *  @brief  CLFLUSH support bitmask flag for CPUID command
 *  
 *  @def    DTS_FLAG        
 *  @brief  DTS support bitmask flag for CPUID command
 *  
 *  @def    ACPI_FLAG       
 *  @brief  APIC support bitmask flag for CPUID command
 *  
 *  @def    MMX_FLAG        
 *  @brief  MMX support bitmask flag for CPUID command
 *  
 *  @def    FXSR_FLAG       
 *  @brief  FXSR support bitmask flag for CPUID command
 *  
 *  @def    SSE_FLAG        
 *  @brief  SSE support bitmask flag for CPUID command
 *  
 *  @def    SSE2_FLAG      
 *  @brief  SSE2 support bitmask flag for CPUID command
 *  
 *  @def    SS_FLAG         
 *  @brief  SS support bitmask flag for CPUID command
 *  
 *  @def    HTT_FLAG        
 *  @brief  HTT support bitmask flag for CPUID command
 *  
 *  @def    TM_FLAG         
 *  @brief  TM support bitmask flag for CPUID command
 *  
 *  @def    IA64_FLAG       
 *  @brief  IA64 support bitmask flag for CPUID command
 *  
 *  @def    PBE_FLAG        
 *  @brief  PBE support bitmask flag for CPUID command
 *  
 *  @def    SSE3_FLAG       
 *  @brief  SSE3 support bitmask flag for CPUID command
 *  
 *  @def    MONITOR_FLAG    
 *  @brief  MONITOR support bitmask flag for CPUID command
 *  
 *  @def    DS_CPL_FLAG     
 *  @brief  DS_CPL support bitmask flag for CPUID command
 *  
 *  @def    EIST_FLAG       
 *  @brief  EIST support bitmask flag for CPUID command
 *  
 *  @def    TM2_FLAG        
 *  @brief  TM2 support bitmask flag for CPUID command
 *  
 *  @def    CID_FLAG        
 *  @brief  CID support bitmask flag for CPUID command
 *  
 *  @def    CX16_FLAG       
 *  @brief  CX16 support bitmask flag for CPUID command
 *  
 *  @def    XTPR_FLAG       
 *  @brief  XTPR support bitmask flag for CPUID command
 *  
 *  @def    LAHF_FLAG       
 *  @brief  LANF support bitmask flag for CPUID command
 *  
 *  @def    SYSCALL_FLAG    
 *  @brief  SYSCALL support bitmask flag for CPUID command
 *  
 *  @def    XD_FLAG         
 *  @brief  XD support bitmask flag for CPUID command
 *  
 *  @def    EM64T_FLAG      
 *  @brief  EM64T support bitmask flag for CPUID command
 * 
 *  @def    IOPL_FLAG
 *          IOPL bitmask for the EFLAGS register
 *  
 *  @def    INTEL_CELERON               
 *  @brief  Celeron Brand ID for CPUID command
 *  
 *  @def    INTEL_PENTIUM_III           
 *  @brief  PIII Brand ID for CPUID command
 *  
 *  @def    INTEL_PENTUMM_III_XEON      
 *  @brief  PIII Xeon Brand ID for CPUID command
 *  
 *  @def    INTEL_PENTIUM_III_PROCESSOR 
 *  @brief  PIII Brand ID for CPUID command
 *  
 *  @def    MOBILE_INTEL_PENTIUM_III_M  
 *  @brief  Mobile PIII Brand ID for CPUID command
 *  
 *  @def    MOBILE_INTEL_CELERON        
 *  @brief  Mobile Celeron Brand ID for CPUID command
 *  
 *  @def    INTEL_PENTIUM_4_GENUINE     
 *  @brief  P4 Brand ID for CPUID command
 *  
 *  @def    INTEL_PENTIUM_4             
 *  @brief  P4 Brand ID for CPUID command
 *  
 *  @def    INTEL_CELERON_PROCESSOR     
 *  @brief  Celeron Brand ID for CPUID command
 *  
 *  @def    INTEL_XEON                  
 *  @brief  Xeon Brand ID for CPUID command
 *  
 *  @def    INT_XEON_MP               
 *  @brief  Xeon MP Brand ID for CPUID command
 *  
 *  @def    MOBILE_PENTUM_4_M           
 *  @brief  P4M Brand ID for CPUID command
 *  
 *  @def    MOBILE_INTEL_CELERON_2      
 *  @brief  Celeron Brand ID for CPUID command
 *  
 *  @def    MOBILE_GENUINE_INTEL        
 *  @brief  Genuine Brand ID for CPUID command
 *  
 *  @def    INTEL_CELERON_M             
 *  @brief  Celeron Brand ID for CPUID command
 *  
 *  @def    MOBILE_INTEL_CELERON_3      
 *  @brief  Mobile Celeron Brand ID for CPUID command
 *  
 *  @def    INTEL_CELERON_2             
 *  @brief  Celeron Brand ID for CPUID command
 *  
 *  @def    MOBILE_GENUINE_INTEL_2      
 *  @brief  Mobile Genuine Brand ID for CPUID command
 *      
 *  @def    INTEL_PENTIUM_M_2           
 *  @brief  Pentium M Brand ID for CPUID command
 *  
 *  @def    MOBILE_INTEL_CELERON_4      
 *  @brief  Mobile Celeron Brand ID for CPUID command
 * 
 *  @def    SYSTEM_BUS_BITMASK
 *  @brief  Bitmask to bring out the System Bus Speed when using the rdmsr command 0x2C
 * 
 *  @def    LOGICAL_CPU_COUNT_BITMASK
 *  @brief  Bitmask to pull out the logical CPU count if Hyperthreading is enabled.
 */






/*          sample forking example
    for (i=0; i < cpu_count; i++) 
    {
        new_pid = fork();
        waitpid(0,&child_status,0);     //wait for and ensure that my child is actually the one exiting...

        if(new_pid == 0) //child process
        {
            if ((affinity_result = sched_setaffinity(new_pid,affinity_mask_length,&affinity_mask[i])) == 0) 
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
*/


