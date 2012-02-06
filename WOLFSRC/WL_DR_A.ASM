	IDEAL
	MODEL	MEDIUM,C
	P286

SCREENSEG	=	0a000h

FINEANGLES	=	3600
DEG90		=	900
DEG180		=	1800
DEG270		=	2700
DEG360		=	3600

OP_JLE		=	07eh
OP_JGE		=	07dh

EXTRN	finetangent:DWORD	; far array, starts at offset 0

EXTRN	HitHorizWall:FAR
EXTRN	HitVertWall:FAR
EXTRN	HitHorizDoor:FAR
EXTRN	HitVertDoor:FAR
EXTRN	HitHorizPWall:FAR
EXTRN	HitVertPWall:FAR


DATASEG

EXTRN	viewwidth:WORD

EXTRN	tilemap:BYTE
EXTRN	spotvis:BYTE
EXTRN	pixelangle:WORD


EXTRN	midangle:WORD
EXTRN	angle:WORD

EXTRN	focaltx:WORD
EXTRN	focalty:WORD
EXTRN	viewtx:WORD
EXTRN	viewty:WORD
EXTRN	viewx:DWORD
EXTRN	viewy:DWORD

EXTRN	xpartialup:WORD
EXTRN	ypartialup:WORD
EXTRN	xpartialdown:WORD
EXTRN	ypartialdown:WORD

EXTRN	tilehit:WORD
EXTRN	pixx:WORD
EXTRN	wallheight:WORD			; array of VIEWWIDTH entries

EXTRN	xtile:WORD
EXTRN	ytile:WORD
EXTRN	xtilestep:WORD
EXTRN	ytilestep:WORD
EXTRN	xintercept:DWORD
EXTRN	yintercept:DWORD
EXTRN	xstep:DWORD
EXTRN	ystep:DWORD

EXTRN	doorposition:WORD		; table of door position values


EXTRN	pwallpos:WORD			; amound a pushable wall has been moved

CODESEG

;-------------------
;
; xpartialbyystep
;
; multiplies long [ystep] (possibly negative), by word [xpartial] (in BX)
;
; returns dx:ax
; trashes bx,cx,di
;
;-------------------

PROC xpartialbyystep NEAR
;
; setup
;
	mov	ax,[WORD ystep]
	mov	cx,[WORD ystep+2]
	or	cx,cx               ; is ystep negatice?
	jns	@@multpos
;
; multiply negative cx:ax by bx
;
	neg	cx
	neg	ax
	sbb	cx,0

	mul	bx					; fraction*fraction
	mov	di,dx				; di is low word of result
	mov	ax,cx				;
	mul	bx					; units*fraction
	add	ax,di
	adc	dx,0

	neg	dx
	neg	ax
	sbb	dx,0
	ret
;
; multiply positive cx:ax by bx
;
EVEN
@@multpos:
	mul	bx					; fraction*fraction
	mov	di,dx				; di is low word of result
	mov	ax,cx				;
	mul	bx					; units*fraction
	add	ax,di
	adc	dx,0

	ret

ENDP



;-------------------
;
; ypartialbyxstep
;
; multiplies long [xstep] (possibly negative), by word [ypartial] (in BP)
;
; returns dx:ax
; trashes cx,di,bp
;
;-------------------

PROC ypartialbyxstep NEAR
;
; setup
;
	mov	ax,[WORD xstep]
	mov	cx,[WORD xstep+2]
	or	cx,cx               ; is ystep negatice?
	jns	@@multpos
;
; multiply negative cx:ax by bx
;
	neg	cx
	neg	ax
	sbb	cx,0

	mul	bp					; fraction*fraction
	mov	di,dx				; di is low word of result
	mov	ax,cx				;
	mul	bp					; units*fraction
	add	ax,di
	adc	dx,0

	neg	dx
	neg	ax
	sbb	dx,0
	ret
;
; multiply positive cx:ax by bx
;
EVEN
@@multpos:
	mul	bp					; fraction*fraction
	mov	di,dx				; di is low word of result
	mov	ax,cx				;
	mul	bp					; units*fraction
	add	ax,di
	adc	dx,0
	ret

ENDP


;============================
;
; AsmRefresh
;
;
;============================

PROC	AsmRefresh
PUBLIC	AsmRefresh

	push	si
	push	di
	push	bp

	mov	[pixx],0
;---------------------------------------------------------------------------
;
; Setup to trace a ray through pixx view pixel
;
; CX : angle of the ray through pixx
; ES : points to segment of finetangent array for this block of code
;
; Upon entrance to initialize block
;
; BX : xpartial
; BP : ypartial
;
;---------------------------------------------------------------------------
	EVEN
pixxloop:
	mov	ax,SEG finetangent
	mov	es,ax
	mov	cx,[midangle]			; center of view area
	mov	bx,[pixx]
	shl	bx,1
	add	cx,[pixelangle+bx]		; delta for this pixel
	cmp	cx,0
	jge	not0
;----------
;
; -90 - -1 degree arc
;
;----------
	add	cx,FINEANGLES			; -90 is the same as 270
	jmp	entry360

not0:
	cmp	cx,DEG90
	jge	not90
;----------
;
; 0-89 degree arc
;
;----------
entry90:
	mov	[xtilestep],1			; xtilestep = 1
	mov	[ytilestep],-1			; ytilestep = -1
	mov	[BYTE cs:horizop],OP_JGE	; patch a jge in
	mov	[BYTE cs:vertop],OP_JLE		; patch a jle in
	mov	bx,DEG90-1
	sub	bx,cx
	shl	bx,2
	mov	ax,[es:bx]
	mov	dx,[es:bx+2]
	mov	[WORD xstep],ax
	mov	[WORD xstep+2],dx		; xstep = finetangent[DEG90-1-angle]
	mov	bx,cx
	shl	bx,2
	mov	ax,[es:bx]
	mov	dx,[es:bx+2]
	neg	dx
	neg	ax
	sbb	dx,0
	mov	[WORD ystep],ax
	mov	[WORD ystep+2],dx		; ystep = -finetangent[angle]

	mov	bx,[xpartialup]			; xpartial = xpartialup
	mov	bp,[ypartialdown]		; ypartial = ypartialdown
	jmp	initvars

not90:
	cmp	cx,DEG180
	jge	not180
;----------
;
; 90-179 degree arc
;
;----------
	mov	ax,-1
	mov	[xtilestep],ax			; xtilestep = -1
	mov	[ytilestep],ax			; ytilestep = -1
	mov	[BYTE cs:horizop],OP_JLE	; patch a jle in
	mov	[BYTE cs:vertop],OP_JLE		; patch a jle in

	mov	bx,cx
	shl	bx,2
	mov	ax,[es:bx-DEG90*4]
	mov	dx,[es:bx+2-DEG90*4]
	neg	dx
	neg	ax
	sbb	dx,0
	mov	[WORD xstep],ax
	mov	[WORD xstep+2],dx		; xstep = -finetangent[angle-DEG90]
	mov	bx,DEG180-1
	sub	bx,cx
	shl	bx,2
	mov	ax,[es:bx]
	mov	dx,[es:bx+2]
	neg	dx
	neg	ax
	sbb	dx,0
	mov	[WORD ystep],ax
	mov	[WORD ystep+2],dx		; ystep = -finetangent[DEG180-1-angle]

	mov	bx,[xpartialdown]		; xpartial = xpartialdown
	mov	bp,[ypartialdown]		; ypartial = ypartialdown
	jmp	initvars

not180:
	cmp	cx,DEG270
	jge	not270
;----------
;
; 180-269 degree arc
;
;----------
	mov	[xtilestep],-1			; xtilestep = -1
	mov	[ytilestep],1			; ytilestep = 1
	mov	[BYTE cs:horizop],OP_JLE	; patch a jle in
	mov	[BYTE cs:vertop],OP_JGE		; patch a jge in

	mov	bx,DEG270-1
	sub	bx,cx
	shl	bx,2
	mov	ax,[es:bx]
	mov	dx,[es:bx+2]
	neg	dx
	neg	ax
	sbb	dx,0
	mov	[WORD xstep],ax
	mov	[WORD xstep+2],dx		; xstep = -finetangent[DEG270-1-angle]
	mov	bx,cx
	shl	bx,2
	mov	ax,[es:bx-DEG180*4]
	mov	dx,[es:bx+2-DEG180*4]
	mov	[WORD ystep],ax
	mov	[WORD ystep+2],dx		; ystep = finetangent[angle-DEG180]

	mov	bx,[xpartialdown]		; xpartial = xpartialdown
	mov	bp,[ypartialup]			; ypartial = ypartialup
	jmp	initvars


not270:
	cmp	cx,DEG360
	jge	not360
;----------
;
; 270-359 degree arc
;
;----------
entry360:
	mov	ax,1
	mov	[xtilestep],ax			; xtilestep = 1
	mov	[ytilestep],ax			; ytilestep = 1
	mov	[BYTE cs:horizop],OP_JGE	; patch a jge in
	mov	[BYTE cs:vertop],OP_JGE		; patch a jge in

	mov	bx,cx
	shl	bx,2
	mov	ax,[es:bx-DEG270*4]
	mov	dx,[es:bx+2-DEG270*4]
	mov	[WORD xstep],ax
	mov	[WORD xstep+2],dx		; xstep = finetangent[angle-DEG270]
	mov	bx,DEG360-1
	sub	bx,cx
	shl	bx,2
	mov	ax,[es:bx]
	mov	dx,[es:bx+2]
	mov	[WORD ystep],ax
	mov	[WORD ystep+2],dx		; ystep = finetangent[DEG360-1-angle]

	mov	bx,[xpartialup]			; xpartial = xpartialup
	mov	bp,[ypartialup]			; ypartial = ypartialup
	jmp	initvars


not360:
;----------
;
; 360-449 degree arc
;
;----------
	sub	cx,FINEANGLES			; -449 is the same as 89
	jmp	entry90

;---------------------------------------------------------------------------
;
; initialise variables for intersection testing
;
;---------------------------------------------------------------------------
initvars:
	call	NEAR xpartialbyystep	; xpartial is in BX
	add	ax,[WORD viewy]
	adc	dx,[WORD viewy+2]
	mov	[WORD yintercept],ax
	mov	[WORD yintercept+2],dx

	mov	si,[focaltx]
	add	si,[xtilestep]
	mov	[xtile],si					; xtile = focaltx+xtilestep
	shl	si,6
	add	si,dx						; xspot = (xtile<<6) + yinttile


	call	NEAR ypartialbyxstep	; ypartial is in BP
	add	ax,[WORD viewx]
	adc	dx,[WORD viewx+2]
	mov	[WORD xintercept],ax
	mov	cx,dx

	mov	bx,[focalty]
	add	bx,[ytilestep]
	mov	bp,bx						; ytile = focalty+ytilestep
	mov	di,dx
	shl	di,6
	add	di,bx						; yspot = (xinttile<<6) + ytile

	mov	bx,[xtile]
	mov	dx,[WORD yintercept+2]
	mov	ax,SCREENSEG
	mov	es,ax						; faster than mov es,[screenseg]


;---------------------------------------------------------------------------
;
; trace along this angle until we hit a wall
;
; CORE LOOP!
;
; All variables are killed when a wall is hit
;
; AX : scratch
; BX : xtile
; CX : high word of xintercept
; DX : high word of yintercept
; SI : xspot (yinttile<<6)+xtile (index into tilemap and spotvis)
; DI : yspot (xinttile<<6)+ytile (index into tilemap and spotvis)
; BP : ytile
; ES : screenseg
;
;---------------------------------------------------------------------------

;-----------
;
; check intersections with vertical walls
;
;-----------

	EVEN
vertcheck:
	cmp	dx,bp
vertop:								; 0x7e = jle (ytilestep==-1)
	jle	horizentry					; 0x7d = jge (ytilestep==1)
vertentry:
	test [BYTE tilemap+si],0ffh		; tilehit = *((byte *)tilemap+xspot);
	jnz	hitvert
passvert:
	mov	[BYTE spotvis+si],1			; *((byte *)spotvis+xspot) = true;
	add	bx,[xtilestep]				; xtile+=xtilestep
	mov	ax,[WORD ystep]
	add	[WORD yintercept],ax		; yintercept += ystep
	adc	dx,[WORD ystep+2]
	mov	si,bx
	shl	si,6
	add	si,dx						; xspot = (xtile<<6)+yinttile
	jmp	vertcheck

	EVEN
hitvert:
	mov	al,[BYTE tilemap+si]		; tilehit = *((byte *)tilemap+xspot);
	mov	[BYTE tilehit],al
	or	al,al						; set flags
	jns	notvertdoor
	jmp	vertdoor
notvertdoor:
	mov	[WORD xintercept],0
	mov	[WORD xintercept+2],bx
	mov	[xtile],bx
	mov	[WORD yintercept+2],dx
	mov	[ytile],dx
	call FAR HitVertWall
	jmp nextpix


;-----------
;
; check intersections with horizontal walls
;
;-----------
	EVEN
horizcheck:
	cmp	cx,bx
horizop:							; 0x7e = jle (xtilestep==-1)
	jle	vertentry					; 0x7d = jge (xtilestep==1)
horizentry:
	test [BYTE tilemap+di],0ffh		; tilehit = *((byte *)tilemap+yspot);
	jnz	hithoriz
passhoriz:
	mov	[BYTE spotvis+di],1			; *((byte *)spotvis+yspot) = true;
	add	bp,[ytilestep]				; ytile+=ytilestep
	mov	ax,[WORD xstep]
	add	[WORD xintercept],ax		; xintercept += xstep
	adc	cx,[WORD xstep+2]
	mov	di,cx
	shl	di,6
	add	di,bp						; yspot = (xinttile<<6)+ytile
	jmp	horizcheck

	EVEN
hithoriz:
	mov	al,[BYTE tilemap+di]		; tilehit = *((byte *)tilemap+yspot);
	mov	[BYTE tilehit],al
	or	al,al						; set flags
	js	horizdoor
	mov	[WORD xintercept+2],cx
	mov	[xtile],cx
	mov	[WORD yintercept],0
	mov	[WORD yintercept+2],bp
	mov	[ytile],bp
	call FAR HitHorizWall
	jmp nextpix

;---------------------------------------------------------------------------
;
; next pixel over
;
;---------------------------------------------------------------------------

nextpix:
	mov	ax,[pixx]
	inc	ax
	mov	[pixx],ax
	cmp	ax,[viewwidth]
	jge	done
	jmp	pixxloop
done:
	pop	bp
	pop	di
	pop	si
	retf

;===========================================================================

;=============
;
; hit a special horizontal wall, so find which coordinate a door would be
; intersected at, and check to see if the door is open past that point
;
;=============
horizdoor:
	mov	[xtile],bx					; save off live register variables
	mov	[WORD yintercept+2],dx

	test al,040h      				; both high bits set == pushable wall
	jnz	horizpushwall

	mov	bx,ax
	and	bx,7fh						; strip high bit
	shl	bx,1                        ; index into word width door table

	mov	ax,[WORD xstep]
	mov	dx,[WORD xstep+2]
	sar	dx,1
	rcr ax,1						; half a step gets to door position

	add	ax,[WORD xintercept]		; add half step to current intercept pos
	adc	dx,cx						; CX hold high word of xintercept

	cmp	cx,dx						; is it still in the same tile?
	je	hithmid
;
; midpoint is outside tile, so it hit the side of the wall before a door
;
continuehoriz:
	mov	bx,[xtile]					; reload register variables
	mov	dx,[WORD yintercept+2]
	jmp	passhoriz					; continue tracing
;
; the trace hit the door plane at pixel position AX, see if the door is
; closed that much
;
hithmid:
	cmp	ax,[doorposition+bx]		; position of leading edge of door
	jb	continuehoriz
;
; draw the door
;
	mov	[WORD xintercept],ax		; save pixel intercept position
	mov	[WORD xintercept+2],cx

	mov	[WORD yintercept],8000h		; intercept in middle of tile
	mov	[WORD yintercept+2],bp

	call	FAR HitHorizDoor
	jmp	nextpix

;============
;
; hit a sliding horizontal wall
;
;============

horizpushwall:
	mov	ax,[WORD xstep+2]			; multiply xstep by pwallmove (0-63)
	mul	[pwallpos]
	mov	bx,ax
	mov	ax,[WORD xstep]
	mul	[pwallpos]
	add	dx,bx

	sar	dx,1						; then divide by 64 to accomplish a
	rcr ax,1						; fixed point multiplication
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1

	add	ax,[WORD xintercept]		; add partial step to current intercept
	adc	dx,cx						; CX hold high word of xintercept

	cmp	cx,dx						; is it still in the same tile?
	jne	continuehoriz				; no, it hit the side

;
; draw the pushable wall at the new height
;
	mov	[WORD xintercept],ax		; save pixel intercept position
	mov	[WORD xintercept+2],dx

	mov	[WORD yintercept+2],bp
	mov	[WORD yintercept],0

	call	FAR HitHorizPWall
	jmp	nextpix



;===========================================================================

;=============
;
; hit a special vertical wall, so find which coordinate a door would be
; intersected at, and check to see if the door is open past that point
;
;=============
vertdoor:
	mov	[xtile],bx					; save off live register variables
	mov	[WORD yintercept+2],dx

	test al,040h      				; both high bits set == pushable wall
	jnz	vertpushwall

	mov	bx,ax
	and	bx,7fh						; strip high bit
	shl	bx,1                        ; index into word width doorposition

	mov	ax,[WORD ystep]
	mov	dx,[WORD ystep+2]
	sar	dx,1
	rcr ax,1						; half a step gets to door position

	add	ax,[WORD yintercept]		; add half step to current intercept pos
	adc	dx,[WORD yintercept+2]

	cmp	[WORD yintercept+2],dx		; is it still in the same tile?
	je	hitvmid
;
; midpoint is outside tile, so it hit the side of the wall before a door
;
continuevert:
	mov	bx,[xtile]					; reload register variables
	mov	dx,[WORD yintercept+2]
	jmp	passvert					; continue tracing
;
; the trace hit the door plane at pixel position AX, see if the door is
; closed that much
;
hitvmid:
	cmp	ax,[doorposition+bx]		; position of leading edge of door
	jb	continuevert
;
; draw the door
;
	mov	[WORD yintercept],ax		; save pixel intercept position
	mov	[WORD xintercept],8000h		; intercept in middle of tile
	mov	ax,[xtile]
	mov	[WORD xintercept+2],ax

	call	FAR HitVertDoor
	jmp	nextpix

;============
;
; hit a sliding vertical wall
;
;============

vertpushwall:
	mov	ax,[WORD ystep+2]			; multiply ystep by pwallmove (0-63)
	mul	[pwallpos]
	mov	bx,ax
	mov	ax,[WORD ystep]
	mul	[pwallpos]
	add	dx,bx

	sar	dx,1						; then divide by 64 to accomplish a
	rcr ax,1						; fixed point multiplication
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1
	sar	dx,1
	rcr ax,1

	add	ax,[WORD yintercept]		; add partial step to current intercept
	adc	dx,[WORD yintercept+2]

	cmp	[WORD yintercept+2],dx		; is it still in the same tile?
	jne	continuevert				; no, it hit the side

;
; draw the pushable wall at the new height
;
	mov	[WORD yintercept],ax		; save pixel intercept position
	mov	[WORD yintercept+2],dx

	mov	bx,[xtile]
	mov	[WORD xintercept+2],bx
	mov	[WORD xintercept],0

	call	FAR HitVertPWall
	jmp	nextpix



ENDP


END


