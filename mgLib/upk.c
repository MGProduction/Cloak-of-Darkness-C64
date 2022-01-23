#if defined(WIN32)
#include "fakeC64.h"
#else
#include <c64.h>
#endif
#include "mgLib.h"
#if defined(WIN32)
//#define BRUTE_C_VERSION
#endif

#if defined(BRUTE_C_VERSION)
u16 unpack(const u8*pbuf,u8*buf)
{
 u16 p=0,i=0;
 u8  chback=pbuf[p++],chbackfar=pbuf[p++],cheof=pbuf[p++];
 while(1)  
  {
   u8 ch=pbuf[p++];
   if(ch==cheof)
    break;
   if((ch==chback)||(ch==chbackfar))
    {
     u16 w,b;
     u8  l; 
     w=pbuf[p++];     
     if(ch==chbackfar)
      {
       u8 ww=pbuf[p++];
       w+=(ww<<8);
      } 
     else
     if(w==0)
      {
       u8 c;
       l=pbuf[p++];
       c=pbuf[p++];
       while(l)
        { 
         buf[i++]=c;
         --l;
        }       
       continue; 
      }       
     l=pbuf[p++];
     b=i-w;
     while(l)
      { 
       buf[i++]=buf[b++];
       --l;
      }       
    }
   else 
    buf[i++]=ch;
  }
 return i; 
}
#else
extern u8* asm_src;
extern u8* asm_dst;
void fastcall asm_bruteunpack(void);

void unpack(const u8*pbuf,u8*buf)
{
 asm_src=(u8*)pbuf;
 asm_dst=buf;
 asm_bruteunpack();
}
#endif