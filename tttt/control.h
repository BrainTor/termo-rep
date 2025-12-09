#ifndef __CONTROL_H
#define __CONTROL_H


#include "sys.h"


#define  TEST_VP1  0x1100
#define  TEST_VP2  0x1101
#define  TEST_VP3  0x1102
#define  TEST_VP4  0x1106
#define  SPEAK_VP  0x00A0


typedef struct _timer_period
{
	u16 timer_time_start;
	u16 timer_time_end;
	u16 timer_speed;
	u16 time_temperture;
}timer_period;








extern u16	Temper_Display_Val;
extern u16	Temper_Display_Val_Dec;
extern u16 	Set_Temper_Display_Val;
extern u16 	Set_Temper_Display_Val_Dec;
extern u16 	Temperature_Val_Old;
extern u16	Set_Temperature_Val_Old;
extern u16 	timer_key[3];
extern timer_period	tim_per_val[3][4];
extern u16 	tim_week_c[3];
extern u16 	timer_week[3][7];
extern u16 	tim_k;
extern u16 	Hour_Min_R;
extern u16 	Timer_Parm_Read_Flag;
extern u8  	timer_trigger_flag[3];
extern u16 	data Wind_Speed;
extern u16 data Wind_Speed_Old;
extern u16 	data Wind_Speed_Val;
extern u16 	Wind_Speed_Old_Val;
extern u16 	data Mode;
extern u16 data Mode_old;
extern u16 	data Mode_Val;
extern u16 	Mode_Old_Val;
extern u16	Wind_Speed_Config_Flag;
extern u16	Mode_Config_Flag;
extern u16	Wind_Speed_Mode_Config_Flag;
extern u16  xdata Temperature_To_Set_Flag;
extern u16	xdata Wind_Set_Flag;
extern u16	xdata Mode_Set_Flag;

extern  u16 movie_icon;
extern  u16 PageNow;






void Control_Function(void);
void Display_Temperature(void);
void Timer_Config_Handler(void);
void TC_Status_Check_Handler(void);
void Wind_Speed_Val_Handler(void);
void Wind_Speed_Control_Handler(void);
void Mode_Val_Handler(void);
void Mode_Control_Handler(void);
void Mode_Temperature_Handler(void);
void Mode_Wind_Speed_Handler(void);

void M4G6_dataRx(void);
void  uart2com(void);
void M4G6_DataDeal(u8 dat);













#endif

