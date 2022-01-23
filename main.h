#if defined(WIN32)
#include "mgLib/fakeC64.h"
#else
#include <c64.h>
#endif

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "mgLib/mgLib.h"

#if defined(WIN32)
void __asm__(const char*cmd,...);
char cgetc();
#endif

#define AUDIO
#define USE_DISK

#define REFRESH vic_wait_offscreen();

// 0x0001 (default value x11 - set to x10 to have also A000-BFFFF as RAM
// %x00: RAM visible in all three areas.
// %x01: RAM visible at $A000-$BFFF and $E000-$FFFF.
// %x10: RAM visible at $A000-$BFFF; KERNAL ROM visible at $E000-$FFFF.
// %x11: BASIC ROM visible at $A000-$BFFF; KERNAL ROM visible at $E000-$FFFF.
// %0xx: Character ROM visible at $D000-$DFFF. (Except for the value %000, see above.)
// %1xx: I/O area visible at $D000-$DFFF. (Except for the value %100, see above.)

// DEFAULT FREE AREA: $0800-$9FFF, 2048-40959
//                    $C000-$CFFF, 49152-53247
// 0 = 0x0000-0x3fff
// 1 = 0x4000-0x7fff
// 2 = 0x8000-0xbfff
// 3 = 0xc000-0xffff
#define VIDEOBANK            2   //   0x8000
// %0000,  0: $0000-$03FF, 0-1023.
// %0001,  1: $0400-$07FF, 1024-2047.
// %0010,  2: $0800-$0BFF, 2048-3071.
// %0011,  3: $0C00-$0FFF, 3072-4095.
// %0100,  4: $1000-$13FF, 4096-5119.
// %0101,  5: $1400-$17FF, 5120-6143.
// %0110,  6: $1800-$1BFF, 6144-7167.
// %0111,  7: $1C00-$1FFF, 7168-8191.
// %1000,  8: $2000-$23FF, 8192-9215.
// %1001,  9: $2400-$27FF, 9216-10239.
// %1010, 10: $2800-$2BFF, 10240-11263.
// %1011, 11: $2C00-$2FFF, 11264-12287.
// %1100, 12: $3000-$33FF, 12288-13311.
// %1101, 13: $3400-$37FF, 13312-14335.
// %1110, 14: $3800-$3BFF, 14336-15359.
// %1111, 15: $3C00-$3FFF, 15360-16383.
#define SCREENBANK           5   //  +0x1400
// %000, 0: $0000-$07FF, 0-2047.
// %001, 1: $0800-$0FFF, 2048-4095.
// %010, 2: $1000-$17FF, 4096-6143.
// %011, 3: $1800-$1FFF, 6144-8191.
// %100, 4: $2000-$27FF, 8192-10239.
// %101, 5: $2800-$2FFF, 10240-12287.
// %110, 6: $3000-$37FF, 12288-14335.
// %111, 7: $3800-$3FFF, 14336-16383.
#define CHARSETBANK          3   //  +0x1800
//
// ROM                               0x9000-0x9FFFF
//
#define BASESPRITE_OFFSET   128   // +0x2000-0x3FFF


#if defined(WIN32)
#define CHAR_BACKSPACE 8
#define CHAR_RETURN '\r'
#else
#define CHAR_BACKSPACE 20
#define CHAR_RETURN '\n'
#endif

//#define NOENEMIES
//#define SINGLEENEMY

#define SCREEN_W 40
#define SCREEN_H 25

#define borderx 24
#define bordery 50

#define frameattr_multicolor 0x80
#define frameattr_widey      0x40
#define frameattr_widex      0x20
#define frameattr_overlay    0x10
#define frameattr_colormark  0x0F

#define end_level 255
#define end_line  254

#define page_end 255
