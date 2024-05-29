/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef MCUBOOT_LOGGING_H
#define MCUBOOT_LOGGING_H

#define MCUBOOT_LOG_LEVEL_OFF      0
#define MCUBOOT_LOG_LEVEL_ERROR    1
#define MCUBOOT_LOG_LEVEL_WARNING  2
#define MCUBOOT_LOG_LEVEL_INFO     3
#define MCUBOOT_LOG_LEVEL_DEBUG    4

#ifndef MCUBOOT_LOG_LEVEL
#define MCUBOOT_LOG_LEVEL MCUBOOT_LOG_LEVEL_DEBUG
#endif

#include <stdio.h>

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_ERROR
#define MCUBOOT_LOG_ERR(fmt, ...) \
     printf("ERR: " fmt "\n", ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_ERR(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_WARNING
#define MCUBOOT_LOG_WRN(fmt, ...) \
     printf("WRN: " fmt "\n", ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_WRN(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_INFO
#define MCUBOOT_LOG_INF(fmt, ...) \
     printf("INF: " fmt "\n", ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_INF(...) IGNORE(__VA_ARGS__)
#endif

#if MCUBOOT_LOG_LEVEL >= MCUBOOT_LOG_LEVEL_DEBUG
#define MCUBOOT_LOG_DBG(fmt, ...) \
     printf("DBG: " fmt "\n", ##__VA_ARGS__)
#else
#define MCUBOOT_LOG_DBG(...) IGNORE(__VA_ARGS__)
#endif

#define MCUBOOT_LOG_MODULE_DECLARE(...)
#define MCUBOOT_LOG_MODULE_REGISTER(...)

#endif /* MCUBOOT_LOGGING_H */
