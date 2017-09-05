

#include <stdio.h>
#include <pthread.h>

#define SLOW_IO_BY_JUMPING 1

#include <sys/io.h>


#include "../CommonLibrary/Common.h"

pthread_t ledThread;
pthread_mutex_t ledStatusChange;


#define TEST_PORT 0x80
#define ALL_GREEN 0x0F
#define ALL_RED   0xF0
#define ALL_YELLOW   0xFF
#define ALL_OFF   0x00



void *ledHandler(void*);


int main()
{
    short status=PASS;
    ioperm(TEST_PORT,2,1);
    int ledStatus=ALL_OFF;
    

    
    unsigned int i=0;

    ledStatus = ALL_GREEN;

    pthread_mutex_init (&ledStatusChange, NULL);
    if (pthread_create (&ledThread, NULL, ledHandler,&ledStatus))
    {
        status = FAIL;
        goto finishTest;
    }

    
   if (askYesNoQuestion("Are all Post LEDs green?") == NO)
   {
       status=FAIL;
       goto finishTest;

   }
   pthread_mutex_lock(&ledStatusChange);   
   ledStatus = ALL_RED;
   pthread_mutex_unlock(&ledStatusChange);
   if (askYesNoQuestion("Are all Post LEDs red?") == NO)
   {
       status=FAIL;
       goto finishTest;
   }


   pthread_mutex_lock(&ledStatusChange);   
   ledStatus = ALL_YELLOW;
   pthread_mutex_unlock(&ledStatusChange);
   if (askYesNoQuestion("Are all Post LEDs yellow?") == NO)
   {
       status=FAIL;

   }

   ledStatus = ALL_OFF;

   finishTest:
   testPrint("Post Code LED Test");
   if (status==FAIL)
       failedMessage();
   else
       passedMessage();


}

void *ledHandler(void *var)
{

    while(*(int*)var != ALL_OFF)
    {
        pthread_mutex_lock(&ledStatusChange);   
        outw(*(int*)var,TEST_PORT);
        pthread_mutex_unlock(&ledStatusChange);
    }
}
