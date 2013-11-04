#include <stdlib.h>
#include "rtty.h"
#include "delay.h"
//#include "utils.h"

void rtty_init()
{
    /* set pins */
    RTTY_DIR |= RTTY_MARK_PIN | RTTY_SPACE_PIN;
    /* radio on */
    P2DIR |= RTTY_ENABLE_PIN;
    P2OUT |= RTTY_ENABLE_PIN;

}

void rtty_txbit (int bit)
{
    /*
     * Sends one bit of data
     *
     */

    if (bit)
    {
        /* mark */
        RTTY_PORT |= RTTY_MARK_PIN;
        RTTY_PORT &= ~(RTTY_SPACE_PIN);
    }
    else
    {
        /* space */
        RTTY_PORT |= RTTY_SPACE_PIN;
        RTTY_PORT &= ~(RTTY_MARK_PIN);

    }
//    __delay_cycles(RTTY_BAUDRATE/2); /* depends on configuration */
 //   __delay_cycles(RTTY_BAUDRATE/2);
 	DELAY_US(2000); //47.5 baud
}

void rtty_txbyte (char c)
{
    /* Simple function to sent each bit of a char to 
     ** rtty_txbit function. 
     ** NB The bits are sent Least Significant Bit first
     **
     ** All chars should be preceded with a 0 and 
     ** proceded with a 1. 0 = Start bit; 1 = Stop bit
     **
     */

    int i;

    rtty_txbit (0); /* Start bit */

    /* Send bits for for char LSB first */
    for (i = 0; i < RTTY_ASCII; i++) /* ASCII bits */
        rtty_txbit((c>>i)&1);

    rtty_txbit (1); /* Stop bit */
}

void rtty_txstring (char * string)
{
    /* Simple function to sent a char at a time to 
     * rtty_txbyte function. 
     * NB Each char is one byte (8 Bits)
     */

    char c;

    c = *string++;

    while ( c != '\0')
    {
        rtty_txbyte (c);
        c = *string++;
    }
}

void rtty_send_temperature(float t)
{
	int temp;
	char buf[4];

	/* negative? */
	if (t<0)
		rtty_txbyte('-');

	/* integer part */	
	temp = (int)t;

	itoa(temp, buf, 10);
	rtty_txstring(buf);
	rtty_txbyte('.');

	/* decimal part */
	temp = (t - temp) * 100;
	itoa(temp, buf, 10);
	rtty_txstring(buf);

	rtty_txstring(" C");
}

void rtty_send_pressure(long p)
{
	int temp;
	char buf[5];
	float t;

	t=p/100.0; /* to hPa */

	/* negative? */
	if (p<0)
		rtty_txbyte('-');

	/* integer part */	
	temp = (int)t;

	itoa(temp, buf, 10);
	rtty_txstring(buf);
	rtty_txbyte('.');

	/* decimal part */
	temp = (t - temp) * 100;
	itoa(temp, buf, 10);
	rtty_txstring(buf);

	rtty_txstring(" hPa");

}
