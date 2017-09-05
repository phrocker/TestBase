#ifndef DEBUG_H
#define DEBUG_H 1

/*! \fn void dumpRegisters()
    \brief dumps all registers
    \return TRUE or FALSE
*/
void dumpRegisters();

/*! \fn void printHighLowRegister(char reg)
    \brief prints high and low register values for pointer register
    \param reg - register identifier
    \return TRUE or FALSE
*/
void printHighLowRegister(char reg);

/*! \fn void printHighLowPointerRegisters(char reg)
    \brief prints high and low register values
    \param reg - register identifier
    \return TRUE or FALSE
*/
void printHighLowPointerRegisters(char reg);

void handle_segfault(int sig, siginfo_t* info, void* stacktrace);

#endif 


