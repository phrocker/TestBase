
///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       driveTest.c
 *
 *  @brief      Functional Drive Test
 *v
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
#include "argtable2.h"
#include "../CommonLibrary/Common.h"
#include "driveTest.h"
#include <fcntl.h>

#include "BlockDeviceLib.h"
#include "ideDeviceLib.h"


#define MYVERSION 0.01

int main(int argc, char *argv[])
{
    iopl(3);

    struct arg_lit *help,*skipL2,*skipL3,*htt;
    struct arg_int *L2Size,*L3Size,*speed, *size;
    struct arg_str *retryarg,*deviceLocation;
    struct arg_end *end;
    
	setTestVersion(MYVERSION);

    // create argument table
     void *argtable[] = {
         deviceLocation = arg_str0("d","device","/dev/hda","Specify an individual device to test."),        
         arg_rem(NULL,"If not set, driveTest will locate devices"),
         help        = arg_lit0("h","help","Displays usage information"),
         size        = arg_int0("s","size","[0-9]","Size of buffer to be written, in bytes"),
         arg_rem(NULL,"If not specified, the blocksize will be used"),
         end         = arg_end(20)
    };

    if (arg_nullcheck(argtable) != 0)
    {
        consolePrint("ERROR! Insufficient memory\n");
        exit(1);
    }

    // parse arguments
    int errors = arg_parse(argc,argv,argtable);

    if (errors)
    {
        arg_print_errors(stdout,end,"driveTest");
        testPrint("Drive Test");
        failedMessage();
        displayUsage(argtable,"driveTest",MYVERSION);
        goto exit_program;
    }


    if (help->count > 0)
    {
        return displayUsage(argtable,"driveTest",MYVERSION);
    }

    unsigned int  passedSize=512;

    if (size->count > 0)
    {
        passedSize = size->ival[0];
        if (passedSize > MAX_SIZE)
        {
            testPrint("Drive Test");
            failedMessage();
            diagnosticPrint("Size is greater than %u\n",MAX_SIZE);
            goto exit_program;
        }
    }

    if (deviceLocation->count > 0)
    {   
            testIndividualDevice(passedSize,(char*)deviceLocation->sval[0]);
    }
    else
    {
            detectAndTestDrives(passedSize);
    }

    exit_program:

    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void testIndividualDevice(unsigned int size,char *deviceLocation)
 */
/////////////////////////////////////////////////////////////////////////////
void testIndividualDevice(unsigned int size,char *deviceLocation)
{
    if (!deviceLocation)
    {
        testPrint("Drive Test");
        failedMessage();
        diagnosticPrint("No drives detected, failing\n");
    }
    else
        testBlockDevice(deviceLocation,deviceLocation,size);
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void detectAndTestDrives(short size)
 */
/////////////////////////////////////////////////////////////////////////////
void detectAndTestDrives(unsigned int  size)
{

    drives ideDevices;
    
    allocateDeviceStructure(&ideDevices,MAXIMUM_DEVICE_COUNT,MAX_DEVICE_STRING_LENGTH);

    int foundIdeDevices = detectIdeDevices(ideDevices.devices,ideDevices.deviceNames);
    short i=0;
    for (; i < foundIdeDevices; i++)
    {
        // test them as block devices
        testBlockDevice(ideDevices.devices[i],ideDevices.deviceNames[i],size);
    }


    int foundFloppyDevices = detectFloppyDevices(ideDevices.devices,ideDevices.deviceNames);
    
    for (i=0; i < foundFloppyDevices; i++)
    {
        // test them as block devices
        testBlockDevice(ideDevices.devices[i],ideDevices.deviceNames[i],size);
    }


    int foundScsiDevices = detectScsiDevices(ideDevices.devices,ideDevices.deviceNames);

    for (i=0; i < foundScsiDevices; i++)
    {
        // test them as block devices
        testBlockDevice(ideDevices.devices[i],ideDevices.deviceNames[i],size);
        
    }



    freeDeviceStructure(&ideDevices,MAXIMUM_DEVICE_COUNT);    

    if ( ! (foundFloppyDevices|foundIdeDevices|foundScsiDevices) )
    {
        testPrint("Drive Test");
        failedMessage();
        diagnosticPrint("No drives detected, failing\n");
    }

    

    


      


}


/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void testBlockDevice(char *device, char *deviceName,short size)
 */
/////////////////////////////////////////////////////////////////////////////
void testBlockDevice(char *device, char *deviceName, unsigned int  size)
{

    int blockSize = 0;

    int calculatedSize=calculateNearestBlockSize(device,size,&blockSize);

    if (calculatedSize == 0)
    {
        testPrint("%s Drive Test",device);
        failedMessage();
        diagnosticPrint("%s is not a valid device\n",device);
        return;
    }

    char *blocks = (char*)malloc(calculatedSize);

    if (!blocks)
        noMemoryHalt();
    // fill the block
    if (calculatedSize != size)
    {
        diagnosticPrint("%u adjusted to %u\n",size,calculatedSize);
    }

    
    
    fillRandomArray(blocks,calculatedSize);


    unsigned long readp=0,writep=0;
    unsigned long readpFirst=0, writepFirst=0;
    unsigned long readpSecond=0,writepSecond=0;
    unsigned long readpThird=0, writepThird=0;
    
    // get the center and final blocks to test the drive thouroughly
    unsigned long long seekBegin = 0,seekMiddle=getDeviceCenterSector(device),seekEnd=getDeviceFinalSector(device,calculatedSize);
    testPrint("%s drive test",deviceName);
    // write and restore three times
    if ( (writeAndRestoreBlock(device,&blockSize,blocks,&seekBegin,&calculatedSize,&readpFirst,&writepFirst)  |
          writeAndRestoreBlock(device,&blockSize,blocks,&seekMiddle,&calculatedSize,&readpSecond,&writepSecond)  |
          writeAndRestoreBlock(device,&blockSize,blocks,&seekEnd,&calculatedSize,&readpThird,&writepThird) 
          ) == FALSE)
        failedMessage();
    else
        passedMessage();
    free(blocks);
    short i=0;
    char *ptr=sizeStructure[0];
    
    readp = (readpFirst+readpSecond+readpThird)/3L;
    
    while(readp > 1000)
    {
        i++;
        readp /= 1000;
        ptr = sizeStructure[i];
        
    }
    // performance characteristics are commented out because they may
    // be wildly inaccurate
  //  diagnosticPrint("%s estimated performance:\n",deviceName);
//    diagnosticPrint("  Read:  %u %s\n",readp,ptr);
    i=0;
    writep = (writepFirst+writepSecond+writepThird)/3L;
    ptr=sizeStructure[0];
    while(writep > 1000)
    {
        i++;
        writep/=1000;
        ptr = sizeStructure[i];
        
    }
    //diagnosticPrint("  Write:  %u %s\n",writep,ptr);
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void allocateDeviceStructure(drives *deviceStructure, 
 *                            unsigned int maxDevices, 
 *                            unsigned int maxDeviceNameLength)
 */
/////////////////////////////////////////////////////////////////////////////
void allocateDeviceStructure(drives *deviceStructure, 
                             unsigned int maxDevices, 
                             unsigned int maxDeviceNameLength)
{
    

    short j;
    deviceStructure->devices = (char**)malloc(sizeof(char*)*maxDevices);
    for (j=0; j < maxDevices; j++)
    {
        deviceStructure->devices[j] = (char*)malloc(maxDeviceNameLength);
        if (!deviceStructure->devices[j])
            noMemoryHalt();
    }


    deviceStructure->deviceNames = (char**)malloc(sizeof(char*)*maxDevices);
    for (j=0; j < maxDevices; j++)
    {
        deviceStructure->deviceNames[j] = (char*)malloc(maxDeviceNameLength);
        if (!deviceStructure->deviceNames[j])
            noMemoryHalt();
    }
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void freeDeviceStructure(drives *deviceStructure,
 *                        unsigned int maxDevices)
 */
/////////////////////////////////////////////////////////////////////////////
void freeDeviceStructure(drives *deviceStructure,
                         unsigned int maxDevices)
{
    short j=0;
    for (j=0; j < maxDevices; j++)
    {
        free(deviceStructure->devices[j]);
    }
    free(deviceStructure->devices);

    for (j=0; j < maxDevices; j++)
    {
        free(deviceStructure->deviceNames[j]);
    }
    free(deviceStructure->deviceNames);
}
