#ifndef BIOSINFOTEST_H
#define BIOSINFOTEST_H


#define BIOS_START 0xF0000
#define BIOS_END   0xFFFFF


#define NO_BIOS "NOT_SUPPORTED"

typedef struct
{
	short Type;
	char Name[10];
	char RevMajor[12];
	char RevMinor[12];
	char CustomName[LINE_BUF];
} BiosInfoType;






void PrintoutBiosInfo(BiosInfoType *biosInfo);

memPointer *getBiosMemory();

void releaseBiosMemory(memPointer *ptr);

int locateSignature(char *bios,unsigned int biosStart,unsigned int biosEnd,int *signatureLocation,char **testList,int size);

void getAMISpecific(char *bios,BiosInfoType *biosInfo);

void getIntelBios(char *bios,char *signatureString,int signature,BiosInfoType *biosInfo);


void initializBiosStructure(BiosInfoType *biosInfo);

#define NO_SIGNATURE_EXIT -1



#endif
