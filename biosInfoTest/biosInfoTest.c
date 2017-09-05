#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include  <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../CommonLibrary/Common.h"

#include "biosInfoTest.h"

char *typeList[]=
    {
        
        NO_BIOS,
        #define NO_SIGNATURE 0
        "$PDM",
        #define PHOENIX_BIOS NO_SIGNATURE+1
        "AMIBIOS"
        #define AMI_BIOS PHOENIX_BIOS+1
        
    };

char *amiBiosList[]=
{
    NO_BIOS,
    "$IBIOSI$"
    #define INTEL_BIOS_STRING 1
    #define INTEL_BIOS AMI_BIOS+1
};

memPointer *bios=0;
    
int main()
{

    
    BiosInfoType currentBios;
    int biosMem=0,signatureLocation=0;
    bios = getBiosMemory(&biosMem);

    char *ptr = bios->ptr;
    if (ptr)
    {   
        int typeSignature = locateSignature(ptr,BIOS_START,BIOS_END,&signatureLocation,typeList,3);
        initializBiosStructure(&currentBios);
        currentBios.Type = typeSignature;
        switch(currentBios.Type)
        {
        case PHOENIX_BIOS:
            consolePrint("Phoenix BIOS not supported\n");
            break;
        case AMI_BIOS:
            {
                getAMISpecific(ptr,&currentBios);
                
                
                typeSignature += locateSignature(ptr,BIOS_START,BIOS_END,&signatureLocation,amiBiosList,2);

                switch (typeSignature)
                {
                case INTEL_BIOS:
                    {
                        getIntelBios(ptr,amiBiosList[INTEL_BIOS_STRING],signatureLocation,&currentBios);
                    }
                    break;
                default:
                    consolePrint("unknown AMI bios\n");
                }
            }
            break;
        default:
            consolePrint("found nothing\n");
        };

        PrintoutBiosInfo(&currentBios);

        if (bios)
            releaseBiosMemory(bios);
    }
    else
        consolePrint("Could not read BIOS Memory\n");
   
    return 0;
}

