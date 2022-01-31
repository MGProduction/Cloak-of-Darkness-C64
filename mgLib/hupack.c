#define MASK_FWD   (1<<6)
#define MASK_BCK   (2<<6)
#define MASK_PLAIN (3<<6)

#if defined(IMPLEMENT_C_HBUNPACK)

#if !defined(WIN32)

extern u8* asm_src;
extern u8* asm_dst;
void fastcall asm_hunpack(void);

void hunpack(const u8*pbuf,u8*buf)
{
 asm_src=(u8*)pbuf;
 asm_dst=buf;
 asm_hunpack();
}
#else

u8 hb_what,hb_len;
u16 hunpack(u8*buf,u8*pbuf)
{
 u16 i=0,j=0;
 u8*tbuf;
 while(buf[i])
  {
   hb_what=buf[i++];
   hb_len=hb_what&0x3F;
   if(hb_len==63)
    hb_len=buf[i++];
   hb_what=hb_what&MASK_PLAIN;
      
   if(hb_what==MASK_PLAIN)
    {     
     while(hb_len--)
      pbuf[j++]=buf[i++];
    }
   else 
   if(hb_what==MASK_BCK)
    {
     hb_what=buf[i++];
     tbuf=pbuf+j-hb_what;
     while(hb_len--)
      pbuf[j++]=*tbuf++;
    }
   else 
   if(hb_what==MASK_FWD)
    {
     hb_what=buf[i++];
     while(hb_len--)
      pbuf[j++]=hb_what;
    }
  }
 return j; 
}
#endif

#else
u16 hunpack(u8*buf,u8*pbuf);
#endif

#if defined(IMPLEMENT_C_HBPACK)

u8 scanfwd(u8*buf,u16 i,u16 hm,u8*w)
{
 u8 ll=1;
 while((ll<255)&&(i+ll<hm)&&(buf[i]==buf[i+ll]))
  ll++;
 if(w) *w=buf[i];
 if(ll>2)
  return ll;  
 else
  return 0; 
}

u8 scanbck(u8*buf,u16 i,u16 hm,u8*w)
{
 int ii,jj,blen=0,bpos=-1;
 u8 ch=buf[i];
 for(ii=max(0,i-256);ii<i;ii++)
  if(buf[ii]==ch)
   for(jj=3;jj<256;jj++)
    if((i-ii<256)&&(ii+jj<i)&&(i+jj<hm))        
     if(memcmp(buf+ii+1,buf+i+1,jj-1)==0)
      if(jj>=blen)
       {
        bpos=ii;
        blen=jj;
       }
 if(w) *w=(u8)(i-bpos);     
 return (u8)blen;  
}

int emitplain(u8*out,u8*in,u16 len)
{
 int l=0,i;
 while(len)
  {
   int llen=min(len,255);
   if(llen<63)
    out[l++]=MASK_PLAIN|llen;
   else
    {
     out[l++]=MASK_PLAIN|63;
     out[l++]=llen;
    } 
   for(i=0;i<llen;i++)
    out[l++]=in[i];
   len-=llen; 
  }  
 return l;
}

int emitcode(u8*out,u8 mask,u8 fwd,u8 code)
{
 int l=0;
 if(fwd<63)
  {
   out[l++]=mask|fwd;
   out[l++]=code;
  }
 else
  {
   out[l++]=mask|63;
   out[l++]=fwd;
   out[l++]=code;
  }  
 return l;
}

int hpack(u8*buf,u16 hm,u8*pbuf)
{
 int i=0,j=0,le=0;
 while(i<hm)
  {
   u8  wf,wb;
   u8  fwd=scanfwd(buf,i,hm,&wf);
   u8  bck=scanbck(buf,i,hm,&wb);
   if(fwd+bck)
    {
     if(i>le)
      j+=emitplain(pbuf+j,buf+le,i-le);
     if(fwd>bck)
      {
       j+=emitcode(pbuf+j,MASK_FWD,fwd,wf);
       i+=fwd;
      } 
     else
      {
       j+=emitcode(pbuf+j,MASK_BCK,bck,wb);
       i+=bck;
      } 
     le=i; 
    }  
   else
    i++;   
  }
 if(i>le)
  j+=emitplain(pbuf+j,buf+le,i-le);
 pbuf[j++]=0; 
 
 {
 int good=0;
 u8  out[64*1024];
 if(hunpack(pbuf,out))
  if(memcmp(out,buf,hm)==0)
   good=1;
  else
  {
   int i,err=0;
   for(i=0;i<hm;i++)
    if(buf[i]!=out[i])
     err++;
   good=0;
  } 
 } 
 return j;
}
#else
int hahpack(u8*buf,u16 hm,u8*pbuf);
#endif

