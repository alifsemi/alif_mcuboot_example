/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(USE_SEMIHOSTING)

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "uart_tracelib.h"
#include "fault_handler.h"

#define UNUSED(x) (void)(x)

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
/* Arm compiler re-targeting */

#include <rt_misc.h>
#include <rt_sys.h>


/* Standard IO device handles. */
#define STDIN  0x8001
#define STDOUT 0x8002
#define STDERR 0x8003

#define RETARGET(fun) _sys##fun

#if __ARMCLIB_VERSION >= 6190004
#define TMPNAM_FUNCTION RETARGET(_tmpnam2)
#else
#define TMPNAM_FUNCTION RETARGET(_tmpnam)
#endif
#define ISTTY_FUNCTION RETARGET(_istty)

#elif __ICCARM__
/* IAR compiler re-targeting */
typedef int FILEHANDLE;

/* Standard IO device handles. */
#define STDIN  0x00
#define STDOUT 0x01
#define STDERR 0x02

#define RETARGET(fun) _##fun

#else
/* GNU compiler re-targeting */

#include <sys/stat.h>


/*
 * This type is used by the _ I/O functions to denote an open
 * file.
 */
typedef int FILEHANDLE;

/*
 * Open a file. May return -1 if the file failed to open.
 */
extern FILEHANDLE _open(const char * /*name*/, int /*openmode*/);

/* Standard IO device handles. */
#define STDIN  0x00
#define STDOUT 0x01
#define STDERR 0x02

#define RETARGET(fun) fun

#define TMPNAM_FUNCTION RETARGET(_tmpnam)
#define ISTTY_FUNCTION RETARGET(_isatty)

#endif /* GNU compiler re-targeting */

/* Standard IO device name defines. */
const char __stdin_name[] __attribute__((aligned(4)))  = "STDIN";
const char __stdout_name[] __attribute__((aligned(4))) = "STDOUT";
const char __stderr_name[] __attribute__((aligned(4))) = "STDERR";

#define RETARGET_BUF_MAX 128
static char retarget_buf[RETARGET_BUF_MAX];
static uint32_t retarget_buf_len = 0;

#ifndef A32
static _Atomic clock_t clock_ticks;
#endif

void flush_uart()
{
    if (retarget_buf_len != 0)
    {
        send_str(retarget_buf, retarget_buf_len);
        retarget_buf_len = 0;
    }
}

__STATIC_FORCEINLINE uint32_t in_interrupt(void)
{
#ifdef A32
    return (__get_mode() == CPSR_M_IRQ || __get_mode() == CPSR_M_FIQ);
#else
    return __get_IPSR() != 0U;
#endif
}

#if __ICCARM__
/*
    Put IAR
    __open()
    __close()
    __lseek()
    here if needed.

    __write()
    __read()
    are implemented together with GCC and ARMCC
*/
#else
FILEHANDLE RETARGET(_open)(const char *name, int openmode)
{
    UNUSED(openmode);

    if (strcmp(name, __stdin_name) == 0) {
        return (STDIN);
    }

    if (strcmp(name, __stdout_name) == 0) {
        return (STDOUT);
    }

    if (strcmp(name, __stderr_name) == 0) {
        return (STDERR);
    }

    return -1;
}

int ISTTY_FUNCTION(FILEHANDLE fh)
{
    switch (fh) {
    case STDIN:
    case STDOUT:
    case STDERR:
        return 1;
    default:
        return 0;
    }
}

int RETARGET(_close)(FILEHANDLE fh)
{
    if (ISTTY_FUNCTION(fh)) {
        return 0;
    }

    return -1;
}

#if defined(__ARMCC_VERSION)
int RETARGET(_seek)(FILEHANDLE fh, long pos)
#else
long RETARGET(_lseek)(FILEHANDLE fh, long pos, int whence)
#endif
{
    UNUSED(fh);
    UNUSED(pos);
#if !defined(__ARMCC_VERSION)
    UNUSED(whence);
#endif

    return -1;
}

int TMPNAM_FUNCTION(char* name, int sig, unsigned int maxlen)
{
    UNUSED(name);
    UNUSED(sig);
    UNUSED(maxlen);

    return 1;
}
#endif // !__ICCARM__

/* read() function for all three compiler variants */
/* IAR has slightly different prototype */
#if __ICCARM__
size_t RETARGET(_read)(FILEHANDLE fh, unsigned char* buf, size_t len)
{
#else
int RETARGET(_read)(FILEHANDLE fh, unsigned char *buf, unsigned int len, int mode)
{
    UNUSED(mode);
#endif

    switch (fh) {
    case STDIN: {
        int c;
        unsigned int read = 0;
        bool eof = false;

        while (read < len) {

            /* NOTE: stdin functionality has not been implemented
                     If stdin is needed: read character from UART here */
            c = -1;

            if (c == EOF) {
                eof = true;
                break;
            }

            *buf++ = (unsigned char)c;
            read++;
        }

#ifdef __ARMCC_VERSION
        /* Return number of bytes not read, combined with an EOF flag */
        return (eof ? (int) 0x80000000 : 0) | (len - read);
#else
        /* Return number of bytes read (GCC and IAR build) */
        if (read > 0) {
            return read;
        } else {
            return eof ? -1 : 0;
        }
#endif
    }
    default:
        return -1;
    }
}

/* write() function for all three compiler variants */
/* IAR has slightly different prototype */
#if __ICCARM__
size_t RETARGET(_write)(FILEHANDLE fh, const unsigned char* buf, size_t len)
{
#else
int RETARGET(_write)(FILEHANDLE fh, const unsigned char *buf, unsigned int len, int mode)
{
    UNUSED(mode);
#endif

    switch (fh) {
    case STDOUT:
    case STDERR: {
        if(in_interrupt() && !in_fault_handler())
        {
           // this is ISR context so don't push to UART
           if(retarget_buf_len < RETARGET_BUF_MAX)
           {
               // if we're full just drop
               if (len > RETARGET_BUF_MAX - retarget_buf_len) {
                   len = RETARGET_BUF_MAX - retarget_buf_len;
               }
               memcpy(retarget_buf + retarget_buf_len, buf, len);
               retarget_buf_len += len;
           }
        }
        else
        {
            flush_uart();
            int ret;
            do
            {
                ret = send_str((const char *) buf, len);
                // If fault handler is hit while printing was ongoing,
                // we need to wait here until it's done before we
                // go on printing fault handler stuff to not lose
                // most of the fault handler prints.
            } while (ret != ARM_DRIVER_OK && in_fault_handler());
        }

#ifdef __ARMCC_VERSION
        // armcc expects to get the amount of characters that were not written
        return 0;
#else
        // GCC AND IAR builds expect to get the amount of characters written
        return len;
#endif
    }
    default:
        return -1;
    }
}

void RETARGET(_exit)(int return_code)
{
    UNUSED(return_code);

    putchar('\n');

    __BKPT(0);
    while(1) {
        __WFE();
    }
}

int system(const char *cmd)
{
    UNUSED(cmd);

    return 0;
}

time_t time(time_t *timer)
{
    time_t current;

    current = 0; // To Do !! No RTC implemented

    if (timer != NULL) {
        *timer = current;
    }

    return current;
}

void _clock_init(void) {}

// We don't want automatically init systick but call it manually if needed.
#ifdef A32

// CMSIS version 6.0.0 introduces these
#if __CM_CMSIS_VERSION_MAIN < 6
__STATIC_FORCEINLINE uint32_t __get_CNTFRQ(void)
{
  uint32_t result;
  __get_CP(15, 0, result, 14, 0, 0);
  return result;
}

__STATIC_FORCEINLINE uint64_t __get_CNTPCT(void)
{
  uint64_t result;
  __get_CP64(15, 1, result, 14);
  return result;
}
#endif

static uint64_t clock_epoch_start;

void clk_init()
{
    // We assume the counter is started at system init
    clock_epoch_start = __get_CNTPCT();
}

void clk_uninit()
{
}

clock_t clock(void)
{
    return (__get_CNTPCT() - clock_epoch_start) / (__get_CNTFRQ() / CLOCKS_PER_SEC);
}
#else
void clk_init()
{
    SysTick_Config(SystemCoreClock/CLOCKS_PER_SEC);
}

#define SysTick_CTRL_DISABLE_Msk            (0UL /*<< SysTick_CTRL_ENABLE_Pos*/)

void clk_uninit()
{
    SysTick->CTRL &= ~(1UL << SysTick_CTRL_ENABLE_Pos); // Disable SysTick IRQ and SysTick Timer
}

clock_t clock(void)
{
    return clock_ticks;
}

void SysTick_Handler(void)
{
    clock_ticks++;
}
#endif

int remove(const char *arg) {
    UNUSED(arg);

    return 0;
}

int rename(const char *oldn, const char *newn)
{
    UNUSED(oldn);
    UNUSED(newn);

    return 0;
}

#if defined(__clang__) && !defined(__ARMCC_VERSION)

// Picolibc retarget
// https://github.com/picolibc/picolibc/blob/main/doc/os.md#system-interfaces-used-by-picolibc

static int clang_putc(char c, FILE* file)
{
    (void)file;
    _write(STDOUT, (const unsigned char*)&c, 1, 0);
    return c;
}

static FILE __stdio = FDEV_SETUP_STREAM(clang_putc, 0, 0, _FDEV_SETUP_WRITE);
FILE *const stdin = &__stdio; __strong_reference(stdin, stdout); __strong_reference(stdin, stderr);

#endif

#ifdef __ARMCC_VERSION
/* ARMCC specific functions */
void _ttywrch(int ch) {
    (void)fputc(ch, stdout);
}

long RETARGET(_flen)(FILEHANDLE fh)
{
    if (ISTTY_FUNCTION(fh)) {
        return 0;
    }

    return -1;
}

char *RETARGET(_command_string)(char *cmd, int len)
{
    UNUSED(len);

    return cmd;
}
#elif __ICCARM__
/* IAR specific functions */

#else
/* More newlib functions (GCC build) */
struct stat;
int _fstat(int f, struct stat *buf)
{
    UNUSED(f);
    UNUSED(buf);

    return -1;
}

int _getpid()
{
    return 1;
}

int _kill(int pid, int sig)
{
    UNUSED(sig);

    if (pid == 1) {
        RETARGET(_exit(1));
    }

    return -1;
}

#ifndef ferror
/* arm-none-eabi-gcc with newlib uses a define for ferror */
int ferror(FILE *f)
{
    UNUSED(f);

    return EOF;
}
#endif /* #ifndef ferror */

#endif // GCC build

#endif // USE_SEMIHOSTING
