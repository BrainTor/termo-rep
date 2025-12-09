/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : uart.c
  版 本 号   : V2.0
  作    者   : chengjing
  生成日期   : 2019年4月1日
  功能描述   : 串口函数
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "uart.h"


/********************************
*   串口接受缓存区
*  缓存区大小可以根据实际进行修改
*  接收到缓存数据请尽快处理，否者
*  下一条数据会往尾端增加
********************************/
u8 uart2_busy=0;
u16 uart2_rx_count=0;
u8 xdata Uart2_Rx[UART2_MAX_LEN];

u8 uart3_busy=0;
u16 uart3_rx_count=0;
u8 xdata Uart3_Rx[UART3_MAX_LEN];

u16 uart5_rx_count=0;
u8 xdata Uart5_Rx[UART5_MAX_LEN];

u8 Uart2_TTL_Status = 0;
u8 Uart3_TTL_Status = 0;

void UART2_Init(void)
{
    ADCON=0x80;
    SCON0=0x50;
    SREL0H=0x03;        //FCLK/64*(1024-SREL1)
    SREL0L=0xE4;
    ES0=1;
    EA=1;
}

void UART2_Sendbyte(u8 dat)
{       
     while(uart2_busy==1);       
     uart2_busy=1;
     SBUF0 = dat;        
}



void UART2_SendStr(u8 *pstr,u8 strlen)
{
    if((NULL == pstr)||(0 == strlen))
    {
        return;
    }
    while(strlen--)
    {
        UART2_Sendbyte(*pstr);
        pstr++;
    }  
}


void UART2_ISR_PC(void)    interrupt 4
{
    u8 res=0;
    EA=0;
    if(RI0==1)
    {
        res=SBUF0;
        Uart2_Rx[uart2_rx_count]=res; 
        uart2_rx_count++;
        RI0=0;       
				Uart2RxCt = 5; 
				Uart2_TTL_Status = 12;
    }
    if(TI0==1)
    {
        TI0=0;
        uart2_busy=0;
    }
    EA=1;
}

void UART3_Init(void)
{
    SCON1=0xD0;				//8N1
//     SCON1=0xd0;				//偶校验
//     SCON1=0xdc;				//奇校验
    SREL1H=0x03;        //FCLK/64*(1024-SREL1)
    SREL1L=0xC8;
    IEN2|=0x01;
    EA=1;
}
 /**
    * @brief UART3发送一个字节   
    * @param [in] 
    *      dat:发送的字节
    * @param [out]
    *      无
    * @retval 
    *      无
    */
void UART3_Sendbyte(u8 dat)
{
     while(uart3_busy==1);       
     uart3_busy=1;
     SBUF1 = dat;  
}

//void UART3_Sendbyte(u8 dat)
//{
//            
//     SBUF1 = dat;    
//     while((SCON1&0x02)==0); 
//     SCON1 &=0xFD; 
//             
//}

 /**
    * @brief UART3发送字符串       
    * @param [in] 
    *      pstr:发送的字符串
    *      strlen：字符串长度
    * @param [out]
    *      无
    * @retval 
    *      无
    */
void UART3_SendStr(u8 *pstr,u8 strlen)
{
    if((NULL == pstr)||(0 == strlen))
    {
        return;
    }
    while(strlen--)
    {
        UART3_Sendbyte(*pstr);
        pstr++;
    }  
}

 /**
    * @brief UART3接受中断函数
    *       接受数据，保存在Uart3_Rx数组中
    * @param [in] 
    *      无
    * @param [out]
    *      无
    * @retval 
    *      无
    */
void UART3_ISR_PC(void)    interrupt 16
{
    u8 res=0;
    EA=0;
    if(SCON1&0x01)
    {
        res=SBUF1;
        Uart3_Rx[uart3_rx_count]=res; 
        uart3_rx_count++;
        SCON1 &= 0xFE;       
        SCON1 &= 0xFE;       
				Uart3RxCt = 5; 
				Uart3_TTL_Status = 12;
    }
    if(SCON1&0x02)
    {
        SCON1&=0xFD;
        SCON1&=0xFD;
        uart3_busy=0;
    }    
    EA=1;
}


/*****************************************************************************
 函 数 名  : void UART5_Init(void)
 功能描述  : 串口4初始化
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月30日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void UART5_Init(void)
{
    SCON3T=0x80;
    SCON3R=0x80;
    BODE3_DIV_H=0x00;     //FCLK/(8*DIV）
    BODE3_DIV_L=0xE0;
    //ES3T=1;
    ES3R=1;
	RS485_TX_EN=0;
    EA=1;
}




/*****************************************************************************
 函 数 名  : void UART5_Sendbyte(u8 dat)
 功能描述  : 串口发送一个字节
 输入参数  :	dat:发送字节值
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月30日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void UART5_Sendbyte(u8 dat)
{
     SBUF3_TX = dat;    
     while((SCON3T&0x01)==0); 
     SCON3T &=0xFE;    
}


/*****************************************************************************
 函 数 名  : void UART5_SendStr(u8 *pstr,u8 strlen)
 功能描述  : 串口发送一个字节
 输入参数  :	pstr:发送字符串首地址
			strlen：发送字符串长度
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月30日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void UART5_SendStr(u8 *pstr,u8 strlen)
{
    if((NULL == pstr)||(0 == strlen))
    {
        return;
    }
	RS485_TX_EN=1;
    while(strlen--)
    {
        UART5_Sendbyte(*pstr);
        pstr++;
    }
	RS485_TX_EN=0;
}


void UART5_TX_ISR_PC(void)    interrupt 12
{
}

/*****************************************************************************
 函 数 名  : void UART5_RX_ISR_PC(void)    interrupt 13
 功能描述  : 串口中断接收函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月30日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void UART5_RX_ISR_PC(void)    interrupt 13
{
    u8 res=0;
    EA=0;
    if((SCON3R&0x01)==0x01)
    {
        res=SBUF3_RX;
        Uart5_Rx[uart5_rx_count]=res; 
        uart5_rx_count++;
        SCON3R&=0xFE;       
    }
    EA=1;
}



