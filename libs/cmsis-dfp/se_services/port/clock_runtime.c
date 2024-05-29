/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     clock_runtime.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     29-Nov-2023
 * @brief    Wrapper to update the system clocks Information
 ******************************************************************************/
#include "clock_runtime.h"
#include "services_lib_api.h"
#include "se_services_port.h"
#include <RTE_Components.h>
#include CMSIS_device_header

/**
  \fn          int32_t system_update_clock_values(void)
  \brief       Update system clock values retrieved from SE services
  \return      0 for SUCCESS
*/
int32_t system_update_clock_values(void)
{
    uint32_t        service_error_code;
    uint32_t        error_code = SERVICES_REQ_SUCCESS;
    run_profile_t   runp = {0};
    uint32_t        frequency = 0;

    /* Get the current run configuration from SE */
    error_code = SERVICES_get_run_cfg(se_services_s_handle,
                                      &runp,
                                      &service_error_code);
    if(error_code)
    {
        return -1;
    }
    SystemCoreClock = runp.cpu_clk_freq;

    error_code = SERVICES_clocks_get_apb_frequency(se_services_s_handle,
                                                   &frequency,
                                                   &service_error_code);
    if(error_code)
    {
        return -1;
    }
    SystemAPBClock = frequency;
    SystemAHBClock = frequency * 2;
    SystemAXIClock = frequency * 4;

    error_code = SERVICES_clocks_get_refclk_frequency(se_services_s_handle,
                                                      &frequency,
                                                      &service_error_code);
    if(error_code)
    {
        return -1;
    }
    SystemREFClock = frequency;

    return 0;
}
