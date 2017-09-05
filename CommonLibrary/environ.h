#ifndef ENVIRON_H
#define ENVIRON_H 1

/*! \file
	\brief Environmental function prototypes 
*/
#include <stdlib.h>

#include "definitions.h"

/*! \fn void captureCurrentTestMode()
    \brief Returns the current test mode
	\return void
*/
int captureCurrentTestMode();


/*! \fn void captureIpcQid()
    \brief Returns the current message queue ID
	\return void
*/

int captureIpcQid();

/*! \fn void setCurrentTestMode()
    \brief Allows user to set the current test mode within the program's userspace
	\return void
*/
void setCurrentTestMode(int currentMode);



#endif 
