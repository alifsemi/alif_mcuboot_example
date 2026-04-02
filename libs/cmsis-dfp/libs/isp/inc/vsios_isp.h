/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     isp_interface.h
 * @author   Shivakumar Malke
 * @email    shivakumar.malke@alifsemi.com
 * @version  V1.0.0
 * @date     07-Jun-2025
 * @brief    This header file configures which ISP modules are enabled/disabled
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef LIB_ISP_INC_VSIOS_ISP_H_
#define LIB_ISP_INC_VSIOS_ISP_H_

#include "vsios_type.h"
#include "vsios_error.h"

int vsios_isp_write_reg(vsi_u32_t dev_id, vsi_reg_t reg, vsi_u32_t val);
int vsios_isp_read_reg(vsi_u32_t dev_id, vsi_reg_t reg, vsi_u32_t *val);

#endif /* LIB_ISP_INC_VSIOS_ISP_H_ */
