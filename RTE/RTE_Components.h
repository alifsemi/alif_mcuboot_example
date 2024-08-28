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
#if defined(M55_HP)
    #define CMSIS_device_header "M55_HP.h"
#elif defined(M55_HE)
    #define CMSIS_device_header "M55_HE.h"
#elif defined(A32)
    #define CMSIS_device_header "a32_device.h"
    #include "system_utils.h"
#else
    #error "Undefined M55 CPU!"
#endif

#define RTE_Drivers_ISSI_FLASH	1
#define RTE_Drivers_GPIO        1
#define RTE_Drivers_PINCONF     1
#define RTE_Drivers_LPTIMER     1
//#define RTE_Drivers_OSPI     1           /* Driver OSPI */

#endif /* RTE_COMPONENTS_H */
