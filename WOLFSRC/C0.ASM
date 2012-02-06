        NAME    c0
        PAGE    60,132
        LOCALS
;[]------------------------------------------------------------[]
;|      C0.ASM -- Start Up Code                                 |
;|                                                              |
;|      Turbo C++ Run Time Library                              |
;|                                                              |
;|      Copyright (c) 1987, 1991 by Borland International Inc.  |
;|      All Rights Reserved.                                    |
;[]------------------------------------------------------------[]

                __C0__ = 1
INCLUDE         RULES.ASI

;       Segment and Group declarations

_TEXT           SEGMENT BYTE PUBLIC 'CODE'
                ENDS
_FARDATA        SEGMENT PARA PUBLIC 'FAR_DATA'
                ENDS
_FARBSS         SEGMENT PARA PUBLIC 'FAR_BSS'
                ENDS
IFNDEF __TINY__
_OVERLAY_       SEGMENT PARA PUBLIC 'OVRINFO'
        ENDS
_1STUB_     SEGMENT PARA PUBLIC 'STUBSEG'
        ENDS
ENDIF
_DATA           SEGMENT PARA PUBLIC 'DATA'
                ENDS
_INIT_          SEGMENT WORD PUBLIC 'INITDATA'
InitStart       label byte
                ENDS
_INITEND_       SEGMENT BYTE PUBLIC 'INITDATA'
InitEnd         label byte
                ENDS
_EXIT_          SEGMENT WORD PUBLIC 'EXITDATA'
ExitStart       label byte
                ENDS
_EXITEND_       SEGMENT BYTE PUBLIC 'EXITDATA'
ExitEnd         label byte
                ENDS
_CVTSEG         SEGMENT WORD PUBLIC 'DATA'
                ENDS
_SCNSEG         SEGMENT WORD PUBLIC 'DATA'
                ENDS
IFNDEF __HUGE__
  _BSS          SEGMENT WORD PUBLIC 'BSS'
                ENDS
  _BSSEND       SEGMENT BYTE PUBLIC 'BSSEND'
                ENDS
ENDIF
IFNDEF __TINY__
  _STACK        SEGMENT STACK 'STACK'
                ENDS
ENDIF

        ASSUME  CS:_TEXT, DS:DGROUP

;       External References

extrn       _main:DIST
extrn       _exit:DIST
extrn       __exit:DIST
extrn       __nfile:word
extrn       __setupio:near          ;required!
extrn       __stklen:word
IF LDATA EQ false
extrn       __heaplen:word
ENDIF

        SUBTTL  Start Up Code
        PAGE
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
;/*     Start Up Code                                   */
;/*     -------------                                   */
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
PSPHigh         equ     00002h
PSPEnv          equ     0002ch
PSPCmd          equ     00080h

                public  __AHINCR
__AHINCR        equ     1000h
                public  __AHSHIFT
__AHSHIFT       equ     12

IFDEF   __NOFLOAT__
MINSTACK        equ     128     ; minimal stack size in words
ELSE
MINSTACK        equ     256     ; minimal stack size in words
ENDIF
;
;       At the start, DS and ES both point to the segment prefix.
;       SS points to the stack segment except in TINY model where
;       SS is equal to CS
;
_TEXT           SEGMENT
IFDEF           __TINY__
                ORG     100h
ENDIF
STARTX          PROC    NEAR
;       Save general information, such as :
;               DGROUP segment address
;               DOS version number
;               Program Segment Prefix address
;               Environment address
;               Top of far heap

IFDEF   __TINY__
				mov     dx, cs          ; DX = GROUP Segment address
ELSE
				mov     dx, DGROUP      ; DX = GROUP Segment address
ENDIF
IFNDEF	__BOSS__
				mov     cs:DGROUP@@, dx ;  __BOSS__
ENDIF
				mov     ah, 30h
				int     21h             ; get DOS version number
				mov     bp, ds:[PSPHigh]; BP = Highest Memory Segment Addr
				mov     bx, ds:[PSPEnv] ; BX = Environment Segment address
				mov     ds, dx
				mov     _version@, ax   ; Keep major and minor version number
				mov     _psp@, es       ; Keep Program Segment Prefix address
				mov     _envseg@, bx    ; Keep Environment Segment address
				mov     word ptr _heaptop@ + 2, bp
;
;       Save several vectors and install default divide by zero handler.
;
				call    SaveVectors

;===================
;
; 		IDsoft - Check to make sure that we're running on a 286 or better

				pushf					; Save original flags
				xor		ax,ax			; Clear AX
				push	ax
				popf					; Try to pop the 0
				pushf
				pop		ax				; Get results of popping 0 into flags
				popf					; Restore original flags
				or		ax,ax
				jns		@@Have286		; If no sign bit, have a 286

				mov     cx, lgth_no286MSG
				mov     dx, offset DGROUP: no286MSG
				jmp		MsgExit3

@@Have286:
;		IDsoft - End of modifications (there's also a code segment string)
;
;===================

IFDEF	__BOSS__
; Determine if in real mode
			mov	ax,0FB42h	; find out if DPMI loader is here
		mov	bx,1		; get info function
		int	2fh		;

		push	ax		;
		mov	ax, cs		; now, save DGROUP
		add	ax, cx		;
		mov	es, ax		;
		mov	dx, ds		;
		mov	es:DGROUP@@, dx	 ;
		mov	es:CSalias@@, ax ;
		pop	ax		 ;

;		cmp	ax,0001h	; if not "TRUE"
;		JNE	InRealMode

; 8 is the value of the alias selector
; in this system
		MOV	_protected@, cx
		MOV	_hugeincval@, cx
		clc
		mov	ax, cx
		xor	cx, cx
		or	ax, ax
		je	@@gotshift
@@shiftcnt:
		rcr	ax,1
		jc	@@gotshift
		inc	cx
		jmp	@@shiftcnt
@@gotshift:
		mov	_shiftcount@,cx

; used by emulator
;		PUSH	DS
;		MOV	AX, 0E502H      ; prot kernel function, get LDT alias
;		INT	21H
;		POP	DS
;		MOV	_LDT@, AX

;		cmp	_protected@,0001h	; if not "TRUE"
;		JNE	InRealMode

		.286P
IFE	LDATA					
		mov	dx, ds			;
;		LSL	AX, DX			;
;		DEC	AX			;
		MOV	AX, 0FFFEh		;
		MOV	SP, AX			;
		MOV	SS, DX			;
ENDIF						
		.8086
;		JMP	BossSkip

InRealMode	label	near

ENDIF

;       Count the number of environment variables and compute the size.
;       Each variable is ended by a 0 and a zero-length variable stops
;       the environment. The environment can NOT be greater than 32k.

                les     di, dword ptr _envLng@
                mov     ax, di
                mov     bx, ax
                mov     cx, 07FFFh      ; Environment cannot be > 32 Kbytes
                cld
@@EnvLoop:
                repnz   scasb
                jcxz    InitFailed      ; Bad environment !!!
IFDEF __BOSS__
                jmp     InitOK
InitFailed:     jmp     near ptr _abort
InitOK:
ENDIF
     
                inc     bx              ; BX = Nb environment variables
                cmp     es:[di], al
                jne     @@EnvLoop       ; Next variable ...
                or      ch, 10000000b
                neg     cx
                mov     _envLng@, cx    ; Save Environment size
                mov     cx, dPtrSize / 2
                shl     bx, cl
                add     bx, dPtrSize * 4
                and     bx, not ((dPtrSize * 4) - 1)
                mov     _envSize@, bx   ; Save Environment Variables Nb.

IFNDEF __BOSS__

;       Determine the amount of memory that we need to keep

IFDEF _DSSTACK_
                mov     dx, ds
ELSE
                mov     dx, ss
ENDIF
                sub     bp, dx          ; BP = remaining size in paragraphs
IF LDATA
                mov     di, seg __stklen
                mov     es, di
                mov     di, es:__stklen ; DI = Requested stack size
ELSE
                mov     di, __stklen    ; DI = Requested stack size
ENDIF
;
; Make sure that the requested stack size is at least MINSTACK words.
;
                cmp     di, 2*MINSTACK  ; requested stack big enough ?
                jae     AskedStackOK
                mov     di, 2*MINSTACK  ; no --> use minimal value
IF LDATA
                mov     es:__stklen, di ; override requested stack size
ELSE
                mov        __stklen, di ; override requested stack size
ENDIF

AskedStackOK    label   near
IFDEF _DSSTACK_
                add     di, offset DGROUP: edata@
                jb      InitFailed      ; DATA segment can NOT be > 64 Kbytes
ENDIF
IF LDATA EQ false
                add     di, __heaplen
                jb      InitFailed      ; DATA segment can NOT be > 64 Kbytes
ENDIF
                mov     cl, 4
                shr     di, cl          ; $$$ Do not destroy CL $$$
                inc     di              ; DI = DS size in paragraphs
                cmp     bp, di
IF LDATA EQ false
                jb      InitFailed      ; Not enough memory
                cmp     __stklen, 0
                je      ExpandDS        ; Expand DS up to 64 Kb
                cmp     __heaplen, 0
                jne     ExcessOfMemory  ; Much more available than needed
ExpandDS        label   near
                mov     di, 1000h
                cmp     bp, di
                ja      ExcessOfMemory  ; Enough to run the program
                mov     di, bp
                jmp     short ExcessOfMemory  ; Enough to run the program
ELSE
                jnb     ExcessOfMemory  ; Much more available than needed
ENDIF

;       All initialization errors arrive here

InitFailed      label   near
                jmp     near ptr _abort

;       Return to DOS the amount of memory in excess
;       Set far heap base and pointer

ExcessOfMemory  label   near
                mov     bx, di
                add     bx, dx
                mov     word ptr _heapbase@ + 2, bx
                mov     word ptr _brklvl@ + 2, bx
                mov     ax, _psp@
                sub     bx, ax          ; BX = Number of paragraphs to keep
                mov     es, ax          ; ES = Program Segment Prefix address
                mov     ah, 04Ah
                push    di              ; preserve DI
                int     021h            ; this call clobbers SI,DI,BP !!!!!!
                pop     di              ; restore  DI

                shl     di, cl          ; $$$ CX is still equal to 4 $$$

                cli                     ; req'd for pre-1983 88/86s
                mov     ss, dx          ; Set the program stack
                mov     sp, di
                sti

IFNDEF _DSSTACK_
                mov     ax, seg __stklen
                mov     es, ax
                mov     es:__stklen, di ; If separate stack segment, save size
ENDIF

ENDIF ; __BOSS__

IFNDEF  __HUGE__

;       Reset uninitialized data area

                xor     ax, ax
                mov     es, cs:DGROUP@@
                mov     di, offset DGROUP: bdata@
                mov     cx, offset DGROUP: edata@
                sub     cx, di
                cld
                rep     stosb
ENDIF

;   If default number of file handles have changed then tell DOS
                cmp     __nfile, 20
                jbe     @@NoChange

                cmp     _osmajor@, 3   ; Check for >= DOS 3.3
                jb      @@NoChange
                ja      @@DoChange
                cmp     _osminor@, 1Eh
                jb      @@NoChange
@@DoChange:
                mov     ax, 5801h      ; Set last fit allocation
                mov     bx, 2
                int     21h
                jc      @@BadInit

                mov     ah, 67h        ; Expand handle table
                mov     bx, __nfile
                int     21h
                jc      @@BadInit

                mov     ah, 48h        ; Allocate 16 bytes to find new
                mov     bx, 1          ;   top of memory address
                int     21h
                jc      @@BadInit
                inc     ax             ; Adjust address to point after block
                mov     word ptr _heaptop@ + 2, ax

                dec     ax             ; Change back and release block
                mov     es, ax
                mov     ah, 49h
                int     21h
                jc      @@BadInit

                mov     ax, 5801h      ; Set first fit allocation
                mov     bx, 0
                int     21h
                jnc     @@NoChange

@@BadInit:      jmp near ptr _abort

@@NoChange:

;       Prepare main arguments

                mov     ah, 0
                int     1ah                     ; get current BIOS time in ticks
                mov     word ptr _StartTime@,dx ; save it for clock() fn
                mov     word ptr _StartTime@+2,cx
		or	al,al			; was midnight flag set?
		jz	@@NotMidnight
		mov	ax,40h			; set BIOS midnight flag
		mov	es,ax			;  at 40:70
		mov	bx,70h
		mov	byte ptr es:[bx],1
			
@@NotMidnight:
                xor     bp,bp                   ; set BP to 0 for overlay mgr

                mov     es, cs:DGROUP@@
                mov     si,offset DGROUP:InitStart      ;si = start of table
                mov     di,offset DGROUP:InitEnd        ;di = end of table
                call    StartExit

;       ExitCode = main(argc,argv,envp);

IF      LDATA
                push    word ptr __C0environ+2
                push    word ptr __C0environ
                push    word ptr __C0argv+2
                push    word ptr __C0argv
ELSE
                push    word ptr __C0environ
                push    word ptr __C0argv
ENDIF
                push    __C0argc
                call    _main

;       Flush and close streams and files

                push    ax
                call    _exit

;---------------------------------------------------------------------------
;       _cleanup()      call all #pragma exit cleanup routines.
;       _checknull()    check for null pointer zapping copyright message
;       _terminate(int) exit program with error code
;
;       These functions are called by exit(), _exit(), _cexit(),
;       and _c_exit().
;---------------------------------------------------------------------------

;       Call cleanup routines

__cleanup       PROC    DIST
                PUBLIC  __cleanup

                mov     es, cs:DGROUP@@
                push    si
                push    di
                mov     si,offset DGROUP:ExitStart
                mov     di,offset DGROUP:ExitEnd
                call    StartExit
                pop     di
                pop     si
                ret
__cleanup       ENDP

;       Check for null pointers before exit

__checknull     PROC    DIST
                PUBLIC  __checknull

IF      LDATA EQ false
  IFNDEF  __TINY__
                push    si
                push    di
                mov     es, cs:DGROUP@@
                xor     ax, ax
                mov     si, ax
                mov     cx, lgth_CopyRight
ComputeChecksum label   near
                add     al, es:[si]
                adc     ah, 0
                inc     si
                loop    ComputeChecksum
                sub     ax, CheckSum
                jz      @@SumOk
                mov     cx, lgth_NullCheck
                mov     dx, offset DGROUP: NullCheck
                call    ErrorDisplay
@@SumOK:        pop     di
                pop     si
  ENDIF
ENDIF
                ret
__checknull     ENDP

;       Exit to DOS

__terminate     PROC    DIST
                PUBLIC  __terminate
                mov     bp,sp
                mov     ah,4Ch
                mov     al,[bp+cPtrSize]
                int     21h                     ; Exit to DOS
__terminate     ENDP

STARTX          ENDP

        SUBTTL  Vector save/restore & default Zero divide routines
        PAGE
;[]------------------------------------------------------------[]
;|                                                              |
;| Interrupt Save/Restore routines and default divide by zero   |
;| handler.                                                     |
;|                                                              |
;[]------------------------------------------------------------[]

ZeroDivision    PROC    FAR
                mov     cx, lgth_ZeroDivMSG
                mov     dx, offset DGROUP: ZeroDivMSG
                jmp     MsgExit3
ZeroDivision    ENDP

;--------------------------------------------------------------------------
;       savevectors()
;
;       Save vectors for 0, 4, 5 & 6 interrupts.  This is for extended
;       signal()/raise() support as the signal functions can steal these
;       vectors during runtime.
;--------------------------------------------------------------------------
SaveVectors     PROC    NEAR
                push    ds
; Save INT 0
                mov     ax, 3500h
                int     021h
                mov     word ptr _Int0Vector@, bx
                mov     word ptr _Int0Vector@+2, es
; Save INT 4
                mov     ax, 3504h
                int     021h
                mov     word ptr _Int4Vector@, bx
                mov     word ptr _Int4Vector@+2, es
; Save INT 5
                mov     ax, 3505h
                int     021h
                mov     word ptr _Int5Vector@, bx
                mov     word ptr _Int5Vector@+2, es
; Save INT 6
                mov     ax, 3506h
                int     021h
                mov     word ptr _Int6Vector@, bx
                mov     word ptr _Int6Vector@+2, es
;
;       Install default divide by zero handler.
;
                mov     ax, 2500h
                mov     dx, cs
                mov     ds, dx
                mov     dx, offset ZeroDivision
                int     21h

                pop     ds
                ret
SaveVectors     ENDP

;--------------------------------------------------------------------------
;       _restorezero() puts back all the vectors that SaveVectors took.
;
;NOTE : TSRs must BE AWARE that signal() functions which take these 
;       vectors will be deactivated if the keep() function is executed.
;       If a TSR wants to use the signal functions when it is active it 
;       will have to save/restore these vectors itself when activated and
;       deactivated.
;--------------------------------------------------------------------------
__restorezero   PROC    DIST
                PUBLIC  __restorezero
IFDEF   __HUGE__
                push    ds
                mov     ds, cs: DGROUP@@
ENDIF
                push    ds
                mov     ax, 2500h
                lds     dx, _Int0Vector@
                int     21h
                pop     ds

                push    ds
                mov     ax, 2504h
                lds     dx, _Int4Vector@
                int     21h
                pop     ds

                push    ds
                mov     ax, 2505h
                lds     dx, _Int5Vector@
                int     21h
                pop     ds

IFNDEF   __HUGE__
                push    ds
ENDIF
                mov     ax, 2506h
                lds     dx, _Int6Vector@
                int     21h
                pop     ds

                ret
                ENDP

;------------------------------------------------------------------
;  Loop through a startup/exit (SE) table, 
;  calling functions in order of priority.
;  ES:SI is assumed to point to the beginning of the SE table
;  ES:DI is assumed to point to the end of the SE table
;  First 64 priorities are reserved by Borland
;------------------------------------------------------------------
PNEAR           EQU     0
PFAR            EQU     1
NOTUSED         EQU     0ffh

SE              STRUC
calltype        db      ?                       ; 0=near,1=far,ff=not used
priority        db      ?                       ; 0=highest,ff=lowest
addrlow         dw      ?
addrhigh        dw      ?
SE              ENDS

StartExit       proc near
@@Start:        cmp     si,offset DGROUP:InitStart      ; startup or exit?
                je      @@StartLow              ; it's startup
                xor     ah,ah                   ; start with high priority
                jmp     short @@SaveEnd
@@StartLow:     mov     ah,0ffh                 ;start with lowest priority
@@SaveEnd:      mov     dx,di                   ;set sentinel to end of table
                mov     bx,si                   ;bx = start of table

@@TopOfTable:   cmp     bx,di                   ;and the end of the table?
                je      @@EndOfTable            ;yes, exit the loop
                cmp     es:[bx.calltype],NOTUSED;check the call type
                je      @@Next
                cmp     si,offset DGROUP:InitStart      ; startup or exit?
                je      @@CompareHigh           ; it's startup
                cmp     ah,es:[bx.priority]     ; it's exit
                jmp     short @@CheckPrior      ; if priority too low, skip
@@CompareHigh:  cmp     es:[bx.priority],ah     ;check the priority
@@CheckPrior:   ja      @@Next                  ;too high?  skip
                mov     ah,es:[bx.priority]     ;keep priority
                mov     dx,bx                   ;keep index in dx
@@Next:         add     bx,SIZE SE              ;bx = next item in table
                jmp     @@TopOfTable

@@EndOfTable:   cmp     dx,di                   ;did we exhaust the table?
                je      @@Done                  ;yes, quit
                mov     bx,dx                   ;bx = highest priority item
                cmp     es:[bx.calltype],PNEAR  ;is it near or far?
                mov     es:[bx.calltype],NOTUSED;wipe the call type
                push    es                      ;save es
                je      @@NearCall

@@FarCall:      call    DWORD PTR es:[bx.addrlow]
                pop     es                      ;restore es
                jmp     short @@Start

@@NearCall:     call    WORD PTR es:[bx.addrlow]
                pop     es                      ;restore es
                jmp     short @@Start

@@Done:         ret
                endp

;------------------------------------------------------------------

ErrorDisplay    PROC    NEAR
                mov     ah, 040h
                mov     bx, 2
                int     021h
                ret
ErrorDisplay    ENDP

_abort          PROC    DIST
                PUBLIC  _abort
                mov     cx, lgth_abortMSG
                mov     dx, offset DGROUP: abortMSG
MsgExit3        label   near
                mov     ds, cs: DGROUP@@
                call    ErrorDisplay
CallExit3       label   near
                mov     ax, 3
                push    ax
                call    __exit           ; _exit(3);
                ENDP

; The DGROUP@ variable is used to reload DS with DGROUP

PubSym@         DGROUP@, <dw    ?>, __PASCAL__

IFDEF	__BOSS__
PubSym@		CSalias@,<dw    ?>, __PASCAL__
ENDIF


; __MMODEL is used to determine the memory model or the default
; pointer types at run time.

                public __MMODEL
__MMODEL        dw      MMODEL

_TEXT           ENDS

                SUBTTL  Start Up Data Area
                PAGE
;[]------------------------------------------------------------[]
;|      Start Up Data Area                                      |
;|                                                              |
;|      WARNING         Do not move any variables in the data   |
;|                      segment unless you're absolutely sure   |
;|                      that it does not matter.                |
;[]------------------------------------------------------------[]

_DATA           SEGMENT

;       Magic symbol used by the debug info to locate the data segment
                public DATASEG@
DATASEG@        label   byte

;       The CopyRight string must NOT be moved or changed without
;       changing the null pointer check logic

CopyRight       db      4 dup(0)
                db      'Borland C++ - Copyright 1991 Borland Intl.',0
lgth_CopyRight  equ     $ - CopyRight

IF      LDATA EQ false
IFNDEF  __TINY__
CheckSum        equ     00D5Ch
NullCheck       db      'Null pointer assignment', 13, 10
lgth_NullCheck  equ     $ - NullCheck
ENDIF
ENDIF

ZeroDivMSG      db      'Divide error', 13, 10
lgth_ZeroDivMSG equ     $ - ZeroDivMSG

abortMSG        db      'Abnormal program termination', 13, 10
lgth_abortMSG   equ     $ - abortMSG

; JAB - Added string for no 286
no286MSG		db		'Sorry, this program requires a 286 or better.', 13, 10
lgth_no286MSG	equ		$ - no286MSG
; JAB - End of modifications

;
;                       Interrupt vector save areas
;       
;       Interrupt vectors 0,4,5 & 6 are saved at startup and then restored
;       when the program terminates.  The signal/raise functions might
;       steal these vectors during execution.
;
;       Note: These vectors save area must not be altered 
;             without changing the save/restore logic.
;
PubSym@         _Int0Vector     <dd     0>,             __CDECL__
PubSym@         _Int4Vector     <dd     0>,             __CDECL__
PubSym@         _Int5Vector     <dd     0>,             __CDECL__
PubSym@         _Int6Vector     <dd     0>,             __CDECL__
;
;                       Miscellaneous variables
;       
PubSym@         _C0argc,        <dw     0>,             __CDECL__
dPtrPub@        _C0argv,        0,                      __CDECL__
dPtrPub@        _C0environ,     0,                      __CDECL__
PubSym@         _envLng,        <dw     0>,             __CDECL__
PubSym@         _envseg,        <dw     0>,             __CDECL__
PubSym@         _envSize,       <dw     0>,             __CDECL__
PubSym@         _psp,           <dw     0>,             __CDECL__
PubSym@         _version,       <label word>,           __CDECL__
PubSym@         _osversion,     <label word>,           __CDECL__
PubSym@         _osmajor,       <db     0>,             __CDECL__
PubSym@         _osminor,       <db     0>,             __CDECL__
PubSym@         errno,          <dw     0>,             __CDECL__
PubSym@         _StartTime,     <dw   0,0>,             __CDECL__


IFDEF __BOSS__
PubSym@		_protected	<dw    0>,		__CDECL__
PubSym@ 	_shiftcount,	<dw    12>,		__CDECL__
PubSym@ 	_hugeincval,	<dw    1000h>,		__CDECL__
ENDIF

;       Memory management variables

IF      LDATA EQ false
PubSym@         __heapbase,     <dw   DGROUP:edata@>,   __CDECL__
ENDIF
IFNDEF __HUGE__
PubSym@         __brklvl,       <dw   DGROUP:edata@>,   __CDECL__
ENDIF
PubSym@         _heapbase,      <dd   0>,       __CDECL__
PubSym@         _brklvl,        <dd   0>,       __CDECL__
PubSym@         _heaptop,       <dd   0>,       __CDECL__

;       If stack in DS and Large data model then override location of __emu

IFDEF   _DSSTACK_
IF      LDATA
public  __emu
__emu   db      044h    DUP (0)
        db      0CCh    DUP (?)
ENDIF
ENDIF

_DATA           ENDS


_CVTSEG         SEGMENT
PubSym@         _RealCvtVector, <label  word>,  __CDECL__
                ENDS

_SCNSEG         SEGMENT
PubSym@         _ScanTodVector,  <label word>,  __CDECL__
                ENDS

IFNDEF __HUGE__
_BSS            SEGMENT
bdata@          label   byte
                ENDS

_BSSEND         SEGMENT
edata@          label   byte
                ENDS
ENDIF

IFNDEF __TINY__
_STACK          SEGMENT
IFDEF __BOSS__
    IF LDATA
                db      1400h dup(?)
    ENDIF
ELSE
                db      128 dup(?)               ;minimum stack size
ENDIF  ; __BOSS__
                ENDS
ENDIF  ; __TINY__
                END     STARTX
