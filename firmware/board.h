#ifndef __BOARD_H__
#define __BOARD_H__

#include "ti_msp_dl_config.h"   /* SysConfig 生成 */
#include <stdint.h>

/*===========================================================================
 * 立创地猛星 MSPM0G3507 板级支持
 *
 * SysConfig 外设:
 *   SYSCTL (80MHz), UART_0 (PA10/11 调试), UART_1 (PA8/9 TJC屏),
 *   TIMA_0 (PB6-9 舵机), SYSTICK (1us), GPIO (LED PA14, I2C PB2/3)
 *=========================================================================*/

void board_init(void);
void delay_us(unsigned long __us);
void delay_ms(unsigned long ms);

#endif
