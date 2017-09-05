#include <sys/reboot.h>
#include "environ.h"
#include "IPCFunctions.h"

/*! \file
	\brief Definitions for operating system related functions
*/

 /************************************************************************************ 
 *	rebootMachine
 *	Calls powerDown
 *
 *	Argument:
 *		void
 *
 *	Return:
 *		void
 *
 ************************************************************************************/
void rebootMachine()
{
   // RB_AUTOBOOT is defined in sys/reboot.h
   // it causes the system to reboot NOW
   powerDown( RB_AUTOBOOT );

}

 /************************************************************************************ 
 *	powerDown
 *	Calls powerDown
 *
 *	Argument:
 *		int mode - Mode to which to powerdown
 *
 *	Return:
 *		void
 *
 ************************************************************************************/
void powerDown(int mode)
{
    // attempt to obtain the main queue id
    int myQid=captureIpcQid();
    if (myQid  >= 0) {

        // get id of shutdown queue
        key_t mykey = TEST_SHUTDOWN_SERVER_QUEUE;

        ipcMakeKey(&mykey); 

        
        mykey = TEST_SHUTDOWN_SERVER_QUEUE;


        int rebootQid=0;

        // open queue

        ipcOpenQueue(mykey, &rebootQid);


        mykey = TEST_SHUTDOWN_SERVER_QUEUE;

        // get Queue ID for question queue
        rebootQid=ipcGetKey(mykey);

        // tell the server that we are going to restart, so save
        // the current status

        IPCPACKET message = {};
        message.mtype=IPCSERVERRESTART;
        message.data.releaseListener=FALSE;
        ipcSendMessage(myQid,&message,sizeof(IPCPACKET));
        

        IPCPACKET readMessage = {};
        readMessage.mtype=IPCSERVERRESTART;
        readMessage.data.releaseListener=FALSE;

        // read message from shutdown queue
        ipcReadMessage(rebootQid,0,(struct IPCPACKET*)&readMessage,sizeof(IPCPACKET),TRUE);

        // destroy shutdown queue
        ipcRemoveQueue(rebootQid);

        // tell the user it's safe to reboot

        diagnosticPrint("It is safe to reboot the machine...\n");

        // just loop until rebooted

        while(1);

    }



}


