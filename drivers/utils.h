#ifndef _UTILS_H
#define _UTILS_H

#include <msp430g2553.h>

#define CPU_CLOCK 1L
#define CYCLES_PER_US CPU_CLOCK // depends on the CPU speed
#define CYCLES_PER_MS (CYCLES_PER_US * 1000L)

#define DELAY_US(x) __delay_cycles((x * CYCLES_PER_US))
#define DELAY_MS(x) __delay_cycles((x * CYCLES_PER_MS))



//void _delay_ms(unsigned int ms);
void blink_led(void);


int update_sector_record(int sector);
void init_sdcard(void);
int sector_flush(int sector);
int sector_start(int sector);
void sector_write(char *data);
void log_to_sdcard(char * data);
void float_to_char(float f);

int sample_gps_data(char *what);

#endif
