///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       BlockDeviceLib.c
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
#ifndef BLOCKDEVICELIB_H
#define BLOCKDEVICELIB_H

/* from <linux/fs.h> */
#define BLKRRPART  _IO(0x12,95)	/* re-read partition table */
#define BLKGETSIZE _IO(0x12,96)	/* return device size */
#define BLKFLSBUF  _IO(0x12,97)	/* flush buffer cache */
#define BLKSSZGET  _IO(0x12,104) /* get block device sector size */
#define BLKGETLASTSECT  _IO(0x12,108) /* get last sector of block device */
#define BLKSETLASTSECT  _IO(0x12,109) /* set last sector of block device */
#define BLKGETSIZE64 _IOR(0x12,114,size_t)	/* return device size in bytes (u64 *arg) */

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned int getDeviceBlockSize(int fd)
 *
 *  @arg    <b>int </b> fd
 *          - File descriptor to the device file
 *
 *  @return The block size of the input device
 *
 *  @brief  Performs an io control call (ioctl) to obtain the block size
 *          of the input device
 *  
 * @note    BLKSSZGET is the call to obtain the block size
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned int getDeviceBlockSize(int fd);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned long long getDeviceSize(int fd)
 *
 *  @arg    <b>unsigned long long </b> fd
 *          - File descriptor to the device file
 *
 *  @return  The size of the device in bytes. A long long is used since
 *          the device may exceed 4 GB. unsigned long long will support
 *          up to 18 exabytes, though, the hardware, nor the kernel, will
 *          support that
 *
 *  @brief  Performs an io control call (ioctl) to obtain the size of the
 *          device
 *  
 *  @note   BLKGETSIZE64 returns the size of the block. the block size
 *  
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned long long getDeviceSize(int fd);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     int readBlockToDevice(const char *device,
 *                      unsigned int *blockSize,
 *                      unsigned char *buffer,
 *                      unsigned long long *beginLocation,
 *                      unsigned int *size,
 *                      unsigned long *bytesPerSecond)
 *
 *  @arg    <b>const char </b> *device
 *          - string to device location
 *
 *  @arg    <b>unsigned char </b> *buffer
 *          - buffer that we will be populated with the data retrieved
 *            from the device
 *
 *  @arg    <b>const long long </b> *beginLocation
 *          - byte location from which we will begin reading
 *
 *  @arg    <b>unsigned int </b> *size
 *          - size of buffer we wish to read
 *
 *  @arg    <b>unsigned long </b> *bytesPerSecond
 *          - bytes per second read from the device
 *
 *
 *  @return  status of our read
 *
 *  @brief  Reads the device file
 *          
 *          If the device is successfully opened and the beginLocation and
 *          size do not exceed the size of the device and are block aligned,
 *          then we seek to beginLocation and read *size bytes, after which
 *          the device file is closed and performance statistics are calculated
 *  
 *  @note   The performance statistics are calculated by simply dividing the
 *          size of the write by the delta
 */ 
/////////////////////////////////////////////////////////////////////////
int readBlockToDevice(const char *device,
                       unsigned int *blockSize,
                       unsigned char *buffer,
                       unsigned long long *beginLocation,
                       unsigned int *size,
                       unsigned long *bytesPerSecond);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     int writeBlockToDevice(const char *device,
 *                      unsigned int *blockSize,
 *                      unsigned char *buffer,
 *                      unsigned long long *beginLocation,
 *                      unsigned int *size,
 *                      unsigned long *bytesPerSecond)
 *
 *  @arg    <b>const char </b> *device
 *          - string to device location
 *
 *  @arg    <b>unsigned char </b> *buffer
 *          - buffer that we will be populated with the data written
 *            to the device
 *
 *  @arg    <b>const long long </b> *beginLocation
 *          - byte location from which we will begin writing
 *
 *  @arg    <b>unsigned int </b> *size
 *          - size of buffer we wish to write
 *
 *  @arg    <b>unsigned long </b> *bytesPerSecond
 *          - bytes per second written to the device
 *
 *
 *  @return  status of our write
 *
 *  @brief  writes a buffer to  the device file
 *          
 *          If the device is successfully opened and the beginLocation and
 *          size do not exceed the size of the device and are block aligned,
 *          then we seek to beginLocation and write *size bytes, after which
 *          the device file is closed and performance statistics are calculated
 *  
 *  @note   The performance statistics are calculated by simply dividing the
 *          size of the write by the delta
 */ 
/////////////////////////////////////////////////////////////////////////
int writeBlockToDevice(const char *device,
                       unsigned int *blockSize,
                       unsigned char *buffer,
                       unsigned long long *beginLocation,
                       unsigned int *size,
                       unsigned long *bytesPerSecond);


////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     int writeAndRestoreBlock(const char *device,
 *                      unsigned int *blockSize,
 *                      unsigned char *buffer,
 *                      unsigned long long *beginLocation,
 *                      unsigned int *size,
 *                      unsigned long *readPerformance,
 *                      unsigned long *writePerformance);
 *
 *  @arg    <b>const char </b> *device
 *          - string to device location
 *
 *  @arg    <b>unsigned char </b> *buffer
 *          - buffer that we will be populated with the data written
 *            to the device
 *
 *  @arg    <b>const long long </b> *beginLocation
 *          - byte location from which we will begin writing
 *
 *  @arg    <b>unsigned int </b> *size
 *          - size of buffer we wish to write
 *
 *  @arg    <b>unsigned long </b> *bytesPerSecond
 *          - bytes per second written to the device
 *
 *
 *  @return  status of our write
 *
 *  @brief  writes a buffer to  the device file
 *          
 *          If the device is successfully opened and the beginLocation and
 *          size do not exceed the size of the device and are block aligned,
 *          then we seek to beginLocation and write *size bytes, after which
 *          the device file is closed and performance statistics are calculated
 *  
 *  @note   The performance statistics are calculated by simply dividing the
 *          size of the write by the delta
 */ 
/////////////////////////////////////////////////////////////////////////
int writeAndRestoreBlock(const char *device,
                       unsigned int *blockSize,
                       unsigned char *buffer,
                       unsigned long long *beginLocation,
                       unsigned int *size,
                       unsigned long *readPerformance,
                       unsigned long *writePerformance);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned long long getDeviceCenterSector(char *device);
 *
 *  @arg    <b>char </b> *device
 *          - string to device location
 *
 *  @return center byte location
 *
 *  @brief  Gets the total size of the device, then divides that by two
 *          to obtain the central sector.
 * 
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned long long getDeviceCenterSector(char *device);

////////////////////////////////////////////////////////////////////////
/** 
 *  @fn     unsigned long long getDeviceFinalSector(char *device);
 *
 *  @arg    <b>char </b> *device
 *          - string to device location
 *
 *  @return final block
 *
 *  @brief  Gets the total size of the device, subtracts the block size
 *          to obtain the final sector in th device
 * 
 */ 
/////////////////////////////////////////////////////////////////////////
unsigned long long getDeviceFinalSector(char *device, unsigned int desiredSize);


unsigned int calculateNearestBlockSize(char *device, unsigned int size, unsigned int *blockSize);

#endif

