#ifndef PCILIB_H
#define PCILIB_H 1

#include <pci/pci.h>
#include "definitions.h"

unsigned char status; // status register

#define ERRORFLAGSET 0x01
/*! \def ERRORFLAGSET
    \brief Flag to indicate an error has occurred
*/
#define DEBUGFLAGSET 0x02
/*! \def ERRORFLAGSET
    \brief Flag to indicate we are in debug mode
*/

/*! \fn void clearPciDebugFlag()
    \brief Clears the PCI debug flag
    \return void
*/
void clearPciDebugFlag();

/*! \fn void setPciDebugFlag()
    \brief Sets the PCI debug flag
    \return void
*/
void setPciDebugFlag();

/*! \fn void clearErrorFlag()
    \brief Clears the error flag
    \return void
*/
void clearErrorFlag();

/*! \fn char pciErrorOccured()
    \brief Returns whether or not a PCI error has occurred
    \return void
*/
char pciErrorOccured();

/*! \fn void pciError()
    \brief pci error function
    \param msg
    \return void
*/
void pciError(char *msg, ...);

// pci access structure
struct pci_access *pciAccess; // don't forget to initialize this

/*! \fn void initializePciInterface()
    \brief initializes the PCI interface
    \return void
*/
void initializePciInterface();

/*! \fn void cleanupPciInterface()
    \brief returns the PCI interface to its initial status
    \return void
*/
void cleanupPciInterface();

/*! \fn Byte pciRegisterReadByte(int bus, int dev, int function,unsigned int dataRegister)
    \brief Reads a byte from the specified PCI device
    \param bus
    \param dev - device
    \param function
    \param dataRegister
    \return data from PCI register
*/
Byte pciRegisterReadByte(int bus, int dev, int function,unsigned int dataRegister);

/*! \fn Word pciRegisterReadWord(int bus, int dev, int function,unsigned int dataRegister)
    \brief Reads a word from the specified PCI device
    \param bus
    \param dev - device
    \param function
    \param dataRegister
    \return data from PCI register
*/
Word pciRegisterReadWord(int bus, int dev, int function,unsigned int dataRegister);

/*! \fn DWord pciRegisterReadDoubleWord(int bus, int dev, int function,unsigned int dataRegister)
    \brief Reads a double word from the specified PCI device
    \param bus
    \param dev - device
    \param function
    \param dataRegister
    \return data from PCI register
*/
DWord pciRegisterReadDoubleWord(int bus, int dev, int function,unsigned int dataRegister);


/*! \fn void pciRegisterWriteByte(int bus, int dev, int function,unsigned int dataRegister, Byte byteToWrite)
    \brief Writes a byte to the specified PCI device
    \param bus
    \param dev - device
    \param function
    \param dataRegister
    \param byteToWrite - data to write
    \return void
*/
void pciRegisterWriteByte(int bus, int dev, int function,unsigned int dataRegister, Byte byteToWrite);

/*! \fn void pciRegisterWriteWord(int bus, int dev, int function,unsigned int dataRegister, Word byteToWrite)
    \brief Writes a word to the specified PCI device
    \param bus
    \param dev - device
    \param function
    \param dataRegister
    \param byteToWrite - data to write
    \return void
*/
void pciRegisterWriteWord(int bus, int dev, int function,unsigned int dataRegister, Word byteToWrite);

/*! \fn void pciRegisterWriteDoubleWord(int bus, int dev, int function,unsigned int dataRegister, DWord byteToWrite)
    \brief Writes a double word to the specified PCI device
    \param bus
    \param dev - device
    \param function
    \param dataRegister
    \param byteToWrite - data to write
    \return void
*/
void pciRegisterWriteDoubleWord(int bus, int dev, int function,unsigned int dataRegister, DWord byteToWrite);

#endif 
