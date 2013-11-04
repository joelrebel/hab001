#include <msp430.h>
#include <stdint.h>
#include "pin_config.h"
#include "spi.h"

#ifndef __MSP430_HAS_USCI__
#error "Error! This MCU doesn't have a USCI peripheral"
#endif

#define SPI_MODE_0 UCCKPH | UCMSB| UCMST | UCSYNC /* CPOL=0 CPHA=0 */
#define SPI_MODE_3 UCCKPL | UCMSB| UCMST | UCSYNC /* CPOL=1 CPHA=1 */

/**
 * spi_initialize() - Initialize and enable the SPI module
 *
 * P2.0 - CS (active low)
 * P1.5 - SCLK
 * P1.6 - SIMO
 * P1.7 - MOSI
 *
 */
void spi_initialize(void)
{
	UCB0CTL1 = UCSSEL_2 | UCSWRST;	// source USCI clock from SMCLK, put USCI in reset mode
	UCB0CTL0 |= SPI_MODE_0;			// SPI MODE 0 - CPOL=0 CPHA=0
									// note: UCCKPH is inverted from CPHA

	P1SEL  |= UCB0CLK_PIN | UCB0SOMI_PIN | UCB0SIMO_PIN;	// configure P1.5, P1.6, P1.7 for USCI
	P1SEL2 |= UCB0CLK_PIN | UCB0SOMI_PIN | UCB0SIMO_PIN;

	UCB0BR0 = LOBYTE(SPI_400kHz);	// set initial speed 16MHz/400000 = 400kHz
	UCB0BR1 = HIBYTE(SPI_400kHz);

	P2OUT |= CS_PIN;					// CS on P2.0. start out disabled
	P2DIR |= CS_PIN;					// CS configured as output

	UCB0CTL1 &= ~UCSWRST;			// release for operation
}

/**
 * spi_send() - send a byte and recv response
 */
uint8_t spi_send(const uint8_t c)
{
	
	while (!(UC0IFG & UCB0TXIFG))
		; // wait for previous tx to complete
//	printf("SPI send ->: %x\r\n", c);
	UCB0TXBUF = c; // setting TXBUF clears the TXIFG flag

	while (!(UC0IFG & UCB0RXIFG))
		; // wait for an rx character?

	return UCB0RXBUF; // reading clears RXIFG flag
}

/**
 * spi_receive() - send dummy btye then recv response
 */
uint8_t spi_receive(void) {
	volatile uint8_t r;
	while (!(UC0IFG & UCB0TXIFG))
		; // wait for any previous xmits to complete

	UCB0TXBUF = 0xFF; // Send dummy packet to get data back.

	while (!(UC0IFG & UCB0RXIFG))
		; // wait to recv a character?
	
	r = UCB0RXBUF;
//	printf("SPI recv <-: %x\r\n", r);
	return r; // reading clears RXIFG flag


}

/**
 * spi_setspeed() - set new clock divider for USCI
 *
 * USCI speed is based on the SMCLK divided by BR0 and BR1
 * initially we start slow (400kHz) to conform to SDCard
 * specifications then we speed up once initialized (16Mhz)
 *
 */
void spi_set_divisor(const uint16_t clkdiv)
{
	UCB0CTL1 |= UCSWRST;		// go into reset state
	UCB0BR0 = LOBYTE(clkdiv);
	UCB0BR1 = HIBYTE(clkdiv);
	UCB0CTL1 &= ~UCSWRST;		// release for operation
}
