#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

#include "stm32f1xx_hal.h"

void EnableDebugIRQ(void);
void DisableDebugIRQ(void);

void EnableUART3IRQ(void);


#endif /* __DRIVER_USART_H */
