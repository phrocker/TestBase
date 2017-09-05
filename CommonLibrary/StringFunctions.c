#include "StringFunctions.h"
#include "definitions.h"
#include <stdio.h>
#include <fcntl.h>
/*! \file
	\brief String function definitions
*/

 /************************************************************************************ 
 *	removeTrailingWhiteSpace
 *	Removes trailing white spaces from the input string
 *
 *	Argument:
 *		char* str 
 *
 *	Return:
 *		void
 *
 ************************************************************************************/
void removeTrailingWhiteSpace(char *str)
{

    int end = strlen(str)-1;
    short i;
    for (i=end; i >= 0; i--)
    {
        if (str[i]!=32)
        {
            break;
            i++;
        }

    }
    if (i <= end)
    {
        str[i+1]=0x00;
    }
    
}


 /************************************************************************************ 
 *	makeLowerCase
 *	Convenience function which makes entire string lowercase
 *
 *	Argument:
 *		char*  - string to 'lowercase'
 *
 *	Return:
 *		void
 *
 ************************************************************************************/
 void makeLowerCase(char *str)
 {
     // continue while the pointer is not 0x00 or NULL
     while(*str)
     {     
         // if the current character is lowercase, set it to lower
         if(*str >= 0x41 && *str<= 0x5A)
             *str|=0x20;
         // continue to next character
         str++;
     }
 }

 /************************************************************************************ 
 *	makeUpperCase
 *	Convenience function which makes entire string uppercase
 *
 *	Argument:
 *		char*  - string to 'uppercase'
 *
 *	Return:
 *		void
 *
 ************************************************************************************/
 void makeUpperCase(char *str)
 {
     // continue while the pointer is not 0x00 or NULL
     while(*str)
     {     
         // if the current character is lowercase, set it to upper case
         if(*str >= 0x61 && *str<= 0x7a)
             *str&=0xdf;
         // continue to next character
         str++;
     }
     
 }


/**********************************************************************
* prefixExpand
*		Expands a string by prefixing string with zeros
*
* Arguments:
*		char array
*
* Return Value:
*		None
*
**********************************************************************/

void prefixExpand(char array[], int LEN)
{
    // set our pointer to null
	char* zeroArray = NULL;
    // allocate enough memory in our pointer
    // to allow for the expansion
	zeroArray = (char*)malloc(LEN*sizeof(char));

    // uh oh, couldn't allocate enough memory
    if (zeroArray==NULL) {
        return;
    }
    // set all to 0s
	memset(zeroArray,0x30,LEN);

    // get the length of the array
	short len = strlen(array);

	int i=0;
	for(;i<len;i++)
		zeroArray[i + (LEN-len)] = array[i];

	memcpy(array, zeroArray,LEN);

    array[LEN]=0x00;

    // free memory for array
	free(zeroArray);

	zeroArray = NULL;


}

///////////////////////////////////////////////////////////////////////////////
/*
 *  @fn         char *getMonth(unsigned short month)
 */
///////////////////////////////////////////////////////////////////////////////
char *getMonth(unsigned short month)
{
    char *monthString[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    if (month < 12)
    {
        return monthString[month];
    }
    else
        return monthString[0];
}


/**********************************************************************
* getDayOfWeek
*		Returns string representation of the day argument
*
* Arguments:
*		unsigned short day
*
* Return Value:
*		Day of week corresponding to input `day`
*
**********************************************************************/
char *getDayOfWeek(unsigned short day)
{
    char *dayOfWeek[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    if (day < 7)
    {
        return dayOfWeek[day];
    }
    else
        return dayOfWeek[0];
}
/**********************************************************************
* find
*		Find function which locates fT in T. Begins searching from the rear
*       The methodology is that if you have two similar strings, they will
*       more likely than not begin differing further down, than at the beginning
*       MP: imported from personal library 
*
* Arguments:
*		char *T - String
*       char *fT- Search string
*
* Return Value:
*		Location of found string, if found
*
**********************************************************************/
int find(char* T,char* fT)
{
	int tL = strlen(T),fL = strlen(fT);    
		
      if (fL  > tL)
		return ERROR;
	
	int d[255];

	int k=0; // general iterator

	for (k = 0; k < 255; k++) d[k] = 1; // set all values of d[] to 1

	for (k = 0; k < fL - 1; k++) d[fT[k]] = fL - k - 1;

	k = fL - 1;
	// begin moving through and checking chars from behind
	while (k < tL)
	{
		register int j = fL - 1;
		register int i = k;
		while (j >= 0 && T[i] == fT[j])
		{
			j--;
			i--;
		}
		if (j == -1) return i + 1;
        k += d[T[k]];
		
	}
	return ERROR;
}



////////////////////////////////////////////////////////////////////////////
/**
 *  @fn     void fillRandomArray(char *array,unsigned int size)
 */
////////////////////////////////////////////////////////////////////////////
 void fillRandomArray(char *array,unsigned int size)
{
    int fd = open("/dev/urandom",O_RDONLY);
    if (fd  < 0) // if file cannot be opened, then populate array with zeros
    {
        memset(array,0,size);
        return;
    }
    unsigned int i=0;
    // read n bytes into array
    read(fd,array,size);
    close(fd);
}
