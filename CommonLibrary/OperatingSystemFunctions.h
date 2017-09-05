#ifndef OPERATINGSYSTEMFUNCTIONS_H
#define OPERATINGSYSTEMFUNCTIONS_H 1

/*! \file
	\brief Prototypes for operating system related functions
*/

/*! \fn void rebootMachine()
    \brief calls powerDown to signal the test dispatcher to power down
    and thus save its data
	\return void
*/
void rebootMachine();

/*! \fn void rebootMachine()
    \brief Signals the test dispatcher to power down and thus save its data
	\return void
*/

void powerDown(int mode);

#endif
