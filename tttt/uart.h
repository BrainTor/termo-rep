#ifndef __UART_H__
#define __UART_H__

#include "sys.h"


sbit RS485_TX_EN=P0^1;

#define UART2_MAX_LEN  128
extern u16 uart2_rx_count;
extern u8 xdata  Uart2_Rx[UART2_MAX_LEN];

#define UART3_MAX_LEN  128
extern u16 uart3_rx_count;
extern u8 xdata  Uart3_Rx[UART3_MAX_LEN];

#define UART5_MAX_LEN 128
extern u16 uart5_rx_count;
extern u8 xdata Uart5_Rx[UART5_MAX_LEN]; 

extern u8 Uart2_TTL_Status;

extern u8 Uart3_TTL_Status;
void UART2_Init(void);
void UART2_Sendbyte(u8 dat);
void UART2_SendStr(u8 *pstr,u8 strlen);
void UART3_Init(void);
void UART3_Sendbyte(u8 dat);
void UART3_SendStr(u8 *pstr,u8 strlen);
void UART5_Init(void);
void UART5_Sendbyte(u8 dat);
void UART5_SendStr(u8 *pstr,u8 strlen);





















#endif

