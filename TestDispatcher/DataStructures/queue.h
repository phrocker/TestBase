///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       queue.h
 *
 *  @brief      Test Execution suite
 *
 *              Copyright (C) 2006 @n@n
 *              Function definitions for basic queue data structure
 *
 *              **** Note: Nothing in this file needs to be called directly. It should not
 *              be included in your source code. **** 
 *  
 *  @author     Marc Parisi
 *  @author     marc.parisi@gmail.com                                                
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
#ifndef QUEUE_H
#define QUEUE_H 1


/*! \file
    \brief Prototypes for basic queue data structure

    **** Note: Nothing in this file needs to be called directly. It should not
    be included in your source code. ****

*/   

#include <stdio.h>
#include <stdlib.h>

 /*! \struct command queue.h
   \brief Command structure used when data is received from the client. Data
   is populated with connection specific information such as the socket's file
   descriptor and the connection type

    Structure can hold the thread number, the socket type, socket file descriptor,
    connection number, and the packet data.
*/
  typedef struct {
    int threadNumber;
    short type;
    uint *fd;
    uint connection;
    char *data;
  } command;

  struct QueueDataPacket;



  typedef struct QueueDataPacket *Queue;


/*! \fn char isQueueEmpty(Queue inputQueue)
    \brief Returns whether or not the input queue is empty
    \param inputQueue Input queue
    \return 1 or 0, for empty or not empty, respectively
*/
char isQueueEmpty(Queue inputQueue);

/*! \fn char isQueueFull(Queue inputQueue)
    \brief Returns whether or not the input queue is empty
    \param inputQueue Input queue
    \return 1 or 0, for empty or not empty, respectively
*/
char isQueueFull(Queue inputQueue);

/*! \fn Queue createAndInitializeQueue(int elements,char growOnDemand)
    \brief Creates and initializes queue with the maximum number of elements
    \param elements The number of initialized queue elements. Can grow beyond
    this if growOnDemand is set to 1
    \param growOnDemand Value to help determine if queue will grow on demand
    \return Initialized queue
*/
Queue createAndInitializeQueue(unsigned short elements,char growOnDemand);

/*! \fn Queue destroyQueueAndFreeElements(Queue inputQueue)
    \brief Destroys and frees all memory associated with the inputQueue
    \param inputQueue Input queue
    \return void
*/
void destroyQueue(Queue inputQueue);

/*! \fn Queue initializeQueue(Queue inputQueue)
    \brief Initializes memory for our queue
    \param inputQueue Input queue
    \param startIndex Index at which to begin initializing data
    \return Returns the newly emptied queue
*/
Queue initializeQueue(Queue inputQueue, int startIndex);

/*! \fn void addElementToQueue(char *data,short size,int i, short type,uint threadNumber,uint fd, Queue inputQueue)
    \brief Adds data to the queue
    \param data Data to add to queue
    \param size Size of data
    \param i Index in queue
    \param type Data type
    \param threadNumber Thread number to which the queue belongs
    \param fd Queue's file descriptor
    \param inputQueue Input queue
    \return void
*/
void addElementToQueue(char *data,short size,int i, short type,uint threadNumber,uint fd, Queue inputQueue);

/*! \fn void incrementIndex(unsigned short *index,unsigned short *capacity)
    \brief Safely increments the index, but not beyond the capacity. It rolls over
    to index 0, if the capacity is exceeded
    \param index Pointer to index
    \param capacity Poiner to capacity
    \return void
*/

void incrementIndex(unsigned short *index,unsigned short *capacity);

/*! \fn command *getTopElement(Queue inputQueue)
    \brief Returns, and dequeues, the top element of the queue
    \param inputQueue Input queue
    \return The command at the top of the queue
*/
command *getFrontElement(Queue inputQueue);

/*! \fn command *getTopElement(Queue inputQueue)
    \brief Returns, but does not dequeue, the top element of the queue
    \param inputQueue Input queue
    \return The command at the top of the queue
*/
command *peekAtTopElement(Queue inputQueue);

/*! \fn void increaseQueueCapacity(Queue inputQueue,unsigned short increaseAmount)
    \brief Dynamically increases the queue capacity
    \param inputQueue Input queue
    \param increaseAmount Number to which we will increase inputQueue's size
    \return void
*/
void increaseQueueCapacity(Queue inputQueue,unsigned short increaseAmount);


#endif 
