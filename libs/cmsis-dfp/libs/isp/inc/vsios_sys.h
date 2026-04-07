/****************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2024 Vivante Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/

#ifndef __VSIOS_SYS_H__
#define __VSIOS_SYS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "vsios_type.h"
#include <stdio.h>

void *vsios_malloc(vsi_u32_t size);
void  vsios_free(void *ptr);
void *vsios_memset(void *s, int c, vsi_u32_t n);
void *vsios_memcpy(void *dest, const void *src, vsi_u32_t n);
int   vsios_memcmp(const void *s1, const void *s2, size_t n);
int   vsios_snprintf(char *str, vsi_u32_t size, const char *format, ...);
void  vsios_usleep(vsi_u32_t usec);

#define vsios_max(type, x, y)            ((type) (x)) > ((type) (y)) ? ((type) (x)) : ((type) (y))
#define vsios_min(type, x, y)            ((type) (x)) < ((type) (y)) ? ((type) (x)) : ((type) (y))
#define vsios_clamp(type, val, min, max) vsios_min(type, vsios_max(type, val, min), max)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
