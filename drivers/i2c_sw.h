#ifndef _I2C_H
#define _I2C_H

#include <msp430.h>

#define I2C_REN P1REN
#define I2C_DIR P1DIR
#define I2C_OUT P1OUT
#define I2C_IN  P1IN
/* port pins */
//#define I2C_SCL BIT4
//#define I2C_SDA BIT3
/* Start and Stop delay, most devices need this */
#define I2C_SDLY		0x20
/* for long lines or very fast MCLK, unremark and set
#define I2C_MASTER_DDLY		0x02
#define I2C_MASTER_UDLY
*/

/* sends a start condition
 * will set sda and scl high and delay before start
 */
void i2c_start( void );

/* send stop condition
 * will set sda low before delay and stop
 */
void i2c_stop( void );

/* Output one byte
 * assumes sda and scl low and leaves sda, scl low if ack.
 * returns true if no ack from device
 */
unsigned char i2c_send( register unsigned int data );

/* input count of bytes into buf[ ]
 * Assumes scl low and leaves scl low
 * sends ack to device until last byte then no ack
 */
void i2c_receive( unsigned char* buf, int count );


/* small pause for i2c */
static void __inline__ brief_pause( register unsigned int n )
{
    __asm__ __volatile__ (
            "1: \n"
            " dec %[n] \n"
            " jne 1b \n" : [n] "+r"(n)
            );
}

#endif
