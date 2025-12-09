/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : V2.0
  作    者   : chengjing
  生成日期   : 2019年4月30日
  功能描述   : 主函数，外设和参数初始化，主循环中主要功能函数入口。
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "t5los8051.h"
#include "sys.h"
#include "uart.h"
#include "config.h"
#include "control.h"
#include "TP_draw.h"
#include "timer.h"
#include "RTC.h"
#include "vcnl4000.h" 



u8 code test[]="uart send...";
u8 ProxResult[2] = {0};
int main(void)
{   
	
    INIT_CPU();
	PORT_Init();
	POWER_ON();
	T0_Init();		
	T1_Init();	
    T2_Init();	
//     WDT_ON();       //打开开门狗    喂狗在定时器T2中
    UART2_Init();			
    UART3_Init();				
    UART5_Init();				
// 	UART5_SendStr(test,sizeof(test));	
	UART3_SendStr(test,sizeof(test));	
	System_Parm_Init();
	VCNL4200_Init();									//初始化VCNL4200
// 		RTC_RunFunction();
		
	while(1)
	{   	
		Time_Update();					//更新时间		
		M4G6_dataRx();
		uart2com();
// 		P8563_Readtime();		
		Parm_Set_Function();			//设置函数入口
		Control_Function();				//数据采集和逻辑控制函数入口
		VCNL4200_MeasurementResult(ProxResult);
// 		if(screen_count>500)
// 		{
// 			screen_count = 0;
// 			UART5_SendStr(test,sizeof(test));
// 			
// 		}
		
// 		Time_Update();
// 		TP_drawin();
	}
}


