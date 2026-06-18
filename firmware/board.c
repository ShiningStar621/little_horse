#include "board.h"
#include <stdio.h>

/*===========================================================================
 * 板级初始化
 *   SYSCFG_DL_init(): 时钟 + UART0 + UART1 + TIMA0 + GPIO + SysTick
 */
void board_init(void)
{
    SYSCFG_DL_init();

    /* UART0 中断 (调试回显) */
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    printf("Board Init [DMX + CI1302 + TJC + Servo]\r\n");
}

/*===========================================================================
 * SysTick 微秒延时 (80MHz, period=80 → 1us/tick)
 */
void delay_us(unsigned long __us)
{
    uint32_t ticks = __us;
    uint32_t told, tnow, tcnt = 0;

    told = SysTick->VAL;
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;
            told = tnow;
            if (tcnt >= ticks) break;
        }
    }
}

void delay_ms(unsigned long ms)
{
    delay_us(ms * 1000);
}

/*===========================================================================
 * printf → UART0 (调试串口)
 */
#if !defined(__MICROLIB)
#if (__ARMCLIB_VERSION <= 6000000)
struct __FILE { int handle; };
#endif
FILE __stdout;
void _sys_exit(int x) { (void)x; }
#endif

int fputc(int ch, FILE *stream)
{
    (void)stream;
    while (DL_UART_isBusy(UART_0_INST) == true) {}
    DL_UART_Main_transmitData(UART_0_INST, (uint8_t)ch);
    return ch;
}

/*===========================================================================
 * UART0 RX 中断 (回显)
 */
void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST)) {
    case DL_UART_IIDX_RX: {
        uint8_t data = DL_UART_Main_receiveData(UART_0_INST);
        while (DL_UART_isBusy(UART_0_INST) == true) {}
        DL_UART_Main_transmitData(UART_0_INST, data);
        break;
    }
    default:
        break;
    }
}
