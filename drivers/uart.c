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
////	IE2 |= UCA0RXIE;					// Enable USCI_A0 RX interrupt
//	UCA0CTL1 |= UCSSEL_2; 				//SMCLK
//										//8,000,000Hz, 9600Baud, UCBRx=52, UCBRSx=0, UCBRFx=1
//	UCA0BR0 = 52;                  		//8MHz, OSC16, 9600
//	UCA0BR1 = 0;                   	 	//((8MHz/9600)/16) = 52.08333
//	UCA0MCTL = 0x10|UCOS16; 			//UCBRFx=1,UCBRSx=0, UCOS16=1
//	UCA0CTL1 &= ~UCSWRST; 				//USCI state machine
////	IE2 |= UCA0RXIE; 					//Enable USCI_A0 RX interrupt

//	rx_flag = 0;						//Set rx_flag to 0
//	tx_flag = 0;						//Set tx_flag to 0

	return;

}

void puts(const char *s) {
while(*s) putc(*s++); 
	// Loops through each character in string 's'
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
