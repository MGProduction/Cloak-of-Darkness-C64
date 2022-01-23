.include "shared.s"
.include "sharedvars.s"

ramone: .byte 0

_asm_bruteunpack:
.export _asm_bruteunpack

  ;lda $01
  ;sta ramone
  ;and #$FE
  ;sta $01

  jsr load_srcdst
  ldy #0
  ; tmp1=shortback
  lda (ptr1),y
  sta tmp1 ; 
  iny 
  ; tmp2=longback
  lda (ptr1),y
  sta tmp2  
  iny 
  ; tmp3=eof
  lda (ptr1),y
  sta tmp3 
  lda #3
  jsr adcptr1
  ldy #0
unpackloop:
  lda (ptr1),y  
  cmp tmp3
  beq unpackeof
  cmp tmp1
  beq shortunpack1  
  cmp tmp2
  beq shortunpack2  
  sta (ptr2),y
  jsr incy
  jmp unpackloop
shortunpack1:
  lda #1
  sta tmp4
  jmp shortunpack
shortunpack2:  
  lda #2
  sta tmp4
shortunpack:
  tya
  jsr adcptr2  ; inc ptr2 di y - così posso usare y = 0
  jsr incy
  tya
  jsr adcptr1  ; inc ptr1 di y - così posso usare y = 0  
  lda ptr2     ; ptr3 = ptr1
  sta ptr3
  lda ptr2+1
  sta ptr3+1
  lda tmp4
  cmp #2
  beq word  
  ldy #0
  lda (ptr1),y ; w (byte)  
  cmp #0
  bne byte
  jmp rle
byte:  
  jsr sbcptr3  ; ptr3 = ptr3 - w  
  jmp ready
word:  
  ldy #0
  lda (ptr1),y ; w (word)
  sta ptr4
  iny
  lda (ptr1),y ; w (word)
  sta ptr4+1
  jsr sbcptr3x  ; ptr3 = ptr3 - w  
ready:  
  iny
  lda (ptr1),y
  iny
  tax         ; x = l
  tya
  jsr adcptr1  ; inc ptr1 di y - così posso usare y = 0  
  ldy #0
backcopy:  
  lda (ptr3),y
  sta (ptr2),y
  iny 
  dex
  bne backcopy
  tya
  jsr adcptr2  ; inc ptr2 di y - così posso usare y = 0
  ldy #0
  jmp unpackloop
unpackeof:  

  ;lda ramone
  ;sta $01
  
  rts

rle:
  iny
  lda (ptr1),y ; l (byte)
  tax
  iny
  lda (ptr1),y ; c (byte)
  iny
  sta tmp4
  tya
  jsr adcptr1  ; inc ptr1 di y - così posso usare y = 0  
  lda tmp4
  ldy #0
backrle:  
  sta (ptr2),y
  iny 
  dex
  bne backrle
  tya
  jsr adcptr2  ; inc ptr2 di y - così posso usare y = 0
  ldy #0
  jmp unpackloop  
  
incy:  
  iny
  bne short
  inc ptr1+1
  inc ptr2+1
short:  
  rts

adcptr1:
        clc
        adc     ptr1
        sta     ptr1
        lda     #0
        adc     ptr1+1
        sta     ptr1+1
        rts

adcptr2:
        clc
        adc     ptr2
        sta     ptr2
        lda     #0
        adc     ptr2+1
        sta     ptr2+1
        rts
        
sbcptr3x:
        sec
		lda     ptr3
        sbc     ptr4
        sta     ptr3
        lda     ptr3+1
        sbc     ptr4+1
        sta     ptr3+1
        rts        

sbcptr3:
        sec
		sta     tmp4
		lda     ptr3
        sbc     tmp4
        sta     ptr3
        lda     ptr3+1
        sbc     #0
        sta     ptr3+1
        rts

incptr1:
        inc     ptr1
        bne     :+
        inc     ptr1+1
:       rts

incptr2:
        inc     ptr2
        bne     :+
        inc     ptr2+1
:       rts

; load in ptr1 / ptr2 _cmemcpy_src and _cmemcpy_dst
  load_srcdst:

  ldx _asm_src
  stx ptr1
  ldx _asm_src+1
  stx ptr1+1

  ldx _asm_dst
  stx ptr2
  ldx _asm_dst+1
  stx ptr2+1

  rts
  
