#ifndef _unpack_h__
#define _unpack_h__

#if defined(WIN32)
#define BRUTE_C_VERSION
#endif

#if defined(BRUTE_C_VERSION)
u16 unpack(const u8*src,u8*dst);
#else
void unpack(const u8*src,u8*dst);
#endif

#endif