#ifndef _vid_h__
#define _vid_h__

extern u8*video_ram;
extern u8*video_colorram;
extern u8*spriteNumberBase;
extern u8 video_scroll_x,video_scroll_y;
extern u8 video_requestupdate,video_new_scroll_x,video_new_scroll_y;

typedef struct{
 u8  *map;
 u16  h;
 u16  y;
 u8   scrY,scrH;
 u8   mode;
}verticalmap;

#define vid_setcolorBKG(col) VIC.bgcolor0=col
#define vid_setcolorBRD(col) VIC.bordercolor=col

#define vid_getcolorBKG(col) VIC.bgcolor0
#define vid_getcolorBRD(col) VIC.bordercolor

#define vid_getvideobank() (3-(CIA2.pra & 3))
#define _vid_setvideobank(bankNr) do { \
  CIA2.pra &= 0xfc; \
  CIA2.pra |= (3-(bankNr));			\
} while(0)
void vid_setvideobank(u8 bank,u8 screenbank);
void vid_setscreenbank(u8 screenbank);

#define vid_getcharactergenerator() ((((u8)VIC.addr) > 1) & (u8)7)

#define vid_setcharactergenerator(bank) do { \
  VIC.addr &= (u8)0xf1; \
  VIC.addr |= (u8)(bank << 1); \
} while(0)

#define vid_getvideoramoffsetindex() (VIC.addr >> 4)
#define vid_setvideoramoffsetindex(index) do { \
  VIC.addr &= (u8)0x0f; \
  VIC.addr |= (u8)(index << 4); \
} while(0)

#define vid_getvideoramoffset() (((unsigned int)vid_getvideoramoffsetindex()) << 10)
#define vid_getvideoramaddress() ((((unsigned int)vid_getvideobank()) << 14) + vid_getvideoramoffset())

void vid_loadScreen(const char*screenA,u8 scrY,u8 scrH,u16 y,u16 h,u8 col,u8 mode);
void vid_loadCharset(const char*charsetData,u8 chbank,u16 firstCH,u16 numCH,u8 mode);
void vid_standardCharset(void);
u8 vid_vscroll(verticalmap*v,short dy);

void fastcall vic_set_scroll(void);
void fastcall vic_reset_scroll(void);
void fastcall vic_wait_offmatrix(void);
void fastcall vic_wait_offscreen(void);
void fastcall vic_wait_rasterpos(u8 pos);

void vid_screenfill(u8 val); // asm
void vid_drawframe(u8 x,u8 y,u8 w,u8 h,u8 ch,u8 col);
void vid_drawstring(u8 x,u8 y,const char*d,u8 start,const char*remap,u8 col);

void vid_setmulticolorcharmode(u8 enable,u8 mcol1,u8 mcol2);
#endif