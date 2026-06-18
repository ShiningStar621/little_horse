/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_1 */
#define PWM_1_INST                                                         TIMA1
#define PWM_1_INST_IRQHandler                                   TIMA1_IRQHandler
#define PWM_1_INST_INT_IRQN                                     (TIMA1_INT_IRQn)
#define PWM_1_INST_CLK_FREQ                                               500000
/* GPIO defines for channel 0 */
#define GPIO_PWM_1_C0_PORT                                                 GPIOA
#define GPIO_PWM_1_C0_PIN                                         DL_GPIO_PIN_15
#define GPIO_PWM_1_C0_IOMUX                                      (IOMUX_PINCM37)
#define GPIO_PWM_1_C0_IOMUX_FUNC                     IOMUX_PINCM37_PF_TIMA1_CCP0
#define GPIO_PWM_1_C0_IDX                                    DL_TIMER_CC_0_INDEX

/* Defines for PWM_2 */
#define PWM_2_INST                                                         TIMA0
#define PWM_2_INST_IRQHandler                                   TIMA0_IRQHandler
#define PWM_2_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define PWM_2_INST_CLK_FREQ                                               500000
/* GPIO defines for channel 0 */
#define GPIO_PWM_2_C0_PORT                                                 GPIOA
#define GPIO_PWM_2_C0_PIN                                          DL_GPIO_PIN_0
#define GPIO_PWM_2_C0_IOMUX                                       (IOMUX_PINCM1)
#define GPIO_PWM_2_C0_IOMUX_FUNC                      IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_PWM_2_C0_IDX                                    DL_TIMER_CC_0_INDEX

/* Defines for PWM_3 */
#define PWM_3_INST                                                        TIMG12
#define PWM_3_INST_IRQHandler                                  TIMG12_IRQHandler
#define PWM_3_INST_INT_IRQN                                    (TIMG12_INT_IRQn)
#define PWM_3_INST_CLK_FREQ                                               500000
/* GPIO defines for channel 0 */
#define GPIO_PWM_3_C0_PORT                                                 GPIOB
#define GPIO_PWM_3_C0_PIN                                         DL_GPIO_PIN_20
#define GPIO_PWM_3_C0_IOMUX                                      (IOMUX_PINCM48)
#define GPIO_PWM_3_C0_IOMUX_FUNC                    IOMUX_PINCM48_PF_TIMG12_CCP0
#define GPIO_PWM_3_C0_IDX                                    DL_TIMER_CC_0_INDEX

/* Defines for PWM_4 */
#define PWM_4_INST                                                         TIMG0
#define PWM_4_INST_IRQHandler                                   TIMG0_IRQHandler
#define PWM_4_INST_INT_IRQN                                     (TIMG0_INT_IRQn)
#define PWM_4_INST_CLK_FREQ                                               500000
/* GPIO defines for channel 0 */
#define GPIO_PWM_4_C0_PORT                                                 GPIOA
#define GPIO_PWM_4_C0_PIN                                         DL_GPIO_PIN_23
#define GPIO_PWM_4_C0_IOMUX                                      (IOMUX_PINCM53)
#define GPIO_PWM_4_C0_IOMUX_FUNC                     IOMUX_PINCM53_PF_TIMG0_CCP0
#define GPIO_PWM_4_C0_IDX                                    DL_TIMER_CC_0_INDEX



/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_4_MHZ_9600_BAUD                                         (26)
#define UART_0_FBRD_4_MHZ_9600_BAUD                                          (3)
/* Defines for UART_1 */
#define UART_1_INST                                                        UART1
#define UART_1_INST_IRQHandler                                  UART1_IRQHandler
#define UART_1_INST_INT_IRQN                                      UART1_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOA
#define GPIO_UART_1_RX_PIN                                         DL_GPIO_PIN_9
#define GPIO_UART_1_TX_PIN                                         DL_GPIO_PIN_8
#define GPIO_UART_1_IOMUX_RX                                     (IOMUX_PINCM20)
#define GPIO_UART_1_IOMUX_TX                                     (IOMUX_PINCM19)
#define GPIO_UART_1_IOMUX_RX_FUNC                      IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                      IOMUX_PINCM19_PF_UART1_TX
#define UART_1_BAUD_RATE                                                  (9600)
#define UART_1_IBRD_4_MHZ_9600_BAUD                                         (26)
#define UART_1_FBRD_4_MHZ_9600_BAUD                                          (3)





/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOA)

/* Defines for PIN_14: GPIOA.14 with pinCMx 36 on package pin 7 */
#define LED_PIN_14_PIN                                          (DL_GPIO_PIN_14)
#define LED_PIN_14_IOMUX                                         (IOMUX_PINCM36)
/* Port definition for Pin Group I2C_CI1302 */
#define I2C_CI1302_PORT                                                  (GPIOB)

/* Defines for SCL: GPIOB.2 with pinCMx 15 on package pin 50 */
#define I2C_CI1302_SCL_PIN                                       (DL_GPIO_PIN_2)
#define I2C_CI1302_SCL_IOMUX                                     (IOMUX_PINCM15)
/* Defines for SDA: GPIOB.3 with pinCMx 16 on package pin 51 */
#define I2C_CI1302_SDA_PIN                                       (DL_GPIO_PIN_3)
#define I2C_CI1302_SDA_IOMUX                                     (IOMUX_PINCM16)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_1_init(void);
void SYSCFG_DL_PWM_2_init(void);
void SYSCFG_DL_PWM_3_init(void);
void SYSCFG_DL_PWM_4_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_UART_1_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
