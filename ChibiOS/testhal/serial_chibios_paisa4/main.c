#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include <stdio.h>
#include <string.h>
#include <serial.h>
#include <stdbool.h>

struct commandArray
{
unsigned char command[16];
};

typedef struct commandArray CommandArray;

struct answerArray
{
unsigned char answer[64];
};

typedef struct answerArray AnswerArray;

void imxRead	(CommandArray *);
void imxWrite	(AnswerArray *);
void elmRead	(AnswerArray *);
void elmWrite	(CommandArray *);
void clearAnswer(AnswerArray *);
void clearCommand(CommandArray *);

unsigned char ans_i[1]={0};
int i=0;

int main(void) {


	CommandArray Command;
	CommandArray *CommandP=&Command;
	AnswerArray Answer;
	AnswerArray *AnswerP=&Answer;
	clearAnswer(AnswerP);
	clearCommand(CommandP);

	halInit();
   	chSysInit();

//---USART Configuration
//---USART 1, talks to the IMX233. Set by default to 115200 Bps
	palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));//uart1 init
    	palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));
		
   		//sdStop(&SD1);

//---USART 2, talks to the ELM327. configured to 38400 Bps 
    	SerialConfig config;
    	config.sc_speed=38400;
	config.sc_cr1=1;
	    	
	palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
    	palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));
	

	

	while(true)
	{
	//chprintf((BaseSequentialStream *)&SD3,"Reading IMX...\n\r");	
	sdStart(&SD1, NULL);
	imxRead(CommandP);
        sdStop(&SD1);
	clearAnswer(AnswerP);
	//chprintf((BaseSequentialStream *)&SD3,"Reading IMX OK...\n\r");	
        chThdSleepMilliseconds(40);	
	sdStart(&SD3, &config);
        	
	elmWrite(CommandP);	
	//chprintf((BaseSequentialStream *)&SD3,"Writing ELM OK...\n\r");	
	elmRead(AnswerP);
	sdStop(&SD3);
	clearCommand(CommandP);
	//chprintf((BaseSequentialStream *)&SD3,"Reading ELM OK...\n\r");	
	chThdSleepMilliseconds(40);	
	sdStart(&SD1, NULL);	
	imxWrite(AnswerP);
	//chprintf((BaseSequentialStream *)&SD3,"Writing IMX OK...\n\n\r");
}

return 0;
}


void imxRead	(CommandArray * Command){
	//unsigned char Command_sig[16];
	//chSequentialStreamRead((BaseSequentialStream *)&SD1, 
	//Command_sig, sizeof(Command_sig));
	chSequentialStreamRead((BaseSequentialStream *)&SD1, 
	Command->command, sizeof(Command->command));
	//chprintf((BaseSequentialStream *)&SD3,(char *)Command->text);
	return;
}

/*void imxWrite	(unsigned char t_answer[]){
	//sdStart(&SD1, NULL);		
	chSequentialStreamWrite((BaseSequentialStream *)&SD1,t_answer, sizeof(t_answer));
	//sdStop(&SD1);
	//clearArray(Answer);	
	return;
}*/

void imxWrite	(AnswerArray * Answer){
	//sdStart(&SD1, NULL);		
	chSequentialStreamWrite((BaseSequentialStream *)&SD1,Answer->answer, sizeof(Answer->answer));
	//sdStop(&SD1);
	//clearArray(Answer);	
	return;
}

/*void elmRead (unsigned char t_answer[]){
	int i=0;
	do{		
	chSequentialStreamRead((BaseSequentialStream *)&SD3, 
	ans_i,1);
	t_answer[i]=ans_i[0];
	i++;	
	}while(t_answer[i-1]!=13);
	i=0;
	ans_i[0]=0;
	chprintf((BaseSequentialStream *)&SD3,(char *)t_answer);
	chprintf((BaseSequentialStream *)&SD3,"\n\r");
return;		
}*/

void elmRead (AnswerArray *Answer){
	do{		
	chSequentialStreamRead((BaseSequentialStream *)&SD3, 
	ans_i,1);
	Answer->answer[i]=ans_i[0];
	i++;	
	}while(Answer->answer[i-1]!=13);
	i=0;
	ans_i[0]=0;
	//chprintf((BaseSequentialStream *)&SD3,(char *)Answer->answer);
	//chprintf((BaseSequentialStream *)&SD3,"\n\r");
return;		
}

void elmWrite	(CommandArray * Command){		
//chprintf((BaseSequentialStream *)&SD3,Command->command);
chSequentialStreamWrite((BaseSequentialStream *)&SD3,Command->command,5);
//clearArray(Command);
return;
}

void clearAnswer(AnswerArray * tAnswer)
{
unsigned int n=0;
for(n=0;n<sizeof(tAnswer->answer);n++)
	tAnswer->answer[n]=0; 
}

void clearCommand(CommandArray * tCommand)
{
unsigned int m=0;
for(m=0;m<sizeof(tCommand->command);m++)
	tCommand->command[m]=0; 
}
