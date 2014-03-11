/* SPI / SDCARD */
#define CS_PIN  BIT0 //port 2
#define UCB0CLK_PIN  BIT5 //port 1
#define UCB0SOMI_PIN BIT6 //port 1
#define UCB0SIMO_PIN BIT7 // port 1

/* rtty / bitbang 50 baud */
#define RTTY_DIR P2DIR
#define RTTY_PORT P2OUT
#define RTTY_MARK_PIN BIT1 
#define RTTY_SPACE_PIN BIT2 
#define RTTY_ENABLE_PIN BIT3 
#define RTTY_ASCII          8

/* I2C / bmp085 pressure sensor */
#define I2C_REN	P1REN
#define I2C_DIR	P1DIR
#define I2C_OUT	P1OUT
#define I2C_IN P1IN
#define I2C_SCL BIT4   
#define I2C_SDA BIT3  

/* red led as indicator */
#define RED_LED	BIT0 //port 1
