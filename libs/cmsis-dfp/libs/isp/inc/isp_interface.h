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
#ifndef ISP_INTERFACE_H_
#define ISP_INTERFACE_H_

#define RTE_ISP_BUFFER_COUNT    1

// <o> ISP Enable AE Module
//     <0=> disable
//     <1=> enable
// <i> defines if AE Module is enabled or not
// <i> default: false
#define RTE_ISP_AE_MODULE       0

// <o> ISP Enable BLS Module
//     <0=> disable
//     <1=> enable
// <i> defines if Black Level Subtraction Module is enabled or not
// <i> default: false
#define RTE_ISP_BLS_MODULE      0

// <o> ISP Enable DMSC Module
//     <0=> disable
//     <1=> enable
// <i> defines if Demosaic Module is enabled or not
// <i> default: false
#define RTE_ISP_DMSC_MODULE     1

// <o> ISP Enable FLT Module
//     <0=> disable
//     <1=> enable
// <i> defines if Noise/Sharpening-Filter Module is enabled or not
// <i> default: false
#define RTE_ISP_FLT_MODULE      0

// <o> ISP Enable CCM Module
//     <0=> disable
//     <1=> enable
// <i> defines if Color Correction Matrix Module is enabled or not
// <i> default: false
#define RTE_ISP_CCM_MODULE      1

// <o> ISP Enable CSM Module
//     <0=> disable
//     <1=> enable
// <i> defines if Color Space Conversion Module is enabled or not
// <i> default: false
#define RTE_ISP_CSM_MODULE      0

// <o> ISP Enable WB Module
//     <0=> disable
//     <1=> enable
// <i> defines if White Balancing Module is enabled or not
// <i> default: false
#define RTE_ISP_WB_MODULE       1

// <o> ISP Enable EXPM Module
//     <0=> disable
//     <1=> enable
// <i> defines if Auto-Exposure Statistics Module is enabled or not
// <i> default: false
#define RTE_ISP_EXPM_MODULE     0

// <o> ISP Enable Gamma-out Module
//     <0=> disable
//     <1=> enable
// <i> defines if Gamma-out Module is enabled or not
// <i> default: false
#define RTE_ISP_GAMMAOUT_MODULE 0

// <o> ISP Enable WBM Module
//     <0=> disable
//     <1=> enable
// <i> defines if White-Balancing Statistics Module is enabled or not
// <i> default: false
#define RTE_ISP_WBM_MODULE      1

// <o> ISP Enable Binning Module
//     <0=> disable
//     <1=> enable
// <i> defines if Binning Module is enabled or not
// <i> default: false
#define RTE_ISP_BINNING_MODULE  0

// <o> ISP Enable Scaling Module
//     <0=> disable
//     <1=> enable
// <i> defines if scaling Module is enabled or not
// <i> default: true
#define RTE_ISP_SCALAR_MODULE   1

#endif /* ISP_INTERFACE_H_ */
