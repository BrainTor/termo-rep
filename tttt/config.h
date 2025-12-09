#ifndef __CONFIG_H
#define __CONFIG_H

#include "sys.h"




typedef enum
{
	STANDBY_BOOT=0x0010,
	SHUT_DOWN_CONFIRM,
	SYSTEM_SET_ENTER=0x0100,
	PASSWORD_INPUT_CONFIRM,
	PASSWORD_INPUT_CANCEL,
	RESET_CONFIRM,
	RESET_CANCEL,
	SYSTEM_SET_RETURN,
	ALARM_SELECT_RETURN,
	ADVANCE_SET_RETURN,
	
	TIMER_SET_ENTER=0x0200,
	TIMER1_SET_ENTER,
	TIMER2_SET_ENTER,
	TIMER3_SET_ENTER,
	TIMER_SET_PERIOD1,
	TIMER_SET_PERIOD2,
	TIMER_SET_PERIOD3,
	TIMER_SET_PERIOD4,
	TIMER_TIME_SET_CONFIRM,
	TIMER_TIME_SET_CANCEL,
	TIMER_SPEED_MODE_SET_CONFIRM,
	TIMER_SPEED_MODE_SET_CANCEL,
	TIMER_WEEK_RETURN,
	TIMER_SELECT_RETURN,
	
	TEMPERATURE_SET_ENTER=0x0300,
	TEMPERATURE_SET_CONFIRM,
	TEMPERATURE_SET_CANCEL,
	TEMPERATURE_SET_INCREASE,
	TEMPERATURE_SET_DECREASE,
	
	TIME_SET_ENTER=0x0400,
	TIME_SET_CONFIRM,
	TIME_SET_CANCEL,
	
	ALARM_SET_ENTER=0x0500,
	ALARM1_SET_ENTER,
	ALARM2_SET_ENTER,
	ALARM_SET_CONFIRM,
	ALARM_SET_CANCEL,
	
	MESSAGE_READ_CONFIRM=0x0601,
	
	SHUT_DOWN=0x0701,
	RETURN_MAIN,
	
	SPEED_AUTO=0x0800,
	SPEED_LOW,
	SPEED_MIDIUM,
	SPEED_HIGH,
	SPEED_ADD=0x0810,
	SPEED_DECREASE,
	
	MODE_ADD=0x901,
	
	ASC_PUTIN1 =0x0A00,
	ASC_PUTIN2,
}Enum_Touch_Event;



typedef struct _time_set
{
	u16 year;
	u16 month;
	u16 day;
	u16 hour;
	u16 min;
}RTC_Time_Set_Val;


typedef struct _alarm_set
{
	u16 alarm_time_hour;
	u16 alarm_time_min;
	u16 alarm_ring;
}alarm_set;





extern u8 	code Parm_Upload_Array[6];
extern u8 	Parm_Change_Flag;
extern u16	data Touch_Event_Flag;
extern u32 	password_val;
extern u16 	t_sel;
extern u16 	p_sel;
extern u16 	timer_set_period[4];
extern u16 	Hour_Min_Set;
extern u16 	Parm_Set_Error_Val;
extern u16 	timer_week_set[7];
extern u16 	Set_Temperature_Set_Val;
extern u16 	Set_Temperature_Set_Val_Dec;
extern RTC_Time_Set_Val RTC_Time_Set;
extern u16 	Alarm_Display[4];
extern u16 	Alarm_Sel;
extern alarm_set	alarm_set_val;
extern u16 	Message_Status;
extern u16	Warning_Val;
extern u16 timer_period_display[8];
extern u16 timer_speed_mode_set;
extern u16 timer_temperature_set;
extern data u16  Asc_num;



void Parm_Set_Function(void);
void Parm_Upload_Handler(void);
void Standby_Boot_Handler(void);
void Shut_Down_Confirm_Handler(void);
void System_Set_Enter_Handler(void);
void Password_Input_Confirm_Handler(void);
void Password_Input_Cancel_Handler(void);
void Reset_Confirm_Handler(void);
void Reset_Cancel_Handler(void);
void System_Set_Return_Handler(void);
void Alarm_Select_Return_Handler(void);
void Advance_Set_Rerurn_Handler(void);
void Week_Val(u16 time_sel);
void Timer_Week_Display(void);
void Timer_Sel_Init_Handler(u16 t_sel);
void Timer_Period_Sel_Init_Handler(u16 t_sel,u16 p_sel);
u8 	 Timer_Time_Set_Confirm_Handler(u16 t_sel,u16 p_sel);
void Timer_Time_Set_Cancel_Handler(void);
u8 	 Timer_Speed_Mode_Set_Confirm_Handler(u16 t_sel,u16 p_sel);
void Timer_Speed_Mode_Set_Cancel_Handler(void);
void Save_Week_Handler(u16 t_sel);
void Timer_Select_Return_Handler(void);
void Temperature_Set_Enter_Init_Handler(void);
void Temperature_Set_Confirm_Handler(void);
void Temperature_Set_Cancel_Handler(void);
void Temperature_Set_Increase(void);
void Tmeperature_Set_Decrease(void);
void Time_Set_Init_Handler(void);
void Time_Set_Confirm_Handler(void);
void Time_Set_Cancel_Handler(void);
void Alarm_Set_Init_Handler(void);
void Alarm_Set_Handler(u16 alarm_sel);
void Alarm_Set_Confirm_Handler(u16 alarm_sel);
void Alarm_Set_Cancel_Handler(void);
void Message_Handler(void);
void Shut_Down_Handler(void);
void Speed_Auto_Set_Handler(void);
void Speed_Low_Set_Handler(void);
void Speed_Medium_Set_Handler(void);
void Speed_High_Set_Handler(void);
void Speed_Add_Set_Handler(void);
void Speed_Decrease_Set_Handler(void);
void Mode_Add_Set_Handler(void);

void Return_Main_page(void);


void Asc_Input_Handler(u16 num);













#endif



