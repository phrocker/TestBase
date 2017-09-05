#ifndef SERIALTESTFUNCTIONS_H
#define SERIALTESTFUNCTIONS_H 1
#include <pty.h>

/*! \fn void gracefulSerialShutdown(int port)
    \brief Attempts to restore port's attributes
    \param port File descriptor
	\return void
*/
void gracefulSerialShutdown(int port);


/*! \fn void test_com(int port, int retry, short debug)
    \brief Tests com port
    \param port File descriptor
    \param retry retry flag
    \param debug debug flag
	\return void
*/
void test_com(int port, int retry, short debug);

/*! \fn void serial_test(int port,short debug)
    \brief Executes serial test
    \param port File descriptor
    \param debug debug flag
	\return void
*/
short serial_test(short port, short debug);
void usage();

#endif 
