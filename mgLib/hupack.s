.include "shared.s"
.include "sharedvars.s"

MASK_FWD    = 64
MASK_BCK    = 128
MASK_PLAIN  = 192

.export _asm_hunpack

_asm_hunpack:

  ldx _asm_src
  stx ptr1
  ldx _asm_src+1
  stx ptr1+1

  ldx _asm_dst
  stx ptr2
  ldx _asm_dst+1
  stx ptr2+1
  
  ;sei
  ;lda $01
  ;sta tmp4
  ;and #$fc
  ;sta $01
  
mainloop:  
  
  ldy #0
  
  lda (ptr1),y
  iny  
  ; sta $D020  
  
  cmp #0
  bne read
  
  jmp hunpack_leave
  
read:

  sta tmp1  ; what
  and #$3f
  sta tmp2  ; len
  cmp #$3f
  bne entercheck  
  lda (ptr1),y
  iny
  sta tmp2  ; len
  
entercheck:  

  sty tmp3 ; tmp
  jsr updateptr1

  lda tmp1    ; what
  and #192 ; MASK_PLAIN
  sta tmp1    ; what
  
  cmp #192 ; MASK_PLAIN
  bne checkbck
  
; -- PLAIN  

  ldy tmp2 ; len
  sty tmp3 ; tmp
  
plainloop:
  
  dey
  
  lda (ptr1),y
  sta (ptr2),y
  
  cpy #0
  bne plainloop
    
  jsr updateptr1
  jsr updateptr2
  
  jmp mainloop
  
; -- PLAIN   
 
checkbck:  

  cmp #128 ; MASK_BCK
  bne checkfwd
  
 ; -- BACK
 
   ldy #0
   lda (ptr1),y
   sta tmp1   ; what
   iny
   sty tmp3 ; tmp
   jsr updateptr1
   
   lda ptr2
   sta ptr3
   lda ptr2+1
   sta ptr3+1

   sec
   lda ptr3
   sbc tmp1   ; what
   sta ptr3
   lda ptr3+1
   sbc #0
   sta ptr3+1
   
   ldy tmp2 ; len
   sty tmp3 ; tmp
  
bckloop:
  
   dey
  
   lda (ptr3),y
   sta (ptr2),y
  
   cpy #0
   bne bckloop
   
   jsr updateptr2
 
   jmp mainloop
 
 ; -- BACK
  
checkfwd:

; -- FWD

   ldy #0
   lda (ptr1),y
   iny
   sta tmp1   ; what   
   sty tmp3 ; tmp
   jsr updateptr1

   ldy tmp2 ; len
   sty tmp3 ; tmp
   
   lda tmp1  ; what
  
fwdloop:
   dey  
   sta (ptr2),y  
   cpy #0
   bne fwdloop   
   
   jsr updateptr2

   jmp mainloop

; -- FWD  
  
hunpack_leave:  

  ;lda tmp4
  ;sta $01
  ;cli
  
  rts
  
  
updateptr1: 
 clc
 lda ptr1
 adc tmp3 ; tmp
 sta ptr1
 lda ptr1+1
 adc #0
 sta ptr1+1
 rts  
 
updateptr2: 
 clc
 lda ptr2
 adc tmp3 ; tmp
 sta ptr2
 lda ptr2+1
 adc #0
 sta ptr2+1
 rts
 


