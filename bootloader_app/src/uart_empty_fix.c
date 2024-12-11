
// Need to hack the uart transmit shift register empty check to the application, current UART driver doesn't support
// it. issue created...
#include "uart.h"
#include "RTE_Components.h"
#include CMSIS_device_header

// assuming default UART configs here.
// If different uarts are to be used, changes are needed to:
// 1. board.h which controls what UART instances STDOUT retargeting uses
// 2. pinconfs in bootloader, blinky and example apps which set up pinmuxing for uarts
// 3. here to control which UART FIFO and transmit shift register are waited to be empty before shutting down UART
#ifdef M55_HE
    UART_Type* regs = (UART_Type *)UART2_BASE;
#elif defined(M55_HP)
    UART_Type* regs = (UART_Type *)UART4_BASE;
#endif


void wait_for_uart_empty(void)
{
    while(!(regs->UART_LSR & UART_LSR_TRANSMITTER_EMPTY));  // wait for the UART transmissions to be fully complete
}
