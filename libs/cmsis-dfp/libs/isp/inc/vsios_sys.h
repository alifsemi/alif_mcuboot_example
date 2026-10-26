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
extern "C"{
#endif
#endif

#include "vsios_type.h"
#include <stdio.h>

//#include <pthread.h>
//#include <semaphore.h>

//typedef struct {
//    pthread_mutex_t mutex;
//} vsios_mutex_t;
//
//typedef struct {
//    sem_t sem;
//} vsios_sem_t;
//
//typedef struct {
//    char name[64];
//    int stacksize;
//    int policy;
//    pthread_attr_t pthread_attr;
//} vsios_thread_attr_t;
//
//typedef struct {
//    pthread_t thread;
//} vsios_thread_t;

//typedef void* vsios_thread_exit_t;

//#define vsios_thread_exit(retval) pthread_exit(retval)

void *vsios_malloc(vsi_u32_t size);
void  vsios_free(void *ptr);
void *vsios_memset(void *s, int c, vsi_u32_t n);
void *vsios_memcpy(void *dest, const void *src, vsi_u32_t n);
int vsios_memcmp(const void *s1, const void *s2, size_t n);
int   vsios_snprintf(char *str, vsi_u32_t size, const char *format, ...);
void  vsios_usleep(vsi_u32_t usec);

//int vsios_mutex_create(vsios_mutex_t *os_mutex);
//int vsios_mutex_destroy(vsios_mutex_t *os_mutex);
//int vsios_mutex_lock(vsios_mutex_t *os_mutex);
//int vsios_mutex_unlock(vsios_mutex_t *os_mutex);

//int vsios_sem_init(vsios_sem_t *os_sem, vsi_u32_t value);
//int vsios_sem_post(vsios_sem_t *os_sem);
//int vsios_sem_wait(vsios_sem_t *os_sem, vsi_u32_t ms);
//int vsios_sem_destroy(vsios_sem_t *os_sem);

//int vsios_thread_attr_init(vsios_thread_attr_t *os_attr);
//int vsios_thread_attr_destroy(vsios_thread_attr_t *os_attr);
//int vsios_thread_attr_setstacksize(vsios_thread_attr_t *os_attr, vsi_u32_t stacksize);
//int vsios_thread_attr_setpriority(vsios_thread_attr_t *os_attr, int priority);

//int vsios_thread_create(vsios_thread_t *os_thread,
//                        const vsios_thread_attr_t *os_attr,
//                        vsios_thread_exit_t (*start_routine) (void *),
//                        void *arg);
//int vsios_thread_destroy(vsios_thread_t *os_thread);

#define vsios_max(type, x, y) ((type)(x)) > ((type)(y)) ? ((type)(x)) : ((type)(y))
#define vsios_min(type, x, y) ((type)(x)) < ((type)(y)) ? ((type)(x)) : ((type)(y))
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
