#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

extern "C"
{
#include <serial_helper.h>
}

using namespace std;

typedef struct  tm ComInfo;

void stmRead	(int,ComInfo*);
void stmWrite	(int,ComInfo*);

ComInfo *comObjectPtr= new ComInfo;
//unsigned char inBuffer[sizeof(*comObjectPtr)];

int main()
{
  int ttyfd = serial_open ("/dev/ttySP1", 115200, CLOCAL | CREAD | CS8, 1);	
	while(true){	
  printf("Read from Stm started ...\n");	
	stmRead(ttyfd,comObjectPtr);
	printf("%s\n",asctime(comObjectPtr));
	tcflush(ttyfd,TCIOFLUSH);
	usleep(1000*1000);
	}
return 0;
}


void stmWrite (int ttyPort,ComInfo* comObjectPtr){
	unsigned char outBuffer[sizeof(*comObjectPtr)];	
	printf("Write to Stm started ...\n");
	memcpy(outBuffer,comObjectPtr,sizeof(*comObjectPtr));
	write (ttyPort, outBuffer, sizeof(*comObjectPtr));
	//printf("Wrote %c to Stm\n",comObjectPtr->command);
	return;
}

void stmRead (int ttyPort,ComInfo* comObjectPtr){
	unsigned char inBuffer[sizeof(*comObjectPtr)];
	//printf("Read from Stm started ...\n");
	
	read  (ttyPort, inBuffer, sizeof(*comObjectPtr));
	printf("Read command OK...\n");
	//cleanComObject(comObjectPtr);
	memcpy(comObjectPtr,inBuffer,sizeof(*comObjectPtr));
	//for(int i=0;i<sizeof(*comObjectPtr);i++)
	//inBuffer[i]= 0;	
	printf("Read from Stm correct\n");
	return;
}


