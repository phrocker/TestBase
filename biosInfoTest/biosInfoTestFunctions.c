
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include  <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../CommonLibrary/Common.h"
#include "biosInfoTest.h"

void initializBiosStructure(BiosInfoType *biosInfo)
{
    memset(biosInfo->Name,0x00,sizeof(biosInfo->Name));
    memset(biosInfo->RevMajor,0x00,sizeof(biosInfo->RevMajor));
    memset(biosInfo->RevMinor,0x00,sizeof(biosInfo->RevMinor));
    memset(biosInfo->CustomName,0x00,sizeof(biosInfo->CustomName));
    biosInfo->Type = 0;
}

short skipPeriods(char *bios, unsigned int *start, unsigned int end, unsigned int periodCount)
{
    unsigned int i=*start;
    char periodCnt=0;
    for (; i < end; i++ )
    {
        if (bios[i]=='.')
            periodCnt++;
        if (periodCnt == periodCount)
        {
            *start=i+1; // change i
            return TRUE;
        }
    }
    return FALSE;
}

void getIntelBios(char *bios,char *signatureString,int signature,BiosInfoType *biosInfo)
{
    // called from AMI Bios list
    unsigned short i=0;
    signature+=strlen(signatureString);
    unsigned int intelBeginning=signature;
    char *tpr = bios+signature;

    int intelBiosEnd=0x10000;
    
    
    int current = signature;
    

    skipPeriods(bios,&signature,intelBiosEnd,1);
    

    signature = current;
    if ( skipPeriods(bios,&signature,intelBiosEnd,4) ==FALSE)
        return;


    signature = current;
    if ( skipPeriods(bios,&signature,intelBiosEnd,2) == FALSE)
        return;

    current = signature;
    if ( skipPeriods(bios,&signature,intelBiosEnd,1) ==FALSE)
        return;

    if ((signature-current) != 5)
        return;

    current = signature;
    if ( skipPeriods(bios,&signature,intelBiosEnd,1) ==FALSE)
        return;

    if ((signature-current) != 2)
        return;

    current = signature = intelBeginning;

    skipPeriods(bios,&signature,intelBiosEnd,2);
    tpr = bios+signature;

    memset(biosInfo->RevMinor,0,sizeof(biosInfo->RevMinor));
    memset(biosInfo->RevMajor,0,sizeof(biosInfo->RevMajor));

    memcpy(biosInfo->RevMinor,tpr,4);
    
    tpr+=5;
    // XXXX.X
    // this is from the BIOSINFO DOS test; however, this doesn't make
    // sense. I think bios revisions are more like XXX.XXXX
    sprintf(biosInfo->RevMajor,"%C",*tpr);
    
}

void getAMISpecific(char *bios,BiosInfoType *biosInfo)
{
    // AMIBIOS struct begins at offset f400

    //bios+=0xF400;

    char buffer[0xFF];
    memcpy(biosInfo->Name, bios+0xf4f0, 8);
    char *tempPtr = bios + 0xf47b;
    
	biosInfo->RevMajor[0] = tempPtr[1];
	biosInfo->RevMajor[1] = '\0';

	biosInfo->RevMinor[0] = tempPtr[2];
	biosInfo->RevMinor[1] = tempPtr[3];
	biosInfo->RevMinor[2] = '\0';

	// Get BIOS Custom Name
	memcpy(biosInfo->CustomName, bios+ 0xF5A0, LINE_LENGTH);
	biosInfo->CustomName[LINE_LENGTH]= '\0';
   
    
}

int locateSignature(char *bios,unsigned int biosStart,unsigned int biosEnd,int *signatureLocation,char **testList,int size)
{
    unsigned int  i=0;
    unsigned short j=0;
    for (; i < biosEnd-biosStart-1; i++)
    {
        for (j=0; j < size; j++)
        {

            if (!(bios+i))
            {
                return NO_SIGNATURE_EXIT;
            }
            if (!memcmp(bios+i,testList[j],strlen(testList[j])))
            {
                *signatureLocation = i;
                return j;
            }
                
        }
    }
    *signatureLocation = -1;
    return NO_SIGNATURE_EXIT;
}




memPointer *getBiosMemory()
{
    return (memPointer*)pointerToMemory(BIOS_START,BIOS_END-BIOS_START);

}

void releaseBiosMemory(memPointer *ptr)
{
    destroyMemoryPointer(ptr);
}


void PrintoutBiosInfo(BiosInfoType *biosInfo)
{   
    consolePrint("%s BIOS Rev. %s.%s %s\n", biosInfo->Name,  biosInfo->RevMajor, biosInfo->RevMinor, biosInfo->CustomName);
}

