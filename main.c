#include  <msp430.h>
#include <stdarg.h>
#include <stdint.h>
#include "drivers/pin_config.h"
#include "drivers/spi.h"
#include "drivers/utils.h"
#include "drivers/diskio.h"
#include "lib/parse_nmea.h"
//#include "drivers/rtty.h"
#include "drivers/i2c_sw.h"
#include "drivers/bmp085.h"


//G2253 - 16KB flash, 512B ram

const char call_sign[]="$$minion1,";
int sector_idx = 1; // this should be a higher type, int with restrict us to sizeof(int) sector writes 
short int bytes_written=0; //we limit this to a max of 500
unsigned long bmp085_temperature, bmp085_pressure;


void init(void){

	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	//BCSCTL2 = 0;

	//smclk_p14_out();
	
	P1DIR |= RED_LED;
	P1OUT &= ~RED_LED;
}

void smclk_p14_out(void){
	P1DIR |= BIT4 + BIT3; 
	P1SEL |= BIT4; //output smclk on P1.4 for measurement
}


void main(void)
{

	init();
	init_uart();
//	ow_portsetup();
	init_sdcard();
//	rtty_init();
	bmp085_calibration();
	
	int i;	
	printf("entering main code loop\n\r");

	//$$callsign,timestamp,long,N,lat,E,alt,internal_temp,external_temp,pressure,battery_voltage
	////<-GPGGA-><-014148.000-><-1922.4914-><-N-><-07247.4391-><-E-><-1-><-7-><-1.54-><-7.3-><-M-><--62.5->|#|
	while(1){
		//printf("inside while loop.. \n\r");
		printf("%s", call_sign);
		log_to_sdcard(call_sign);
//		rtty_txstring(call_sign);
		
		if (sample_gps_data() == 0 ){
			for(i=0;i<=9;i++){
				log_to_sdcard(get_gps_string(i));
				log_to_sdcard(",");
			//	rtty_txstring(strcat(get_gps_string(i), ", ");
				printf("%s,", get_gps_string(i));
			}

		}
		
//		rtty_txbyte(',');		


		//output values are in Pa. 1Pa = 0.001hPa == 0.01mbar
		printf("T%n", bmp085_read_temperature()/10); 
		log_to_sdcard(bmp085_read_temperature()/10); 
		log_to_sdcard("_TE,");
//		rtty_txstring(strcat((bmp085_read_temperature()/10), "_external, "));

		printf(" P%n", bmp085_read_pressure()/100); 
		log_to_sdcard(bmp085_read_pressure()/100); 
		log_to_sdcard("_P");

//		log_to_sdcard(strcat((bmp085_read_pressure()/100), "_pressure\n"));
//		rtty_txstring(strcat((bmp085_read_pressure()/100), "_pressure"));

		log_to_sdcard("|");
		printf("\n\r");
		DELAY_MS(200);

	}	
}

int check_last_write(void)
{
	int sector=3, ret;
	char buf[2];
	char pbuf[2];

	while(1){
		ret = disk_readp(buf, sector, 1, 2);

		if ( ret == 0 ){

//			printf("read ret - %i \n\r", ret);
			if (buf[0] == '\0'){
				if(pbuf[0] != '\0'){
					if ( strcmp(pbuf,"$$") == 0 ){ //to continue writes
						printf("sector empty, starting at  : %i \n\r", sector);
						return sector; //the current sector is empty, lets start here.
					}
				}
				sector++;
				if (sector >= 2){ //if disk is empty
					printf("last 2 sectors were empty, starting at sector : %i \n\r", sector-1);
					return sector-1;
				}
			}else{
				printf("sector : %i full, checking next sector \n\r", sector);		
				strcpy(pbuf, buf);
				sector++;
			}

		}else{ //ret !=0
			printf("unable to read, sdcard returned: %i \n\r", ret );
			return 1;
		}

		DELAY_MS(1);		
	}
}

int sector_flush(void)
{
	//add checks for errors other than 1
	if(disk_writep(0,0) == 1){
		printf("failed to write sector : %i \n\r", sector_idx);
		return 1;
	}else{

//		check_last_write();	
		printf("sector flushed : %i \n\r", sector_idx);
		return 0;
	}
}

int sector_start(int sector)
{
	int ret;
	ret = disk_writep(0, sector);
	if(ret == 0){
		return 0;
	}else{
		printf("sector start failed with - %i at sector - %i \n\r", ret, sector);
		return 1;
	}

}

void sector_write(char *data)
{
	 int c=0;
	 while(data[c] != '\0'){ c++; }
	 bytes_written += c;
//	 printf("bytes written - %i : %s \n\r", bytes_written, data);
	 disk_writep(data, c);
}

void init_sdcard(void){
	spi_initialize();
	while(1){//wait for sdcard to be inserted
			if ( disk_initialize() == 0){
				sector_idx=check_last_write();
				printf("sdcard initialized \n\r", sector_idx);
				if(sector_start(sector_idx) == 0){
					break;
				}else{

					P1OUT &= ~RED_LED;
					continue;
				}	
			}else{
				printf("disk initialization failed :<\n\r");
				P1OUT ^= RED_LED;
			}
		DELAY_MS(2000);
	}
}

void log_to_sdcard(char *data)
{

    short int c=0;
    while(data[c] != '\0'){ c++; }
	bytes_written += c;
	disk_writep(data, c);
  //	sector_write(data);

//	printf("to write: %s", dat);	
	if (bytes_written >= 500){ //we are close to the sector boundary, lets flush and initiate a new sector
		if(sector_flush() == 0){	
			sector_idx++; //lets point to the next sector
			printf("Total bytes written -> %i : sector idx -> %i\n\r", bytes_written, sector_idx);
		}else{
			//seems like the sdcard was disconnected, lets 
			init_sdcard();	
		}
		
		sector_start(sector_idx);
		bytes_written=0;
	}

}

int sample_gps_data(void){
	unsigned char c;
	int ret;
	int i;
	while(c = getc()){
		ret = parse_gps(c);
		if(ret > 0 && ret < ( sizeof(gpi) / sizeof(gpi[0]) ) ) {
			if(strcmp("GPGGA", gpi[ret -1].sentence) == 0){
//<-GPGGA-><-014148.000-><-1922.4914-><-N-><-07247.4391-><-E-><-1-><-7-><-1.54-><-7.3-><-M-><--62.5->|#|
//$$callsign,timestamp,long,N,lat,E,alt,kmpr,internal_temp,external_temp
			
//				for(i = 0; i < nbits(gpi[ret - 1].words); i++){
//
			//	for(i = 1; i < 9; i++){
			//		printf("<-%s->", get_gps_string(i));
//					strcat(data,get_gps_string(i));
//					if (i != 9){ strcat(data,sep); };
		//		}
				return 0;
			}
		}
	}
return 1;
}

