#include "utils.h"
#include "pin_config.h"

void _delay_ms(unsigned int ms)
{
    while (ms--)
    {
        __delay_cycles(CPU_CLOCK/8000); /* 1000 us = 1000 cycles at 1MHz */
    }
}

void blink_led()
{
	P1OUT &= ~RED_LED;
	_delay_ms(100);
	P1OUT |= RED_LED;
	_delay_ms(100);
}



