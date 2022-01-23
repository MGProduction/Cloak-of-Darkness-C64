; .importzp	tmp1, tmp2, tmp3, tmp4
; .importzp	ptr1, ptr2, ptr3, ptr4
ptr1    := $0A
ptr2    := $0C
ptr3    := $0E
ptr4    := $10

tmp1    := $12
tmp2    := $13
tmp3    := $14
tmp4    := $15

; ---------------

.macro  lda16   addr,ptr
                lda addr
                sta ptr
                lda addr+1
                sta ptr+1
.endmacro

.macro  inc16   addr
                inc     addr
                bne     :+
                inc     addr+1
:                
.endmacro

.macro  dec16   addr
                dec     addr
                bne     :+
                dec     addr+1
:                
.endmacro

.macro  adc16   addr1
                clc
                adc     addr1
                sta     addr1
                lda     addr1+1
                adc     #$00
                sta     addr1+1
.endmacro

.macro  sbc16   addr1
                sec
                sbc     addr1
                sta     addr1
                lda     addr1+1
                sbc     #$00
                sta     addr1+1
.endmacro

.macro  sum16   addr1,addr2
                clc
                lda     addr1
                adc     addr2
                sta     addr1
                lda     addr1+1
                adc     addr2+1
                sta     addr1+1
.endmacro

.macro  sub16   addr1,addr2
                sec
                lda     addr1
                sbc     addr2
                sta     addr1
                lda     addr1+1
                sbc     addr2+1
                sta     addr1+1
.endmacro

.macro  mul8x8  num1,num2
            .local doAdd
            .local loop
            .local enterLoop
            lda #$00
            tay
            sty tmp4  ; remove this line for 16*8=16bit multiply
            beq enterLoop
doAdd:
            clc
            adc num1
            tax
            tya
            adc tmp4
            tay
            txa
loop:
            asl num1
            rol tmp4
enterLoop:  ; accumulating multiply entry point (enter with .A=lo, .Y=hi)
            lsr num2
            bcs doAdd
            bne loop
 .endmacro
; ---------------