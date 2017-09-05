#ifndef DB_RESULT_FUNCTIONS_H
#define DB_RESULT_FUNCTIONS_H 1

#include <stdlib.h>

/*! \file
	\brief Prototypes for database result functions
*/

/*! \struct Result_Set /Database/DBResultFunctions.h
   \brief Data structure which will contain query result data upon
   completion of a successfull SQL Query

   Structure contains row, column numbers, and data for SQL Query. Data
   is dynamically allocated, and as such, needs to be freed with freeResultData
   following use.
 
*/
typedef struct {

	/*! \var int rows
        \brief Number of rows returned from databaseExecSQL
    */
   
	
	/*! \var int columns
		\brief Number of column returned from databaseExecSQL
	*/
	
	/*! \var char ***resultData
		\brief Dynamic array for result data
	*/
	int rows; 

	int rowsAffected;

   
	int columns;

	char ***resultData;



} Result_Set;


/*! \fn cha* getResultData(Result_Set *,int int)
    \brief Returns data specified at the row and column
    \param PTESTRESULTS data set from which to extract the row/column
	\param row row from which to extract data
    \param column column from which to extract data
	\return char* pointer to data
*/

char* getResultData(Result_Set *PTESTRESULTS,int row, int column);


/*! \fn void createResultDataSet(Result_Set *PTESTRESULTS)
    \brief Allocates memory for PTESTRESULTS
    \param PTESTRESULTS result set
	\return void
*/
void createResultDataSet(Result_Set *PTESTRESULTS);

/*! \fn void freeResultData(Result_Set *PTESTRESULTS)
    \brief De-allocates memory for PTESTRESULTS
    \param PTESTRESULTS result set
	\return void
*/

void freeResultData(Result_Set *PTESTRESULTS);




#endif
