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
	\brief This file holds declarations for functions in myIPC.c
	\n Functions declared in this file use System V IPC functions to create messaging functions for the TestDispatcher Project
*/

#ifndef IPCFUNCTIONS_H
#define IPCFUNCTIONS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "Common.h"
#include "BoardInfo.h"


#define TEST_DISPATCHER_PROJECT_ID 	0x1FAB
#define TEST_EXECUTE_QUESTION_QUEUE 0x1FAC
#define TEST_SHUTDOWN_SERVER_QUEUE  0x1BAC
#define TEST_VERSION_QUEUE          0x2FAC
#define BUFFER_NOT_FULL 			0x02			//the entire buffer was not filled
#define NO_MESSAGE					0x03
#define QUESTION                    0x04
#define YESNO                       0x10
#define RETRYFAIL                   0x20
#define SET_VERSION                 0x05

/*! \def TEST_DISPATCHER_PROJECT_ID
    \brief ID used in IPC Key creation
*/
/*! \def TEST_EXECUTE_QUESTION_QUEUE
    \brief ID used for QUESTION message queue
*/
/*! \def BUFFER_NOT_FULL
    \brief IPC Return Flag
*/
/*! \def NO_MESSAGE
    \brief IPC Return Flag
*/

int qID;
/*! \def NO_MESSAGE
    \brief IPC Return Flag
*/
int userQid;
/*! \def NO_MESSAGE
    \brief IPC Return Flag
*/




#define IPCSERVERTOALL	0x01	//IPC messge type: from Dispather to All
#define IPCSERVERTOCLIENT	0x02	//IPC messge type: from Server to Dispatcher
#define IPCCLIENTTOSERVER	0x03	//IPC messge type: from Dispatcher to Server
#define IPCSERVERTOTEST	0x04	//IPC messge type: from Server to a Test
#define IPCTESTTOSERVER	0x05	//IPC messge type: from a Test to Server
#define IPCSERVERTOSERVER 0x06
#define IPCSERVERRESTART  0x07
#define IPCREGULARPRINT 0x00
#define IPCREGULARPRINTSTATUS 0x10
#define IPCDIAGNOSTICPRINT 0x20
#define IPCREGULARPRINTUPDATE 0x40
#define IPCDIAGNOSTICPRINTUPDATE 0x50
#define IPCGETATTEMPTFROMUSER    0x60
#define IPCGETREPAIRUSERLIST    0x70

//message types used to identify intended recepients and senders
/*! \def IPCSERVERTOALL
    \brief IPC message type: from Dispatcher to All
*/
/*! \def IPCSERVERTOCLIENT
    \brief IPC message type: from Server to Dispatcher
*/
/*! \def IPCCLIENTTOSERVER
    \brief IPC message type: from Dispatcher to Server
*/
/*! \def IPCSERVERTOTEST
    \brief IPC message type: from Server to a Test
*/
/*! \def IPCTESTTOSERVER
    \brief IPC message type: from a Test to Server
*/
/*! \def IPCSERVERTOSERVER
    \brief IPC message type: from a Test to Server
*/


 /*! \struct IPCCOMMAND CommonLib/IPCFunctions.h
   \brief Structure used during IPC interactions

   Structure has two character arrays for data transmission, and a short
   variable to allow the server to determine when shutdown is has 
   occurred, as this is its indicator to exit gracefully
 
*/
typedef struct
{
	short releaseListener;
    char info[BUF_LEN];
    char response[BUF_LEN];

    /*! \var short releaseListener
		\brief flag to determine when to exit the IPC thread
	*/

	/*! \var char info[BUF_LEN]
		\brief buffer for IPC command data
	*/

    /*! \var char response[BUF_LEN]
		\brief buffer for response to question, if one is posed
	*/


} IPCCOMMAND;

/*! \var typedef IPCCOMMAND
    \brief Type definition for IPC command structure
*/


/*! \struct IPCBOARDINFO myIPC.h 
   \brief This struct encapsulates an instance of BOARDINFO with a IPC key

	This structure adheres the TESTBOARDINFO to the IPC buffer structure by 
	combining it with a message type variable.
	PIPCBOARDINFO is a pointer to this structure. 
*/
typedef struct {
	
    long 			mtype; 		//require ID for our message type
	TESTBOARDINFO	Info;		//instance of the TESTBOARDINFO structure

    /*! \var long mtype
		\brief identifier for IPC message retrieval
	*/

	/*! \var TESTBOARDINFO Info
		\brief instance of struct containing board info
	*/

} IPCBOARDINFO, *PIPCBOARDINFO;

/*! \var typedef IPCBOARDINFO *PIPCBOARDINFO
    \brief A pointer TypeDef for the IPCBOARDINFO struct
*/


/*! \struct IPCBUFFER myIPC.h 
   \brief This struct privides a basic IPC Message structure.

	This structure adheres a CHAR buffer to the IPC buffer structure by 
	combining it with a message type variable.
	PIPCBUFFER is a pointer to this structure. 
*/


 /*! \struct IPCPACKET CommonLib/IPCFunctions.h
   \brief Structure which is passed test software and Test dispatcher

   This structure contains an instance of IPCCOMMAND as its packet data
   and a long used to indicate the packet type
 
*/
typedef struct {
	
	/*! \var long mtype
		\brief identifier for IPC message retrieval
	*/

	/*! \var char Buffer[1024]
		\brief char buffer for sending message strings
	*/

	long 	mtype; 		//require ID for our message type
	
	IPCCOMMAND data;

} IPCPACKET, *PIPPACKET;

/*! \var typedef IPCBUFFER *PIPCBUFFER
    \brief A pointer TypeDef for the IPCBUFFER struct
*/


//Wrapper functions for IPC function calls
// 
/*! \fn int ipcMakeKey( key_t *msgkey )
    \brief This function produces a key used to access the IPC messaging system.
	\param msgkey pointer to a key_t variable that will be assigned the key value.
*/
int ipcMakeKey( key_t *msgkey );


/*! \fn int ipcOpenQueue( key_t keyval, int* qid )
    \brief This function attempts to create a Message Queue within the Kernel.
	\param keyval The key value we will be assigning to the new queue
	\param qid pointer to an INT where the queue's ID can be stored.
*/
int ipcOpenQueue( key_t keyval, int* qid );

/*! \fn int ipcSendMessage( int qid, void* qbuf, int len )
    \brief This function attempts to send a messge to the queue identified by qid.
	\param qid The queue ID we are trying to write to
	\param qbuf Poointer to the struct whose data will be placed in the queue.
	\param len the size of the structure
*/
int ipcSendMessage( int qid, void* qbuf, int len );


/*! \fn int ipcReadMessage( int qid, long type, void* qbuf , int len,  int block)
    \brief This function attempts to read a messge from the queue identified by qid and of type 'type'.
	\param qid  The queue ID we are trying to read from
	\param type The type of message we are looking for [compared to mtype in each message]
	\param qbuf Pointer to the struct where the retrieved data will be store
	\param len the size of the structure
	\param block flag to determine if msgrcv will block or not
*/
int ipcReadMessage( int qid, long type, void* qbuf , int len,  int block);


/*! \fn int ipcPeekMessage( int qid, long type )
    \brief This function basically checks to see if there is a message in the queue identified by qid and of type 'type'
    \param qid The queue ID we are trying to read from
	\param type The type of message we are looking for [compared to mtype in each message]
*/
int ipcPeekMessage( int qid, long type );


/*! \fn int ipcRemoveQueue( int qid )
    \brief 	This function removes the Message Queue from the Kernel. 
	\param  qid The queue ID we will be removing
*/
int ipcRemoveQueue( int qid );


/*! \fn int ipcStoreQid( int* qid )
    \brief Stores the qID in central location
	\param qid pointer to an INT where the queue's ID can be stored.
*/
int ipcStoreQid( int *qid );


/*! \fn int ipcGetKey( key_t msgkey )
    \brief Retrieves Queue ID for specified key
	\param msgkey value representing key for Queue ID
*/
int ipcGetKey( key_t msgkey );


#endif 
