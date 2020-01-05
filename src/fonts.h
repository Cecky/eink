#ifndef fonts_h
#define fonts_h

#include <avr/pgmspace.h>

// All font data from Benedikt K.
// http://www.mikrocontroller.net/topic/54860

//Font selection (select only one font)
#define FONT_5X8
//#define FONT_5X12
//#define FONT_6X8
//#define FONT_6X10

//if defined char range 0x20-0x7F otherwise 0x20-0xFF
#define FONT_END7F
#define FONT_START (0x20) //first character

#if defined(FONT_5X8)
  #define FONT_WIDTH   (5)
  #define FONT_HEIGHT  (8)
#elif defined(FONT_5X12)
  #define FONT_WIDTH   (5)
  #define FONT_HEIGHT (12)
#elif defined(FONT_6X8)
  #define FONT_WIDTH   (6)
  #define FONT_HEIGHT  (8)
#elif defined(FONT_6X10)
  #define FONT_WIDTH   (6)
  #define FONT_HEIGHT (10)
#else
  #warning No font defined
#endif

extern const PROGMEM uint8_t font_PGM[];

#endif //fonts_h
