#include <msp430.h>
void putc(unsigned c); 
unsigned char getc(void);
void init_uart(void);
void puts(const char *s);

/**
 * Initializes the UART for 9600 baud with a RX interrupt
 **/
void init_uart(void) {
	P1SEL = BIT1 + BIT2 ;				// P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;				// P1.1 = RXD, P1.2=TXD

	UCA0CTL1 |= UCSSEL_1;				// CLK = ACLK; slau144j, page 412; ACLK is sourced from the crystal
	UCA0BR0 = 0x03;						// 32kHz/9600 = 3.41
	UCA0BR1 = 0x00;
	UCA0MCTL = UCBRS1 + UCBRS0;			// Modulation UCBRSx = 3
	UCA0CTL1 &= ~UCSWRST;				// **Initialize USCI state machine**
//	IE2 |= UCA0RXIE;					// Enable USCI_A0 RX interrupt
}

void puts(const char *s) {
	char c;

	// Loops through each character in string 's'
	while (c = *s++) {
		putc(c);
	}
}

void putc(unsigned c) {
	while(!(IFG2 & UCA0TXIFG)); //wait until the UCA0TXIFG flag is set
	IFG2 &= ~UCA0TXIFG; //set UCA0TXIFG flag to zero in IFG2
	UCA0TXBUF = c;
//	IE2 &= ~UCA0TXIE;
}


unsigned char getc(void){
	while(!(IFG2 & UCA0RXIFG));
	IFG2 &= ~UCA0RXIFG;
	return UCA0RXBUF;
}
