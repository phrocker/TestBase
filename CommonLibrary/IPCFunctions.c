 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*! \file
	\brief Function definitions for all IPC functionality 
*/

#include "IPCFunctions.h"

/***************************************************************************
	ipcMakeKey
	
	This function produces a key used to access the IPC messaging system.
	An existing, accessable file and project ID are required to create a 
	key. Using the same file and id will always result in the same key value.
	Thus, all functions and processes calling this function will get the 
	same key and therefore access the same message queue. 

	Arguments:
		key_t* - pointer to a key_t variable that will be assigned the key value.
	Returns:
		TRUE or FALSE

***************************************************************************/
 
int ipcMakeKey( key_t *msgkey )
{

	/* Generate our IPC key value by passing in a file name and a project ID*/
    
	*msgkey = ftok(".", TEST_DISPATCHER_PROJECT_ID);
    
	if( *msgkey == -1 ) //error
		return FALSE; 

	return TRUE; 
}

/***************************************************************************
	ipcOpenQueue
	
	This function attempts to create a Message Queue within the Kernel. If
	a queue with the key value already exists, its ID is returned. 

	Arguments:
		key_t - The key value we will be assigning to the new queue
		int*  - pointer to an INT where the queue's ID can be stored.
		
	Returns:
		TRUE or FALSE

***************************************************************************/

int ipcOpenQueue( key_t keyval, int* qid )
{
	
	if((*qid = msgget( keyval, IPC_CREAT | 0660 )) == -1){
        perror("msgget");
		return FALSE;
	}
	
	return TRUE;
}

/***************************************************************************
	ipcSendMessage
	
	This function attempts to send a messge to the queue identified by qid.

	Arguments:
		int - The queue ID we are trying to write to
		void* - The struct whose data will be placed in the queue.
		len - the size of the structure
	Returns:
		TRUE or FALSE
***************************************************************************/

int ipcSendMessage( int qid, void* qbuf, int len )
{
	int     result=0, length=0;

	/* The length is essentially the size of the structure minus sizeof(mtype) */

	length = len - sizeof(long);        

	if(msgsnd( qid, qbuf, length, IPC_NOWAIT) == -1){
		return FALSE;
	}

	//msgsnd returns 0 on success
	
	return TRUE; 
}

/***************************************************************************
	ipcReadMessage
	
	This function attempts to read a messge from the queue identified by qid
	and of type 'type'. [This is the mtype variable in the struct]. If
	type is 0 then the oldest message in the queue will be returned, regardless
	of type. 
    
	Arguments:
		int - The queue ID we are trying to read from
		long - The type of message we are looking for [compared to mtype in each message]
		void* - The struct where the retrieved data will be store
		len - the size of the structure
		block - flag to determine if msgrcv will block or not
		
	Returns:
		TRUE or FALSE or NO_MESSAGE or BUFFER_NOT_FULL

***************************************************************************/

int ipcReadMessage( int qid, long type, void* qbuf , int len, int block )
{
	int result, length;
	int waitFlag = IPC_NOWAIT; 

        
	if(block)
		waitFlag = 0; 

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = len - sizeof(long);        
    
	// If no message is ready and no_block is true, -1 is returned and
	// errno is ENOMSG 
    
	
	if((result = msgrcv( qid, qbuf, length, type, waitFlag )) == -1){
		return FALSE;
	}
    
	//msgrcv returns the number of bytes copied into the message buffer
	//if we didn't fill the buffer there something may have gone wrong. 
	
	if(result != (length + sizeof(long)) && block)
		return BUFFER_NOT_FULL;

	//there was no message and we didn't block for one
	if(result == -1 && !block && errno == ENOMSG)	
		return NO_MESSAGE; 

	return TRUE;
}


/***************************************************************************
	ipcPeekMessage
	
	This function basically checks to see if there is a message in the queue identified by qid
	and of type 'type'. [This is the mtype variable in struct]. Because there is
	no buffer specified, if there is a matching message the recieve will fail with errno E2BIG
	and the message remains in the Queue. 
    The IPC_NOWAIT flag keeps the receive call from blocking until a message arrives.
	Arguments:
		int - The queue ID we are trying to read from
		long - The type of message we are looking for [compared to mtype in each message]
		
	Returns:
		TRUE or FALSE if there is a matching message or not

***************************************************************************/

int ipcPeekMessage( int qid, long type )
{
        int result;

		//see if there's a message of type 'type'
        if((result = msgrcv( qid, NULL, 0, type,  IPC_NOWAIT)) == -1)
        {
                if(errno == E2BIG)
                        return TRUE;	//there is one present in the queue
        }
        
        return FALSE;
}

/***************************************************************************
	ipcRemoveQueue
	
	This function removes the Message Queue from the Kernel. 
	This function must be called to remove the queue, even if the queue is 
	empty. 
    
	Arguments:
		int - The queue ID we will be removing
		
	Returns:
		TRUE or FALSE

***************************************************************************/

int ipcRemoveQueue( int qid )
{
	//remove our queue from memory
	if( msgctl( qid, IPC_RMID, 0) == -1){
			return FALSE;
	}
	
	return TRUE;
}


/***************************************************************************
	ipcGetKey
	
	Obtains Queue ID for the key specified in msgkey
    
	Arguments:
		key_t - Key for Queue ID
		
	Returns:
		int - Queue ID

***************************************************************************/
int ipcGetKey( key_t msgkey )
{

    int msqid=0;

    if ((msqid = msgget(msgkey, 0666)) < 0) {
        return ERROR;
    }
    else 
        return msqid;

}

