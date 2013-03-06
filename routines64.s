
; ============================================================================
;  bandwidth 0.16, a benchmark to estimate memory transfer bandwidth.
;  Copyright (C) 2005,2007-2009 by Zack T Smith.
;
;  This program is free software; you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation; either version 2 of the License, or
;  (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this program; if not, write to the Free Software
;  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;
;  The author may be reached at fbui@comcast.net.
; =============================================================================

cpu		x64
bits	64

global	WriterSSE2

; Note to self:
; Unix ABI says integer param are put in these registers in this order:
;	rdi, rsi, rdx, rcx, r8, r9

	section .text

;------------------------------------------------------------------------------
; Name:		WriterSSE2
; Purpose:	Writes 128-bit value sequentially to an area of memory.
; Params:	rdi = ptr to memory area
; 		rsi = loops
; 		rdx = length in bytes
; 		rcx = quad to write
;------------------------------------------------------------------------------

WriterSSE2:
	mov	r11, rdi
	add	r11, rdx	; r11 points to end of area.
   xor   r13,r13
	;movq	xmm0, rcx

   push rax
   push rdx

   rdtsc  
   shl rdx, 32
   or rdx, rax
   mov r12, rdx

w_outer2:
	mov	r10, rdi

w_inner2:
	; Move a quadriword (128 bit) directly to memory without cache pollution
	movntdq	[r10], xmm0
	movntdq	[16+r10], xmm0
	movntdq	[32+r10], xmm0
	movntdq	[48+r10], xmm0
	movntdq	[64+r10], xmm0
	movntdq	[80+r10], xmm0
	movntdq	[96+r10], xmm0
	movntdq	[112+r10], xmm0

	add	r10, 128
	cmp	r10, r11
	jb	w_inner2

   inc r13

   rdtsc
   shl rdx, 32
   or rdx, rax
   sub rdx, r12

   cmp rdx, rsi
   jb w_outer2

	;dec	rsi
	;jnz	w_outer2

   pop rdx
   pop rax

   mov rax, r13
	ret
