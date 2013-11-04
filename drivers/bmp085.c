#include "bmp085.h"
#include "i2c_sw.h"
#include "utils.h"

char bmp085_read(unsigned char address)
{
	unsigned char data; 

	/* i2c start condition */
	i2c_start();

	/* sends slave address */
	i2c_send(BMP085_ADDRESS<<1);
	/* sends register address */
	i2c_send(address);
	/* i2c stop condition */
	i2c_stop();

	/* reads byte */
	i2c_start();
	i2c_send(BMP085_ADDRESS<<1|0x01); /* read command */
	i2c_receive(&data, 1);
	i2c_stop();

	return data;
}

int bmp085_read_int(unsigned char address)
{
	unsigned char data[2]; 

	/* i2c start condition */
	i2c_start();

	/* sends i2c slave address */
	i2c_send(BMP085_ADDRESS<<1);
	/* sends register address */
	i2c_send(address);
	/* i2c stop condition */
	i2c_stop();

	/* reads int */
	i2c_start();
	i2c_send(BMP085_ADDRESS<<1|0x01); /* read command */
	i2c_receive(data, 2);
	i2c_stop();

	return (int) data[0]<<8|data[1];
}

void bmp085_calibration(void)
{
	ac1 = bmp085_read_int(0xAA);
	ac2 = bmp085_read_int(0xAC);
	ac3 = bmp085_read_int(0xAE);
	ac4 = bmp085_read_int(0xB0);
	ac5 = bmp085_read_int(0xB2);
	ac6 = bmp085_read_int(0xB4);
	b1 = bmp085_read_int(0xB6);
	b2 = bmp085_read_int(0xB8);
	mb = bmp085_read_int(0xBA);
	mc = bmp085_read_int(0xBC);
	md = bmp085_read_int(0xBE);
}

unsigned int bmp085_read_utemperature(void)
{
	unsigned int ut;

	/* writing 0x2E into register 0xF4 requests a temperature reading */
	i2c_start();
	i2c_send(BMP085_ADDRESS<<1);
	i2c_send(0xF4);
	i2c_send(0x2E);
	i2c_stop();

	/* wait at least 4.5ms */
	_delay_ms(5);

	ut = bmp085_read_int(0xF6);

	return ut;
}

long bmp085_read_temperature(void)
{
	long x1, x2;
	  
	x1 = (((long)bmp085_read_utemperature() - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long)mc << 11)/(x1 + md);
	b5 = x1 + x2;

	return ((b5 + 8)>>4);  
}

unsigned int bmp085_read_upressure(void)
{
	unsigned char data[3];
	unsigned long up = 0;

	/* writing 0x34+(oversampling<<6) into register 0xF4 requests */ 
	/* a pressure reading with oversampling setting */
	i2c_start();
	i2c_send(BMP085_ADDRESS<<1);
	i2c_send(0xF4);
	i2c_send(0x34 + (BMP085_OVERSAMPLING<<6));
	i2c_stop();

	/* wait for conversion, depends on oversampling */
	_delay_ms(2 + (3<<BMP085_OVERSAMPLING));

	/* read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB) */
	i2c_start();
	i2c_send(BMP085_ADDRESS<<1);
	i2c_send(0xF6);
	i2c_stop();

	i2c_start();
	i2c_send(BMP085_ADDRESS<<1|0x01); /* read */
	i2c_receive(data, 3);

	up = (((unsigned long) data[0] << 16) | ((unsigned long) data[1] << 8) | (unsigned long) data[2]) >> (8-BMP085_OVERSAMPLING);

	return up;
}


long bmp085_read_pressure(void)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<BMP085_OVERSAMPLING) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(bmp085_read_upressure()-b3) * (50000>>BMP085_OVERSAMPLING));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}



