#include "bsp/board_api.h"
#include <stdbool.h>

#if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_FREERTOS
#include "board_config.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "Driver_IO.h"
#include "uart_tracelib.h"

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_LEDRGB1_R_GPIO_PORT);
static ARM_DRIVER_GPIO* led_port = &ARM_Driver_GPIO_(BOARD_LEDRGB1_R_GPIO_PORT);

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_JOY_SW_CENTER_GPIO_PORT);
static ARM_DRIVER_GPIO* button_port = &ARM_Driver_GPIO_(BOARD_JOY_SW_CENTER_GPIO_PORT);

#endif

#if CFG_TUSB_OS == OPT_OS_ZEPHYR
#include <zephyr/devicetree.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
#endif

/**
 * @brief Board init: configure LED and button pins
 */
void board_init(void) {
#if CFG_TUSB_OS == OPT_OS_NONE
    // Configure Systick for each millisec
    SysTick_Config(SystemCoreClock / 1000);
#elif CFG_TUSB_OS == OPT_OS_FREERTOS
    // Initialize System Core Clock
    SystemCoreClockUpdate();
#endif

#if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_FREERTOS
    // Initialize pinmuxes
    int32_t board_init_ret = board_pins_config();
    if(board_init_ret) {
        __BKPT(0);
    }

    board_init_ret = board_gpios_config();
    if(board_init_ret) {
        __BKPT(0);
    }

    // Initialize LED
    led_port->Initialize(BOARD_LEDRGB1_R_GPIO_PIN, NULL);
    led_port->PowerControl(BOARD_LEDRGB1_R_GPIO_PIN, ARM_POWER_FULL);
    led_port->SetDirection(BOARD_LEDRGB1_R_GPIO_PIN, GPIO_PIN_DIRECTION_OUTPUT);

    // Initialize button
    button_port->Initialize(BOARD_JOY_SW_CENTER_GPIO_PIN, NULL);
    button_port->PowerControl(BOARD_JOY_SW_CENTER_GPIO_PIN, ARM_POWER_FULL);
    button_port->SetDirection(BOARD_JOY_SW_CENTER_GPIO_PIN, GPIO_PIN_DIRECTION_INPUT);
   
    // Initialize serial output
    tracelib_init(NULL, NULL);
    
#endif

#if CFG_TUSB_OS == OPT_OS_ZEPHYR
    if (device_is_ready(led.port)) {
        gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    }
    
    if (gpio_is_ready_dt(&button)) {
        gpio_pin_configure_dt(&button, GPIO_INPUT);
    }
#endif
}

/**
 * @brief Control board LED
 */
void board_led_write(bool state) {
#if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_FREERTOS
    led_port->SetValue(BOARD_LEDRGB1_R_GPIO_PIN, state ?
                    GPIO_PIN_OUTPUT_STATE_HIGH :
                    GPIO_PIN_OUTPUT_STATE_LOW);
#endif
#if CFG_TUSB_OS == OPT_OS_ZEPHYR
    if (device_is_ready(led.port)) {
        gpio_pin_set(led.port, led.pin, state ? 1 : 0);
    }
#endif  
}

/**
 * @brief Read button state (returns 1 if pressed, 0 otherwise)
 */
uint32_t board_button_read(void) {
#if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_FREERTOS
    uint32_t button_state;
    button_port->GetValue(BOARD_JOY_SW_CENTER_GPIO_PIN, &button_state);
    return GPIO_PIN_STATE_LOW == button_state;
#endif
#if CFG_TUSB_OS == OPT_OS_ZEPHYR
    if (!device_is_ready(button.port)) {
        return 0;
    }

    int val = gpio_pin_get_dt(&button);

    return val == 0;    // Pin pulled low when pressed
#endif  
}

#if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_FREERTOS

/**
 * @brief UART read handler
 */
int board_uart_read(uint8_t* buf, int len) {
    // NOTE: stdin functionality has not been implemented
    (void) buf, (void) len;
    return 0;
}

/**
 * @brief UART write handler
 */
int board_uart_write(void const* buf, int len) {
    int ret = send_str((const char *) buf, len);
    return (ret == ARM_DRIVER_OK) ? len : 0;
}

// Stubs to suppress missing stdio definitions
int _close(int fh);
int _lseek(int fh, long pos, int whence);
struct stat;
int _fstat(int f, struct stat *buf);
int _isatty(int fh);
int _getpid(void);
int _kill(int pid, int sig);

int _close(int fh) {
    (void) fh;
    return -1;
}

int _lseek(int fh, long pos, int whence) {
    (void) fh, (void) pos, (void) whence;
    return -1;
}

struct stat;
int _fstat(int f, struct stat *buf) {
    (void) f, (void) buf;
    return -1;
}

int _isatty(int fh) {
    (void) fh;
    return 0;
}

int _getpid(void) {
    return 1;
}

int _kill(int pid, int sig) {
    (void) sig;

    if (pid == 1) {
        __BKPT(0);
        while(1) {
            __WFE();
        }
    }

    return -1;
}

#endif

#if CFG_TUSB_OS == OPT_OS_NONE
volatile uint32_t system_ticks = 0;

void SysTick_Handler(void);
void SysTick_Handler(void) {
  system_ticks++;
}

/**
 * @brief Returns the current time in milliseconds since boot
 */
uint32_t board_millis(void) {
  return system_ticks;
}
#endif


#if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_FREERTOS
void USB_IRQHandler(void);
void USB_IRQHandler(void) {
    dcd_int_handler(0);
}
#endif

#if CFG_TUSB_OS == OPT_OS_ZEPHYR
void USBD_IRQHandler(void) {
    tud_int_handler(0);
}
#endif
