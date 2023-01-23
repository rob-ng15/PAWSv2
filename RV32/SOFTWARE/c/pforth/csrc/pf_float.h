/* @(#) pf_float.h 98/01/28 1.1 */
#ifndef _pf_float_h
#define _pf_float_h

/***************************************************************
** Include file for PForth, a Forth based on 'C'
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
***************************************************************/

typedef float PF_FLOAT;

/* Define pForth specific math functions. */

#define fp_acos   acosf
#define fp_asin   asinf
#define fp_atan   atanf
#define fp_atan2  atan2f
#define fp_cos    cosf
#define fp_cosh   coshf
#define fp_fabs   fabsf
#define fp_floor  floorf
#define fp_log    logf
#define fp_log10  log10f
#define fp_pow    powf
#define fp_sin    sinf
#define fp_sinh   sinhf
#define fp_sqrt   sqrtf
#define fp_tan    tanf
#define fp_tanh   tanhf
#define fp_round  roundf

#endif
