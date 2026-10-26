/*
 * vsios_isp.h
 *
 *  Created on: 05-Sep-2025
 *      Author: yogender
 */

#ifndef LIB_ISP_INC_VSIOS_ISP_H_
#define LIB_ISP_INC_VSIOS_ISP_H_

#include "vsios_type.h"
#include "vsios_error.h"

int vsios_isp_write_reg(vsi_u32_t dev_id, vsi_reg_t reg, vsi_u32_t val);
int vsios_isp_read_reg(vsi_u32_t dev_id, vsi_reg_t reg, vsi_u32_t *val);

#endif /* LIB_ISP_INC_VSIOS_ISP_H_ */
