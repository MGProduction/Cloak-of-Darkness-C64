.include "shared.s"
.include "sharedvars.s"


_asm_screenfill:
.export _asm_screenfill

  lda16 _asm_dst,  ptr1
  lda16 _asm_cdst, ptr2    

  lda _asm_chattr
  ldx #4
loopA0:
  dex    
  ldy #0
  cpx #0
  bne loopA1
  ldy #235
loopA1:  
  dey
  sta (ptr1),y
  sta (ptr2),y  
  cpy #0
  bne loopA1
  inc ptr1+1
  inc ptr2+1
  cpx #0
  bne loopA0
  rts



;---------------------------------------------------------------------------------------
; vic_wait_offscreen - waits for the raster beam to leave the visible screen (line 300).
;
; parameters: none.
; returns: none.
;---------------------------------------------------------------------------------------

_vic_wait_offscreen:
.export _vic_wait_offscreen

vwos0: lda $d011 ; wait for raster msb clear..
      and #$80
      bne vwos0
vwos1: lda $d011 ; wait for raster msb set (line 256)..
      and #$80
      beq vwos1
vwos2: lda $d012 ; wait for raster line 300 (256 + 44)...
      cmp #44
      bne vwos2
      rts

