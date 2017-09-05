#include "PCILib.h"


/*! \file
	\brief PCI functions

    Provides basic PCI bus access. It is important that initializePciInterface
    be called BEFORE attempting to access the PCI bus, else an error will occur.

    Root user access and sufficient privileges must exist in order to access
    the PCI bus from user space
*/

void clearPciDebugFlag()
{
    status&=0xfd;
}

void setPciDebugFlag()
{
    status|=DEBUGFLAGSET;
}

void clearErrorFlag()
{
    status&=0xfe;
}

char pciErrorOccured()
{
    return (status&ERRORFLAGSET);
}

/***************************************************************************
*	initializePciInterface
*	
*	Initializes PCI interface. The PCI interface must be initialized before
*   it can be accessed
*    
*	Arguments:
*       void
*
*	Returns:
*		void
*
***************************************************************************/
void initializePciInterface()
{  
    pciAccess = pci_alloc();		/* Get the pci_access structure */
    pciAccess->error = pciError;
    pciAccess->warning = pciError;
    if (!pciAccess)
    {
        consolePrint("Could not gain access to pci bus\n");
        exit(1);
    }
	status=0x00;
	pci_init(pciAccess); // initialize interface
}

/***************************************************************************
*	pciError
*	
*	Sets the status flag to indicate an error has occurred
*    
*	Arguments:
*       char *msg - error message
*
*	Returns:
*       void
*
***************************************************************************/
void pciError(char *msg, ...)
{
    status|=ERRORFLAGSET;
}


/***************************************************************************
*	cleanupPciInterface
*	
*	Releases the pci interface so it can be used by other processes
*    
*	Arguments:
*       void
*
*	Returns:
*       void
*
***************************************************************************/
void cleanupPciInterface()
{
    pci_cleanup(pciAccess);
}


/***************************************************************************
*	pciRegisterReadByte
*	
*	Reads a byte from the specified device and associated register
*    
*	Arguments:
*       int bus
*       int dev - device
*       int function 
*       unsigned int dataRegister - register
*
*	Returns:
*       data from PCI register
*
***************************************************************************/
Byte pciRegisterReadByte(int bus, int dev, int function,unsigned int dataRegister)
{
    Byte PCIData=0x00;
    // get the device from the arguments
    struct pci_dev *device = pci_get_dev(pciAccess,0,(int)bus,(int)dev,(int)function);
    if (!device || pciErrorOccured()) // catch an error
    {
        if (status&DEBUGFLAGSET)
            consolePrint("Could not gain access to pci device\n");
        return PCIData;
    }
    else
    {
        // read a byte from the device
        PCIData = pci_read_byte(device, dataRegister);
        pci_free_dev(device);
    }

    if (pciErrorOccured() &&status&DEBUGFLAGSET)
        consolePrint("Could not gain access to pci device\n");

    return PCIData;
}


/***************************************************************************
*	pciRegisterReadWord
*	
*	Reads a word from the specified device and associated register
*    
*	Arguments:
*       int bus
*       int dev - device
*       int function 
*       unsigned int dataRegister - register
*
*	Returns:
*       data from PCI register
*
***************************************************************************/
Word pciRegisterReadWord(int bus, int dev, int function,unsigned int dataRegister)
{
    Word PCIData=0x00;
    // capture the associated PCI device
    struct pci_dev *device = pci_get_dev(pciAccess,0,(int)bus,(int)dev,(int)function);
    if (!device || pciErrorOccured()) // catch any errors
    {

        if (status&DEBUGFLAGSET)
            consolePrint("Could not gain access to pci device\n");
        return PCIData;
    }
    else
    {
        // read a word from the device
        PCIData = pci_read_word(device, dataRegister);
        // free the device now
        pci_free_dev(device);
    }

    if (pciErrorOccured() &&status&DEBUGFLAGSET)
        consolePrint("Could not gain access to pci device\n");

    return PCIData;
}

/***************************************************************************
*	pciRegisterReadDoubleWord
*	
*	Reads a double word from the specified device and associated register
*    
*	Arguments:
*       int bus
*       int dev - device
*       int function 
*       unsigned int dataRegister - register
*
*	Returns:
*       data from PCI register
*
***************************************************************************/

DWord pciRegisterReadDoubleWord(int bus, int dev, int function,unsigned int dataRegister)
{
    DWord PCIData=0x00;
    // capture out PCI device
    struct pci_dev *device = pci_get_dev(pciAccess,0,(int)bus,(int)dev,(int)function);
    // catch any errors during access to the PCI device
    if (!device || pciErrorOccured())
    {
        if (status&DEBUGFLAGSET)
            consolePrint("Could not gain access to pci device\n");
        return PCIData;
    }
    else
    {
        // read a double word ( long ) from the device
        PCIData = pci_read_long(device, dataRegister);
        pci_free_dev(device);
    }

    if (pciErrorOccured() &&status&DEBUGFLAGSET)
        consolePrint("Could not gain access to pci device\n");

    return PCIData;
}

/***************************************************************************
*	pciRegisterWriteByte
*	
*	Writes a byte to the specified device and associated register
*    
*	Arguments:
*       int bus
*       int dev - device
*       int function 
*       unsigned int dataRegister - register
*       Byte byteToWrite - data being written to PCI device
*
*	Returns:
*       void
*
***************************************************************************/
void pciRegisterWriteByte(int bus, int dev, int function,unsigned int dataRegister, Byte byteToWrite)
{
    // capture the pci device
    struct pci_dev *device = pci_get_dev(pciAccess,0,(int)bus,(int)dev,(int)function);
    // catch any errors we may have
    if (!device || pciErrorOccured())
    {
    
        if (status&DEBUGFLAGSET)
            consolePrint("Could not gain access to pci device\n");
        return;
    }
    else
    {
        // write our byte to the device
        pci_write_byte(device, dataRegister,byteToWrite);
        // free our device
        pci_free_dev(device);
    }

    if (pciErrorOccured() &&status&DEBUGFLAGSET)
        consolePrint("Could not gain access to pci device\n");
}


/***************************************************************************
*	pciRegisterWriteWord
*	
*	Writes a word to the specified device and associated register
*    
*	Arguments:
*       int bus
*       int dev - device
*       int function 
*       unsigned int dataRegister - register
*       Word byteToWrite - data being written to PCI device
*
*	Returns:
*       void
*
***************************************************************************/
void pciRegisterWriteWord(int bus, int dev, int function,unsigned int dataRegister, Word byteToWrite)
{
    // capture the PCI device
    struct pci_dev *device = pci_get_dev(pciAccess,0,(int)bus,(int)dev,(int)function);
    // catch errors
    if (!device || pciErrorOccured())
    {
        if (pciErrorOccured() &&status&DEBUGFLAGSET)
            consolePrint("Could not gain access to pci device\n");
        return;
    }
    else
    {
        // write a word to the device
        pci_write_word(device, dataRegister,byteToWrite);
        // free the device
        pci_free_dev(device);
    }

    if (pciErrorOccured() &&status&DEBUGFLAGSET)
        consolePrint("Could not gain access to pci device\n");
    
}

/***************************************************************************
*	pciRegisterWriteDoubleWord
*	
*	Writes a double word to the specified device and associated register
*    
*	Arguments:
*       int bus
*       int dev - device
*       int function 
*       unsigned int dataRegister - register
*       DWord byteToWrite - data being written to PCI device
*
*	Returns:
*       void
*
***************************************************************************/
void pciRegisterWriteDoubleWord(int bus, int dev, int function,unsigned int dataRegister, DWord byteToWrite)
{
    // capture the PCI device
    struct pci_dev *device = pci_get_dev(pciAccess,0,(int)bus,(int)dev,(int)function);
    if (!device || pciErrorOccured())
    {
    
        if (status&DEBUGFLAGSET)
            consolePrint("Could not gain access to pci device\n");
    }
    else
    {
        // write a double word to the device
        pci_write_long(device, dataRegister,byteToWrite);
        pci_free_dev(device);
    }

    if (pciErrorOccured() &&status&DEBUGFLAGSET)
        consolePrint("Could not gain access to pci device\n");
    
}
   
