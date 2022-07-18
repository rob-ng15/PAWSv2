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

#ifdef __MRISC32__

#include "asm_draw.h"

; d_scan_mr32.s
; MRISC32 assembler implementations of span drawing functions.

;-----------------------------------------------------------------------------
; void D_DrawSpans8 (espan_t *pspan)
;
; r1 = pspan
;-----------------------------------------------------------------------------

#ifdef __MRISC32_HARD_FLOAT__

    .p2align    5
    .global     D_DrawSpans8
    .type       D_DrawSpans8, @function

D_DrawSpans8:
    ; Store callee saved registers on the stack.
    stw     lr, [sp, #-4]
    stw     fp, [sp, #-8]
    stw     tp, [sp, #-12]
    stw     r26, [sp, #-16]
    stw     r25, [sp, #-20]
    stw     r24, [sp, #-24]
    stw     r23, [sp, #-28]
    stw     r22, [sp, #-32]
    stw     r21, [sp, #-36]
    stw     r20, [sp, #-40]
    stw     r19, [sp, #-44]
    stw     r18, [sp, #-48]
    stw     r17, [sp, #-52]
    stw     r16, [sp, #-56]

    ; Load hot global variables into registers.
    ldwpc   r2, #cacheblock@pc              ; r2 = pbase (unsigned char *)
    ldwpc   r3, #d_viewbuffer@pc            ; r3 = d_viewbuffer (byte *)
    ldwpc   r4, #screenwidth@pc             ; r4 = screenwidth (int)
    ldwpc   r5, #d_sdivzstepu@pc            ; r5 = d_sdivzstepu (float)
    ldwpc   r6, #d_tdivzstepu@pc            ; r6 = d_tdivzstepu (float)
    ldwpc   r7, #d_zistepu@pc               ; r7 = d_zistepu (float)
    ldwpc   r8, #sadjust@pc                 ; r8 = sadjust (fixed16_t)
    ldwpc   r9, #tadjust@pc                 ; r9 = tadjust (fixed16_t)
    ldwpc   r10, #bbextents@pc              ; r10 = bbextents (fixed16_t)
    ldwpc   r11, #bbextentt@pc              ; r11 = bbextentt (fixed16_t)
    ldwpc   r12, #cachewidth@pc             ; r12 = cachewidth (int)
    ldwpc   r26, #d_sdivzstepv@pc           ; r26 = d_sdivzstepv (float)

    ; Pre-calculate 8.0 * x (and store on the stack since we're out of regs).
    ldi     lr, #0x41000000             ; 8.0
    fmul    r13, r7, lr
    fmul    r14, r6, lr
    fmul    r15, r5, lr
    stw     r13, [sp, #-60]             ; 8.0 * d_zistepu
    stw     r14, [sp, #-64]             ; 8.0 * d_tdivzstepu
    stw     r15, [sp, #-68]             ; 8.0 * d_sdivzstepu

    ; Pre-load constants.
    ldi     tp, #0x47800000             ; tp = 65536.0

    ; Outer loop: Loop over spans.
1:
    ; pdest = (unsigned char *)&viewbuffer[(screenwidth * pspan->v) + pspan->u]
    ; NOTE: Schedule instructions and use madd to avoid stalls.
    ldw     r13, [r1, #espan_t_v]       ; r13 = pspan->v (int)
    ldw     r14, [r1, #espan_t_u]       ; r14 = pspan->u (int)
    mul     r15, r13, r4
    itof    r13, r13, z                 ; dv = (float)pspan->v
    ldw     r16, [r1, #espan_t_count]   ; r16 = count (int)
    add     r15, r15, r14
    itof    r14, r14, z                 ; du = (float)pspan->u
    ldea    r15, [r3, r15]              ; r15 = pdest

    ; Calculate the initial s/z, t/z, 1/z, s, and t and clamp.
    ; sdivz = d_sdivzorigin + dv*d_sdivzstepv + du*d_sdivzstepu
    ; tdivz = d_tdivzorigin + dv*d_tdivzstepv + du*d_tdivzstepu
    ; zi = d_ziorigin + dv*d_zistepv + du*d_zistepu
    ; NOTE: Schedule instructions to avoid stalls.
    fmul    r20, r14, r5
    fmul    r17, r13, r26
    ldwpc   lr, #d_tdivzstepv@pc
    fmul    r21, r14, r6
    fmul    r18, r13, lr
    ldwpc   lr, #d_zistepv@pc
    fmul    r22, r14, r7
    fmul    r19, r13, lr
    fadd    r17, r17, r20
    fadd    r18, r18, r21
    ldwpc   lr, #d_sdivzorigin@pc
    fadd    r19, r19, r22
    fadd    r17, r17, lr                ; r17 = sdivz (float)
    ldwpc   lr, #d_tdivzorigin@pc
    ldwpc   r20, #d_ziorigin@pc
    fadd    r18, r18, lr                ; r18 = tdivz (float)
    fadd    r19, r19, r20               ; r19 = zi (float)

    ; Invert zi and prescale to 16.16 fixed-point
    fdiv    r20, tp, r19                ; r20 = z = (float)0x10000 / zi

    ; s = clamp((int)(sdivz * z) + sadjust, 0, bbextents)
    ; t = clamp((int)(tdivz * z) + tadjust, 0, bbextentt)
    fmul    r21, r17, r20
    fmul    r22, r18, r20
    ftoi    r21, r21, z
    ftoi    r22, r22, z
    add     r21, r21, r8
    max     r21, r21, #0
    min     r21, r21, r10               ; r21 = s
    add     r22, r22, r9
    max     r22, r22, #0
    min     r22, r22, r11               ; r22 = t

    ; Inner loop: Loop over pixels, up to 8 pixels per iteration.
2:
    minu    vl, r16, #8                 ; vl = spancount = min(count, 8)
    sub     r16, r16, vl                ; count -= spancount

    ; Calculate s and t steps.
    bz      r16, 3f

    ; spancount is 8, so calculate s/z, t/z, zi->fixed s and t at far end of
    ; span, and calculate s and t steps across span by shifting.
    ldw     r23, [sp, #-60]             ; 8.0 * d_zistepu
    ldw     r24, [sp, #-64]             ; 8.0 * d_tdivzstepu
    ldw     r25, [sp, #-68]             ; 8.0 * d_sdivzstepu
    fadd    r19, r19, r23               ; zi += 8.0 * d_zistepu
    fadd    r18, r18, r24               ; tdivz += 8.0 * d_tdivzstepu
    fadd    r17, r17, r25               ; sdivz += 8.0 * d_sdivzstepu
    fdiv    r20, tp, r19                ; r20 = z = (float)0x10000 / zi

    ; snext = clamp((int)(sdivz * z) + sadjust, 8, bbextents)
    ; tnext = clamp((int)(tdivz * z) + tadjust, 8, bbextentt)
    fmul    r23, r17, r20
    fmul    r24, r18, r20
    ftoi    r23, r23, z
    ftoi    r24, r24, z
    add     r23, r23, r8
    max     r23, r23, #8
    min     r23, r23, r10               ; r23 = snext
    add     r24, r24, r9
    max     r24, r24, #8
    min     r24, r24, r11               ; r24 = tnext

    ; sstep = (snext - s) >> 3
    sub     r25, r23, r21
    asr     r25, r25, #3                ; r25 = sstep

    ; tstep = (tnext - t) >> 3
    sub     lr, r24, r22
    asr     lr, lr, #3                  ; lr = tstep

4:
#ifdef __MRISC32_VECTOR_OPS__
    ldea    v1, [r21, r25]              ; v1[k] = s + sstep * k
    ldea    v2, [r22, lr]               ; v2[k] = t + tstep * k
    lsr     v1, v1, #16                 ; v1[k] = v1[k] >> 16
    lsr     v2, v2, #16                 ; v2[k] = v2[k] >> 16
    madd    v1, v2, r12                 ; v1[k] = v1[k] + v2[k] * cachewidth
    ldub    v1, [r2, v1]                ; v1[k] = pbase[v1[k]]
    stb     v1, [r15, #1]               ; pdest[k] = v1[k]
    ldea    r15, [r15, vl]              ; pdest += spancount
#else
#error "Support for non-vectorized operation not implemented yet"
#endif

    mov     r21, r23                    ; s = snext
    mov     r22, r24                    ; t = tnext

    bnz     r16, 2b                     ; while (count > 0)

    ldw     r1, [r1, #espan_t_pnext]    ; pspan = pspan->pnext
    bnz     r1, 1b                      ; while (pspan != NULL)

    ; Restore callee saved registers from the stack.
    ldw     lr, [sp, #-4]
    ldw     fp, [sp, #-8]
    ldw     tp, [sp, #-12]
    ldw     r26, [sp, #-16]
    ldw     r25, [sp, #-20]
    ldw     r24, [sp, #-24]
    ldw     r23, [sp, #-28]
    ldw     r22, [sp, #-32]
    ldw     r21, [sp, #-36]
    ldw     r20, [sp, #-40]
    ldw     r19, [sp, #-44]
    ldw     r18, [sp, #-48]
    ldw     r17, [sp, #-52]
    ldw     r16, [sp, #-56]
    ret

3:
    ; spancount is <8, so calculate s/z, t/z, zi->fixed s and t at last pixel
    ; in span (so can't step off polygon), clamp, calculate s and t steps
    ; across span by division, biasing steps low so we don't run off the
    ; texture.
    add     fp, vl, #-1                 ; fp = spancount - 1
    itof    lr, fp, z                   ; (float)(spancount - 1)

    bz      fp, 4b                      ; Early-out if spancount == 1
                                        ; (and hide some of the itof latency)

    fmul    r23, r7, lr
    fmul    r24, r6, lr
    fmul    r25, r5, lr
    fadd    r19, r19, r23               ; zi += (spancount - 1) * d_zistepu
    fadd    r18, r18, r24               ; tdivz += (spancount - 1) * d_tdivzstepu
    fadd    r17, r17, r25               ; sdivz += (spancount - 1) * d_sdivzstepu
    fdiv    r20, tp, r19                ; r20 = z = (float)0x10000 / zi

    ; snext = clamp((int)(sdivz * z) + sadjust, 8, bbextents)
    ; tnext = clamp((int)(tdivz * z) + tadjust, 8, bbextentt)
    fmul    r23, r17, r20
    fmul    r24, r18, r20
    ftoi    r23, r23, z
    ftoi    r24, r24, z
    add     r23, r23, r8
    max     r23, r23, #8
    min     r23, r23, r10               ; r23 = snext
    add     r24, r24, r9
    max     r24, r24, #8
    min     r24, r24, r11               ; r24 = tnext

    ; sstep = (snext - s) / (spancount - 1)
    sub     r25, r23, r21
    div     r25, r25, fp                ; r25 = sstep

    ; tstep = (tnext - t) / (spancount - 1)
    sub     lr, r24, r22
    div     lr, lr, fp                  ; lr = tstep

    b       4b

    .size   D_DrawSpans8, .-D_DrawSpans8

#endif  /* __MRISC32_HARD_FLOAT__ */


;-----------------------------------------------------------------------------
; void D_DrawZSpans (espan_t *pspan)
;
; r1 = pspan
;-----------------------------------------------------------------------------

#ifdef __MRISC32_HARD_FLOAT__

    .p2align    5
    .global     D_DrawZSpans
    .type       D_DrawZSpans, @function

D_DrawZSpans:
    ldwpc   r2, #d_pzbuffer@pc          ; r2 = d_pzbuffer (short *)
    ldwpc   r3, #d_zwidth@pc            ; r3 = d_zwidth (unsigned int)
    ldwpc   r4, #d_ziorigin@pc          ; r4 = d_ziorigin (float)
    ldwpc   r5, #d_zistepu@pc           ; r5 = d_zistepu (float)
    ldwpc   r6, #d_zistepv@pc           ; r6 = d_zistepv (float)

    ldi     r7, #31                     ; r7 = 31 (used for ftoi)
    ftoir   r12, r5, r7                 ; r12 = izistep = (int)(d_zistepu * 2^31)

    ; Outer loop.
1:
    ; Calculate the initial 1/z
    ; NOTE: Schedule instructions and use madd to avoid stalls.
    ldw     r11, [r1, #espan_t_v]       ; r11 = pspan->v (int)
    ldw     r10, [r1, #espan_t_u]       ; r10 = pspan->u (int)
    mul     r8, r11, r3
    itof    r11, r11, z                 ; r11 = dv = (float)pspan->v
    ldw     r9, [r1, #espan_t_count]    ; r9 = count (int)
    add     r8, r8, r10
    itof    r10, r10, z                 ; r10 = du = (float)pspan->u
    ldea    r8, [r2, r8*2]              ; r8 = pdest (short*)
    fmul    r11, r11, r6
    fmul    r10, r10, r5
    fadd    r10, r10, r11
    fadd    r10, r10, r4                ; r10 = zi = d_ziorigin + dv*d_zistepv + du*d_zistepu
    ftoir   r10, r10, r7                ; r10 = izi = (int)(zi * 2^31)

    ; Handle un-aligned head.
    and     r11, r8, #2
    bz      r11, 2f
    lsr     r11, r10, #16               ; r11 = (short)(izi >> 16)
    add     r10, r10, r12               ; izi += izistep
    add     r8, r8, #2                  ; pdest++
    add     r9, r9, #-1                 ; count--
    sth     r11, [r8, #-2]              ; *pdest = r11

2:
    lsr     r13, r9, #1                 ; r13 = doublecount
    bz      r13, 4f

#ifdef __MRISC32_VECTOR_OPS__
    ; Inner loop (vectorized).
    getsr   vl, #0x10
    mul     r14, r12, vl                ; r14 = izistep * VL
3:
    min     vl, vl, r13
    sub     r13, r13, vl

    ldea    v1, [r10, r12*2]            ; v1 = izi (even)
    add     v2, v1, r12                 ; v2 = izi (odd)
#ifdef __MRISC32_PACKED_OPS__
    packhi  v1, v2, v1                  ; v1 = (v2 & 0xffff0000 | (v1 >> 16)
#else
    shuf    v3, v2, #0b0011010100100    ; v3 = v1 & 0xffff0000
    lsr     v4, v1, #16
    or      v1, v3, v4                  ; v1 = (v2 & 0xffff0000 | (v1 >> 16)
#endif
    ldea    r10, [r10, r14*2]           ; izi += izistep * VL * 2

    stw     v1, [r8, #4]                ; *(int*)pdest = (v2 & 0xffff0000 | (v1 >> 16))
    ldea    r8, [r8, vl*4]              ; pdest += 2 * VL

    bnz     r13, 3b
#else
#error "Support for non-vectorized operation not implemented yet"
#endif

4:
    ldw     r1, [r1, #espan_t_pnext]    ; pspan = pspan->pnext

    ; Handle un-aligned tail.
    and     r9, r9, #1
    bz      r9, 5f
    lsr     r11, r10, #16               ; r11 = (short)(izi >> 16)
    sth     r11, [r8]                   ; *pdest = r11

5:
    ; while (pspan != NULL)
    bnz     r1, 1b

    ret

    .size   D_DrawZSpans, .-D_DrawZSpans

#endif  /* __MRISC32_HARD_FLOAT__ */


;-----------------------------------------------------------------------------
; void D_DrawTurbulent8Span (void)
;-----------------------------------------------------------------------------

#ifdef __MRISC32_HARD_FLOAT__

    .p2align    5
    .global     D_DrawTurbulent8Span
    .type       D_DrawTurbulent8Span, @function

D_DrawTurbulent8Span:
    ldwpc   r1, #r_turb_s@pc            ; r1 = r_turb_s (fixed16_t)
    ldwpc   r2, #r_turb_t@pc            ; r2 = r_turb_t (fixed16_t)
    ldwpc   r3, #r_turb_sstep@pc        ; r3 = r_turb_sstep (fixed16_t)
    ldwpc   r4, #r_turb_tstep@pc        ; r4 = r_turb_tstep (fixed16_t)
    ldwpc   r5, #r_turb_spancount@pc    ; r5 = r_turb_spancount (int)
    ldwpc   r6, #r_turb_turb@pc         ; r6 = r_turb_turb (int*)
    ldwpc   r7, #r_turb_pbase@pc        ; r7 = r_turb_pbase (unsigned char*)
    ldwpc   r8, #r_turb_pdest@pc        ; r8 = r_turb_pdest (unsigned char*)

#ifdef __MRISC32_VECTOR_OPS__
    getsr   vl, #0x10
    ldea    v1, r1, r3
    ldea    v2, r2, r4

    madd    r1, r3, r5                  ; r_turb_s += r_turb_sstep * r_turb_spancount
    madd    r2, r4, r5                  ; r_turb_t += r_turb_tstep * r_turb_spancount

    mul     r3, r3, vl
    mul     r4, r4, vl

1:
    min     vl, vl, r5
    sub     r5, r5, vl

    ; sturb = ((r_turb_s + r_turb_turb[(r_turb_t >> 16) & (CYCLE-1)]) >> 16) & 63
    ebfu    v9, v2, #<16:7>
    ldw     v9, [r6, v9 * 4]
    add     v9, v1, v9
    ebfu    v9, v9, #<16:6>

    ; tturb = ((r_turb_t + r_turb_turb[(r_turb_s >> 16) & (CYCLE-1)]) >> 16) & 63
    ebfu    v10, v1, #<16:7>
    ldw     v10, [r6, v10 * 4]
    add     v10, v2, v10
    ebfu    v10, v10, #<16:6>

    ; *r_turb_pdest++ = r_turb_pbase[(tturb << 6) + sturb]
    ibf     v9, v10, #<6:6>
    ldub    v9, [r7, v9]
    stb     v9, [r8, #1]
    add     r8, r8, vl

    ; r_turb_s += r_turb_sstep
    add     v1, v1, r3

    ; r_turb_t += r_turb_tstep
    add     v2, v2, r4

    ; while (--r_turb_spancount > 0)
    bgt     r5, 1b
#else
#error "Support for non-vectorized operation not implemented yet"
#endif

    stwpc   r1, #r_turb_s@pc
    stwpc   r2, #r_turb_t@pc
    stwpc   r8, #r_turb_pdest@pc

    ret

    .size   D_DrawTurbulent8Span, .-D_DrawTurbulent8Span

#endif  /* __MRISC32_HARD_FLOAT__ */


;-----------------------------------------------------------------------------
; void D_WarpScreenKernel (int src_w, int src_h, int *turb, byte **rowptr,
;                          int *column, byte *dest, int dest_stride)
;
; r1 = src_w
; r2 = src_h
; r3 = turb
; r4 = rowptr
; r5 = column
; r6 = dest
; r7 = dest_stride
;-----------------------------------------------------------------------------

    .p2align    5
    .global     D_WarpScreenKernel
    .type       D_WarpScreenKernel, @function

D_WarpScreenKernel:
    ble     r2, 3f
    ble     r1, 3f

    mov     r10, r3                 ; r10 = &turb[v]

1:
    ; col = &column[turb[v]]
    ldw     r9, [r10]
    ldea    r9, [r5, r9*4]          ; r9 = col

    mov     r11, r3                 ; r11 = &turb[u]

#ifdef __MRISC32_VECTOR_OPS__
    getsr   vl, #0x10
    mov     r8, r1
2:
    min     vl, vl, r8
    sub     r8, r8, vl

    ; *dest = row[turb[u]][col[u]]
    ldw     v1, [r11, #4]           ; *turb
    ldw     v1, [r4, v1*4]          ; row[*turb]
    ldw     v2, [r9, #4]            ; *col
    add     v1, v1, v2
    ldub    v1, [z, v1]
    stb     v1, [r6, #1]

    ldea    r6, [r6, vl]            ; dest++
    ldea    r11, [r11, vl*4]        ; turb++
    ldea    r9, [r9, vl*4]          ; col++

    bgt     r8, 2b
#else
#error "Support for non-vectorized operation not implemented yet"
#endif

    add     r10, r10, #4            ; turb++
    add     r4, r4, #4              ; rowptr++

    add     r2, r2, #-1
    bgt     r2, 1b

3:
    ret

    .size   D_WarpScreenKernel, .-D_WarpScreenKernel

#endif  /* __MRISC32__ */

