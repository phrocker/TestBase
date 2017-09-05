///////////////////////////////////////////////////////////////////////////
/** 
 *  @file       queue.c
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
#include "queue.h"

#include <stdlib.h>

#define FALSE 0
#define TRUE 1

/*! \file
    \brief Function definitions for basic queue data structure

    **** Note: Nothing in this file needs to be called directly. It should not
    be included in your source code. ****


*/   

/*! \struct QueueDataPacket queue.c
   \brief QueueDataPacket is the common data structure used throughout this queue
   It contains data regarding the capacity, size, and front and Rear indexes. In
   addition, a pointer to the command, localDataStructure, is used to indicate
   to house the data in our queue

       
*/
struct QueueDataPacket {
  unsigned short currentCapacity;
  unsigned short queueLock;
  unsigned short currentFrontIndex;
  unsigned short currentRearIndex;
  unsigned short currentSize;
  unsigned char  growOnDemand;
  command *localDataStructure;
};



/************************************************************************************
*
*	isQueueEmpty
*
*	Returns whether or not the inputQueue is empty
*      
*	Arguments:
*
*      Queue inputQueue -
*
*	Return Value:
*
*		YES or NO
*
*************************************************************************************/
char isQueueEmpty(Queue inputQueue) {
    // return whether or not the queue is empty
  return (inputQueue->currentSize <= 0);
}

/************************************************************************************
*
*	isQueueFull
*
*	Returns whether or not the inputQueue is full
*      
*	Arguments:
*
*      Queue inputQueue -
*
*	Return Value:
*
*		YES or NO
*
*************************************************************************************/
char isQueueFull(Queue inputQueue) {
    // return whether the current size
    // is greater than or equal to the capacity
  return (inputQueue->currentSize >= inputQueue->currentCapacity);
}

/************************************************************************************
*
*	createAndInitializeQueue
*
*	Creates and initializes data for our queue
*      
*	Arguments:
*
*      unsigned short elements - the number of elements for our queue
*      char growOnDemand - Tells us whether or not the queue should grow if the capacity is reached
*
*	Return Value:
*
*		The initialized queue
*
*************************************************************************************/
Queue createAndInitializeQueue(unsigned short elements,char growOnDemand)
{
  Queue newQueue;

  // allocate memory for our queue
  newQueue = malloc (sizeof(struct QueueDataPacket));

  if (newQueue== NULL) {
      exit(1); // return null pointer if memory isn't allocated
  }

  newQueue->localDataStructure = (command*)malloc( sizeof(command) * elements );

  if (newQueue->localDataStructure == NULL) {
      exit(1);// return null pointer if memory isn't allocated
  }

  // assign values to our new queue
  newQueue->currentCapacity=elements;

  newQueue->growOnDemand=growOnDemand; 

  newQueue->queueLock=FALSE;
  // make our queue empty
  return initializeQueue(newQueue,0);

}

/************************************************************************************
*
*	freeElements
*
*	Makes our queue empty
*      
*	Arguments:
*
*      unsigned short elements - the number of elements for our queue
*      char growOnDemand - Tells us whether or not the queue should grow if the capacity is reached
*
*	Return Value:
*
*		The initialized queue
*
*************************************************************************************/
Queue initializeQueue(Queue inputQueue, int startIndex)
{
    

    if (startIndex == 0)
    {
        inputQueue->currentSize=inputQueue->currentRearIndex=0;
        inputQueue->currentFrontIndex=1;
    }
  


  int i=0;
  for (i=startIndex; i < inputQueue->currentCapacity; i++) {
      inputQueue->localDataStructure[i].data = (char*)malloc(1*sizeof(char));
      inputQueue->localDataStructure[i].fd = (int*)malloc(1*sizeof(int));
      inputQueue->localDataStructure[i].threadNumber=-1;
  }
  
  return inputQueue;
}

/************************************************************************************
*
*	destroyQueue
*
*	Destroys the input queue, and frees all memory
*      
*	Arguments:
*
*      unsigned short elements - the number of elements for our queue
*      char growOnDemand - Tells us whether or not the queue should grow if the capacity is reached
*
*	Return Value:
*
*		The initialized queue
*
*************************************************************************************/
void destroyQueue(Queue inputQueue) {

    
    // if our input queue is not empty
    if (inputQueue != NULL) {
        int i=0;
        // free memory from 0 to the queue's capacity
        for (i=0; i < inputQueue->currentCapacity; i++) {
            // free memory for the data in our queue
            if (inputQueue->localDataStructure[i].data) {
                
                free(inputQueue->localDataStructure[i].data);
                free(inputQueue->localDataStructure[i].fd);
                inputQueue->localDataStructure[i].fd = NULL;
                inputQueue->localDataStructure[i].data=NULL;
            }

        }
        // if our array is not empty, free its memory
        // and assign the pointer to NULL
        if (inputQueue->localDataStructure) {
            free(inputQueue->localDataStructure);
            inputQueue->localDataStructure=NULL;
        }
        
        
        // now, free the memory for the entire queue
        // and assign its pointer to NULL
        free(inputQueue);
        inputQueue=NULL;
        
    }
}

void incrementIndex(unsigned short *index,unsigned short *capacity)
{
    // rollover minutes....
    // well, not exactly
    *index = (*index+1)%*capacity;

}

/************************************************************************************
*
*	increaseQueueCapacity
*
*	Increases the capacity of the queue
*      
*	Arguments:
*
*      Queue inputQueue - The input queue
*      unsigned short increaseAmount - Amount to which we will increase the size of
*       our queue
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void increaseQueueCapacity(Queue inputQueue,unsigned short increaseAmount)
{

    

    inputQueue->currentCapacity+=increaseAmount;

    inputQueue->localDataStructure = (command*)realloc(inputQueue->localDataStructure, sizeof(command) * inputQueue->currentCapacity );

    if (inputQueue->localDataStructure == NULL) {
        exit(1); // return null pointer if memory isn't allocated
    }

    initializeQueue(inputQueue,inputQueue->currentCapacity-increaseAmount-1);

    
}


/************************************************************************************
*
*	addElementToQueue
*
*	Adds a new data element to the queue
*      
*	Arguments:
*
*     char *data - localDataStructure of data
*     short size - Size of data array
*     int i     -  ?
*     short type - Type of data
*     uint threadNumber - Thread number to which this queue belongs
*     uint fd - Queue's file descriptor
*     Queue inputQueue - Input queue
*
*	Return Value:
*
*		void
*
*************************************************************************************/
void addElementToQueue(char *data,short size,int i, short type,uint threadNumber,uint fd, Queue inputQueue) {

    if (isQueueFull(inputQueue))
    {
        if (inputQueue->growOnDemand)
        {
          // effectively double the size of the capacity
          increaseQueueCapacity(inputQueue,inputQueue->currentCapacity);
        }
    }

    
    
    
    // increase the current size of our queue
    inputQueue->currentSize++;

    incrementIndex(&inputQueue->currentRearIndex,&inputQueue->currentCapacity);
    //inputQueue->Rear = Succ(inputQueue->currentRearIndex, inputQueue);
    // free data from the previously set packet
    free(inputQueue->localDataStructure[inputQueue->currentRearIndex].data);  
    // free data for the file descriptor in our packet
    free(inputQueue->localDataStructure[inputQueue->currentRearIndex].fd);
    //int i=0;
    // allocate memory for our data
    inputQueue->localDataStructure[inputQueue->currentRearIndex].data = (char*)malloc((size)*sizeof(char));
    // set data to all nulls
    memset(inputQueue->localDataStructure[inputQueue->currentRearIndex].data,0x00,size);
    // allocate memory for our file descriptor
    inputQueue->localDataStructure[inputQueue->currentRearIndex].fd = (int*)malloc(sizeof(int));
    inputQueue->localDataStructure[inputQueue->currentRearIndex].threadNumber=threadNumber;
    inputQueue->localDataStructure[inputQueue->currentRearIndex].connection=i;
    *inputQueue->localDataStructure[inputQueue->currentRearIndex].fd=fd;
    inputQueue->localDataStructure[inputQueue->currentRearIndex].type=type;
    // copy the data into our packet
    strncpy(inputQueue->localDataStructure[inputQueue->currentRearIndex].data,data,size);
    
    
}

/************************************************************************************
*
*	peekAtFrontElement
*
*	Allows the programmer to peek at the top element, without removing it from the queue
*      
*	Arguments:
*
*      Queue inputQueue
*
*	Return Value:
*
*		Pointer to the element at the front of the queue
*
*************************************************************************************/
command *peekAtFrontElement(Queue inputQueue) 
{
    // if queue is empty, return null
    if (isQueueEmpty(inputQueue))
        return NULL;

    
    command *cmd = &inputQueue->localDataStructure[inputQueue->currentFrontIndex];
    
    return cmd;

}

/************************************************************************************
*
*	getFrontElement
*
*	Removes and returns the element at the front of the queue
*      
*	Arguments:
*
*      Queue inputQueue
*
*	Return Value:
*
*		Pointer to the element at the front of the queue
*
*************************************************************************************/
command *getFrontElement(Queue inputQueue) {

  command *X = NULL;


  if (!isQueueEmpty(inputQueue)) 
  {
      inputQueue->currentSize--;
      X = &inputQueue->localDataStructure[inputQueue->currentFrontIndex];
      
      incrementIndex(&inputQueue->currentFrontIndex,&inputQueue->currentCapacity);
      
      //inputQueue->Front = Succ(inputQueue->Front, inputQueue);
  }

  return X;

}
