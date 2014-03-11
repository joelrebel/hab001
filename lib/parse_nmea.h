
static uint8_t gps_parse_buf[64];                   //
                                                    //
typedef struct {                                    // GPS Parse Info Structure
    const char *sentence;                           // Sentence
    const uint16_t words;                           // Words in sentence
} TGPI;                                             //
                                                    //
static const TGPI gpi[] = {                         //
   { "GPRMC",    0x03FE },                         //
   { "GPGSV",    0x0FFF },                         //
   { "GPGGA",    0x0FFF },                         //setting this to the equivalent bits enables parsing for those bits
//	 {"GPVTG", 0x3FE},
//    { "GPGLL",    0x005E },
    { 0,          3      }                          //This says, parse x sentences (2 if we want to parse GPGGA and GPVTG)
};                                                  //

static uint16_t hex(const uint8_t c);
unsigned parse_gps(uint8_t c);
uint8_t *get_gps_string(unsigned n);

