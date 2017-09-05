///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       ideDeviceLib.h
 *
 *  @brief      Simple device library to handle ide devices interactions with
 *              he block device library
 *
 *              Copyright (C) 2006 @n@n
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

#ifndef IDEDEVICELIB_H
#define IDEDEVICELIB_H

#define MAXIMUM_DEVICE_COUNT 256
#define DEVICE_FILE "/proc/devices"
#define DISKSTAT_FILE "/proc/diskstats"
#define MAX_DEVICE_STRING_LENGTH 80


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
 *          as ideX, where X is the device number. This is implementation dependent.\
 *          The varying factor will be the sata driver.
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
int detectIdeDevices(char **ideDevices, char **ideDeviceNames);

#endif
