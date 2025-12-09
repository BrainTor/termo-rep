#ifndef __TIMER_H__
#define __TIMER_H__

#include "sys.h"

/************************************************************************************************************************************************
						用户配置区
							
调用此功能模块时要先修改此配置区参数							
*************************************************************************************************************************************************/

#define bus_delay 			{u8 bus_ticks = 15; while(bus_ticks--);}
#define Delay_20ticks		{u8 bus_ticks = 20; while(bus_ticks--);}
#define  TIME_HOUR_DISPLAY  0x2014
#define  TIME_MIN_DISPLAY  0x2015
//定义端口
sbit RTC_SCLDATA_PORT=P3^2;	//SCL端口数值引脚定义，注意引脚的数字IO口属性修改
sbit RTC_SDADATA_PORT=P3^3;	//SDA端口数值引脚定义，注意引脚的数字IO口属性修改

#define RTC_SCL_OUT      {P3MDOUT|=0x04;  bus_delay;}  
#define RTC_SCL_OUTPUTH  {RTC_SCLDATA_PORT = 1; bus_delay;}
#define RTC_SCL_OUTPUTL  {RTC_SCLDATA_PORT = 0; bus_delay;}

#define RTC_SDA_INPUTH   (RTC_SDADATA_PORT)
#define RTC_SDA_INPUTL   (!RTC_SDADATA_PORT)

#define RTC_SDA_OUT  	 {P3MDOUT|= 0x08; bus_delay;}  
#define RTC_SDA_IN       {P3MDOUT&= 0xF7; bus_delay;}  
#define RTC_SDA_OUTPUTH  {P3MDOUT|= 0x08; RTC_SDADATA_PORT = 1; bus_delay;}
#define RTC_SDA_OUTPUTL  {P3MDOUT|= 0x08; RTC_SDADATA_PORT = 0; bus_delay;}


// typedef struct _dev_time
// {
// 	u8 year;
// 	u8 month;
// 	u8 day;
// 	u8 week;
// 	u8 hour;
// 	u8 min;
// 	u8 sec;
// 	u8 rec;
// }rtc_time;

extern rtc_time	real_time;
extern u8 RTC_fault;
extern data u8 date_time[7];//存放RTC时钟，date_time[7]={年，月，日，周，时，分，秒}

u8 hextobcd(u8 number);
u8 bcdtohex(u8 number);
void RTC_Start(void);
void RTC_Stop(void);
void RTC_Send_Byte(u8 Byte);
void RTC_Ack(void);
void RTC_Send_Ack(void);
void RTC_NOAck(void);
u8  RTC_Receive_Byte(void);
void I2C_RTC_Write(u8 W_addr, u8 *word);
void I2C_RTC_Read(u8 R_addr, u8 *RTC_buffer);

void RTC_init(void);
void Time_Update(void);
void RTC_RunFunction(void);
void RTC_Reset(u8* buf);


#endif