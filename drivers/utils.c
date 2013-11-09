#include "utils.h"
#include "pin_config.h"


/* 1000 us = 1000 cycles at 1MHz */
/*
void _delay_ms(unsigned int ms)
{
    while (ms--)
    {
        __delay_cycles(8000000/8000); 

    }
}
*/

void blink_led()
{
	P1OUT &= ~RED_LED;
	DELAY_MS(100);
	P1OUT |= RED_LED;
	DELAY_MS(100);
}





