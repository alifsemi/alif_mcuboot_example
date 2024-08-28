#include "os_mgmt/os_mgmt_impl.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "Driver_LPTIMER.h"
#include <stdio.h>
#include <inttypes.h>


extern ARM_DRIVER_LPTIMER DRIVER_LPTIMER0;
static ARM_DRIVER_LPTIMER* timer = &DRIVER_LPTIMER0;

#define LPTIMER_CLOCK_FREQUENCY 32768

static void lptimer_event_callback(uint8_t event)
{
    (void)event;
    timer->Stop(0);
    NVIC_SystemReset();
}

int os_mgmt_impl_reset(unsigned int delay_ms)
{
    // calculate counter value to achieve desired delay
    uint64_t ticks64 = ((uint64_t)delay_ms) * LPTIMER_CLOCK_FREQUENCY / 1000;
    if(ticks64 > UINT32_MAX) {
        ticks64 = UINT32_MAX;
    }
    uint32_t ticks = (uint32_t)ticks64;

    uint32_t ret = timer->Control(0, ARM_LPTIMER_SET_COUNT1, &ticks);
    if(ret) {
        printf("LPTIMER Control: %" PRId32 "\n", ret);
        return ret;
    }

    ret = timer->Start(0);
    if(ret) {
        printf("LPTIMER Start: %" PRId32 "\n", ret);
        return ret;
    }

    return 0;
}

int32_t os_mgmt_impl_init()
{
    int32_t ret = timer->Initialize(0, lptimer_event_callback);
    if(ret) {
        printf("LPTIMER initialize: %" PRId32 "\n", ret);
        return ret;
    }

    ret = timer->PowerControl(0, ARM_POWER_FULL);
    if(ret) {
        printf("LPTIMER PowerControl: %" PRId32 "\n", ret);
        return ret;
    }

    return 0;
}
