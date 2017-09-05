
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       driveTest.h
 *
 *  @brief      Functional Drive Test
 *
 *  @return     an integer specifying PASS(1) or FAIL(0)
 *
 *              Copyright (C) 2006 @n@n
 *              This program is intended to be a full functional test of 
 *              drives
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
#ifndef DRIVETEST_H
#define DRIVETEST_H


typedef struct
{
    char **devices;
    char **deviceNames;
} drives;


#define MAX_SIZE 1024*1024

char *sizeStructure[] = {"B/s","kB/s","MB/s","GB/s","TB/s"};


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void allocateDeviceStructure(drives *deviceStructure, 
 *                            unsigned int maxDevices, 
 *                            unsigned int maxDeviceNameLength);
 *
 *  @arg    <b>drives </b> *deviceStructure
 *          - drive structure that contains the device location and name
 *
 *  @arg    <b>unsigned int</b> maxDevices
 *          - Maximum number of devices, so we can allocate our array
 *
 *  @arg    <b>unsigned int</b> maxDeviceNameLength
 *          - Maximum number of characters in device names
 *
 *  @brief  Allocates memory for a device structure array, so that we can
 *          cleanly send our device names and locations to our test functions
 *  
 * @note    call freeDeviceStructure following our tests
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void allocateDeviceStructure(drives *deviceStructure, 
                             unsigned int maxDevices, 
                             unsigned int maxDeviceNameLength);



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void freeDeviceStructure(drives *deviceStructure,
                         unsigned int maxDevices);
 *
 *  @arg    <b>drives </b> *deviceStructure
 *          - drive structure that contains the device location and name
 *
 *  @arg    <b>unsigned int</b> maxDevices
 *          - Maximum number of devices, so we can deallocate our array
 *
 *
 *  @brief  De-allocates memory for a device structure array
 *  
 *  @note   should be called following allocateDeviceStructure
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void freeDeviceStructure(drives *deviceStructure,
                         unsigned int maxDevices);



////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void testBlockDevice(char *device, char *deviceName)
 *
 *  @arg    <b>char </b> *device
 *          - Pointer to string carrying the location of our device to test
 *
 *  @arg    <b>char</b> *deviceName
 *          - Name of the device we are testing
 *
 *  @arg    <b>short</b> size
 *          - user specified size to write to device
 *
 *  @brief  Tests the block device by first locating the center and end
 *          blocks, then performing a read/write/restore test on each
 *          location
 *          
 *          The read/restore/restore test should allow functioning drives
 *          to be returned to the customer without any data loss
 *
 *          Performance results are returned from the tests. They are printed
 *          to the detailed view 
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void testBlockDevice(char *device, char *deviceName,unsigned int  size);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     int detectIdeDevices(char **ideDevices, char **ideDeviceNames)
 *
 *  @arg    <b>char </b> **ideDevices
 *          - Two dimensional array that will contain the device location
 *            for all detected ide drives
 *
 *  @arg    <b>char</b> **ideDeviceNames
 *          - Two dimensional array which will contain the names of 
 *            located devices
 *
 *  @brief  detectIdeDevices first analyzes /proc/devices to located all
 *          ide devices. The device number is taken, then used to crossreference
 *          the device name from /proc/diskstats, which will have the device
 *          location. The following is an example to better illustrate the process
 *              /proc/devices
 *                  3  ide0
 *                  4  ide1
 *          Three and four, whose names are saved, will be cross referenced
 *          with /proc/diskstats to obtain the device location
 *                  3   0 /dev/hda
 *                  3   1 /dev/hda1
 *                  4   0 /dev/hdc
 *          Note, that in /proc/diskstats contains partitions. These are ignored
 *          /proc/devices does not contain patition information
 *  
 * @note    Sata drives may be listed as /dev/sda, but will most likely list
 *          as ideX, where X is the device number. This is implementation dependent.
 *          The varying factor will be the sata driver. A sata drive may
 *          also be listed as sd, since it's may also be seen as a scsi device.
 *          This is a known issue in some of the 2.6.* kernels. I believe this
 *          has been standardized in the later versions. Nonetheless, the drive
 *          should be detected one way or another.
 *
 */ 
/////////////////////////////////////////////////////////////////////////
void detectAndTestDrives(unsigned int  size);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     void testIndividualDevice(unsigned int size,char *deviceLocation)
 *
 *  @arg    <b>unsigned int </b> size
 *          - Size to test device
 *
 *  @arg    <b>char</b> *deviceLocation
 *          - location ( or physical path ) to device that needs to be tested
 *
 *  @brief  Tests an individual device, as opposed to a range, or many devices,
 *          that are tested in the detection process in detectAndTestDrives.
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
void testIndividualDevice(unsigned int size,char *deviceLocation);
#endif
