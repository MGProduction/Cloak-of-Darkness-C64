// ---------------------------------------------------------------
// Copyright (c) 2021/2022 Marco Giorgini
// ---------------------------------------------------------------
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// ---------------------------------------------------------------

#include "C64_main.h"

// ---------------------------------------------------------------
// MEMORY DEFINITIONS
// ---------------------------------------------------------------

#define TEMPAREA   ADDR(0xB000)
#define FILECACHE2 ADDR(0xBC00)

#define FILECACHE1 ADDR(0xC000)

#define TOPBITMAP  ADDR(0xE000)
#define VIDEOMEM   ADDR(0xF000)

#if defined(USE_FONT)
#define TTVIDEOMEM 0xF400
#define TVIDEOMEM  ADDR(TTVIDEOMEM)
#else
#define TVIDEOMEM  ADDR(0x0400)
#endif
#define FONTMEM    ADDR(0xF800)

u8*video_ram=TVIDEOMEM;
u8*video_colorram=ADDR(0xD800);
u8*bitmap_image=TOPBITMAP;

u8*basecachemem,*cachemem;
u16 csize;

#define MAX_CMD   80
#define MAX_TMP   40
#define MAX_TMP2 120
#define VRBLEN    10
u8*tmp=TEMPAREA;
u8*vrb=TEMPAREA+MAX_TMP;
u8*strcmd=TEMPAREA+MAX_TMP+VRBLEN;
u8*tmp2=TEMPAREA+MAX_TMP+MAX_CMD+VRBLEN;
u8*loadram=TEMPAREA+MAX_TMP+MAX_CMD+MAX_TMP2+VRBLEN;
u8 icmd=0;

u8*ttemp;
u8*tbitmap_image;
u16 ww;

u8* m_bitmap;
u8* m_bitmapcol;
u8* m_bitmapscrcol;
u16 m_bitmap_w,m_bitmap_ox;
u8  m_bitmap_h,m_bitmap_oy;
u8  load;


#if defined(USE_DISK)
#if defined(WIN32)
char*FILENAME(char*nm)
{
 static char nn[256];
 sprintf(nn,"%simg/%s",basepath,nm);
 return nn;
}
#else
#define FILENAME(nm) nm
#endif
#endif

// ---------------------------------------------------------------
#include "include_storytllr64.c"
// ---------------------------------------------------------------
#include "include_irq.c"
// ---------------------------------------------------------------
#include "include_ui.c"
// ---------------------------------------------------------------
#if defined(USE_HIMAGE)
#include "images.h"
#endif
// ---------------------------------------------------------------

// ---------------------------------------------------------------
// GLOBALS
// ---------------------------------------------------------------

#if defined(WIN32)
u8 video_scroll_x,video_scroll_y;
#endif



// ---------------------------------------------------------------

u8 cached_room[2]={255,255};
u8*cached_mem[2];

void irq_detach()
{
 IRQ_gfx_reset();
 do_textmode();
 REFRESH
}

void irq_attach()
{
 IRQ_gfx_init();
}

#if !defined(USE_HIMAGE)
void os_core_roomimage_load()
{ 
 FILE*fp;  
 char nm[]="room01";
 u8 r=imageid+1;
 nm[4]='0'+(r/10); 
 nm[5]='0'+(r%10); 
    
 //vid_setcolorBRD(COLOR_WHITE);
 fp = fopen(FILENAME(nm), "rb");  
 if(fp)
  {
   u16 psize,lsize;
   csize=0;         
   fread(cachemem,14,1,fp);
   csize+=14;
   lsize=0;
   
   psize=*(u16*)(cachemem+8);*(u16*)(cachemem+8)=lsize;lsize+=psize;
   
   psize=*(u16*)(cachemem+10);*(u16*)(cachemem+10)=lsize;lsize+=psize;
   
   psize=*(u16*)(cachemem+12);*(u16*)(cachemem+12)=lsize;lsize+=psize;
   
   cachemem+=csize;
   psize=fread(cachemem,lsize,1,fp);
   cachemem+=lsize;

   fclose(fp);
  }  
}
#endif

void os_roomimage_load()
{  
#if !defined(USE_HIMAGE)
 irq_detach();
#endif
 #if defined(USE_DISK)
 load=0;
 imagemem=NULL;  
 if(roomimg[room]!=255)
  {
   if(cached_room[0]!=roomimg[room])   
    {     
     cached_room[0]=roomimg[room];
     
     imageid=roomimg[room];
     cachemem=basecachemem;   
          
#if defined(USE_HIMAGE)
     cached_mem[0]=imagesdata+imagesidx[imageid]; 
#else          
     cached_mem[0]=cachemem;     
     os_core_roomimage_load();
#endif     
     load=1;
    }   
   imagemem=cached_mem[0];  
  }
 else
  load=1; 
 
#endif

 if(load)
  ui_room_update_start(); 

 ui_room_update();
 if(roomovrimg[room]!=255)
  {
   if(cached_room[1]!=roomovrimg[room])
    {
     imageid=roomovrimg[room];
     cached_room[1]=roomovrimg[room];
#if defined(USE_HIMAGE)
     cached_mem[1]=imagesdata+imagesidx[imageid]; 
#else          
     cached_mem[1]=cachemem;
     os_core_roomimage_load();
#endif               
    } 
   imagemem=cached_mem[1];  
   ui_room_update();
  }
 #if !defined(USE_HIMAGE) 
 irq_attach();
 #endif
 
 REFRESH
 

 //str=advdesc;strid=roomdescid[room];getstring();txt=ostr;
 //text_write(ostr);

 /*m_bitmapscrcol=char01_screencol;
 m_bitmapcol=char01_col;
 m_bitmap=char01_bitmap;
 m_bitmap_h=char01_h;
 m_bitmap_w=char01_w;  
 m_bitmap_ox=8;
 m_bitmap_oy=8;

 IMAGE_load();

 m_bitmap_ox=320-char01_w-8;

 IMAGE_load();

 m_bitmapscrcol=obj01_screencol;
 m_bitmapcol=obj01_col;
 m_bitmap=obj01_bitmap;
 m_bitmap_h=obj01_h;
 m_bitmap_w=obj01_w;  
 m_bitmap_ox=8;
 m_bitmap_oy=8+char01_h+8;

 IMAGE_load();

 m_bitmap_ox=8+obj01_w;
 IMAGE_load();*/

}

#if defined(USE_FONT)
#include "font.h"
void font_load()
{
 u8*tmp=FILECACHE1; 
 hunpack(font,tmp);
 memcpy(FONTMEM,tmp,128*8);
 for(i=0;i<128*8;i++)
  tmp[i]=255-tmp[i];
 memcpy(FONTMEM+128*8,tmp,128*8);
}
#endif

#if defined(advcartridgeondisk)
void loadcartridge()
{
 FILE*f;
// vid_setcolorBRD(COLOR_YELLOW);
#if defined(WIN32) 
 char card[256];
 sprintf(card,"%sadvcartridge",basepath);
 f=fopen(card,"rb");
#else
 f=fopen("advcartridge","rb");
#endif  
 if(f)
  {
   u16 iln,ln;   
   
   fread(&iln,sizeof(iln),1,f); // size of cartridge
   
   #if defined(WIN32) 
   advcartridge=ADDR(0x5000);
   #else
   advcartridge=malloc(16);
   #endif
   
   fread(advcartridge,iln,1,f);
       
   freemem=ADDR(0xC000)-(advcartridge+iln);
   
   fread(tmp2,64,1,f);
   
   iln=*(u16*)tmp2;tmp2+=sizeof(iln);   
   //fread(&iln,sizeof(iln),1,f);
   
   //fread(&ln,sizeof(ln),1,f);
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   opcode_vrbidx_count=ln;
   
   //fread(&ln,sizeof(ln),1,f);
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   obj_count=(u8)ln;
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   room_count=(u8)ln;
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   var_count=(u8)ln;
   
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   advnames=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   advdesc=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   msgs=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   verbs=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   objs=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   packdata=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   opcode_vrbidx=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   opcode_pos=(u16*)(advcartridge+ln);    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   opcode_len=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   opcode_data=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   objnameid=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   objdescid=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   objimg=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   objattr=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   objloc=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   roomstart=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   roomnameid=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   roomdescid=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   roomimg=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   roomovrimg=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   roomattr=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   bitvars=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   vars=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   origram=advcartridge+ln;    
   ln=*(u16*)tmp2;tmp2+=sizeof(ln);   
   origram_len=ln;
   
   basecachemem=origram+ln;
   
//   vid_setcolorBRD(ch++);
   fclose(f);
//   vid_setcolorBRD(ch++);
   
   tmp2=TEMPAREA+MAX_TMP+MAX_CMD;
   
  }
// vid_setcolorBRD(COLOR_BLACK); 
}
#endif 

void os_init()
{
 vid_setcolorBKG(COLOR_BLACK);
 vid_setcolorBRD(COLOR_BLACK);

#if defined(USE_FONT) 
 font_load();
#endif 

 // key autorepeat OFF
 __asm__("lda $028A");
 __asm__("and #$3f");
 __asm__("ora #$40");
 __asm__("sta $028A");
 
 memset(video_ram,' ',1000);
 memset(video_colorram,COLOR_BLACK,1000);
 REFRESH
 
 IRQ_gfx_init();  
 
 

 
}

void os_reset()
{
 __asm__("JSR $FCE2");
}

#if defined(WIN32)
int c64_main()
#else
int main()
#endif
{

 os_init(); 

 while(1)
 {
 
#if defined(advcartridgeondisk)
   irq_detach();
   loadcartridge();
   irq_attach();
#else
#if defined(WIN32)
   basecachemem=ADDR(0x8000);
#else
   basecachemem=origram+origram_len;   
#endif   
#endif 

   IMAGE_clear();
   adv_start();
   
   parser_update();
   
   quit_request=ch=0;
   while(quit_request==0)
   {  
  #if defined(WIN32)
    ch=cgetc();
  #else
    __asm__("JSR %w",SCNKEY);
    __asm__("JSR %w",GETIN);
    __asm__("STA %v",ch);
  #endif
    if(ch)
     {
      hide_blink();
      if(ch==CHAR_RETURN)
       execute();
      else
       {
        if(ch==CHAR_BACKSPACE)
        {
         if(icmd)
          strcmd[--icmd]=0;
        }
        else
        {
         if(icmd<MAX_CMD)
          {
           ch=charmap(ch);
           if(ch)
            {
             strcmd[icmd++]=ch;
             strcmd[icmd]=0;
            } 
          }    
        }
        parser_update();
       }
     }
    else
     do_blink();
     
    REFRESH
   }
   
  if(quit_request>=2)
   {
    if(quit_request==2)
     adv_reset();
    else
     adv_load();
    quit_request=0;
   }
  else
   break;
 }
 
 os_reset();
 
 return 0;
}

