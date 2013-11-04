/*
    Copyright (C) 2012, 2013  Kevin Timmerman

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "parse_nmea.h"
                                                    //
static uint16_t hex(const uint8_t c)                //
{                                                   //
    if(c >= '0' && c <= '9') return c - '0';        //
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;   //
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;   //
    return 0x0F00;                                  //
}                                                   //
                                                    //
unsigned parse_gps(uint8_t c)                       //
{                                                   //
	static uint16_t gps_word_index = 0;             //
	static uint16_t gps_word_mask;                  //
	static uint8_t *gps_char;                       //
    static uint_fast8_t check_digit;                //
    static uint16_t checksum;                       //
    static unsigned bad_char;                       //
    static const TGPI *i;                           //
                                                    //
    switch(c) {                                     //
        case '$':						            // - Begining of sentence
            gps_word_index = 1;				        // Setup word index
            gps_word_mask = 1;				        // Parse first word
            check_digit = 0;                        // Initialize checksum
            checksum = 0;                           //
            bad_char = 0;                           //
            i = 0;                                  //
            gps_char = gps_parse_buf;		        // Setup parse buffer
            break;                                  //
       case ',':				                    // - Word seperator
            checksum ^= c;                          //
            if(check_digit) ++bad_char;             //
            if(gps_word_index & gps_word_mask) {    //
                *gps_char++ = 0;			        // Null terminate string
                if(gps_word_index == 1) {           //
                    i = gpi;                        //
                    while(i->sentence) {            //
                        if(!strcmp((char*)gps_parse_buf, i->sentence)) break;
                        ++i;                        //
                    }                               //
                    gps_word_mask = i->words;       //
                    if(!(gps_word_mask & 1)) gps_char = gps_parse_buf; 
                }                                   //
            }                                       //
            gps_word_index <<= 1;			        // Increment word index
            break;                                  //
        case '*':                                   // - Checksum
            if(check_digit)                         //
                ++bad_char;                         //
            else                                    //
                ++check_digit;                      //
            break;                                  //
        case '\r':                                  // - End of sentence
        case '\n':                                  //
            if(gps_word_index & gps_word_mask)      //
                *gps_char = 0;                      // Null terminate string

            if((gps_word_index > (gps_word_mask >> 1))
                && i                                //
                && !checksum                        //
                && !bad_char) {                     //
                    gps_word_index = 0;             // Clear word index, disable parsing
                    return (i - gpi) + 1;           //
            }                                       //
            gps_word_index = 0;                     // Clear word index, disable parsing
            break;                                  //
        default:                                    //
            if(c >= ' ' && c <= '~') {	            // - Printable char
                switch(check_digit) {               //
                    case 0:                         //
                        checksum ^= c;              //
                                                    // Check if this word should be parsed
                        if(gps_word_index & gps_word_mask) {
                            *gps_char++ = c;	    // Save char in parse buffer
                            if(gps_char >= (gps_parse_buf + sizeof(gps_parse_buf)))
                                gps_word_mask = 0;  //
                        }						    //
                        break;                      //
                    case 1:                         //
                        checksum ^= (hex(c) << 4);  //
                        ++check_digit;              //
                        break;                      //
                    case 2:                         //
                        checksum ^= hex(c);         //
                        ++check_digit;              //
                        break;                      //
                    default:                        //
                        ++bad_char;                 //
                        break;                      //
                }                                   //
            } else ++bad_char;						// - Ignore all invalid chars
            break;                                  //
    }                                               //
	return 0;                                       //
}                                                   //
                                                    //
uint8_t *get_gps_string(unsigned n)                 //
{                                                   //
    uint8_t *p = gps_parse_buf;                     //
    while(n--) while(*p++);                         //
    return p;                                       //
}                                                   //

uint16_t nbits(uint16_t n)
{
	n = n - ((n >> 1) & 0x55555555);
	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
	return (((n + (n >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

/*
int main(int argc, char **argv)
{
	FILE *fp;
	int c;
	int ret;
	int i;
 	const char gga[5]="GPGGA";

	fp = fopen("/opt/msp430/gps/gps_test_dump.cap2", "r");

	while(!feof(fp) && (c = fgetc(fp))) // for each char in fp
	{
		ret = parse_gps(c);
		if(ret > 0 && ret < ( sizeof(gpi) / sizeof(gpi[0]) ) ) {
			if (strcmp(gga, gpi[ret -1].sentence) == 0){
				printf("type = %s ->", gpi[ret - 1].sentence);
				for(i = 0; i < nbits(gpi[ret - 1].words); i++){
					printf("%d. word = %s ", i, get_gps_string(i));
				}
				puts("");
		    }
		}
	}

	fclose(fp);

	return 0;
}

*/

/*

--- Some common NMEA GPS sentences ---

3FE --- GPRMC Position and Time            0x03FE = 0b00000 0 1 1 1 1 1 1 1 1 1 0
                                                          | | | | | | | | | | | |
     0 GPRMC                            ------------------|-|-|-|-|-|-|-|-|-|-|-+
0    1 UTC Time HHMMSS                  ------------------|-|-|-|-|-|-|-|-|-|-+
1    2 Status  A = Valid, V = Invalid   ------------------|-|-|-|-|-|-|-|-|-+
2    3 Latitude DDMM.MMMM               ------------------|-|-|-|-|-|-|-|-+
3    4 Latitude N/S                     ------------------|-|-|-|-|-|-|-+
4    5 Longitude DDDMM.MMMM             ------------------|-|-|-|-|-|-+
5    6 Longitude E/W                    ------------------|-|-|-|-|-+
6    7 Speed over ground  Knots         ------------------|-|-|-|-+
7    8 Course over ground  Degrees      ------------------|-|-|-+
8    9 UTC Date  DDMMYY                 ------------------|-|-+
    10 Magnetic variation  Degrees      ------------------|-+
    11 Magnetic variation  E/W          ------------------+


7E	--- GPGGA Fix Data
	 0 GPGGA
0	 1 Time HHMMSS
1	 2 Latitude DDMM.MMMM
2	 3 Latitude N/S
3	 4 Longitude DDDMM.MMMM
4	 5 Longitude E/W
5	 6 Fix  0 = Invalid  1 = GPS  2 = DGPS  3 = PPS
	 7 Number of sats in view NN
	 8 Horizonal dilution of precision N.N
	 9 Altitude NNN.N
	10 Altitude units M/F
	11 Height of geoid NN.N
	12 Height of geoid units M/F
	13 Time since last DGPS update
	14 DGPS reference station ID
    

5E  --- GPGLL Position
     0 GPGLL
0    1 Latitude DDMM.MMMM
1    2 Latitude N/S
2    3 Longitude DDDMM.MMMM
3    4 Longitude E/W
     5 UTC Time HHMMSS.SSS
4    6 Status  A = Valid, V = Invalid


    --- GPVTG Course Over Ground
     0 GPVTG   
     1 Course in degrees
     2 Reference   T = True heading
     3 Course in degrees
     4 Reference   M = Magnetic heading
     5 Horizontal speed
     6 Units  N = Knots
     7 Horizontal speed
     8 Units  K = KM/h


    --- GPZDA Date & Time
     0 GPZDA
     1 UTC Time HHMMSS
     2 Day 01 to 31 DD 
     3 Month 01 to 12 MM 
     4 Year YYYY 
     5 Local zone hours, -13 to +13  HH 
     6 Local zone minutes 0 to 59 (same sign as hours) MM


    --- GPGSA Active Satellites
     0 GPGSA   
     1 Mode  A = Auto 2D/3D, M = Forced 2D/3D
     2 Mode  1 = No fix  2 = 2D  3 = 3D
     3 Satellite used on channel 1
     4 Satellite used on channel 2
     5 Satellite used on channel 3
     6 Satellite used on channel 4
     7 Satellite used on channel 5
     8 Satellite used on channel 6
     9 Satellite used on channel 7
    10 Satellite used on channel 8
    11 Satellite used on channel 9
    12 Satellite used on channel 10
    13 Satellite used on channel 11
    14 Satellite used on channel 12
    15 Position dilution of precision
    16 Horizontal dilution of precision
    17 Vertical dilution of precision


    --- GPGSV Satellites in View
     0 GPGSV   
     1 Number of messages in complete message (1-3)
     2 Sequence number of this entry (1-3)
     3 Satellites in view   
     4 Satellite ID 1  Range is 1-32
     5 Elevation 1     Elevation in degrees (0-90)
     6 Azimuth 1       Azimuth in degrees (0-359)
     7 SNR 1           Signal to noise ration in dBHZ (0-99)
     8 Satellite ID 2  Range is 1-32
     9 Elevation 2     Elevation in degrees (0-90)
    10 Azimuth 2       Azimuth in degrees (0-359)
    11 SNR 2           Signal to noise ration in dBHZ (0-99)
    12 Satellite ID 3  Range is 1-32
    13 Elevation 3     Elevation in degrees (0-90)
    14 Azimuth 3       Azimuth in degrees (0-359)
    15 SNR 3           Signal to noise ration in dBHZ (0-99)
    16 Satellite ID 4  Range is 1-32
    17 Elevation 4     Elevation in degrees (0-90)
    18 Azimuth 4       Azimuth in degrees (0-359)
    19 SNR 4           Signal to noise ration in dBHZ (0-99)
*/
