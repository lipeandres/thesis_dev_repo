#include "ch.h"
#include "hal.h"
#include "chprintf.h"


int main(void) {


    halInit();
    chSysInit();
    //palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));
    //palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

    sdStart(&SD2, NULL);


    chprintf((BaseSequentialStream *)&SD2,"Hola");

    while (TRUE) {

    	chprintf((BaseSequentialStream *)&SD2,"Hola");
        chThdSleepMilliseconds(1000);

    }
}
