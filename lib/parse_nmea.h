
static uint8_t gps_parse_buf[64];                   //
                                                    //
typedef struct {                                    // GPS Parse Info Structure
    const char *sentence;                           // Sentence
    const uint16_t words;                           // Words in sentence
} TGPI;                                             //
                                                    //
static const TGPI gpi[] = {                         //
    { "GPRMC",    0x03FE },                         //
//    { "GPGGA",    0x007E },                         //

    { "GPGGA",    0x0FFF },                         //setting this to the equivalent bits enables parsing for those bits
    { "GPGLL",    0x005E },
    { 0,          1      }                          //
};                                                  //

static uint16_t hex(const uint8_t c);
unsigned parse_gps(uint8_t c);
uint8_t *get_gps_string(unsigned n);

