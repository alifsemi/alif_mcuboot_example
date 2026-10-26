/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     : demo_wdt_freertos.c
 * @author   : Rohan Gautam
 * @version  : V1.1.0
 * @date     : 11-March-2026
 * @brief    : FreeRTOS demo to verify Watchdog Driver functionality
 ******************************************************************************/

/* Project Includes */
#include <stdio.h>
#include <inttypes.h>

#include "Driver_WDT.h"
#include "app_utils.h"

#include "RTE_Components.h"
#include CMSIS_device_header

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#if defined(RTE_CMSIS_Compiler_STDOUT)
#include "retarget_init.h"
#include "retarget_stdout.h"
#endif

/* Watchdog Driver Instance */
extern ARM_DRIVER_WDT Driver_WDT0;
static ARM_DRIVER_WDT *WDTdrv = &Driver_WDT0;

/* FreeRTOS Stack Configuration */
#define WDT_TASK_STACK_SIZE 256
#define TIMER_SERVICE_TASK_STACK_SIZE configTIMER_TASK_STACK_DEPTH
#define IDLE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

/* Static allocation buffers */

static StackType_t IdleStack[IDLE_TASK_STACK_SIZE];
static StaticTask_t IdleTcb;

static StackType_t TimerStack[TIMER_SERVICE_TASK_STACK_SIZE];
static StaticTask_t TimerTcb;

/***************************************************************
 *               FreeRTOS Hooks
 ***************************************************************/

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &IdleTcb;
    *ppxIdleTaskStackBuffer = IdleStack;
    *pulIdleTaskStackSize = IDLE_TASK_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &TimerTcb;
    *ppxTimerTaskStackBuffer = TimerStack;
    *pulTimerTaskStackSize = TIMER_SERVICE_TASK_STACK_SIZE;
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    ARG_UNUSED(pxTask);
    ARG_UNUSED(pcTaskName);

    ASSERT_HANG_LOOP
}

void vApplicationIdleHook(void)
{
    __NOP();
}

/***************************************************************
 *                 Watchdog Task
 ***************************************************************/

TaskHandle_t wdt_xHandle;

void watchdog_demo_entry(void *arg)
{
    (void)arg;

    uint32_t wdog_timeout_msec = 5000;
    uint32_t time_to_reset = 0;
    uint32_t iter = 3;
    int32_t ret;
    ARM_DRIVER_VERSION version;

    const TickType_t delay = pdMS_TO_TICKS(3000);

    printf("\n>>> Watchdog FreeRTOS demo starting <<<\n");

    version = WDTdrv->GetVersion();

    printf("Watchdog driver version API:%" PRIx16 " Driver:%" PRIx16 "\n",
           version.api, version.drv);

    /* Initialize watchdog */
    ret = WDTdrv->Initialize(wdog_timeout_msec);
    if (ret != ARM_DRIVER_OK) {
        printf("Error: watchdog init failed\n");
        vTaskDelete(NULL);
        return;
    }

    /* Power up watchdog */
    ret = WDTdrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf("Error: watchdog power failed\n");
        goto error_uninitialize;
    }

    /* Unlock watchdog registers */
    ret = WDTdrv->Control(ARM_WATCHDOG_UNLOCK, 0);
    if (ret != ARM_DRIVER_OK) {
        printf("Error: watchdog unlock failed\n");
        goto error_poweroff;
    }

    /* Start watchdog */
    ret = WDTdrv->Start();
    if (ret != ARM_DRIVER_OK) {
        printf("Error: watchdog start failed\n");
        goto error_stop;
    }

    while (iter--) {

        vTaskDelay(delay);

        ret = WDTdrv->GetRemainingTime(&time_to_reset);
        if (ret != ARM_DRIVER_OK) {
            printf("Error: remaining time read failed\n");
            goto error_stop;
        }

        printf("Remaining time: %" PRIu32 " ms\n", time_to_reset);

        printf("Feeding watchdog (%" PRIu32 ")\n", iter);

        ret = WDTdrv->Feed();
        if (ret != ARM_DRIVER_OK) {
            printf("Error: watchdog feed failed\n");
            goto error_stop;
        }
    }

    printf("Stop feeding watchdog. System will reset soon...\n");
    WAIT_FOREVER_LOOP

error_stop :
    /* First Unlock and then Stop watchdog peripheral. */
    ret = WDTdrv->Control(ARM_WATCHDOG_UNLOCK, 0);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: watchdog unlock failed\n");
    }

    /* Stop watchdog peripheral */
    ret = WDTdrv->Stop();
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: watchdog Stop failed.\r\n");
    }

error_poweroff:
    /* Power off watchdog peripheral */
    ret = WDTdrv->PowerControl(ARM_POWER_OFF);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: watchdog Power OFF failed.\r\n");
    }

error_uninitialize:
    /* Un-initialize watchdog driver */
    ret = WDTdrv->Uninitialize();
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: watchdog Uninitialize failed.\r\n");
    }

    printf("Watchdog demo exiting\n");

    vTaskDelete(NULL);
}

/***************************************************************
 *                    NMI Handler
 ***************************************************************/

void NMI_Handler(void)
{
    printf("\nNMI_Handler: Watchdog interrupt received\n");
    WAIT_FOREVER_LOOP
}

/***************************************************************
 *                         MAIN
 ***************************************************************/

int main(void)
{

#if defined(RTE_CMSIS_Compiler_STDOUT_Custom)

    extern int stdout_init(void);

    if (stdout_init() != ARM_DRIVER_OK) {
        WAIT_FOREVER_LOOP
    }

#endif

    /* System Initialization */
    SystemCoreClockUpdate();

    printf("\nSystem booting...\n");

    /* Create Watchdog Task */
    BaseType_t xReturned;

    xReturned = xTaskCreate(
                    watchdog_demo_entry,
                    "WDT_FreeRTOS",
                    WDT_TASK_STACK_SIZE,
                    NULL,
                    configMAX_PRIORITIES - 1,
                    &wdt_xHandle);

    if (xReturned != pdPASS) {
        printf("Task creation failed\n");
        WAIT_FOREVER_LOOP
    }

    /* Start scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    WAIT_FOREVER_LOOP
}

