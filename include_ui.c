// -----------------------------
// UI CODE
// -----------------------------

u8 x,y;
u8 blink;

void scrollup()
{
 REFRESH
 memmove(video_ram+text_ty*40,video_ram+(text_ty+1)*40,9*40);
 memmove(video_colorram+text_ty*40,video_colorram+(text_ty+1)*40,9*40);
 memset(video_ram+24*40,' ',40);
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

void ui_clear()
{
 text_y=0,text_x=0;
 if(clearfull)
  {
   memset(video_ram+status_y*40,' ',(SCREEN_H-status_y)*40);
   memset(video_colorram+status_y*40,0,(SCREEN_H-status_y)*40);
   clearfull=0;
  }
 else
  {
  memset(video_ram+text_ty*40,' ',(SCREEN_H-text_ty-1)*40);
  memset(video_colorram+text_ty*40,0,(SCREEN_H-text_ty-1)*40);
  }
}

void core_cr()
{
 REFRESH
 txt_x=0;
 txt_y++;
 if((*txt==' ')||(*txt==FAKE_CARRIAGECR)) txt++; 
 if(txt_y>=SCREEN_H)
 {
  scrollup();
  txt_y--;
 }
}

#define ALIGN_LEFT   0
#define ALIGN_RIGHT  1
#define ALIGN_CENTER 2

u16 al,ii,ll,sp,spl,align=0;
void core_drawtext()
{ 
  al=0;
  while(*txt)
   {
    u8 c=*txt;
    if(c==FAKE_CARRIAGECR)
     {
      txt++;
      core_cr();al++;
     }
    else
     {
      align=ALIGN_LEFT;spl=sp=ii=0;
      ll=txt_x;
      while(txt[ii]&&(ll<SCREEN_W)&&(txt[ii]!=FAKE_CARRIAGECR))
      {
       if(txt[ii]==ESCAPE_CHAR)
        {
         ii++;
         if(txt[ii]==3)
          align=ALIGN_CENTER;
         ii++;
        }
       else
        {
        if(txt[ii]==' ')
         {
          sp=ii;
          spl=ll;
         }
        ii++;ll++;
        }
      }
      if(ll>=SCREEN_W)
       {ii=sp;ll=spl;}
      switch(align)
       {
       case ALIGN_CENTER:
         txt_x+=(SCREEN_W-ll)>>1;
        break;
        case ALIGN_RIGHT:
         txt_x+=(SCREEN_W-ll);
        break;
       }
      while(ii--)
       {
        c=*txt++;
        if(c==ESCAPE_CHAR)
        {
         c=*txt++;
         switch(c)
         {
         case 'g'-'a'+1:
          txt_col=COLOR_GRAY2;
          break;
          case 'y'-'a'+1:
          txt_col=COLOR_YELLOW;
          break;
         case 'w'-'a'+1:
          txt_col=COLOR_WHITE;
          break;
         }
         ii--;
        }
        else
         {
         video_colorram[txt_y*40+txt_x]=txt_col;                         
         video_ram[txt_y*40+txt_x]=c+txt_rev;
         txt_x++;
         }
      }     
     c=*txt;
     if(c==0)
      break;
     else
     {
      core_cr();
      al++;
     }
     if(al>=8)
      return;
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
  txt_col=COLOR_YELLOW;txt_rev=128;txt_x=0;txt_y=status_y;
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
   video_ram[txt_y*40+(txt_x)]=100;
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
      if(c=='>')
      c=62;
     else
      if(c=='_')
       c=100;
      else
       c=c;
 return c;
}

void parser_update()
{  
 txt=">";
 txt_col=COLOR_GRAY2;txt_rev=0;txt_x=0;txt_y=text_ty+text_y;
 core_drawtext();
 txt=strcmd;
 txt_col=COLOR_GRAY2;
 core_drawtext(); 
 video_colorram[txt_y*40+txt_x]=COLOR_BLACK;                         
 video_ram[txt_y*40+txt_x]=' ';
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
  if(*txt==0)
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

void IMAGE_load()
{ 
 //u8*t;

 if((m_bitmap_w==320)&&(m_bitmap_h==96))
 {
  memcpy(video_colorram,m_bitmapcol,12*40);
  memcpy(VIDEOMEM,m_bitmapscrcol,12*40);
  memcpy(bitmap_image,m_bitmap,96*40);
  /*
  else
   { 
    unpack((u8*)m_bitmapcol,video_colorram);    
    
    unpack((u8*)m_bitmapscrcol,VIDEOMEM);    
    
    unpack((u8*)m_bitmap,bitmap_image);
   } 
  */  
  /*{
   u8 bBUF[64000];
   u8 ret,*s,*d;
   int i,err=0;
   FILE*f=fopen("c:\\git\\due.vsf","rb");
   ret=fread(bBUF,1,0x84,f);
   ret=fread(bBUF,1,64*1024,f);
   s=VIDEOMEM;
   d=bBUF+(VIDEOMEM-bMEM);
   for(i=0;i<96/8*40;i++)
    if(s[i]!=d[i])
     err++;
   s=video_colorram;
   d=bBUF+(video_colorram-bMEM);
   for(i=0;i<96/8*40;i++)
    if(s[i]!=d[i])
     err++;
   s=bitmap_image;
   d=bBUF+(bitmap_image-bMEM);
   for(i=0;i<96*40;i++)
    if(s[i]!=d[i])
     err++;  
   ret=memcmp(VIDEOMEM,bBUF+(VIDEOMEM-bMEM),96/8*40);
   ret=memcmp(video_colorram,bBUF+(video_colorram-bMEM),96/8*40);
   ret=memcmp(bitmap_image,bBUF+(bitmap_image-bMEM),96*40);
   fclose(f);
  }*/
  REFRESH
 }
 else
  { 
   /*
   unpack((u8*)m_bitmap,temp);
   ttemp=temp;tbitmap_image=bitmap_image;
   if(m_bitmap_ox==255)
    tbitmap_image+=(320-m_bitmap_w)/2;
   else
    tbitmap_image+=m_bitmap_ox;
   if(m_bitmap_oy==255)
    tbitmap_image+=(96-m_bitmap_h)/2*320;
   else
   {
    tbitmap_image+=(m_bitmap_oy*320)/8;
    t=tbitmap_image-320+7;
    x=0;
    while(x<m_bitmap_w)
    {
     *t=0;t+=8;
     x+=8;
    }
    t=tbitmap_image+m_bitmap_h*320/8;
    x=0;
    while(x<m_bitmap_w)
    {
     *t=0;t+=8;
     x+=8;
    }
   }
   ww=m_bitmap_w;
   for(y=0;y<m_bitmap_h/8;y++)
    {   
     memcpy(tbitmap_image,ttemp,ww);
     ttemp+=ww;   
     tbitmap_image-=8;
     x=8;
     while(x--)
     {*tbitmap_image&=0xfc;tbitmap_image++;   }     
     tbitmap_image+=ww; 
     x=8;
     while(x--)
     {*tbitmap_image&=0x3f;tbitmap_image++;   }     
     tbitmap_image+=320-ww-8; 
    } 
    
   unpack((u8*)m_bitmapscrcol,temp);
   ttemp=temp;tbitmap_image=VIDEOMEM;
   if(m_bitmap_ox==255)
    tbitmap_image+=(320-m_bitmap_w)/2/8;
   else
    tbitmap_image+=m_bitmap_ox/8;
   if(m_bitmap_oy==255)
    tbitmap_image+=(96-m_bitmap_h)/2*40;
   else
    tbitmap_image+=(m_bitmap_oy*40)/8;
   ww=m_bitmap_w/8;
   for(y=0;y<m_bitmap_h/8;y++)
   {  
    memcpy(tbitmap_image,ttemp,ww);
    ttemp+=ww;
    tbitmap_image+=SCREEN_W;
   }

   unpack((u8*)m_bitmapcol,temp);
   ttemp=temp;tbitmap_image=video_colorram;
   if(m_bitmap_ox==255)
    tbitmap_image+=(320-m_bitmap_w)/2/8;
   else
    tbitmap_image+=m_bitmap_ox/8;
   if(m_bitmap_oy==255)
    tbitmap_image+=(96-m_bitmap_h)/2*40;
   else
    tbitmap_image+=(m_bitmap_oy*40)/8;
   ww=m_bitmap_w/8;
   for(y=0;y<m_bitmap_h/8;y++)
   {
    memcpy(tbitmap_image,ttemp,ww);
    ttemp+=ww;
    tbitmap_image+=SCREEN_W;
   }
   */
  }

 
}

void IMAGE_clean()
{
 memset(VIDEOMEM,0,(96/8)*40);   
 memset(video_colorram,0,(96/8)*40); 
 memset(bitmap_image,0,4000);
 REFRESH  
}

void ui_room_update()
{
 IMAGE_clean();
 
 status_update(); 
 
 if(buf_w&&buf_h)
  {
   m_bitmap_w=buf_w;
   m_bitmap_h=buf_h;
   m_bitmap=buf_bitmap;
   m_bitmapscrcol=buf_bitmapscrcol;
   m_bitmapcol=buf_bitmapcol;

   m_bitmap_ox=255;
   m_bitmap_oy=255;
   IMAGE_load();
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

   IMAGE_load();
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

   IMAGE_load();
  }
}