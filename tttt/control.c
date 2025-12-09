/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : control.c
  版 本 号   : V2.0
  作    者   : chengjing
  生成日期   : 2019年5月14日
  功能描述   : 串口函数
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "control.h"
#include "config.h"
#include "uart.h"
#include "stdlib.h"



//实时温度显示值
u16	Temper_Display_Val=0;
u16	Temper_Display_Val_Dec=0;
//设定温度显示值
u16 Set_Temper_Display_Val=0;
u16 Set_Temper_Display_Val_Dec=0;
//实时温度旧值
u16 Temperature_Val_Old=0;
//设定温度旧值
u16	Set_Temperature_Val_Old=0;
//定时器123开启标志位
u16 timer_key[3]={0};
//定时器时段风速模式数据保存
timer_period	tim_per_val[3][4]={0};
//云变量定时器星期值
u16 tim_week_c[3]={0};
//定时器123定时星期
u16 timer_week[3][7]={0};
//定时器开关选择
u16 tim_k=0;
//时分
u16 Hour_Min_R=0;
//有进行时间设置
u16 Timer_Parm_Read_Flag=0;
//定时器123触发标志
u8 timer_trigger_flag[3]={0};
//主界面配置参数
u16 data Wind_Speed=0;
u16 data Wind_Speed_Old=0;
u16 data Wind_Speed_Val=0;
u16 Wind_Speed_Old_Val=0;
u16 data Mode=0;
u16 data Mode_old=0;
u16 data Mode_Val=0;
u16 Mode_Old_Val=0;
u16 movie_icon = 0;
//风速模式配置标志
u16	Wind_Speed_Config_Flag=0;
u16	Mode_Config_Flag=0;
u16	Wind_Speed_Mode_Config_Flag=0;
//实际温度是否达到设定温度值标志
u16 xdata Temperature_To_Set_Flag=0;
//风速是否改变标志	0：风速没有改变，0x01：风速改变，风速阀没有关闭，0x10：风速阀已关闭
u16	xdata Wind_Set_Flag=0;	
//模式改变标志，需要改变继电器
u16	xdata Mode_Set_Flag=0;		

u8  yuyin_state[2]={0};


u8  SpeakCmd[4] = {0x01,0x01,0x40,0x00};

/******************CRC 校验表格************************/
unsigned char code CRCTABH[256]=
{0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1, 
	0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
	0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,
	0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
	0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,
	0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
	0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,
	0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
	0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,
	0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
	0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
	0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
	0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,
	0x81,0x40,0x00,0xc1,0x81, 0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
	0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41, 0x00,0xc1,0x81,0x40,0x01,0xc0,
	0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01, 0xc0,0x80,0x41,
	0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0, 
	0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81, 0x40};
unsigned char code CRCTABL[256]=
{0x00,0xc0,0xc1,0x01,0xc3,0x03,0x02,0xc2,0xc6,0x06,0x07,0xc7,0x05,0xc5,0xc4,0x04,0xcc, 
	0x0c,0x0d,0xcd,0x0f,0xcf,0xce,0x0e,0x0a,0xca,0xcb,0x0b,0xc9,0x09,0x08,0xc8,0xd8,0x18, 
	0x19,0xd9,0x1b,0xdb,0xda,0x1a,0x1e,0xde,0xdf,0x1f,0xdd,0x1d,0x1c,0xdc,0x14,0xd4,0xd5, 
	0x15,0xd7,0x17,0x16,0xd6,0xd2,0x12,0x13,0xd3,0x11,0xd1,0xd0,0x10,0xf0,0x30,0x31,0xf1, 
	0x33,0xf3,0xf2,0x32,0x36,0xf6,0xf7,0x37,0xf5,0x35,0x34,0xf4,0x3c,0xfc,0xfd,0x3d,0xff,0x3f, 
	0x3e,0xfe,0xfa,0x3a,0x3b,0xfb,0x39,0xf9,0xf8,0x38,0x28,0xe8,0xe9,0x29,0xeb,0x2b,0x2a,0xea, 
	0xee,0x2e,0x2f,0xef,0x2d,0xed,0xec,0x2c,0xe4,0x24,0x25,0xe5,0x27,0xe7,0xe6,0x26,0x22,0xe2, 
	0xe3,0x23,0xe1,0x21,0x20,0xe0,0xa0,0x60,0x61,0xa1,0x63,0xa3,0xa2,0x62,0x66,0xa6,0xa7, 
	0x67,0xa5,0x65,0x64,0xa4,0x6c,0xac,0xad,0x6d,0xaf,0x6f,0x6e,0xae,0xaa,0x6a,0x6b,0xab,0x69, 
	0xa9,0xa8,0x68,0x78,0xb8,0xb9,0x79,0xbb,0x7b,0x7a,0xba,0xbe,0x7e,0x7f,0xbf,0x7d,0xbd,0xbc, 
	0x7c,0xb4,0x74,0x75,0xb5,0x77,0xb7,0xb6,0x76,0x72,0xb2,0xb3,0x73,0xb1,0x71,0x70,0xb0, 
	0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9c, 
	0x5c,0x5d,0x9d,0x5f,0x9f,0x9e,0x5e,0x5a,0x9a,0x9b,0x5b,0x99,0x59,0x58,0x98,0x88,0x48, 
	0x49,0x89,0x4b,0x8b,0x8a,0x4a,0x4e,0x8e,0x8f,0x4f,0x8d,0x4d,0x4c,0x8c,0x44,0x84,0x85, 
	0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40};



void Calculate_CRC16(unsigned char *Buff, unsigned int len);

void ModePage_Set(void)
{
	read_dgus_vp(PIC_NOW,(u8*)&PageNow,1);
	read_dgus_vp(MODE,(u8 *)&Mode,1);
	switch (Mode)
	{
		case 1: 
		{
			if((PageNow==2)||(PageNow==1))
			{
				Return_Main_page();break;
			}	
			break;
		}			//制冷
		case 2: 
		{
			if((PageNow==1)||(PageNow==3))
			{
				Return_Main_page();break;
			}	
			break;		//制热
		}	
		case 3:
		{
			if((PageNow==2)||(PageNow==3))
			{
				Return_Main_page();break;
			}	
		break;			//送风
		}	
		default: Return_Main_page();break;
	}
}

/*****************************************************************************
 函 数 名  : void Control_Function(void)
 功能描述  : 温控器控制函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月30日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Control_Function(void)
{
	Key_Handler();						//按键处理
	Display_Temperature();				//显示实时温度和设定温度
	ModePage_Set();		//模式及页面处理
	Get_Temperature(Temp_time,Temp_Freq);					//得到实时温度
	Timer_Config_Handler();				//定时器处理
	if((Close_Valve_Flag&0x01)==0x01)	//休眠状态不能进行亮度处理
	{
		Close_Valve();						//关闭继电器		
	}
	else
	{
		Mode_Wind_Speed_Handler();		//温度风速控制
		Brightness_Handler();			//亮度处理
	}
	Alarm_Handler();					//闹钟处理	T5L暂时没有闹钟功能
	Sleep_Handler();					//休眠处理	
	TC_Status_Check_Handler();			//检查温控器状态是否休眠
}






/*****************************************************************************
 函 数 名  : void Display_Temperature(void)
 功能描述  : 当前温度显示和设定温度显示，有小数点显示，整数位当前温度值/10，
			小数位当前温度%10
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Display_Temperature(void)
{
	read_dgus_vp(TEMP_CURRENT,(u8*)&Temperature_Real,1);
	if(Temperature_Real!=Temperature_Val_Old)
	{
		Temperature_Val_Old=Temperature_Real;
		Temper_Display_Val=Temperature_Real/10;
		Temper_Display_Val_Dec=Temperature_Real%10;
		write_dgus_vp(TEMP_REAL_VALUE,(u8*)&Temper_Display_Val,1);
		write_dgus_vp(TEMP_REAL_VAL_DEC,(u8*)&Temper_Display_Val_Dec,1);
	}
	read_dgus_vp(TEMP_SET,(u8*)&Temperature_Set_Val,1);
	if(Set_Temperature_Val_Old!=Temperature_Set_Val)
	{
		Set_Temperature_Val_Old=Temperature_Set_Val;
		Set_Temper_Display_Val=Temperature_Set_Val/10;
		Set_Temper_Display_Val_Dec=Temperature_Set_Val%10;
		write_dgus_vp(SET_TEMP_VALUE,(u8*)&Set_Temper_Display_Val,1);
		write_dgus_vp(SET_TEMP_VAL_DEC,(u8*)&Set_Temper_Display_Val_Dec,1);
	}
}


/*****************************************************************************
 函 数 名  : void Timer_Config_Handler(void)
 功能描述  : 定时器配置处理，根据定时器设置参数，处理定时器配置参数
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月15日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Timer_Config_Handler(void)
{
	u8 i=0;
	read_dgus_vp(TIMER_KEY,(u8*)timer_key,3);
	if((timer_key[0]>0)||(timer_key[1]>0)||(timer_key[2]>0))
	{
		for(tim_k=0;tim_k<3;tim_k++)
		{
			if(timer_key[tim_k]==1)
			{								
				if(timer_week[tim_k][real_time.week]==1)		//星期有处于设置状态
				{			
					Hour_Min_R=(u16)real_time.hour*256+real_time.min;
					for(i=0;i<4;i++)
					{
						if(tim_per_val[tim_k][i].timer_time_start==tim_per_val[tim_k][i].timer_time_end)
						{
							return;
						}
						else
						{
							if((Hour_Min_R==tim_per_val[tim_k][i].timer_time_start)&&(timer_trigger_flag[tim_k]!=i+1))
							{
								if((tim_per_val[tim_k][i].timer_speed==0)||(tim_per_val[tim_k][i].time_temperture==0))
								{
									return;
								}
								else
								{
									if(tim_per_val[tim_k][i].timer_speed==5)
									{
										Standby_Handler();
									}
									else
									{
										write_dgus_vp(SPEED,(u8*)&tim_per_val[tim_k][i].timer_speed,1);
										write_dgus_vp(TEMP_SET,(u8*)&tim_per_val[tim_k][i].time_temperture,1);
									}
									timer_trigger_flag[tim_k]=i+1;
								}
							}
						}
					}
				}							
			}		
		}
		Timer_Status=1;	
		write_dgus_vp(TIMER_STATUS,(u8*)&Timer_Status,1);
	}
	else
	{
		Timer_Status=0;	
		write_dgus_vp(TIMER_STATUS,(u8*)&Timer_Status,1);
	}
	return;
}

/*****************************************************************************
 函 数 名  : void TC_Status_Check_Handler(void)
 功能描述  : 温控器状态监测，主要是处理APP端下发的休眠和开机
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void TC_Status_Check_Handler(void)
{
	read_dgus_vp(SWITCH,(u8*)&TC_Status,1);
	if((TC_Status==2)&&(TC_Status!=TC_Status_Old))
	{
		write_dgus_vp(LED_CONFIG,led_off,2);							
		Close_Valve_Flag = 0x01;			//不干扰判断休眠态
		Valve_Config_Flag=0;
		TC_Status_Old=TC_Status;
	}
	if((TC_Status==1)&&(TC_Status!=TC_Status_Old))
	{
		Boot_Handler();
	}
}

/*****************************************************************************
 函 数 名  : void Wind_Speed_Val_Handler(void)
 功能描述  : 将风速智能模式改为普通模式，简化控制逻辑
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Wind_Speed_Val_Handler(void)
{
	read_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
	if(Wind_Speed==4)
	{
		if(abs(Temperature_Real-Temperature_Set_Val)>=40)
		{
			Wind_Speed_Val=3;
		}
		else if(abs(Temperature_Real-Temperature_Set_Val)>=20)
		{
			Wind_Speed_Val=2;
		}
		else
		{
			Wind_Speed_Val=1;
		}
	}
	else
	{
		Wind_Speed_Val=Wind_Speed;
	}
	if(Wind_Speed_Val!=Wind_Speed_Old_Val)
	{
		if((Wind_Set_Flag==0x01)||(Wind_Set_Flag==0x10))
		{
			//空
		}
		else
		{
			Wind_Set_Flag = 0x01;
			Wait_Count=0;
		}
	}
	else
	{
		if(Wind_Set_Flag!=0x10)
		{
			Wind_Set_Flag=0;
		}
	}
}


/*****************************************************************************
 函 数 名  : void Wind_Speed_Control_Handler(void)
 功能描述  : 风速控制
			将风速智能模式改为普通模式，简化控制逻辑
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Wind_Speed_Control_Handler(void)
{	
	if(Temperature_To_Set_Flag==0)		//温度没有达到设定温度
	{
		Wind_Speed_Val_Handler();				
		if(Wind_Set_Flag==0)			//风速没有改变
		{
			if(Wind_Speed_Config_Flag==0)		//首次配置
			{
				if(Wind_Speed_Val==1)
				{
					Low_Speed_Valve=1;
					Medium_Speed_Valve=0;
					High_Speed_Valve=0;
				}
				else if(Wind_Speed_Val==2)
				{
					Low_Speed_Valve=0;
					Medium_Speed_Valve=1;
					High_Speed_Valve=0;
				}
				else if(Wind_Speed_Val==3)
				{
					Low_Speed_Valve=0;
					Medium_Speed_Valve=0;
					High_Speed_Valve=1;
				}
				Wait_Count=0;
				Wind_Speed_Config_Flag=1;
				Speed_Valve_Status=1;
				write_dgus_vp(SPEED_VALVE,(u8*)&Speed_Valve_Status,1);
			}			
		}
		else	//风速改变
		{			
			if(Wind_Set_Flag==0x01)
			{
				if(Wait_Count>1000)
				{
					Medium_Speed_Valve=0;
					High_Speed_Valve=0;
					Low_Speed_Valve=0;
					Wind_Set_Flag = 0x10;	//已关闭风速
					Wait_Count=0;		//重新计数					
					Wind_Speed_Old_Val=Wind_Speed_Val;
				}
			}
			if(Wind_Set_Flag==0x10)
			{
				if(Wait_Count>1000)
				{
					Wind_Speed_Old_Val=Wind_Speed_Val;
					Wind_Set_Flag=0;
					Wind_Speed_Config_Flag=0;
				}
			}
		}
	}
}

/*****************************************************************************
 函 数 名  : void Mode_Val_Handler(void)
 功能描述  : 将模式简化为开启和关闭两个状态
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Mode_Val_Handler(void)
{
	read_dgus_vp(MODE,(u8*)&Mode,1);
	if(Temperature_Real>Temperature_Set_Val)
	{
		if(Mode==1)
		{
			Mode_Val=1;
		}
		else
		{
			Mode_Val=0;
		}
		if(Mode_Val!=Mode_Old_Val)
		{			
			if(Mode_Set_Flag!=0x01)
			{
				Mode_Set_Flag=0x01;
				Wait_Count=0;
			}
		}
	}
	else if(Temperature_Real<Temperature_Set_Val)
	{
		if(Mode==2)
		{
			Mode_Val=1;
		}
		else
		{
			Mode_Val=0;
		}
		if(Mode_Val!=Mode_Old_Val)
		{			
			if(Mode_Set_Flag!=0x01)
			{
				Mode_Set_Flag=0x01;
				Wait_Count=0;
			}
		}
	}
}

/*****************************************************************************
 函 数 名  : void Mode_Control_Handler(void)
 功能描述  : 依据开启或关闭水阀值，实现模式控制
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Mode_Control_Handler(void)
{
	if((Temperature_To_Set_Flag==0)&&(Wind_Set_Flag==0))		//温度没有达到设定温度
	{
		Mode_Val_Handler();
		if(Mode_Set_Flag==0)			//模式没有改变
		{
			if(Mode_Config_Flag==0)		//首次配置
			{
				if(Mode_Val==1)
				{
					Water_Valve=1;
					Water_Valve_Status=1;
					write_dgus_vp(WATER_VALVE,(u8*)&Water_Valve_Status,1);
				}
				else
				{
					Water_Valve=0;
					Water_Valve_Status=0;
					write_dgus_vp(WATER_VALVE,(u8*)&Water_Valve_Status,1);
				}
				Mode_Config_Flag=1;
			}
		}
		else
		{
			if(Mode_Set_Flag==0x01)
			{
				if(Wait_Count>1000)
				{
					Mode_Config_Flag=0;
					Mode_Set_Flag=0;
					Mode_Old_Val=Mode_Val;
				}
			}
		}
	}
}


/*****************************************************************************
 函 数 名  : void Mode_Temperature_Handler(void)
 功能描述  : 处于个模式下，温度是否达标和达标后降温检测处理
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Mode_Temperature_Handler(void)
{
	if((Wind_Set_Flag==0)&&(Mode_Set_Flag==0))
	{
		if(Temperature_To_Set_Flag==0)		//温度没有达到设定温度
		{
			if(Mode==1)				//制冷
			{
				if(Temperature_Real<=Temperature_Set_Val)
				{
					Temperature_To_Set_Flag=1;
					Wait_Count=0;
					Close_Valve_Flag=0x01;
					Valve_Config_Flag=0;
				}
			}
			else if(Mode==2)		//制热
			{			
				if(Temperature_Real>=Temperature_Set_Val)
				{
					Temperature_To_Set_Flag=1;
					Wait_Count=0;
					Close_Valve_Flag=0x01;
					Valve_Config_Flag=0;
				}
			}
		}
		else
		{
			if(Mode==1)		//制冷
			{
				if((Temperature_Real-20)>=Temperature_Set_Val)
				{
					Temperature_To_Set_Flag=0;
					Wind_Speed_Old_Val=0;
					Mode_Old_Val=0;
				}
			}
			else if(Mode==2)		//制热
			{
				if((Temperature_Set_Val-20)>=Temperature_Real)
				{
					Temperature_To_Set_Flag=0;
					Wind_Speed_Old_Val=0;
					Mode_Old_Val=0;
				}
			}
			else if(Mode==3)		//送风
			{
				Temperature_To_Set_Flag=0;
				Wind_Speed_Old_Val=0;
			}
		}
	}
}


/*****************************************************************************
 函 数 名  : void Mode_Wind_Speed_Handler(void)
 功能描述  : 模式风速处理函数
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Mode_Wind_Speed_Handler(void)
{
	Wind_Speed_Control_Handler();
	Mode_Control_Handler();
	Mode_Temperature_Handler();
}
//串口2处理函数
void  uart2com(void)
{
	u32   DGUS_addr;
	u8		trans_data[200]={0};
	if(uart2_rx_count != 0)
	{
		if(Uart2_TTL_Status == 0)
		{
			uart2_rx_count = 0;
			if((Uart2_Rx[0] == 0x5A)&&(Uart2_Rx[1] == 0xA5))
			{
				if(Uart2_Rx[3] == 0x82)
				{
					DGUS_addr = (u32)(Uart2_Rx[4]<<8)|Uart2_Rx[5];
					write_dgus_vp(DGUS_addr,(u8*)&Uart2_Rx[6],Uart2_Rx[2]-3);
					trans_data[0]=0x5A;
					trans_data[1]=0xA5;
					trans_data[2]=0x4F;
					trans_data[3]=0x4B;
					UART2_SendStr(trans_data,4);
				}
				if(Uart2_Rx[3] == 0x83)
				{
					DGUS_addr = (u32)(Uart2_Rx[4]<<8)|Uart2_Rx[5];
					read_dgus_vp(DGUS_addr,(u8*)&trans_data[7],Uart2_Rx[6]);
					trans_data[0]=0x5A;
					trans_data[1]=0xA5;
					trans_data[2]=Uart2_Rx[6]*2+4;
					trans_data[3]=0x83;
					trans_data[4]=Uart2_Rx[4];
					trans_data[5]=Uart2_Rx[5];
					trans_data[6]=Uart2_Rx[6];
					UART2_SendStr(trans_data,trans_data[2]+3);
				}
			}
		}
	}
}
void M4G6_DataDeal(u8 dat)
{
	u16  Ttemp,ii;
				page_set[0]=0x5A;
				page_set[1]=0x01;
				page_set[2]=0x00;
				page_set[3]=30;
		ii = dat;
// 				write_dgus_vp(0x3400,(u8*)&ii,1);
	
		switch(dat)
		{
			case  0:  	//"接收超时"
				page_set[0]=0x00;
// 				SpeakCmd[0] = 0;	
// 				SpeakCmd[1] = 1;		
// 				SpeakCmd[2] = 0x18;
// 				SpeakCmd[3] = 0;				
// 				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"没有听清"
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
			break;
			case  1:  	//"你好小迪"
				SpeakCmd[0] = 1;	
				SpeakCmd[1] = 1;		
				SpeakCmd[2] = 0x18;
				SpeakCmd[3] = 0;				
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"主人我在"
// 							write_dgus_vp(TEST_VP1,SpeakCmd,2);
				do
				{
					read_dgus_vp(SPEAK_VP,(u8*)&SpeakCmd[0],1);
				}while(SpeakCmd[1]);
				yuyin_state[0]=1;
				yuyin_state[1]=0;
				
				break;	
			case  2:		//"升高温度"
				SpeakCmd[0] = 2;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"当前温度"
			
				yuyin_state[0]=2;
				yuyin_state[1]=1;		//0201,表示2级菜单，温度增加
				delay_ms(1500);
				SpeakCmd[0] = Temperature_Real/100;
			if(SpeakCmd[0]!=0)
			{
				SpeakCmd[0] += 29;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"温度十位"
				delay_ms(1500);
			}
				SpeakCmd[0] = (Temperature_Real%100)/10+ 20;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"温度个位"
				
				delay_ms(1000);
				SpeakCmd[0] = 40;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"主人想要多少度"
				
				delay_ms(2500);
				break;
			case  3:    //"开始制冷"
				SpeakCmd[0] = 5;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"制冷启动"
				
				Mode=1;
				write_dgus_vp(MODE,(u8*)&Mode,1);
				Speed_Auto_Set_Handler();
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
				break;
// 						//"设定风速"
// 							SpeakCmd[0] = 03;			
// 							SpeakCmd[1] = 1;		
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"当前风速"
// 							delay_ms(1000);
// 							
// 							
// 							SpeakCmd[0] = Wind_Speed_Val+20;
// 							SpeakCmd[1] = 1;		
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"档位"
// 							delay_ms(500);
// 							SpeakCmd[0] = 4;
// 							SpeakCmd[1] = 1;		
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"想要多少档"
// 							delay_ms(500);
// 							break;
			case  4: 		
				SpeakCmd[0] = 6;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"制热启动" 
				Mode=2;
				write_dgus_vp(MODE,(u8*)&Mode,1);
				Speed_Auto_Set_Handler();
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
				break;
			case  5: 
				SpeakCmd[0] = 7;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"停止工作" 
				
				Standby_Handler();			//休眠
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
				break;
			case  6:  
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				if(Mode == 1)
				{
					Speed_Low_Set_Handler();
				}
				else
				{
					Wind_Speed++;
					if(Wind_Speed>3)
					{
						Wind_Speed=3;
					}
					write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
				}
				Temperature_Set_Increase();
				Temperature_Set_Confirm_Handler();
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
			break;
			case  7:  
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				if(Mode == 1)
				{
					Close_Valve_Flag=0x01;
					Valve_Config_Flag=0;
					TC_Status=2;
					TC_Status_Old=TC_Status;
					write_dgus_vp(SWITCH,(u8*)&TC_Status,1);	//温控器状态休眠
				}
				else
				{
					
					Speed_High_Set_Handler();
					Temperature_Set_Increase();
					Temperature_Set_Increase();
					read_dgus_vp(TEMPERATURE_PRECISION,(u8*)&ii,1);
					if(ii==0)
					{
						Temperature_Set_Increase();
						Temperature_Set_Increase();
					}
					Temperature_Set_Confirm_Handler();
				}
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
				break;
			case  8:  
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				if(Mode == 2)
				{
					Speed_Low_Set_Handler();
				}
				else
				{
					Wind_Speed++;
					if(Wind_Speed>3)
					{
						Wind_Speed=3;
					}
					write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
				}
				Tmeperature_Set_Decrease();
				Temperature_Set_Confirm_Handler();
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
				break;
			case  9:  
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				if(Mode == 2)
				{
					Close_Valve_Flag=0x01;
					Valve_Config_Flag=0;
					TC_Status=2;
					TC_Status_Old=TC_Status;
					write_dgus_vp(SWITCH,(u8*)&TC_Status,1);	//温控器状态休眠
				}
				else
				{
					
					Speed_High_Set_Handler();
					Tmeperature_Set_Decrease();
					Tmeperature_Set_Decrease();
					read_dgus_vp(TEMPERATURE_PRECISION,(u8*)&ii,1);
					if(ii==0)
					{
						Tmeperature_Set_Decrease();
						Tmeperature_Set_Decrease();
					}
					Temperature_Set_Confirm_Handler();
				}
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
			break;
			case  10:  
				SpeakCmd[0] = 18;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"当前天气"
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
				break;
			case  11:  
				SpeakCmd[0] = 9;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		 //"开启闹钟"
				Alarm_Key[0]=1;
				Alarm_Key[1]=0;
				write_dgus_vp(ALARM_KEY,(u8*)Alarm_Key,2);
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
			break;
			case  12:  
				SpeakCmd[0] = 10;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		 //"关闭闹钟"
				Alarm_Key[0]=0;
				Alarm_Key[1]=0;
				write_dgus_vp(ALARM_KEY,(u8*)Alarm_Key,2);
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
			break;
			case  13:  
// 							SpeakCmd[0] = 11;			
// 							SpeakCmd[1] = 1;		
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		 //"当前状态"
			
				SpeakCmd[0] = 2;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"当前温度"
			
				yuyin_state[0]=0;
				yuyin_state[1]=0;		//0202,表示2级菜单，温度降低
				delay_ms(1000);
				SpeakCmd[0] = Temperature_Real/100;
				if(SpeakCmd[0]!=0)
				{
					SpeakCmd[0] += 29;
					SpeakCmd[1] = 1;		
					write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"温度十位"
					delay_ms(1000);
				}
				SpeakCmd[0] = (Temperature_Real%100)/10+ 20;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"温度个位"
					delay_ms(1000);
			
				SpeakCmd[0] = 14;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"当前风速"
				
					delay_ms(1700);
// 								read_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
				SpeakCmd[0] = Wind_Speed_Val+20;			
				SpeakCmd[1] = 1;
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);	
// 							read_dgus_vp(MODE,(u8*)&Mode,1);
// 							SpeakCmd[0] =Mode+14;
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		 //"模式"
// 							yuyin_state[0]=0;
// 							yuyin_state[1]=0;	
				break;
			case  14:  
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				Wind_Speed--;
					if(Wind_Speed<1)
					{
						Wind_Speed=1;
					}
					write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
					break;
			case  15:  
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				Wind_Speed++;
					if(Wind_Speed>3)
					{
						Wind_Speed=3;
					}
					write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
				yuyin_state[0]=0;
				yuyin_state[1]=0;	
					break;
			case  16:		//"降低温度"
				SpeakCmd[0] = 2;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"当前温度"
			
				yuyin_state[0]=2;
				yuyin_state[1]=2;		//0202,表示2级菜单，温度降低
				delay_ms(1500);
				SpeakCmd[0] = Temperature_Real/100;
			if(SpeakCmd[0]!=0)
			{
				SpeakCmd[0] += 29;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"温度十位"
				delay_ms(1500);
			}
				SpeakCmd[0] = (Temperature_Real%100)/10+ 20;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"温度个位"
				
				delay_ms(1000);
				SpeakCmd[0] = 40;
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"主人想要多少度"
				
				delay_ms(2500);
				break;
			case  17:		//"调整一度"
			case  18:		//"调整两度"
			case  19:		//"调整三度"
			case  20:		//"调整四度"
			case  21:		//"调整五度"
			case  22:		//"调整六度"
			case  23:		//"调整七度"
			case  24:		//"调整八度"
			case  25:		//"调整九度"
				
					write_dgus_vp(0x1110,yuyin_state,1);
				if((yuyin_state[0]==2)&&(yuyin_state[1]==1))  //增加温度
				{
					yuyin_state[0]=0;
					yuyin_state[1]=0;	
					read_dgus_vp(SET_TEMP_VALUE,(u8*)&Ttemp,1);
					Ttemp=Ttemp+dat-16;
					if(Ttemp>32)
					{
						Ttemp=32;
					}
					write_dgus_vp(SET_TEMP_VALUE,(u8*)&Ttemp,1);
					Ttemp=Ttemp*10;
					write_dgus_vp(TEMP_SET,(u8*)&Ttemp,1);
// 							SpeakCmd[0] = 10;			
// 							SpeakCmd[1] = 1;		
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				}
				else if((yuyin_state[0]==2)&&(yuyin_state[1]==2))  //降低温度
				{
					yuyin_state[0]=0;
					yuyin_state[1]=0;	
					read_dgus_vp(SET_TEMP_VALUE,(u8*)&Ttemp,1);
					Ttemp=Ttemp-dat+16;
					if(Ttemp<16)
					{
						Ttemp=16;
					}
					write_dgus_vp(SET_TEMP_VALUE,(u8*)&Ttemp,1);
					Ttemp=Ttemp*10;
					write_dgus_vp(TEMP_SET,(u8*)&Ttemp,1);
// 							SpeakCmd[0] = 10;			
// 							SpeakCmd[1] = 1;		
// 							write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				}
				SpeakCmd[0] = 8;			
				SpeakCmd[1] = 1;		
				write_dgus_vp(SPEAK_VP,SpeakCmd,2);		//"已调节设备" 
				break;
				default:
				page_set[0]=0x00;break;
		}
		
				write_dgus_vp(PIC_SET,page_set,2);
				M4G6_ct = 10000;
}
void M4G6_dataRx(void)
{
	u8 *rxp;
// 	u8  SpeakId;
// 	u8 *crctst;
	SpeakCmd[2] = 0x18;
	SpeakCmd[3] = 0;	
//  				write_dgus_vp(TEST_VP4,(u8 *)&uart3_rx_count,1);
	if(uart3_rx_count != 0)
	{
//  				write_dgus_vp(TEST_VP4,Uart3_Rx,3);
		if(Uart3_TTL_Status == 0)
		{
			uart3_rx_count = 0;
			if((Uart3_Rx[0] == 0x5A)&&(Uart3_Rx[1] == 0xA5))
			{
				rxp = Uart3_Rx;
//  				write_dgus_vp(TEST_VP4,Uart3_Rx,3);
				Calculate_CRC16(rxp,3);
				Uart3_Rx[0] = 0;
				Uart3_Rx[1] = 0;
//  				write_dgus_vp(TEST_VP1,rxp,3);
 				if((rxp[3] == Uart3_Rx[3])&&(rxp[4] == Uart3_Rx[4]))
				{
					read_dgus_vp(TEMP_CURRENT,(u8*)&Temperature_Real,1);
					M4G6_DataDeal(rxp[2]);
					
				}
			}
		}
		
	}
}
void Calculate_CRC16(unsigned char *Buff, unsigned int len)
{
unsigned int i;
unsigned char index,crch,crcl;
crch=0xff;
crcl=0xff;
for(i=0;i<len;i++)
{
index=crch^Buff[i];
crch=crcl^CRCTABH[index];
crcl=CRCTABL[index];
}
Buff[i]=crch;
Buff[i+1]=crcl;
}



























































