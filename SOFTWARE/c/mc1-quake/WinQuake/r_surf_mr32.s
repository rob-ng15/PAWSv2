/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2021 Marcus Geelnard

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifdef __MRISC32_VECTOR_OPS__

; r_surf_mr32.s
; MRISC32 assembler implementations of surface block drawing.

;-----------------------------------------------------------------------------
; We describe the R_DrawSurfaceBlock8 function for each mip level as a macro.
;
;  count   = Loop count   (16, 8, 4 or 2)
;  l2count = log2(count)  (4, 3, 2 or 1)
;-----------------------------------------------------------------------------

.macro R_DrawSurfaceBlock8 count, l2count
    ; Get the outer loop counter (and skip to the end if zero).
    ldwpc   r10, #r_numvblocks@pc            ; r10 = v (= r_numvblocks)
    bz      r10, 2$

    ; Save callee saved registers.
    stw     vl, [sp, #-4]

    ; Pre-load global variables into registers.
    ldwpc   r1, #sourcetstep@pc              ; r1 = sourcetstep
    ldwpc   r2, #surfrowbytes@pc             ; r2 = surfrowbytes
    ldwpc   r3, #r_lightwidth@pc             ; r3 = r_lightwidth
    ldwpc   r4, #r_sourcemax@pc              ; r4 = r_sourcemax
    ldwpc   r5, #r_stepback@pc               ; r5 = r_stepback
    ldwpc   r6, #r_lightptr@pc               ; r6 = r_lightptr
    ldwpc   r7, #vid+4@pc                    ; r7 = vid.colormap (vid+4)
    ldwpc   r8, #pbasesource@pc              ; r8 = pbasesource
    ldwpc   r9, #prowdestbase@pc             ; r9 = prowdestbase

    ; Set up vector operation (we use the same VL for all iterations).
    ldi     vl, #\count

    ; Outer loop.
1$:
    ; Set up light step & boundaries for this iteration.
    ldw     r11, [r6, #0]       ; r11 = lightleft = r_lightptr[0]
    ldw     r12, [r6, #4]       ; r12 = lightright = r_lightptr[1]
    ldea    r6, [r6, r3 * 4]    ; r_lightptr += r_lightwidth
    ldw     r13, [r6, #0]
    ldw     r14, [r6, #4]
    sub     r13, r13, r11
    asr     r13, r13, #\l2count ; r13 = lightleftstep = (r_lightptr[0] - lightleft) >> l2count
    sub     r14, r14, r12
    asr     r14, r14, #\l2count ; r14 = lightrightstep = (r_lightptr[1] - lightright) >> l2count

    ; Unroll the inner loop.
    .rept   \count
        sub     r15, r12, r11
        asr     r15, r15, #\l2count ; r15 = lightstep = (lightright - lightleft) >> l2count
        ldub    v2, [r8, #1]        ; v2 = pbasesource[b]
        ldea    v1, [r11, r15]      ; v1 = lightleft + lightstep*b
        ibf     v1, v2, #<0:8>      ; v1 = (v1 & 0xffffff00) | v2
        ldub    v1, [r7, v1]        ; v1 = vid.colormap[(v2 & 0xff00) | v1]
        stb     v1, [r9, #1]        ; prowdestbase[b] = v1
        ldea    r8, [r8, r1]        ; pbasesource += sourcetstep
        add     r11, r11, r13       ; lightleft += lightleftstep
        add     r12, r12, r14       ; lightright += lightrightstep
        ldea    r9, [r9, r2]        ; prowdestbase += surfrowbytes
    .endr

    sle     r15, r4, r8         ; pbasesource >= r_sourcemax?
    bns     r15, 1f
    sub     r8, r8, r5          ; pbasesource -= r_stepback
1:
    add     r10, r10, #-1       ; v--
    bnz     r10, 1$

    ; Store the final value of r_lightptr.
    stwpc   r6, #r_lightptr@pc

    ; Restore callee saved registers.
    ldw     vl, [sp, #-4]

2$:
    ret
.endm


	.text

;-----------------------------------------------------------------------------
; void R_DrawSurfaceBlock8_mip0 (void)
;-----------------------------------------------------------------------------

    .p2align    5
    .global     R_DrawSurfaceBlock8_mip0
    .type       R_DrawSurfaceBlock8_mip0, @function

R_DrawSurfaceBlock8_mip0:
    R_DrawSurfaceBlock8 16, 4

    .size   R_DrawSurfaceBlock8_mip0, .-R_DrawSurfaceBlock8_mip0


;-----------------------------------------------------------------------------
; void R_DrawSurfaceBlock8_mip1 (void)
;-----------------------------------------------------------------------------

    .p2align    5
    .global     R_DrawSurfaceBlock8_mip1
    .type       R_DrawSurfaceBlock8_mip1, @function

R_DrawSurfaceBlock8_mip1:
    R_DrawSurfaceBlock8 8, 3

    .size   R_DrawSurfaceBlock8_mip1, .-R_DrawSurfaceBlock8_mip1


;-----------------------------------------------------------------------------
; void R_DrawSurfaceBlock8_mip2 (void)
;-----------------------------------------------------------------------------

    .p2align    5
    .global     R_DrawSurfaceBlock8_mip2
    .type       R_DrawSurfaceBlock8_mip2, @function

R_DrawSurfaceBlock8_mip2:
    R_DrawSurfaceBlock8 4, 2

    .size   R_DrawSurfaceBlock8_mip2, .-R_DrawSurfaceBlock8_mip2


;-----------------------------------------------------------------------------
; void R_DrawSurfaceBlock8_mip3 (void)
;-----------------------------------------------------------------------------

    .p2align    5
    .global     R_DrawSurfaceBlock8_mip3
    .type       R_DrawSurfaceBlock8_mip3, @function

R_DrawSurfaceBlock8_mip3:
    R_DrawSurfaceBlock8 2, 1

    .size   R_DrawSurfaceBlock8_mip3, .-R_DrawSurfaceBlock8_mip3

#endif  /* __MRISC32_VECTOR_OPS__ */

