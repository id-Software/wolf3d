; WOLFHACK.ASM

.386C
IDEAL
MODEL	MEDIUM,C


;============================================================================

DATASEG

EXTRN	mr_rowofs:WORD
EXTRN	mr_count:WORD
EXTRN	mr_xstep:WORD
EXTRN	mr_ystep:WORD
EXTRN	mr_xfrac:WORD
EXTRN	mr_yfrac:WORD
EXTRN	mr_dest:WORD

FARDATA

planepics	db	8192 dup(?)	;	// 4k of ceiling, 4k of floor
PUBLIC	planepics


;============================================================================

CODESEG

;============================
;
; MapRow
;
;
;============================

PROC	MapRow
PUBLIC	MapRow
	push	esi
	push	edi
	push	ebp
	push 	ds

	mov     bp,[mr_rowofs]
	mov		cx,[mr_count]
	mov		dx,[mr_ystep]
	shl		edx,16
	mov		dx,[mr_xstep]
	mov		si,[mr_yfrac]
	shl		esi,16
	mov		si,[mr_xfrac]
	mov		di,[mr_dest]
	mov		ax,SEG planepics
	mov		ds,ax
	mov		ax,0a000h
	mov		es,ax
	mov		ax,1111111111110b

; eax		color lookup
; ebx		scratch offset and pixel values
; ecx     	loop counter
; edx		packed x / y step values
; esi		packed x / y fractional values
; edi		write pointer
; ebp		toprow to bottomrow delta
; es:		screenseg
; ds:		pictures

; mov	al,[esi]
; mov	al,[eax]
; mov	[edi],al

; mov   ax,[_variable+ebx+2]


pixelloop:
	shld	ebx,esi,22				; shift y units in
	shld	ebx,esi,7				; shift x units in and one extra bit
	and		bx,63*65*2				; mask off extra top bits and 0 low bit
	add		esi,edx					; position += step
	mov		al,[bx]
	mov		al,[eax]
	mov     [es:di],al     			; write ceiling pixel
	mov		al,[bx+1]
	mov		al,[eax]
	mov     [es:di+bp],al     		; write floor pixel
	inc		di
	loop	pixelloop

	pop		ds
	pop		ebp
	pop		edi
	pop		esi

	retf

ENDP

END

