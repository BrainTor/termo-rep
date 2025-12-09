/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : config.c
  版 本 号   : V2.0
  作    者   : chengjing
  生成日期   : 2019年5月14日
  功能描述   : 串口函数
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "config.h"
#include "control.h"
#include "timer.h"



/************************************************/
//参数主动上传
u8 	code Parm_Upload_Array[6]={0x5A,0xA5,0x26,0x00,0x00,0x60};
//参数改变标志
u8 	Parm_Change_Flag=0;
//触摸事件参数
u16	data Touch_Event_Flag=0;
//恢复默认参数
u32 password_val=0;
//定时器选择
u16	t_sel=0;
//定时器时段选择
u16	p_sel=0;
//设置定时时间段
u16 timer_set_period[4]={0};
//时分设置值
u16 Hour_Min_Set=0;
//参数设置错误值
u16 Parm_Set_Error_Val=0;
//定时器星期设置
u16 timer_week_set[7]={0};
//设置温度设置值
u16 Set_Temperature_Set_Val=0;
u16 Set_Temperature_Set_Val_Dec=0;
//时间设置结构体
RTC_Time_Set_Val RTC_Time_Set;
//闹钟显示时分，单独地址
u16 Alarm_Display[4]={0};
//闹钟设置选择
u16 Alarm_Sel=0;
//闹钟设置值
alarm_set	alarm_set_val;
//消息状态
u16 Message_Status=0;
//告警事件值
u16	Warning_Val=0;
//定时器时段星期显示
u16 timer_period_display[8]={0};

//定时器模式温度设置
u16 timer_speed_mode_set=0;
u16 timer_temperature_set=0;
//文本录入参数
data u16  Asc_num = 0;			//文本编号
u16  input_Ascdata[20] = (0);
//键盘录入相关
u8  datact = 0;
u8  datainput[20] = {0};

void ASC_Data_Handler(void)
{
	
		u8  input1[2],i;
// 	write_dgus_vp(0x3200,(u8*)&Asc_num,1);
		read_dgus_vp(ACS_IN,input1,1);
		if(input1[0]==0xa5)
		{
				input1[0] = 0;
				write_dgus_vp(ACS_IN,input1,1);
				if(input1[1]==0x01)								//删除单个字符
				{
						if(datact!=0)
						{
							datainput[datact-1] = 0;
							datact--;
						}
				}
				else if(input1[1]==0x02)					//取消录入，删除所有字符
				{
						for(i = 0;i<sizeof(datainput);i++)
						{
							datainput[i]=0;
						}
						datact = 0;
				}
				else if(input1[1]==0x03)					//确认，录入完毕
				{
					if(datainput[0]!=0)		//有录入
					{
						switch(Asc_num)
						{
							case 0: 
								write_dgus_vp(0x04B0,datainput,(sizeof(datainput))/2);
								break;
							case 1: 
								write_dgus_vp(0x04C0,datainput,(sizeof(datainput))/2);
								break;
						}
						
						for(i = 0;i<sizeof(datainput);i++)
						{
							datainput[i]=0;
						}
						datact = 0;
					}
				}
				write_dgus_vp(0x2704,datainput,(sizeof(datainput))/2);
		}
		if(input1[0]==0x5a)
		{
				input1[0] = 0;
				write_dgus_vp(ACS_IN,input1,1);
				if(datact<(sizeof(datainput)-1))
				{
						datainput[datact] = input1[1];
						datainput[datact+1] = '\0';
						datact++;
				}
				write_dgus_vp(0x2704,datainput,(sizeof(datainput))/2);
		}
	
	
}
void M4G6_Handler(void)
{
	u8  m4g6data[2]=(0);
	read_dgus_vp(M4G6data_IN,m4g6data,1);
	if(m4g6data[0]==0x5A)
	{
		m4g6data[0] = 0;
		write_dgus_vp(M4G6data_IN,m4g6data,1);
		M4G6_DataDeal(m4g6data[1]);
		
	}
	
}
/*****************************************************************************
 函 数 名  : void Parm_Set_Function(void)
 功能描述  : 温控器设置处理函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月14日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Parm_Set_Function(void)
{
	M4G6_Handler();
	read_dgus_vp(TOUCH_EVENT_FLAG,(u8*)&Touch_Event_Flag,1);
	if(Touch_Event_Flag>0)
	{
		switch(Touch_Event_Flag)
		{
			case	STANDBY_BOOT:
				Standby_Boot_Handler();						
				break;
			case	SHUT_DOWN_CONFIRM:
				Shut_Down_Confirm_Handler();		//页面跳转						
				break;
			case	SYSTEM_SET_ENTER:
				System_Set_Enter_Handler();					
				break;
			case	PASSWORD_INPUT_CONFIRM:
				Password_Input_Confirm_Handler();
				break;
			case	PASSWORD_INPUT_CANCEL:
				Password_Input_Cancel_Handler();
				break;
			case	RESET_CONFIRM:
				Reset_Confirm_Handler();
				break;
			case	RESET_CANCEL:
				Reset_Cancel_Handler();
				break;
			case	SYSTEM_SET_RETURN:
				System_Set_Return_Handler();
				break;
			case	ALARM_SELECT_RETURN:
				Alarm_Select_Return_Handler();
				break;
			case	ADVANCE_SET_RETURN:
				Advance_Set_Rerurn_Handler();
				break;			
			case	TIMER_SET_ENTER:
				Timer_Week_Display();
				break;
			case	TIMER1_SET_ENTER:
				t_sel=0;
				Timer_Sel_Init_Handler(t_sel);
				break;
			case	TIMER2_SET_ENTER:
				t_sel=1;
				Timer_Sel_Init_Handler(t_sel);
				break;
			case	TIMER3_SET_ENTER:
				t_sel=2;
				Timer_Sel_Init_Handler(t_sel);
				break;
			case	TIMER_SET_PERIOD1:
				p_sel=0;
				Save_Week_Handler(t_sel);
				Timer_Period_Sel_Init_Handler(t_sel,p_sel);
				break;
			case	TIMER_SET_PERIOD2:
				p_sel=1;
				Save_Week_Handler(t_sel);
				Timer_Period_Sel_Init_Handler(t_sel,p_sel);
				break;
			case	TIMER_SET_PERIOD3:
				p_sel=2;
				Save_Week_Handler(t_sel);
				Timer_Period_Sel_Init_Handler(t_sel,p_sel);
				break;
			case	TIMER_SET_PERIOD4:
				p_sel=3;
				Save_Week_Handler(t_sel);
				Timer_Period_Sel_Init_Handler(t_sel,p_sel);
				break;
			case	TIMER_TIME_SET_CONFIRM:
				Timer_Time_Set_Confirm_Handler(t_sel,p_sel);
				break;
			case	TIMER_TIME_SET_CANCEL:
				Timer_Time_Set_Cancel_Handler();
				break;
			case	TIMER_SPEED_MODE_SET_CONFIRM:
				Timer_Speed_Mode_Set_Confirm_Handler(t_sel,p_sel);
				Timer_Sel_Init_Handler(t_sel);
				break;
			case	TIMER_SPEED_MODE_SET_CANCEL:
				Timer_Speed_Mode_Set_Cancel_Handler();
				break;
			case	TIMER_WEEK_RETURN:
				Save_Week_Handler(t_sel);
				Timer_Week_Display();
				break;
			case	TIMER_SELECT_RETURN:
				Timer_Select_Return_Handler();
				break;
			case	TEMPERATURE_SET_ENTER:
				Temperature_Set_Enter_Init_Handler();
				break;
			case	TEMPERATURE_SET_CONFIRM:
				Temperature_Set_Confirm_Handler();
				break;
			case	TEMPERATURE_SET_CANCEL:
				Temperature_Set_Cancel_Handler();
				break;
			case	TEMPERATURE_SET_INCREASE:
				Temperature_Set_Increase();
				break;
			case	TEMPERATURE_SET_DECREASE:
				Tmeperature_Set_Decrease();
				break;
			case	TIME_SET_ENTER:
				Time_Set_Init_Handler();
				break;
			case	TIME_SET_CONFIRM:
				Time_Set_Confirm_Handler();
				break;
			case	TIME_SET_CANCEL:
				Time_Set_Cancel_Handler();
				break;
			case	ALARM_SET_ENTER:
				Alarm_Set_Init_Handler();
				break;
			case	ALARM1_SET_ENTER:
				Alarm_Sel=0x01;
				Alarm_Set_Handler(Alarm_Sel);				
				break;
			case	ALARM2_SET_ENTER:
				Alarm_Sel=0x02;
				Alarm_Set_Handler(Alarm_Sel);
				break;
			case	ALARM_SET_CONFIRM:
				Alarm_Set_Confirm_Handler(Alarm_Sel);
				Alarm_Set_Init_Handler();
				break;
			case	ALARM_SET_CANCEL:
				Alarm_Set_Cancel_Handler();
				break;
			case	MESSAGE_READ_CONFIRM:
				Message_Handler();
				break;
			case	SHUT_DOWN:
				Shut_Down_Handler();
				break;
			case	RETURN_MAIN:
				Return_Main_page();
				break;
			case	SPEED_AUTO:
				Speed_Auto_Set_Handler();
				break;
			case	SPEED_LOW:
				Speed_Low_Set_Handler();
				break;
			case	SPEED_MIDIUM:
				Speed_Medium_Set_Handler();
				break;
			case	SPEED_HIGH:
				Speed_High_Set_Handler();
				break;
			case	SPEED_ADD:
				Speed_Add_Set_Handler();
				break;
			case	SPEED_DECREASE:
				Speed_Decrease_Set_Handler();
				break;
			case	MODE_ADD:
				Mode_Add_Set_Handler();
				break;
			case  ASC_PUTIN1:
				Asc_Input_Handler(ASC_PUTIN1);
				break;
			case  ASC_PUTIN2:
				Asc_Input_Handler(ASC_PUTIN2);
				break;
			default:
				break;
		}
		Parm_Change_Flag=0x01;				//参数改变，保存标志位
		Touch_Event_Flag=0;					//清零
		write_dgus_vp(TOUCH_EVENT_FLAG,(u8*)&Touch_Event_Flag,1);
		Parm_Upload_Handler();
	}
	Parm_Set_Error_Handler();			//参数设置错误，跳转页面
	Warning_Event_Handler();	
	Save_Data_Handler();				//保存nor flash
	ASC_Data_Handler();
}

//返回主界面
void Return_Main_page(void)
{
	page_set[0]=0x5A;
	page_set[1]=0x01;
	page_set[2]=0x00;
	switch (Mode)
	{
		case 1: page_set[3]= 3;break;		//制冷
		case 2: page_set[3]= 2;break;		//制热
		case 3: page_set[3]= 1;break;			//送风
	}
	write_dgus_vp(PIC_SET,page_set,2);
}
void Parm_Upload_Handler(void)
{
	write_dgus_vp(RMA,(u8*)&Parm_Upload_Array,3);
}


void Standby_Boot_Handler(void)
{
	read_dgus_vp(SWITCH,(u8*)&TC_Status,1);
	if(TC_Status==1)
	{
		Standby_Handler();			//休眠		
	}
	else if(TC_Status==2)
	{
		Boot_Handler();				//开机		
	}
}

void Shut_Down_Confirm_Handler(void)
{
	Warning_Val=0x1001;
	write_dgus_vp(WARNING_EVENT,(u8*)&Warning_Val,1);		//写值
}

void System_Set_Enter_Handler(void)
{
	
// 		page_set[0]=0x5A;
// 		page_set[1]=0x01;
// 		page_set[2]=0x00;
// 		page_set[3]=(u8)Screen_Saver_Parm[1];
// 		write_dgus_vp(PIC_SET,page_set,2);
}


void Password_Input_Confirm_Handler(void)
{
	read_dgus_vp(PASSWORD_INPUT,(u8*)&password_val,2);
	if(password_val!=Password)
	{
		Parm_Set_Error_Val=0x1003;
		write_dgus_vp(PARM_SET_ERROR,(u8*)&Parm_Set_Error_Val,1);
	}
	else
	{
		
	}
}

void Password_Input_Cancel_Handler(void)
{
	password_val=0;
	write_dgus_vp(PASSWORD_INPUT,(u8*)&password_val,2);
}

void Reset_Confirm_Handler(void)
{
	Parm_Reset_Init();				//系统参数复位
	password_val=0;
	write_dgus_vp(PASSWORD_INPUT,(u8*)&password_val,2);
	Return_Main_page();
}

void Reset_Cancel_Handler(void)
{
	//可以不需要处理
}


void System_Set_Return_Handler(void)
{
	//可以不需要处理
}


void Alarm_Select_Return_Handler(void)
{
	//可以不需要处理
}

void Advance_Set_Rerurn_Handler(void)
{
	//可以不需要处理
}



/*****************************************************************************
 函 数 名  : void Week_Val(u16 time_sel)
 功能描述  : 定时器星期处理函数
 输入参数  :	 time_sel：定时器星期保存值	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Week_Val(u16 time_sel)
{
	if((tim_week_c[time_sel]&0x01)==0x01)
		timer_week[time_sel][0]=1;
	else
		timer_week[time_sel][0]=0;
	if((tim_week_c[time_sel]&0x02)==0x02)
		timer_week[time_sel][1]=1;
	else 
		timer_week[time_sel][1]=0;
	if((tim_week_c[time_sel]&0x04)==0x04)
		timer_week[time_sel][2]=1;
	else
		timer_week[time_sel][2]=0;
	if((tim_week_c[time_sel]&0x08)==0x08)
		timer_week[time_sel][3]=1;
	else
		timer_week[time_sel][3]=0;
	if((tim_week_c[time_sel]&0x10)==0x10)
		timer_week[time_sel][4]=1;
	else
		timer_week[time_sel][4]=0;
	if((tim_week_c[time_sel]&0x20)==0x20)
		timer_week[time_sel][5]=1;
	else
		timer_week[time_sel][5]=0;
	if((tim_week_c[time_sel]&0x40)==0x40)
		timer_week[time_sel][6]=1;
	else
		timer_week[time_sel][6]=0;
}



/*****************************************************************************
 函 数 名  : void Timer_Week_Display(void)
 功能描述  : 定时器星期显示函数
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Timer_Week_Display(void)
{
	u8 i=0;
	read_dgus_vp(TIMER1_WEEK,(u8*)&tim_week_c[0],1);
	read_dgus_vp(TIMER2_WEEK,(u8*)&tim_week_c[1],1);
	read_dgus_vp(TIMER3_WEEK,(u8*)&tim_week_c[2],1);
	for(i=0;i<3;i++)
	{
		Week_Val(i);
	}
	write_dgus_vp(TIMER_WEEK_DISPLAY,(u8*)&timer_week[0][0],21);	
}





/*****************************************************************************
 函 数 名  : void Timer_Select_Init_Handler(u16 t_sel)
 功能描述  : 定时器选择初始化赋值
 输入参数  :	 t_sel：定时器选择
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月14日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Timer_Sel_Init_Handler(u16 t_sel)
{
	u8 i=0;
	if(t_sel==0)
	{
		read_dgus_vp(TIMER1_TIME_SEED_MODE,(u8*)&tim_per_val[0][0],16);				
	}
	else if(t_sel==1)
	{
		read_dgus_vp(TIMER2_TIME_SEED_MODE,(u8*)&tim_per_val[1][0],16);
	}
	else if(t_sel==2)
	{
		read_dgus_vp(TIMER3_TIME_SEED_MODE,(u8*)&tim_per_val[2][0],16);
	}
	for(i=0;i<4;i++)
	{
		timer_period_display[i*2]=tim_per_val[t_sel][i].timer_time_start+25700;
		timer_period_display[i*2+1]=tim_per_val[t_sel][i].timer_time_end+25700;
	}
	write_dgus_vp(TIMER_DISPLAY_PERIOD,(u8*)timer_period_display,8);	
	write_dgus_vp(TIMER_WEEK_SET,(u8*)&timer_week[t_sel][0],7);
}



void Timer_Period_Sel_Init_Handler(u16 t_sel,u16 p_sel)
{	
	timer_set_period[0]=tim_per_val[t_sel][p_sel].timer_time_start/256;
	timer_set_period[1]=tim_per_val[t_sel][p_sel].timer_time_start%256;
	timer_set_period[2]=tim_per_val[t_sel][p_sel].timer_time_end/256+100;
	timer_set_period[3]=tim_per_val[t_sel][p_sel].timer_time_end%256+100;
	write_dgus_vp(TIMER_SET_TIME_START,(u8*)&timer_set_period[0],1);
	write_dgus_vp(TIMER_SET_TIME_END,(u8*)&timer_set_period[1],1);
	write_dgus_vp(TIMER_DISPLAY_TIME_ST,(u8*)&timer_set_period[2],2);
	timer_speed_mode_set=tim_per_val[t_sel][p_sel].timer_speed;
	timer_temperature_set=tim_per_val[t_sel][p_sel].time_temperture/10;
	write_dgus_vp(TIMER_SET_SPEED,(u8*)&timer_speed_mode_set,1);
	write_dgus_vp(TIMER_SET_TEMP,(u8*)&timer_temperature_set,1);	
}

u8 Timer_Time_Set_Confirm_Handler(u16 t_sel,u16 p_sel)
{
	u8 i=0;
	read_dgus_vp(TIMER_SET_TIME_START,(u8*)&timer_set_period[0],1);
	read_dgus_vp(TIMER_SET_TIME_END,(u8*)&timer_set_period[1],1);
	Hour_Min_Set=timer_set_period[0]*256+timer_set_period[1];
	if((Hour_Min_Set<tim_per_val[t_sel][p_sel-1].timer_time_start)&&(p_sel>=1))		//输入时间小于上一个输入时间，判断为输入时间错误
	{		
		Parm_Set_Error_Val=0x1004;		//跳转到参数设置错误页面,必须返回
		write_dgus_vp(PARM_SET_ERROR,(u8*)&Parm_Set_Error_Val,1);
		return 1;
	}
	if((Hour_Min_Set>tim_per_val[t_sel][p_sel].timer_time_end)&&(p_sel<=2))		//输入时间大于后一个设置时间,后面设置时间清零
	{
		for(i=p_sel;i<3;i++)
		{
			tim_per_val[t_sel][i].timer_time_end=0;
			tim_per_val[t_sel][i+1].timer_time_start=0;
		}
	}
	if(p_sel==0)
	{
		tim_per_val[t_sel][0].timer_time_start=Hour_Min_Set;
		tim_per_val[t_sel][3].timer_time_end=Hour_Min_Set;
	}
	else
	{
		tim_per_val[t_sel][p_sel].timer_time_start=Hour_Min_Set;
		tim_per_val[t_sel][p_sel-1].timer_time_end=Hour_Min_Set;
	}
	return 0;
}



void Timer_Time_Set_Cancel_Handler(void)
{
	
}



u8 Timer_Speed_Mode_Set_Confirm_Handler(u16 t_sel,u16 p_sel)
{
	read_dgus_vp(TIMER_SET_SPEED,(u8*)&timer_speed_mode_set,1);
	read_dgus_vp(TIMER_SET_TEMP,(u8*)&timer_temperature_set,1);
	if(timer_speed_mode_set==0)
	{
		Parm_Set_Error_Val=0x1005;
		write_dgus_vp(PARM_SET_ERROR,(u8*)&Parm_Set_Error_Val,1);		//风速设置错误，跳转页面
		return 1;
	}
	if(timer_temperature_set==0)
	{
		Parm_Set_Error_Val=0x1005;
		write_dgus_vp(PARM_SET_ERROR,(u8*)&Parm_Set_Error_Val,1);		//温度设置错误，跳转页面	
		return 2;
	}
	tim_per_val[t_sel][p_sel].timer_speed=timer_speed_mode_set;
	tim_per_val[t_sel][p_sel].time_temperture=timer_temperature_set*10;
	if(t_sel==0)
	{
		write_dgus_vp(TIMER1_TIME_SEED_MODE,(u8*)&tim_per_val[0][0],16);	//写入修改参数			
	}
	else if(t_sel==1)
	{
		write_dgus_vp(TIMER2_TIME_SEED_MODE,(u8*)&tim_per_val[1][0],16);
	}
	else if(t_sel==2)
	{
		write_dgus_vp(TIMER3_TIME_SEED_MODE,(u8*)&tim_per_val[2][0],16);
	}
	return 0;
}

void Timer_Speed_Mode_Set_Cancel_Handler(void)
{
	
}


/*****************************************************************************
 函 数 名  : void Save_Week_Handler(u16 t_sel)
 功能描述  : 保存星期处理函数
 输入参数  :	 t_sel：定时器选择
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月15日
    作    者   : chengjing
    修改内容   : 修改
*****************************************************************************/
void Save_Week_Handler(u16 t_sel)
{
	u8 i=0;
	read_dgus_vp(TIMER_WEEK_SET,(u8*)timer_week_set,7);
	for(i=0;i<7;i++)
	{
		if(timer_week_set[i]==1)
		{
			timer_week[t_sel][i]=1;
			tim_week_c[t_sel] |= (0x01<<i);
		}
		else
		{
			timer_week[t_sel][i]=0;
			tim_week_c[t_sel] &= ~(0x01<<i);
		}
	}
	write_dgus_vp(TIMER_WEEK_DISPLAY,(u8*)&timer_week[0][0],21);
	if(t_sel==0)
	{
		write_dgus_vp(TIMER1_WEEK,(u8*)&tim_week_c[0],1);
	}
	else if(t_sel==1)
	{
		write_dgus_vp(TIMER2_WEEK,(u8*)&tim_week_c[1],1);
	}
	else if(t_sel==2)
	{
		write_dgus_vp(TIMER3_WEEK,(u8*)&tim_week_c[2],1);
	}
}


void Timer_Select_Return_Handler(void)
{

}



void Temperature_Set_Enter_Init_Handler(void)
{
	read_dgus_vp(TEMP_SET,(u8*)&Temperature_Set_Val,1);
	Set_Temperature_Set_Val=Temperature_Set_Val/10;
	Set_Temperature_Set_Val_Dec=Temperature_Set_Val%10;
	write_dgus_vp(SET_TEMP_SET_VALUE,(u8*)&Set_Temperature_Set_Val,1);
	write_dgus_vp(SET_TEMP_SET_VAL_DEC,(u8*)&Set_Temperature_Set_Val_Dec,1);
}

void Temperature_Set_Confirm_Handler(void)
{
	read_dgus_vp(SET_TEMP_SET_VALUE,(u8*)&Set_Temperature_Set_Val,1);	
	read_dgus_vp(SET_TEMP_SET_VAL_DEC,(u8*)&Set_Temperature_Set_Val_Dec,1);
	if((Set_Temperature_Set_Val_Dec<=9)&&(Set_Temperature_Set_Val<=32)&&(Set_Temperature_Set_Val>=16))
	{
		Temperature_Set_Val=Set_Temperature_Set_Val*10+Set_Temperature_Set_Val_Dec;
	}
	else
	{
		return;
	}
	write_dgus_vp(TEMP_SET,(u8*)&Temperature_Set_Val,1);
	return;
}

void Temperature_Set_Cancel_Handler(void)
{
	
}

void Temperature_Set_Increase(void)
{
	u16 temp;
	read_dgus_vp(SET_TEMP_SET_VALUE,(u8*)&Set_Temperature_Set_Val,1);	
	read_dgus_vp(SET_TEMP_SET_VAL_DEC,(u8*)&Set_Temperature_Set_Val_Dec,1);
	temp=Set_Temperature_Set_Val*10+Set_Temperature_Set_Val_Dec+Temp_Pre;
	if(temp>Temperature_Upper)
	{
		temp=Temperature_Upper;
	}
	if(temp<Temperature_Lower)
	{
		temp=Temperature_Lower;
	}
	Set_Temperature_Set_Val=temp/10;
	Set_Temperature_Set_Val_Dec=temp%10;
	write_dgus_vp(SET_TEMP_SET_VALUE,(u8*)&Set_Temperature_Set_Val,1);	
	write_dgus_vp(SET_TEMP_SET_VAL_DEC,(u8*)&Set_Temperature_Set_Val_Dec,1);
}

void Tmeperature_Set_Decrease(void)
{
	u16 temp;
	read_dgus_vp(SET_TEMP_SET_VALUE,(u8*)&Set_Temperature_Set_Val,1);	
	read_dgus_vp(SET_TEMP_SET_VAL_DEC,(u8*)&Set_Temperature_Set_Val_Dec,1);
	temp=Set_Temperature_Set_Val*10+Set_Temperature_Set_Val_Dec-Temp_Pre;
	if(temp>Temperature_Upper)
	{
		temp=Temperature_Upper;
	}
	if(temp<Temperature_Lower)
	{
		temp=Temperature_Lower;
	}
	Set_Temperature_Set_Val=temp/10;
	Set_Temperature_Set_Val_Dec=temp%10;
	write_dgus_vp(SET_TEMP_SET_VALUE,(u8*)&Set_Temperature_Set_Val,1);	
	write_dgus_vp(SET_TEMP_SET_VAL_DEC,(u8*)&Set_Temperature_Set_Val_Dec,1);
}


void Time_Set_Init_Handler(void)
{
	RTC_Time_Set.year=real_time.year+25600;
	RTC_Time_Set.month=real_time.month+25600;
	RTC_Time_Set.day=real_time.day+25600;
	RTC_Time_Set.hour=real_time.hour+25600;
	RTC_Time_Set.min=real_time.min+25600;
	write_dgus_vp(TIME_SET_VALUE,(u8*)&RTC_Time_Set,5);
}

void Time_Set_Confirm_Handler(void)
{
	read_dgus_vp(TIME_SET_VALUE,(u8*)&RTC_Time_Set,5);
	RTC_Time_Set.year-=25600;
	RTC_Time_Set.month-=25600;
	RTC_Time_Set.day-=25600;
	RTC_Time_Set.hour-=25600;
	RTC_Time_Set.min-=25600;
	if(RTC_Time_Set.day>mon_table[RTC_Time_Set.month-1])
	{
		if(RTC_Time_Set.month==2)
		{
			if(Is_Leap_Year(RTC_Time_Set.year))
			{
				if(RTC_Time_Set.day>29)
				{
					RTC_Time_Set.day=mon_table[RTC_Time_Set.month-1]+1;
				}
			}
		}
		else
		{
			RTC_Time_Set.day=mon_table[RTC_Time_Set.month-1];
		}		
	}	
	if(RTC_Time_Set.month==0)
	{
		RTC_Time_Set.month=1;
	}
	if(RTC_Time_Set.day==0)
	{
		RTC_Time_Set.day=1;
	}
	time_calibra[0]=0x5A;
	time_calibra[1]=0xA5;
	time_calibra[2]=RTC_Time_Set.year;
	time_calibra[3]=RTC_Time_Set.month;
	time_calibra[4]=RTC_Time_Set.day;
	time_calibra[5]=RTC_Time_Set.hour;
	time_calibra[6]=RTC_Time_Set.min;
	time_calibra[7]=0;
// 	write_dgus_vp(RTC_Set,(u8*)time_calibra,4);	
	RTC_Reset(time_calibra);
				Return_Main_page();
}


void Time_Set_Cancel_Handler(void)
{
	
				Return_Main_page();
}




void Alarm_Set_Init_Handler(void)
{
	read_dgus_vp(ALARM_VAL,(u8*)&alarm_val,4);
	Alarm_Display[0]=alarm_val.alarm1_time/256+100;
	Alarm_Display[1]=alarm_val.alarm1_time%256+100;
	Alarm_Display[2]=alarm_val.alarm2_time/256+100;
	Alarm_Display[3]=alarm_val.alarm2_time%256+100;
	write_dgus_vp(ALARM_DISPLAY_TIME,(u8*)Alarm_Display,4);
}

void Alarm_Set_Handler(u16 alarm_sel)
{
	read_dgus_vp(ALARM_VAL,(u8*)&alarm_val,4);
	if(alarm_sel==0x01)
	{
		alarm_set_val.alarm_time_hour=alarm_val.alarm1_time/256+25600;
		alarm_set_val.alarm_time_min=alarm_val.alarm1_time%256+25600;
		alarm_set_val.alarm_ring=alarm_val.alarm1_ring;
	}
	else if(alarm_sel==0x02)
	{
		alarm_set_val.alarm_time_hour=alarm_val.alarm2_time/256+25600;
		alarm_set_val.alarm_time_min=alarm_val.alarm2_time%256+25600;
		alarm_set_val.alarm_ring=alarm_val.alarm2_ring;
	}
	write_dgus_vp(ALARM_SET_VAL,(u8*)&alarm_set_val,3);	
}

void Alarm_Set_Confirm_Handler(u16 alarm_sel)
{
	read_dgus_vp(ALARM_SET_VAL,(u8*)&alarm_set_val,3);	
	if(alarm_sel==0x01)
	{
		alarm_val.alarm1_time=(alarm_set_val.alarm_time_hour-25600)*256+(alarm_set_val.alarm_time_min-25600);
		alarm_val.alarm1_ring=alarm_set_val.alarm_ring;
	}
	else if(alarm_sel==0x02)
	{
		alarm_val.alarm2_time=(alarm_set_val.alarm_time_hour-25600)*256+(alarm_set_val.alarm_time_min-25600);
		alarm_val.alarm2_ring=alarm_set_val.alarm_ring;
	}
	write_dgus_vp(ALARM_VAL,(u8*)&alarm_val,4);
}


void Alarm_Set_Cancel_Handler(void)
{
	
}

/*****************************************************************************
 函 数 名  : void Message_Handler(void)
 功能描述  : 消息处理函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月30日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Message_Handler(void)
{
	Message_Status=0;
	write_dgus_vp(MESSAGE_STATUS,(u8*)&Message_Status,1);
	
				Return_Main_page();
}


void Shut_Down_Handler(void)
{	
	Write_Nor_Flash();
	POWER_OFF();
}



void Speed_Auto_Set_Handler(void)
{
	Wind_Speed=4;
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
}


void Speed_Low_Set_Handler(void)
{
	Wind_Speed=1;
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
}

void Speed_Medium_Set_Handler(void)
{
	Wind_Speed=2;
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
}


void Speed_High_Set_Handler(void)
{
	Wind_Speed=3;
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
}


void Speed_Add_Set_Handler(void)
{
	Wind_Speed++;
	if(Wind_Speed>4)
	{
		Wind_Speed=1;
	}
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
}


void Speed_Decrease_Set_Handler(void)
{
	Wind_Speed--;
	if(Wind_Speed<1)
	{
		Wind_Speed=4;
	}
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);
}



void Mode_Add_Set_Handler(void)
{
	Mode++;
	if(Mode>3)
	{
		Mode=1;
	}
	write_dgus_vp(MODE,(u8*)&Mode,1);
}

void Asc_Input_Handler(u16 num)
{
	u8 i;
	Asc_num = num&0x00FF;
// 	Asc_num = 12;
	write_dgus_vp(0x3200,(u8*)&num,1);
	for(i=0;i<25;i++)
	{
		input_Ascdata[i] = 0;
	}
}





