/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef SYS_CTRL_ANALOG_H
#define SYS_CTRL_ANALOG_H

#include "soc.h"
#include "analog_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CMP_CTRL_CMP0_CLKEN (1U << 0U) /* Enable CMP0 clock */

#define VBAT_ANA_REG2_VAL   0x00C00000 /* Enable analog peripheral LDO and precision bandgap */

#define DAC6_REF_VAL        (DAC6_VREF_SCALE | DAC6_CONT | DAC6_EN)

#define DAC12_REF_VAL       (DAC12_VREF_CONT | ADC_VREF_BUF_EN | ADC_VREF_BUF_RDIV_EN)

#define ADC_REF_VAL         (ADC_VREF_BUF_RDIV_EN | ADC_VREF_BUF_EN | ADC_VREF_CONT)

#define CMP_REG2_BASE       (CMP0_BASE + 0x00000004) /* CMP register2 base address */

extern uint32_t analog_ldo_ref_cnt;
extern uint32_t adc_vref_cnt;
extern uint32_t dac6_vref_cnt;
extern uint32_t dac12_vref_cnt;
extern uint32_t cmp_clk_ref_cnt;

/**
 * @fn          void enable_analog_peripherals(void)
 * @brief       Enable LDO and precision bandgap for analog peripherals
 * @param[in]   none
 * @return      none
 */
static inline void enable_analog_peripherals(void)
{
    __disable_irq();
    /* Analog configuration Vbat register2 */
    ANA->VBAT_ANA_REG2 |= VBAT_ANA_REG2_VAL;

    analog_ldo_ref_cnt++;
    __enable_irq();
}

/**
 * @fn          void disable_analog_peripherals(void)
 * @brief       Disable LDO and precision bandgap for analog peripherals
 * @param[in]   none
 * @return      none
 */
static inline void disable_analog_peripherals(void)
{
    __disable_irq();
    if (analog_ldo_ref_cnt > 0) {
        analog_ldo_ref_cnt--;
        if (analog_ldo_ref_cnt == 0) {
            /* Analog configuration Vbat register2 */
            ANA->VBAT_ANA_REG2 &= ~VBAT_ANA_REG2_VAL;
        }
    }
    __enable_irq();
}

/**
 * @fn        void enable_dac6_ref_voltage(void)
 * @brief     Enable DAC6 as a negative input reference for HSCMP.
 *            This function sets the DAC6 configuration register to output
 *            a specific voltage (0.9V) that can be used as a negative
 *            input for the High-Speed Comparator (HSCMP).
 * @param[in] none
 * @return    none
 */
static inline void enable_dac6_ref_voltage(void)
{
    __disable_irq();
#if SOC_FEAT_HSCMP_REG_ALIASING
    ADC_VREF->ADC_VREF_REG |= ADC_VREF_BUF_EN;

    DAC6->DAC6_REG |= DAC6_REF_VAL;
#else
    *((volatile uint32_t *) CMP_REG2_BASE) |= (DAC6_REF_VAL | ADC_VREF_BUF_EN);
#endif

    dac6_vref_cnt++;
    __enable_irq();
}

/**
 * @fn        void disable_dac6_ref_voltage(void)
 * @brief     Disable DAC6 as a negative input reference for HSCMP.
 * @param[in] none
 * @return    none
 */
static inline void disable_dac6_ref_voltage(void)
{
    __disable_irq();
    if (dac6_vref_cnt > 0) {
        dac6_vref_cnt--;
        if (dac6_vref_cnt == 0) {
#if SOC_FEAT_HSCMP_REG_ALIASING
            ADC_VREF->ADC_VREF_REG &= ~ADC_VREF_BUF_EN;

            DAC6->DAC6_REG &= ~DAC6_REF_VAL;
#else
            *((volatile uint32_t *) CMP_REG2_BASE) &= ~(DAC6_REF_VAL | ADC_VREF_BUF_EN);
#endif
        }
    }
    __enable_irq();
}

/**
 * @fn        void enable_dac12_ref_voltage(void)
 * @brief     Enables DAC12 voltage reference and internal buffer
 *            for DAC operation.
 * @param[in] none
 * @return    none
 */
static inline void enable_dac12_ref_voltage(void)
{
    __disable_irq();
#if SOC_FEAT_DAC_REG_ALIASING
    ADC_VREF->ADC_VREF_REG |= (ADC_VREF_BUF_EN | ADC_VREF_BUF_RDIV_EN);

    *((volatile uint32_t *) CMP_REG2_BASE) |= DAC12_VREF_CONT;
#else
    *((volatile uint32_t *) CMP_REG2_BASE) |= DAC12_REF_VAL;
#endif

    dac12_vref_cnt++;
    __enable_irq();
}

/**
 * @fn        void disable_dac12_ref_voltage(void)
 * @brief     Disables DAC12 voltage reference and internal buffer
 *            for DAC operation.
 * @param[in] none
 * @return    none
 */
static inline void disable_dac12_ref_voltage(void)
{
    __disable_irq();

    if (dac12_vref_cnt > 0) {
        dac12_vref_cnt--;
        if (dac12_vref_cnt == 0) {
#if SOC_FEAT_DAC_REG_ALIASING
            ADC_VREF->ADC_VREF_REG &= ~(ADC_VREF_BUF_EN | ADC_VREF_BUF_RDIV_EN);

            *((volatile uint32_t *) CMP_REG2_BASE) &= ~DAC12_VREF_CONT;
#else
            *((volatile uint32_t *) CMP_REG2_BASE) &= ~DAC12_REF_VAL;
#endif
        }
    }
    __enable_irq();
}

/**
 * @fn        void enable_adc_ref_voltage(void)
 * @brief     Configures the analog control register to enable the reference
 *            divider, turn on the VREF buffer, and set the appropriate control
 *            level for the ADC reference voltage.
 * @param[in] none
 * @return    none
 */
static inline void enable_adc_ref_voltage(void)
{
    __disable_irq();
#if SOC_FEAT_ADC_REG_ALIASING
    ADC_VREF->ADC_VREF_REG |= ADC_REF_VAL;
#else
    *((volatile uint32_t *) CMP_REG2_BASE) |= ADC_REF_VAL;
#endif

    adc_vref_cnt++;
    __enable_irq();
}

/**
 * @fn        void disable_adc_ref_voltage(void)
 * @brief     Disable the reference divider, VREF buffer,
 *            and ADC reference voltage.
 * @param[in] none
 * @return    none
 */
static inline void disable_adc_ref_voltage(void)
{
    __disable_irq();
    if (adc_vref_cnt > 0) {
        adc_vref_cnt--;
        if (adc_vref_cnt == 0) {
#if SOC_FEAT_ADC_REG_ALIASING
            ADC_VREF->ADC_VREF_REG &= ~ADC_REF_VAL;
#else
            *((volatile uint32_t *) CMP_REG2_BASE) &= ~ADC_REF_VAL;
#endif
        }
    }
    __enable_irq();
}

/**
 * @fn        static inline void enable_analog_periph_clk(void)
 * @brief     Enable Analog peripheral clock.
 * @param[in] none
 * @return    none.
 */
static inline void enable_analog_periph_clk(void)
{
    __disable_irq();
    CLKCTL_PER_SLV->CMP_CTRL |= CMP_CTRL_CMP0_CLKEN;

    cmp_clk_ref_cnt++;
    __enable_irq();
}

/**
 * @fn        static inline void disable_analog_periph_clk(void)
 * @brief     Disable Analog peripheral clock.
 * @param[in] none
 * @return    none.
 */
static inline void disable_analog_periph_clk(void)
{
    __disable_irq();
    if (cmp_clk_ref_cnt > 0) {
        cmp_clk_ref_cnt--;
        if (cmp_clk_ref_cnt == 0) {
            CLKCTL_PER_SLV->CMP_CTRL &= ~CMP_CTRL_CMP0_CLKEN;
        }
    }
    __enable_irq();
}

#ifdef __cplusplus
}
#endif

#endif /* SYS_CTRL_ANALOG_H */
