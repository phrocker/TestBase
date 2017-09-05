
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       floppyDeviceLib.c
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

#include "../CommonLibrary/Common.h"
#include "ideDeviceLib.h"
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int detectFloppyDevices(char **ideDevices, char **ideDeviceNames)
 */
/////////////////////////////////////////////////////////////////////////////
int detectFloppyDevices(char **ideDevices, char **ideDeviceNames)
{
    // first, open /proc/devices to locate all block devics that begin with ide
    int deviceNumbers[MAXIMUM_DEVICE_COUNT]={0};
    FILE *fd = fopen(DEVICE_FILE,"r");
    if (fd == NULL)
    {
        return 0;
    }
    
    char buffer[256];
    int deviceCount=0;
    int deviceNumber=0,ideDeviceNumber=0;
    char sstring[3];
    while(fgets(buffer,sizeof(buffer),fd))
    {
        // scan for the ide string
        
        if (sscanf (buffer, "%i %2s", &deviceNumber,sstring)  == 2)
        {
            if (!strcmp(sstring,"fd"))
            {
                
                if (ideDeviceNumber != deviceCount)
                {
                    consolePrint("out of order\n");
                    return 0;
                }
                // record the device number
                deviceNumbers[deviceCount] = deviceCount;
                
                snprintf(ideDeviceNames[deviceCount],MAX_DEVICE_STRING_LENGTH,"fd%i",deviceCount);
                deviceCount++;
            }
        }
    }
    fclose(fd);
    if (deviceCount == 0)
    {
        // no devices
        return 0;
    }

    fd = fopen(DISKSTAT_FILE,"r");
    if (fd == NULL)
    {
        return 0;
    }
    // use the device counter for locating devices
    short deviceCounter=0;
    char deviceCharacter=0,deviceType=0;
    short i=0;
    ideDeviceNumber=0;
    while(fgets(buffer,sizeof(buffer),fd))
    {
        // scan for the %cd%c string, which will be like hdc or sda
        if (sscanf (buffer, "%i %i %2s%i", &deviceNumber,&deviceCounter,&sstring,&ideDeviceNumber)  ==4 )
        {
            if (deviceCounter == 0 && deviceNumber == deviceNumbers[i] && !strcmp(sstring,"fd"))
            {
                // populate the device location string
                snprintf(ideDevices[i],MAX_DEVICE_STRING_LENGTH,"/dev/fd%i",deviceType,ideDeviceNumber);
                
                i++;
            }
        }
    }

    // let's remember to close the file
    fclose(fd);
    if (i != deviceCount)
    {
        deviceCount = i;
    }
    return deviceCount;

}
