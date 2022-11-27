
#ifndef __U_USART_H__
#define __U_USART_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "user_main.h"

    void receive_uart_start_it(void);
    void TxPrintf(char *Form, ...);
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* __U_USART_H__ */
