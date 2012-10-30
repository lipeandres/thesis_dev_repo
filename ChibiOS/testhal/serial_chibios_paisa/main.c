#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include <serial.h>


int main(void) {

SerialConfig config;
    	config.sc_speed=38400;
	config.sc_cr1=1;
    halInit();
    chSysInit();
    palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(7));

    sdStart(&SD3, &config);


    chprintf((BaseSequentialStream *)&SD3,"Hola");

    while (TRUE) {


    	chprintf((BaseSequentialStream *)&SD3,"Hola");
        chThdSleepMilliseconds(1000);

    }
}
