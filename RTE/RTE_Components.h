/*
 * Auto generated Run-Time-Environment Component Configuration File
 *      *** Do not modify ! ***
 *
 */
#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

/*
 * Define the Device Header File:
*/
#if defined(M55_HP) || defined(M55_HE)
    #define CMSIS_device_header "alif.h"
#elif defined(A32)
    #define CMSIS_device_header "a32_device.h"
    #include "sys_utils.h"
#else
    #error "Undefined M55 CPU!"
#endif

#define RTE_Drivers_ISSI_FLASH
#define RTE_Drivers_GPIO
#define RTE_Drivers_IO
#define RTE_Drivers_PINCONF
#define RTE_Drivers_LPTIMER
#define RTE_Drivers_USART
#define RTE_Drivers_CRC
//#define RTE_Drivers_OSPI           /* Driver OSPI */

#endif /* RTE_COMPONENTS_H */
