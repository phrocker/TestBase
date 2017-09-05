#include "environ.h"
#include "IPCFunctions.h"


/*! \file
	\brief Definitions for the environmental functions
*/

/***********************************************************************
*	getEnvironmentValue
*   Returns pointer to environmental variable, if it exists
*	
*	Arguments: 
*	    char *env_var -- name of environmental variable
*	Return Value:
*		char * value of env_var
*		
***********************************************************************/
char *getEnvironmentValue(char *env_var)
{

	char *var=NULL; // value of env_var

	makeUpperCase(env_var); // make env_var all uppercase 


	var = getenv(env_var); // get environmental variable

    // if the value is NULL, it means that the environmental variable
    // does not exist
	if(var == NULL)
	{
//        char String[LINE_BUF];
	//	sprintf(String, "ERROR - Environment Variable '%s' Not Defined.\n", env_var);
		//printf(String);
		//stop_test(String);
	}

	return(var);
}

/***********************************************************************
*	setEnvironmentValue
*   Sets the environmental variable, but ONLY within the scope
*   of the calling process
*	
*	Arguments: 
*	    char *env_name -- name of environmental variable
*	    char *env_value -- value of the environmental variable
*	Return Value:
*		int -- value of setenv
*		
***********************************************************************/
int setEnvironmentValue(char *env_name, char *env_value)
{
	return setenv(env_name,env_value,1);
}


/***********************************************************************
*	setCurrentTestMode
*   Sets the current test mode, but ONLY within the scope
*   of the calling process
*	
*	Arguments: 
*       int currentMode -- current test Mode
*	Return Value:
*		VOID
*		
***********************************************************************/
void setCurrentTestMode(int currentMode)
{
    // create static array for mode
    char mode[20]="";

    sprintf(mode,"%i",currentMode);

    // set the current test mode
    setEnvironmentValue("CURRENT_TEST_MODE",mode);

}


/***********************************************************************
*	captureIpcQid
*   Attempts to obtain the key for the Test Dispatcher IPC window
*	
*	Arguments: 
*       VOID
*	Return Value:
*		int -- IPC key
*		
***********************************************************************/
int captureIpcQid()
{
    key_t mykey=TEST_DISPATCHER_PROJECT_ID;
    return ipcGetKey(mykey);
}


/***********************************************************************
*	captureCurrentTestMode
*   Attempts to obtain the key for the Test Dispatcher IPC window or
*   from the environmentla variable
*	
*	Arguments: 
*       VOID
*	Return Value:
*		int -- current test Mode
*		
***********************************************************************/
int captureCurrentTestMode()
{
    // create static buffer for environmental variable
    char environmentValue[LINE_BUF];

    sprintf(environmentValue,"current_test_mode");
    char *v = getEnvironmentValue(environmentValue);

    /*
     * if the pointer for the environmental value is NULL,
     * attempt to determine if the Test Dispatcher IPC window is open. If
     * so, return REMOTE_CONSOLE_MODE, otherwise, return CONSOLE_MODE
     */
    
    if (!v) {
        return (  captureIpcQid() > 0 ? REMOTE_CONSOLE_MODE : CONSOLE_MODE  );
    }

    // if V is not null and we are not in REMOTE_CONSOLE_MODE
    // return CONSOLE_MODE
    if (atoi(v) > REMOTE_CONSOLE_MODE)
        return (CONSOLE_MODE);

    return ( atoi(v));

}
