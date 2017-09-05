#ifndef RIJ_H
#define RIJ_H 1
#include "rijndael.h"

#define KEYBITS 256

unsigned long rk[RKLENGTH(KEYBITS)];
unsigned char key[KEYLENGTH(KEYBITS)];


void createKey(char *myKey)
{
    int i = 0;
	for (; i < sizeof(key); i++)
        key[i]  = ( i >= strlen(myKey) ? '0' : (unsigned char)myKey[i] );
}

short encryptText(char *plainText,char *encryptedTextBuffer,unsigned int sizeofEncryptedTextBuffer)
{
	if ( sizeofEncryptedTextBuffer < strlen(plainText)*16)
		return -1;

	unsigned char plaintext[16];
    unsigned char ciphertext[16];
    int nrounds;
    nrounds = rijndaelSetupEncrypt(rk,key, 256);
	unsigned int encryptedTextIterator=0;
    int i =0;
    for (; i < strlen(plainText); )
    {
            short j;
            for (j =0; j<16; j++)
            {
                if (j+i >= strlen(plainText))
                    plaintext[j]= 0x00;
                else
                    plaintext[j]=(unsigned char)plainText[j+i];
            }

        rijndaelEncrypt(rk, nrounds, plaintext, ciphertext);
        int d = 0;
       for (; d<16; d++)
        {
            encryptedTextBuffer[encryptedTextIterator++]=ciphertext[d];
        }
        i+=j;
    }
    return encryptedTextIterator;
}


short decryptText(char *cipherText,unsigned int cipherTextSize, char *decryptedTextBuffer)
{
//    cout << "we received " << cipherText << endl;
    int nrounds = rijndaelSetupDecrypt(rk, key, 256);
    if (cipherTextSize% 16 > 0) return -1;
    unsigned char plaintext[16];
    unsigned char ciphertext[16];
    unsigned int decryptedTextIterator=0;
    int i =0;
    for (; i < cipherTextSize; )
    {
//            cout << i << endl;
            int j;
            for (j =0; j<16; j++)
            {
                if (j+i >= cipherTextSize)
                    ciphertext[j]= 0x00;
                else
                    ciphertext[j]=(unsigned char)cipherText[j+i];
            }

       rijndaelDecrypt(rk, nrounds, ciphertext, plaintext);
       int d = 0;
       for (; d<16; d++)
        {
            decryptedTextBuffer[decryptedTextIterator++]=plaintext[d];
        }
        i+=j;
    }
    return decryptedTextIterator;
}

#endif

