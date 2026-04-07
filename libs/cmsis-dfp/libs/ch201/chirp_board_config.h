/*
 * chirp_board_config.h
 *
 * This file defines default values for the required symbols used to build an
 * application with the Chirp SonicLib API and driver. These symbols are used
 * for static array allocations and counters in SonicLib (and often
 * applications), and are based on the number of specific resources on the
 * target board.
 *
 * This file is derived from invn/soniclib/details/chirp_board_config.h and
 * includes board-specific changes to the macro definitions.
 */

#ifndef CHIRP_BOARD_CONFIG_H_
#define CHIRP_BOARD_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "RTE_Device.h"
#include "board_defs.h"

#define INCLUDE_WHITNEY_SUPPORT 1

/* Settings for the Chirp driver test setup */
#define CHIRP_MAX_NUM_SENSORS   BOARD_CH201_MAX_NUM_SENSORS

#define CHIRP_NUM_BUSES         BOARD_CH201_NUM_BUSES

#define CHIRP_RTC_CAL_PULSE_MS  RTE_CH201_RTC_CAL_PULSE_MS

#define CHIRP_SENSOR_INT_PIN    BOARD_CH201_SENSOR_INT_PIN
#define CHIRP_SENSOR_TRIG_PIN   BOARD_CH201_SENSOR_TRIG_PIN

#define CHIRP_I2C_SPEED_HZ      RTE_CH201_I2C_SPEED_HZ

#ifdef __cplusplus
}
#endif

#endif /* CHIRP_BOARD_CONFIG_H_ */
