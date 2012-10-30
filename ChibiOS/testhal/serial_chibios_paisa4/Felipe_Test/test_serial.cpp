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
bool flag=false;
int i=0;
char Command[16];
char Answer[64];
void stmRead	(int, char[]);
void stmWrite	(int, char[]);
void cleanComObject(unsigned char[]);


int main()
{

	int ttyfd = serial_open ("/dev/ttySP1", 115200, CLOCAL | CREAD | CS8, 1);
    
     
	strcpy(Command,"ATE0\r");
  // while(true)
    //{	
	
	stmWrite(ttyfd, Command);
	
	printf("write Stm done ...\n");	
	stmRead(ttyfd, Answer);
	printf("Read from Stm done ...\n");	
	printf("\n");	
	while(Answer[i]!=0)
	{
	printf("%c",Answer[i]);
	i++;
	}
	i=0;	
	printf("\n");
	for(int l=0;l<sizeof(Command);l++)
	Command[i]=0;
	
  // }*/



return 0;

}


void stmWrite (int ttyPort,char Answer[]){
	write (ttyPort, Command, sizeof(Command));
	return;
}

void stmRead (int ttyPort,char Command[]){
	read  (ttyPort, Answer, sizeof(Answer));
	return;
}

//void cleanComObject(ComInfo* comObjectPtr){}

