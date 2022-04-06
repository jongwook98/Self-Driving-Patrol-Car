/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:ffs=unix */
#ifndef SDPC_UART_H
#define SDPC_UART_H

int sdpc_uart_init(void);
int sdpc_uart_exit(void);

int sdpc_uart_send(const char *send_msg);
int sdpc_uart_receive(char *read_buf);

#endif /* SDPC_UART_H */
