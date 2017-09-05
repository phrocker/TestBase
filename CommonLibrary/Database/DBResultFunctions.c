#include "DBResultFunctions.h"

/*! \file
	\brief Definitions for the database result functions
*/

/***********************************************************************
*	getResultData
*   Returns pointer to data at the specified row/column, assuming it
*   exists
*	
*	Arguments: 
*		Result_Set *PTESTRESULTS -- pointer to result set
*       int row -- row from which to extract data
*       int column -- column from which to extract data
*	Return Value:
*		char * data
*		
***********************************************************************/
char * getResultData(Result_Set *PTESTRESULTS,int row, int column)
{
    // if result set is NULL, return NULL
    if (PTESTRESULTS==NULL) {
       return NULL;
    }
    // return NULL if row or column exceeds bounds
    if (row >= PTESTRESULTS->rows || column >= PTESTRESULTS->columns) {
        return NULL;
    }

    // if all is fine, return the data represented by the row/column
    return PTESTRESULTS->resultData[row][column];

}

/***********************************************************************
*	createResultDataSet
*   Allocates memory for result set character pointers
*	
*	Arguments: 
*		Result_Set *PTESTRESULTS -- pointer to result set
*       int row -- row from which to extract data
*       int column -- column from which to extract data
*	Return Value:
*		char * data
*		
***********************************************************************/
void createResultDataSet(Result_Set *PTESTRESULTS)
{
    int i=0,j=0;

    // go ahead, allocate memory for resultData
    PTESTRESULTS->resultData = (char***)malloc( PTESTRESULTS->rows*sizeof(char**));

    // now, allocate memory for each element of resultData
    for (i=0; i < PTESTRESULTS->rows; i++ ) {
        PTESTRESULTS->resultData[i] = (char**)malloc( PTESTRESULTS->columns*sizeof(char**));
    }

    // finally, set each subelement of resultData's elements to null,
    // so memory can later be allocated for each pointer
    for (i=0; i < PTESTRESULTS->rows; i++ ) {
        for (j=0; j < PTESTRESULTS->columns; j++ ) {
            PTESTRESULTS->resultData[i][j]=NULL;
        }
    }
  
}


/***********************************************************************
*	freeResultData
*   Frees data within result set
*	
*	Arguments: 
*		Result_Set *PTESTRESULTS -- pointer to result set
*	Return Value:
*		VOID
*		
***********************************************************************/
void freeResultData(Result_Set *PTESTRESULTS)
{
   
    // if result set is null, exit
    if (PTESTRESULTS == NULL) {
        return;
    }

    int i=0,j=0;
   
    // free sub-elements
    for (i=0; i < PTESTRESULTS->rows; i++) {
        for (j=0; j < PTESTRESULTS->columns; j++) {
            // if pointer is null, don't free it
            if (PTESTRESULTS->resultData[i][j]!= NULL) {
                free(PTESTRESULTS->resultData[i][j]);
            }
        }

        if (PTESTRESULTS->resultData[i]!=NULL) {
            free( PTESTRESULTS->resultData[i]);
        }

    }

  
   
    // finally, free resultData
    if (PTESTRESULTS->resultData!=NULL) {
        free( PTESTRESULTS->resultData );
    }
    

    // set data pointer to NULL
    PTESTRESULTS->resultData=NULL;

}

