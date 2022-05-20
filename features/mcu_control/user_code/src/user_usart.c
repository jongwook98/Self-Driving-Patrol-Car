
#include "user_usart.h"
#include "usart.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 128
static uint8_t buf;
static uint8_t rxBuffer[BUF_SIZE];
static uint8_t txBuffer[BUF_SIZE];

#define LAT_BUF_SIZE 5
static uint8_t buf_latte[LAT_BUF_SIZE];
static uint8_t rx_latte[BUF_SIZE];
// static uint8_t tx_latte[BUF_SIZE];

st_protocol st_ptcl;

uint8_t txERR[] = "Something's wrong\r\n";
uint32_t tim_cnt = 0;

char CR[] = {'\r'};

void USARTx_TxString(volatile UART_HandleTypeDef *USARTx, uint8_t *buffer,
                     char *Str)
{
    uint16_t strCnt = 0;
    while (*Str)
    {
        if (*Str == '\n')
        {
            strncat((char *)buffer, CR, 1);
            strCnt++;
        }
        strncat((char *)buffer, Str++, 1);
        strCnt++;
    }

    HAL_UART_Transmit((UART_HandleTypeDef *)USARTx, (uint8_t *)buffer, strCnt,
                      100);

    memset((void *)buffer, 0x00, sizeof(uint8_t) * BUF_SIZE);
}

void TxPrintf(char *Form, ...)
{
    TX_LED_ON;

    // tim_cnt = htim1.Instance->CNT;
    static char Buff[BUF_SIZE];
    va_list ArgPtr;
    va_start(ArgPtr, Form);
    vsprintf(Buff, Form, ArgPtr);
    va_end(ArgPtr);
    USARTx_TxString(&huart1, txBuffer, Buff);
    // tim_cnt -= htim1.Instance->CNT;

    TX_LED_OFF;
}

void RxBuffer(uint8_t *buffer, uint8_t *_buf)
{
    if (*_buf == '\r' || *_buf == '\n')
        memset((void *)buffer, 0x00, sizeof(uint8_t) * BUF_SIZE);
    else
        strcat((char *)buffer, (char *)_buf);
}

void linux_protocol(uint8_t *buffer, uint8_t *_buf)
{
    if (*_buf == 0x55 && *(_buf + 4) == 0x77)
    {
        st_ptcl.start = *_buf;
        st_ptcl.mode = *(_buf + 1);
        st_ptcl.flag.all = *(_buf + 2);
        st_ptcl.angle = *(_buf + 3);
        st_ptcl.stop = *(_buf + 4);

        control_flow.inputs->Input1.input_angle_r32 = (real_T)st_ptcl.angle;
        control_flow.inputs->flag.steer = st_ptcl.flag.bit.blinker;
        control_flow.inputs->flag.turn = st_ptcl.flag.bit.turn_onoff;
        control_flow.inputs->flag.move = st_ptcl.flag.bit.move_onoff;
    }

    memset((void *)_buf, 0x00, sizeof(uint8_t) * LAT_BUF_SIZE);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    RX_LED_ON;

    if (huart->Instance == USART1)
    {
        RxBuffer(rxBuffer, &buf);
        // linux_protocol(rxBuffer, &buf);

        switch (buf)
        {
        case 'q':
            control_flow.inputs->Input1.input_velo_r32 += 100;
            // control_flow_latte.input_angle_r64 += (float)10.0;
            break;
        case 'a':
            control_flow.inputs->Input1.input_velo_r32 -= 100;
            // control_flow_latte.input_angle_r64 -= (float)10.0;
            break;
        case 'w':
            control_flow.inputs->Input1.input_angle_r32 += (float)10.0;
            break;
        case 's':
            control_flow.inputs->Input1.input_angle_r32 -= (float)10.0;
            break;
        case 'e':
            if (control_flow.inputs->flag.turn == 0)
                control_flow.inputs->flag.turn = 1;
            else
            {
                if (control_flow.inputs->flag.steer == 1)
                    control_flow.inputs->flag.steer = 0;
                else
                    control_flow.inputs->flag.steer = 1;
            }
            break;
        case 'r':
            if (control_flow.inputs->flag.move == 1)
                control_flow.inputs->flag.move = 0;
            else
                control_flow.inputs->flag.move++;
            break;
        }

        HAL_UART_Receive_IT(&huart1, &buf, 1);
    }

    if (huart->Instance == USART2)
    {
        linux_protocol(rx_latte, buf_latte);
        HAL_UART_Receive_IT(&huart2, buf_latte, LAT_BUF_SIZE);
    }

    RX_LED_OFF;
}

void receive_uart_start_it(void)
{
    HAL_UART_Receive_IT(&huart1, &buf, 1);
    HAL_UART_Receive_IT(&huart2, buf_latte, LAT_BUF_SIZE);
}
