// -----------------------------
// UI CODE
// -----------------------------

u8 x,y;
u8 blink;

void scrollup()
{
 REFRESH 
 #if defined(WIN32)
 memmove(video_ram+text_ty*40,video_ram+(text_ty+1)*40,9*40); 
 memset(video_ram+24*40,' ',40);
 #else 
 __asm__("lda $1");
 __asm__("sta %v",ch);
 __asm__("sei"); 
 __asm__("and $fc");
 __asm__("sta $1");
 
 __asm__("ldx #40"); 
 __asm__("scrollloop:"); 
 __asm__("dex");
 
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+0+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+0)*40);
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+1+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+1)*40);
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+2+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+2)*40);
 
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+3+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+3)*40); 
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+4+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+4)*40);
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+5+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+5)*40);
 
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+6+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+6)*40);
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+7+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+7)*40); 
 __asm__("lda %w,x",TTVIDEOMEM+(text_ty+8+1)*40);
 __asm__("sta %w,x",TTVIDEOMEM+(text_ty+8)*40); 
 
 //__asm__("inx");
 __asm__("cpx #0");
 __asm__("bne scrollloop");  

 __asm__("lda %v",ch);
 __asm__("sta $1");
 __asm__("cli");
 
 __asm__("lda #32");
 __asm__("ldx #39"); 
 __asm__("scrollloop2:"); 
 __asm__("dex"); 
 __asm__("sta %w,x",TTVIDEOMEM+24*40);
 __asm__("cpx #0");
 __asm__("bne scrollloop2");  
 
 #endif
 memmove(video_colorram+text_ty*40,video_colorram+(text_ty+1)*40,9*40); 
}

u16 ii,ll,spl,align=0;

void ui_clear()
{
 text_y=0,text_x=0;al=0;
 if(clearfull)
  {
   memset(TVIDEOMEM+status_y*40,' ',SCREEN_W);
   memset(video_colorram+status_y*40,0,SCREEN_W);
   clearfull=0;
  }
 memset(TVIDEOMEM+TVIDEORAM_OFFSET,' ',TVIDEORAM_SIZE);
 memset(video_colorram+TVIDEORAM_OFFSET,0,TVIDEORAM_SIZE);
}

#define ALIGN_LEFT   0
#define ALIGN_RIGHT  1
#define ALIGN_CENTER 2

u8 _pch,b_bch,_ech,_cpl[2],_buffer[SCREEN_W+2],_cbuffer[SCREEN_W+2];
u8*btxt;

void _savechpos()
{
 btxt=txt;
 b_bch=_bch;
}
void _restorechpos()
{
 txt=btxt;
 _bch=b_bch;
}
void _getnextch()
{
 _pch=_ch;
 #if defined(packed_strings)
  if(_ech)
   {
    _ch=_ech;
    _ech=0;
   }
  else  
  if(_bch)
   {
    _ch=_bch;
    _bch=0;
   }  
  else
   if(txt==etxt)
    _ch=0;
   else     
    {
     _ch=*txt++;
     if(_ch&0x80)
      {
       _ch=_ch&0x7f;
       memcpy(_cpl,packdata+(_ch<<1),2);
       _ch=_cpl[0];
       _bch=_cpl[1];
      }
    }  
 #else
 if(txt==etxt)
  _ch=0;
 else  
  _ch=*txt++;
 #endif
}

void cr()
{
 REFRESH
 text_x=0;
 text_y++;
 if(text_ty+text_y>=SCREEN_H)
  {  
   scrollup();
   text_y--;
  }
}

void core_cr()
{
 REFRESH
 txt_x=0;
 txt_y++;
 if((_ch==' ')||(_ch==FAKE_CARRIAGECR)) 
  _getnextch();
 if(txt_y>=SCREEN_H)
 {
  scrollup();
  txt_y--;
 }
 al++;
}

u8 v,u;

void core_drawtext()
{  
  _getnextch();
  while(_ch)
   {        
    if(al+1>=text_stoprange) 
     {
      _ech=_ch;     
      return;
     }
    if(_ch==FAKE_CARRIAGECR)
     {
      core_cr();
      _getnextch();
     }
    else
     {
      align=ALIGN_LEFT;spl=ll=0;
      while(_ch&&(ll+txt_x<SCREEN_W)&&(_ch!=FAKE_CARRIAGECR))
      {
       if(_ch==ESCAPE_CHAR)
        {
         _getnextch();
         switch(_ch)
         {
         case 'c'-'a'+1:
          align=ALIGN_CENTER;
         break; 
         case 'r'-'a'+1:
          align=ALIGN_RIGHT;
         break; 
         case 'l'-'a'+1:
          align=ALIGN_LEFT;
         break; 
         case 'g'-'a'+1:
          txt_col=COLOR_GRAY2;
          break;
          case 'y'-'a'+1:
          txt_col=COLOR_YELLOW;
          break;
         case 'w'-'a'+1:
          txt_col=COLOR_WHITE;
          break;
          case 'V'-'A'+65:
           u=1;
          case 'v'-'a'+1:
           v=0;
           while(vrb[v])
            {
             _buffer[ll]=vrb[v]+txt_rev;
             if(u)
              {_buffer[ll]+=64;u=0;}
             _cbuffer[ll]=txt_col; 
             ll++;v++;
            }
          break;
         }
         _getnextch();
        }
       else
        {
        if(_ch==' ')
         {
          spl=ll;
          _savechpos();
         }
        _buffer[ll]=_ch+txt_rev;_cbuffer[ll]=txt_col; 
        ll++;
        _getnextch();
        }
      }
      if(ll+txt_x>=SCREEN_W)
       {
        _restorechpos();
        ll=spl;
        _getnextch(); 
       }
      switch(align)
       {
       case ALIGN_CENTER:
         txt_x+=(SCREEN_W-ll)>>1;
        break;
        case ALIGN_RIGHT:
         txt_x+=(SCREEN_W-ll);
        break;
       }
      memcpy(video_ram+txt_y*40+txt_x,_buffer,ll);
      memcpy(video_colorram+txt_y*40+txt_x,_cbuffer,ll);
      txt_x+=ll;      
      if(_ch==0)
       break;
      else
       core_cr();           
    }
  }
}

void status_update()
{
 strid=roomnameid[room];
 if(strid!=255)
  {str=advnames;_getstring();txt=ostr;}
 else
  txt="";
 if(*txt)
  {
  memset(video_colorram+status_y*40,COLOR_YELLOW,40);
  memset(video_ram+status_y*40,160,40);
  al=0;txt_col=COLOR_YELLOW;txt_rev=128;txt_x=0;txt_y=status_y;
  core_drawtext();
  }
}

void hide_blink()
{
 video_colorram[txt_y*40+(txt_x)]=COLOR_BLACK;
}

void do_blink()
{
 blink++;
 if(blink>90)
  {
   u8 ch=video_colorram[txt_y*40+(txt_x)];
   if(ch==COLOR_BLACK)
    ch=COLOR_GRAY2;
   else
    ch=COLOR_BLACK;
   video_colorram[txt_y*40+(txt_x)]=ch;
   video_ram[txt_y*40+(txt_x)]=108;
   blink=0;
  }
}

char charmap(char c)
{
 if((c>='0')&&(c<='9'))
   c=c-'0'+48;
  else
  if((c>='A')&&(c<='Z'))
   c=c-'A'+65;
  else
   if((c>='a')&&(c<='z'))
    c=c-'a'+1;
   else
    if(c==' ')
     c=32;
    else
     if(c=='.')
      c=46;
     else
      if(c==',')
       c=44;
      else      
       c=0;
 return c;
}

void parser_update()
{  
 txt=">";
 al=0;txt_col=COLOR_GRAY2;txt_rev=0;txt_x=0;txt_y=text_ty+text_y;
 core_drawtext();
 txt=strcmd;
 txt_col=COLOR_GRAY2;
 core_drawtext(); 
 video_colorram[txt_y*40+txt_x]=COLOR_BLACK;                         
 video_ram[txt_y*40+txt_x]=' ';
 al=0;
 REFRESH
}

#define SCNKEY 0xFF9F
#define GETIN  0xFFE4

void ui_getkey()
{
 while(1)
  {
#if defined(WIN32)
 ch=cgetc();
#else
 __asm__("JSR %w",SCNKEY);
 __asm__("JSR %w",GETIN);
 __asm__("STA %v",ch);
#endif
 if(ch)
  break;
 REFRESH
  }
 #if defined(WIN32)
 if((ch>='a')&&(ch<='z'))
  ch=ch-'a'+'A';
 #endif
}

void ui_waitkey()
{
 ll=18;
 while(ll<21)
  {
  video_colorram[24*40+ll]=COLOR_GRAY1;
  video_ram[24*40+ll]=46;
  ll++;
  }
 ui_getkey();
 ll=18;
 while(ll<21)
  {
  video_colorram[24*40+ll]=COLOR_GRAY1;
  video_ram[24*40+ll]=' ';
  ll++;
  }
 al=0; 
}

void ui_text_write(u8*text)
{ 
 txt=text;
 txt_col=COLOR_WHITE; 
 if(text_attach)
  text_attach=0;
 else
  {
   txt_rev=0;txt_x=0;txt_y=text_ty+text_y;
  }
 while(1)
  {  
  core_drawtext();
  if(_ch==0)
   {
   if(txt[-1]=='+')
    {text_attach=1;if(txt_x) txt_x--;}
   else
    {
    text_y=txt_y-text_ty;
     cr();
    }
   break;
   }
  else
   ui_waitkey();
  }  
 al++; 
}

void room_load()
{
 while(1)
 {
 cmd=vrb_onleave;obj1=255;adv_run();

 room=newroom;
 os_roomimage_load(); 

 executed=0;
 if((roomattr[room]&1)==0)
  {
   roomattr[room]|=1;
   cmd=vrb_onfirst;
   obj1=255;adv_run();
  }
 if(executed==0)
  {
   cmd=vrb_onenter;
   obj1=255;
   adv_run();
  }
  if(nextroom!=meta_nowhere)
  {
   newroom=nextroom;
   nextroom=meta_nowhere;
  }
  else
   break;
 }
}

void execute()
{
 cr();
 
 str=strcmd;
 adv_parse();
 
 icmd=0;strcmd[icmd]=0;
 parser_update();
}

void IMAGE_clear()
{
 memset(TOPBITMAP,0,4*1000); 
 memset(video_colorram,0,status_y*40-1);
 memset(VIDEOMEM,0,status_y*40-1);
 ui_clear();
}

u8*t1,*t2,*t3,*ot1,*ot2,*ot3;
u16 wC,wwC,oxC,oxB;

void bytemem()
{
 hunpack(t1,ADDR(0xC000));
 t1=ADDR(0xC000);
 if(oxC)
  ot1+=oxC;
 for(y=0;y<m_bitmap_h;y+=8) 
  {memcpy(ot1,t1,wC);t1+=wC;ot1+=wwC;}
}

void ui_image_draw()
{ 
 if(m_bitmap_ox||m_bitmap_oy)
  oxB=m_bitmap_ox+(m_bitmap_oy>>3)*320;
 else
  oxB=(320-m_bitmap_w)>>1;
 
 oxC=oxB>>3;
 wC=m_bitmap_w>>3;wwC=SCREEN_W;
 t1=m_bitmapcol;
 ot1=video_colorram;
 bytemem();
 t1=m_bitmapscrcol;
 ot1=VIDEOMEM;
 bytemem();
 
 oxC=oxB;
 wC=m_bitmap_w;wwC=320;
 t1=m_bitmap;
 ot1=bitmap_image;
 bytemem();
  
 
 t2=m_bitmapscrcol;
 t3=m_bitmap;
  
 ot1=video_colorram;
 ot2=VIDEOMEM;
 ot3=bitmap_image;
 /*
 if(oxC)
  {
#if defined(ONTHEFLYCLEAN)  
   memset(ot1,0,oxC);
   memset(ot2,0,oxC);
#endif   
   ot1+=oxC;
   ot2+=oxC;
   ot3+=oxB;
  }
 
 for(y=0;y<m_bitmap_h;y+=8)
  {
   memcpy(ot1,t1,wC);t1+=wC;ot1+=wC;
#if defined(ONTHEFLYCLEAN)     
   memset(ot1,0,oxC<<1);
#endif   
   ot1+=oxC<<1;
   memcpy(ot2,t2,wC);t2+=wC;ot2+=wC;
#if defined(ONTHEFLYCLEAN)     
   memset(ot2,0,oxC<<1);
#endif   
   ot2+=oxC<<1;
   memcpy(ot3,t3,m_bitmap_w);t3+=m_bitmap_w;ot3+=320;
  }
 */
}

void ui_image_clean()
{
 memset(VIDEOMEM,0,(96/8)*40);   
 memset(video_colorram,0,(96/8)*40); 
 memset(bitmap_image,0,4000);
 
}

void ui_room_update_start()
{
 REFRESH
 
 #if !defined(ONTHEFLYCLEAN)  
 ui_image_clean();
 #endif
 
 status_update(); 
}


void ui_room_update()
{
 REFRESH
 if(imagemem)
  {
   m_bitmap_w=*(u16*)(imagemem+0);   
   m_bitmap_h=*(u8*)(imagemem+2);
   m_bitmap_oy=*(u8*)(imagemem+5);
   m_bitmap_ox=*(u16*)(imagemem+3);
      
   m_bitmapscrcol=imagemem+14+*(u16*)(imagemem+8);
   m_bitmapcol=imagemem+14+*(u16*)(imagemem+10);
   m_bitmap=imagemem+14+*(u16*)(imagemem+12);
   
   ui_image_draw();
  }
 
 if(rightactorimg!=meta_none)
  {
   /*m_bitmapscrcol=itembitmap02_screencol;
   m_bitmapcol=itembitmap02_col;
   m_bitmap=itembitmap02_bitmap;
   m_bitmap_h=itembitmap02_h;
   m_bitmap_w=itembitmap02_w;  
   m_bitmap_ox=8;
   m_bitmap_oy=8;*/

   ui_image_draw();
  }
 if(leftactorimg!=meta_none)
  {
   /*m_bitmapscrcol=itembitmap01_screencol;
   m_bitmapcol=itembitmap01_col;
   m_bitmap=itembitmap01_bitmap;
   m_bitmap_h=itembitmap01_h;
   m_bitmap_w=itembitmap01_w;  
   m_bitmap_ox=320-itembitmap01_w-8;
   m_bitmap_oy=8;*/

   ui_image_draw();
  }
   
}