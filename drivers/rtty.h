#include "msp430.h"

#ifndef __MSP430_RTTY_H
#define __MSP430_RTTY_H

/******* LIBRARY CONFIGURATION *******/
#define CPU_CLOCK           1000000
//#define RTTY_DIR            P2DIR
//#define RTTY_PORT           P2OUT
//#define RTTY_MARK_PIN       BIT2
//#define RTTY_SPACE_PIN      BIT1
//#define RTTY_ENABLE_PIN		BIT3
#define RTTY_BAUDRATE       RTTY_BAUDRATE_50
#define RTTY_ASCII          RTTY_ASCII_8
/******* CONFIGURATION END *******/

#ifndef CPU_CLOCK
#error "You have to define CPU_CLOCK in Hz"
#endif

#define RTTY_BAUDRATE_50    20210*(CPU_CLOCK/1000000)
#define RTTY_BAUDRATE_300   3370*(CPU_CLOCK/1000000)
#define RTTY_BAUDRATE_600   1680*(CPU_CLOCK/1000000)
#define RTTY_ASCII_7        7
#define RTTY_ASCII_8        8

int rtty_baudrate;
int rtty_bits;

void rtty_init();
void rtty_txbit(int bit);
void rtty_txbyte(char c);
void rtty_txstring(char * string);
void rtty_send_temperature(float t);
void rtty_send_pressure(long p);

#endif
