///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       BlockDeviceLib.c
 *
 *  @brief      Device library to handle read/writes, and other miscellaneous
 *              functions for device libraries
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
#include "BlockDeviceLib.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned int getDeviceBlockSize(int fd)
 */
/////////////////////////////////////////////////////////////////////////////
unsigned int getDeviceBlockSize(int fd)
{
    unsigned int blockSize=0;
    // perform io control calls
    ioctl (fd, BLKSSZGET, &blockSize);
    return blockSize;
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long long getDeviceSize(int fd)
 */
/////////////////////////////////////////////////////////////////////////////
unsigned long long getDeviceSize(int fd)
{
    // get the number of bytes
    unsigned long long deviceSize=0;
    ioctl (fd, BLKGETSIZE64, &deviceSize);
    
    return deviceSize;
    
}


/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int readBlockToDevice(const char *device,
 *                      unsigned int *blockSize,
 *                      unsigned char *buffer,
 *                      unsigned long long *beginLocation,
 *                      unsigned int *size,
 *                      unsigned long *bytesPerSecond)
 */
/////////////////////////////////////////////////////////////////////////////
int readBlockToDevice(const char *device,
                       unsigned int *blockSize,
                       unsigned char *buffer,
                       unsigned long long *beginLocation,
                       unsigned int *size,
                       unsigned long *bytesPerSecond)
{
    int readSize=0;
    unsigned int reportedBlockSize;
    int fd = open(device,O_RDWR); // be able to read and write to it
   

    if (fd < 0) // make sure we can read the device file
    {
        perror("open");
        return FALSE;
    }
    else
    {
        // if block size is not equal to the device block size
        // then print an error an exit
        if (*blockSize != getDeviceBlockSize(fd))
        {
            consolePrint("Input block size not equal to device block size\n");
            close(fd);
            goto closeAndReturn;
        }
        // if our location is greater than the device size
        // print an error an exit
        
        if ((*beginLocation+*size) > getDeviceSize(fd))
        {
            consolePrint("Begin read location is beyond device size\n");
            close(fd);
            goto closeAndReturn;
        }
        // seek to the location
        lseek64(fd,*beginLocation,SEEK_SET);
        unsigned long beginReadTime,finishReadTime,beginReadTimeSec,finishReadTimeSec;

        ioctl (fd, BLKFLSBUF);
        beginReadTimeSec = time(NULL);
        beginReadTime = getMicroSeconds();
        
        readSize = read(fd,buffer,*size);
        
        // close the file to obtain the performace of the full read
        // since close will also flush the buffer
        
        
        close(fd);
        finishReadTime = getMicroSeconds();
        finishReadTimeSec = time(NULL);
        
        
        if (readSize >= 0 )
        {
            // obtain the bytes per second written
            
            
            unsigned long long divisor = (finishReadTime-beginReadTime) == 0 ? 1 : (finishReadTime-beginReadTime);
            divisor += ((finishReadTimeSec-beginReadTimeSec)*1000000);
            *bytesPerSecond = ((double)(*size)/(double)divisor)*1000000;
            
        }
        else
        {
            perror("read");
            *bytesPerSecond =0;
        }
        
        
    }
    
    closeAndReturn:
    return readSize;
}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int writeBlockToDevice(const char *device,
 *                      unsigned int *blockSize,
 *                      unsigned char *buffer,
 *                      unsigned long long *beginLocation,
 *                      unsigned int *size,
 *                      unsigned long *bytesPerSecond)
 */
/////////////////////////////////////////////////////////////////////////////
int writeBlockToDevice(const char *device,
                       unsigned int *blockSize,
                       unsigned char *buffer,
                       unsigned long long *beginLocation,
                       unsigned int *size,
                       unsigned long *bytesPerSecond)
{
    int writtenSize=0;
    int fd = open(device,O_RDWR); // be able to read and write to it
    if (fd < 0)
    {
        perror("open");
        return FALSE;
    }
    else
    {
        // if block size is not equal to the device block size
        // then print an error an exit
        if (*blockSize != getDeviceBlockSize(fd))
        {
            consolePrint("Input block size not equal to device block size\n");
            close(fd);
            goto closeAndReturn;
        }
        // if our location is greater than the device size
        // print an error an exit
        if ((*beginLocation+*size) > getDeviceSize(fd))
        {
            consolePrint("Begin read location is beyond device size\n");
            close(fd);
            goto closeAndReturn;
        }

        lseek64(fd,*beginLocation,SEEK_SET);

        unsigned long beginWriteTime,finishWriteTime,beginWriteTimeSec,finishWriteTimeSec;

        ioctl (fd, BLKFLSBUF);
        beginWriteTimeSec = time(NULL);
        // get the current number of micro seconds
        
        beginWriteTime =  getMicroSeconds();

        
        writtenSize = write(fd,buffer,*size);
        
        // close the file to obtain the performace of the full write
        // since close will also flush the buffer
        close(fd);

        finishWriteTime = getMicroSeconds();
        finishWriteTimeSec = time(NULL);
        
        
        if (writtenSize >= 0 )
        {
            // obtain the bytes per second written
            unsigned long long divisor =  (finishWriteTime-beginWriteTime) == 0 ? 1 : (finishWriteTime-beginWriteTime); 
            divisor += ((finishWriteTimeSec-beginWriteTimeSec)*1000000);
            *bytesPerSecond = ((double)(*size)/(double)divisor)*1000000;
        }
        else
        {
            perror("write");
            *bytesPerSecond =0;
        }      
        
    }

    closeAndReturn:
    return writtenSize;

}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     int writeAndRestoreBlock(const char *device,
 *                      unsigned int *blockSize,
 *                      unsigned char *buffer,
 *                      unsigned long long *beginLocation,
 *                      unsigned int *size,
 *                      unsigned long *readPerformance, 
 *                      unsigned long *writePerformance)
 */
/////////////////////////////////////////////////////////////////////////////
int writeAndRestoreBlock(const char *device,
                       unsigned int *blockSize,
                       unsigned char *buffer,
                       unsigned long long *beginLocation,
                       unsigned int *size,
                       unsigned long *readPerformance, 
                       unsigned long *writePerformance)
{
    if (*size%*blockSize)
    {
        consolePrint("Size is not a factor of the block size\n");
        return FALSE;
    }
    
    char *blockDevice=(char*)malloc(*size),*compareDevice=(char*)malloc(*size);

    if (!blockDevice)
        noMemoryHalt();
    
    // buffer the current data at the location we are writing tos
    
    readBlockToDevice(device,blockSize,blockDevice,beginLocation,size,readPerformance);
    
    // overwrite the previous data
    writeBlockToDevice(device,blockSize,buffer,beginLocation,size,writePerformance);

    // read what was written
    readBlockToDevice(device,blockSize,compareDevice,beginLocation,size,readPerformance);
    
    if (memcmp(buffer,compareDevice,*size))
    {
        consolePrint("Regions not equal, OR read and/or write failed\n");
        free(blockDevice);
        free(compareDevice);
        return FALSE;
    }
    unsigned long writeTemp = *writePerformance;   
    // finally, restore the data that we buffered earlier, so we can return
    // the device to the customer in the same condition we received it...assuming, of course,
    // that the device is not faulty
    writeBlockToDevice(device,blockSize,blockDevice,beginLocation,size,writePerformance);
    *writePerformance = (writeTemp+*writePerformance)/2;

    // free our memory
    free(blockDevice);
    free(compareDevice);
    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long long getDeviceCenterSector(char *device)
 */
/////////////////////////////////////////////////////////////////////////////
unsigned long long getDeviceCenterSector(char *device)
{
    
    int fd = open(device,O_RDONLY);
    if (fd < 0)
    {
        return 0LL;
    }
    
    // divide the total size by two to get the block size
    // this isn't block aligned, but probably should be
    
    unsigned long long center = getDeviceSize(fd)/2LL;

    close(fd);

    return center;

}

/////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     unsigned long long getDeviceFinalSector(char *device)
 */
/////////////////////////////////////////////////////////////////////////////
unsigned long long getDeviceFinalSector(char *device,unsigned int desiredSize)
{
    // open the device 
    int fd = open(device,O_RDONLY);
    if (fd < 0)
    {
        return 0LL;
    }
    // gets the final sector by subtracting a single block from the total
    // size of the file
    int subtractSize = getDeviceBlockSize(fd);
    if (desiredSize > subtractSize)
    {
        subtractSize=desiredSize;
        
    }
    unsigned long long center = getDeviceSize(fd)-subtractSize;

    close(fd);

    return center;

}

unsigned int calculateNearestBlockSize(char *device, unsigned int size, unsigned int *blockSize)
{

    int fd = open(device,O_RDONLY);
    if (fd < 0)
    {
        return 0;
    }

    *blockSize= getDeviceBlockSize(fd);
    int returnSize =  (int)(((double)size/(double)getDeviceBlockSize(fd))+0.5)*getDeviceBlockSize(fd);
    if (returnSize == 0)
    {
        returnSize = getDeviceBlockSize(fd);
    }
    close(fd);
    return returnSize;

}
