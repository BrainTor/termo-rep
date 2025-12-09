/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : sys.c
  版 本 号   : V2.0
  作    者   : chengjing
  生成日期   : 2019年4月1日
  功能描述   : 温控器数据采集和逻辑控制
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "sys.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"
#include "control.h"
#include "TP_draw.h"
#include "timer.h"
#include "RTC.h"
#include "uart.h"



const u16 code TabNTC_10k[TempSize]={
	32040,30490,29022,27633,26317,25071,23889,22769,21707,20700,19788,18838,17977,17160,16383,15646,14945,14280,13647,13045,
	12472,11928,11409,10916,10447,10000,9574,9168,8781,8413,8062,7727,7407,7103,6812,6534,6270,6017,5775,5545,
	5324,5114,4913,4720,4536,4360,4192,4031,3877,3730,3572,3454,3324,3201,3082,2968,2859,2755,2654,2558,
	2466,2378,2293,2212,2134,2059,1987,1918,1851,1788,1726,1668,1611,1557,1504,1454,1406,1359,1314,1271,
	1230,1190,1151,1114,1079,1045,1011,980,949,919,891,863,837,811,786,763,740,718,696,675
};
const u16 code TabNTC2_10k[TempSize]={
	32800,31141,29572,28090,26690,25366,24116,22935,21820,20767,19773,18835,17949,17113,16323,15578,14874,14208,13579,12983,
	12419,11885,11378,10896,10437,10000,9513,9064,8649,8263,7904,7570,7256,6960,6682,6420,6170,5932,5707,5491,
	5284,5085,4895,4712,4536,4367,4204,4047,3896,3750,3610,3475,3346,3221,3101,2987,2876,2771,2670,2573,
	2480,2391,2307,2226,2148,2074,2004,1936,1871,1809,1750,1693,1586,1535,1486,1439,1393,1348,1305,1262,
	1220,1178,1137,1097,1056,1023,991,960,931,903,876,850,825,801,777,755,733,713,692,673
};

const u8 code table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表

const u8 code time_set_init[6]={19,5,1,12,00,00};

const u8 code mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//读写Nor Flash命令
u8 code read_flash[8]={0x5A,0x00,0x00,0x00,0x25,0xE0,0x01,0x20};
u8 code write_flash[8]={0xA5,0x00,0x00,0x00,0x25,0xE0,0x01,0x20};
//读写Nor Flash后查询状态
u8 read_flash_status[8]={0};
//软件版本号
u16	code Soft_Ver_Num=21;
u8 	code Soft_Ver[]="V2.1";
//魔数，nor flash读取标志位
u16 Magic_Number=0;
//计算key2延时
u16 data Key_Count=0;
//息屏
u8 	code led_off[4]={0x00,0x00,0x00,0x64};
//等待计时
u16	data Wait_Count=0;
//休眠标志位  0：正常，1：自动休眠,2：电源按键息屏
u16 Sleep_Flag=0;
//闹钟选择
u16 Alarm_Select=0;
//闹钟时间铃声
alarm	alarm_val;
//闹钟开关
u16	Alarm_Key[2]={0};
//实时时分
u16 Real_Hour_Min=0;
//KEY按下关机标志，关继电器息屏
u8 	Close_Valve_Flag=0;
//继电器配置标志
u16	Valve_Config_Flag=0;
//T1定时器计数
static u16 data SysTick_RTC=0;
//T2定时器计数
static u16 data SysTick=0;	
//串口2接收超时定时器
u16 data Uart2RxCt=0;	
//串口3接收超时定时器
u16 data Uart3RxCt=0;	
//串口4接收超时定时器
u16 data Uart4RxCt=0;	
//串口5接收超时定时器
u16 data Uart5RxCt=0;	
//休眠时间计数
u16 Sleep_Count=0;
u16 Sleep_Count_M=0;
//是否开机标志位
u8 	Power_Flag=0;	
//页面切换
u8 	code page_set_to_0[4]={0x5A,0x01,0x00,0x00};	
//TP status
u8 	TP_Status_Old[8]={0};
u8 	TP_Status_New[8]={0};
//背光调节数组
u16 data led_new=0;
u16 led_old=0;
u16 led_icon = 0;
u8 	led_on[4]={0x64,0x00,0x00,0x64};
//AD延时计数
u16 AD_Count=0;
//adc采样值
u16 adc_value[3]={0};
//ADC数值暂存
u16 adc_val[8]={0};
//电阻值
u16 R1_value=0;		//ADC7处NTC电阻
u16 R2_value=0;		//ADC0处NTC电阻
//温度值
u16 R1_Temperature=0;
u16 R2_Temperature=0;
//温度值  由adc采用计算的处
u16 Temperature_Real=0;	
u16	Temperatrue_Real_Old=0;
//屏保时间，类型
u16 xdata sleep_time=0;
u16 xdata Sleep_Type=1;
u8	xdata page_set[4]={0x5A,0x01,0x00,0x3C};
u16 screen_count=0;
u8 	xdata Screen_Flag=0;
//闹钟定时器状态，首页右上角图标显示
u16 Alarm_Status=0;
u16 Timer_Status=0;
//保存Nor Flash时间计数
u16 Parm_Change_Count=0;
u16 Parm_Change_Count_Val=0;
//电源按键状态值
u8	Sleep_Val=0;
u8	Shut_Dwon_Flag=0;
//风阀水阀状态
u16	Water_Valve_Status=0;
u16 Speed_Valve_Status=0;
//远程更新
u16	Remote_Update_Status=0;
//设定温度值
u16 Temperature_Set_Val=0;
//温度精度标志和温度精度
u16	Temperature_Precision=0;
u16	Temp_Pre=0;
//温度上下限
u16 Temperature_Upper=0;
u16 Temperature_Lower=0;
//温度系数
short Temp_Coef[3]={0};
//时间更新
u16 time_display[7]={0};
//秒标志位
u16 Second_Updata_Flag=0;
//时间校准
u8 time_calibra[8]={0};
//按键状态
u16 Key_Status=0;
u16 Key_Value=0;
//温控器状态
u16	TC_Status=0;
u16	TC_Status_Old=0;
//切换页面
u16	Page_Change[10]={0};
//屏保参数
u16	Screen_Saver_Parm[3]=0;
//人体接近感应功能
u16	ALS_Function=0;
//年参数
u16	xdata year_real=0; 
u8 	xdata yearH=0;
u8 	xdata yearL=0;
//实时数据结构体
rtc_time	real_time;
//温度采样时间和次数
u16 Temp_time=0;
u16	Temp_Freq=0;
//信息控件清空
u16 Message_Val=0xFFFF;
//页面切换变量值
u16	Page_Change_Val=0;
//密码
u32 Password=0;
//当前页面
u16 PageNow = 0;
//语音计时
u16 M4G6_ct = 0;






/*****************************************************************************
 函 数 名  : void INIT_CPU(void)
 功能描述  : CPU初始化函数
			根据实际使用外设修改或单独配置
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月1日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void INIT_CPU(void)
{
    EA=0;
    RS0=0;
    RS1=0;

//     CKCON=0x00;
//     T2CON=0x70;
//     DPC=0x00;
//     PAGESEL=0x01;
//     D_PAGESEL=0x02;   //DATA RAM  0x8000-0xFFFF
    MUX_SEL=0x60;   //UART2,UART2开启，WDT关闭
    RAMMODE=0x00;
    PORTDRV=0x01;   //驱动强度+/-8mA
    IEN0=0x00;      //关闭所有中断
    IEN1=0x00;
    IEN2=0x00;
    IP0=0x00;      //中断优先级默认
    IP1=0x00;

    WDT_OFF();      //关闭开门狗

    //IO口配置
    P0=0x00;
    P1=0x00;
    P2=0x00;
    P3=0x00;
    P0MDOUT=0x10;
    P1MDOUT=0x00;
    P2MDOUT=0x00; 
    P3MDOUT=0x00;
	
	//UART2配置8N1  115200       有倍频，和UART3不一样
    ADCON=0x80;
    SCON0=0x50;
    SREL0H=0x03;        //FCLK/64*(1024-SREL1)
    SREL0L=0xE4;
		
	//UART3配置8N1  115200 
		SCON1=0xD0;				//8N1
//     SCON1=0xd0;				//偶校验
//     SCON1=0xdc;				//奇校验
    SREL1H=0x03;        //FCLK/64*(1024-SREL1)
    SREL1L=0xC8;
		
    //UART4配置8N1      115200
    SCON2T=0x80;
    SCON2R=0x80;
    BODE2_DIV_H=0x00;     //FCLK/8*DIV
    BODE2_DIV_L=0xE0;

    //UART5配置8N1      115200
    SCON3T=0x80;
    SCON3R=0x80;
    BODE3_DIV_H=0x00;       //FCLK/8*DIV
    BODE3_DIV_L=0xE0;

    
    TMOD=0x11;          //16位定时器
    //T0
    TH0=0x00;
    TL0=0x00;
    TR0=0x00;

    //T1
    TH1=0x00;
    TL1=0x00;
    TR1=0x00;
    

    //T2  Autoload模式
    T2CON=0x70;
    TH2=0x00;
    TL2=0x00;
    TRL2H=0xBC;
    TRL2L=0xCD;        //1ms的定时器
}

/*****************************************************************************
 函 数 名  : void PORT_Init(void)
 功能描述  : 端口初始化函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月1日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void PORT_Init(void)
{
	P0MDOUT |= 0x02;	//初始化P0.1为输出，485控制
	P1MDOUT |= 0x1E;	//初始化P1.1 P1.2 P1.3 P1.4为输出
	P2MDOUT |= 0x02;	//初始化P2.1为输出
}


/*****************************************************************************
 函 数 名  : void RTC_Init(void)
 功能描述  : RTC时间初始化
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
// void RTC_Init(void)
// {
// 	real_time.year=time_set_init[0];
// 	real_time.month=time_set_init[1];
// 	real_time.day=time_set_init[2];
// 	real_time.hour=time_set_init[3];
// 	real_time.min=time_set_init[4];
// 	real_time.sec=time_set_init[5];
// 	write_dgus_vp(0x0010,(u8*)&real_time.year,4);
// }


/*****************************************************************************
 函 数 名  : void System_Parm_Init(void)
 功能描述  : 系统参数初始化，读取保存Nor Flash区域和22.bin文件系统参数，并配置软件
			版本号等参数。
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月16日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void System_Parm_Init(void)
{
	Read_Nor_Flash();		//读Nor Flash
	delay_ms(5);
	read_dgus_vp(MAGIC_NUMBER,(u8*)&Magic_Number,1);	
	if(Magic_Number!=0x5AA5)
	{
		Magic_Number=0x5AA5;
		write_dgus_vp(MAGIC_NUMBER,(u8*)&Magic_Number,1);
		Parm_Reset_Init();
		Write_Nor_Flash();
		delay_ms(5);
	}
	write_dgus_vp(SOFT_VER_ADDR,(u8*)Soft_Ver,(sizeof(Soft_Ver)/2+1));		//写软件版本
	TC_Status=1;
	write_dgus_vp(SWITCH,(u8*)&TC_Status,1);								//写入温控器状态
	Remote_Update_Status=0;
	write_dgus_vp(REMOTE_UPDATE,(u8*)&Remote_Update_Status,1);				//远程更新无
	read_dgus_vp(PASSWORD_ADDR,(u8*)&Password,2);		//读温度设置数
	if(Password==0)
	{
		Password=666666;
	}	
	read_dgus_vp(TEMPERATURE_PRECISION,(u8*)&Temperature_Precision,1);		//读温度设置数
	read_dgus_vp(TEMP_SET,(u8*)&Temperature_Set_Val,1);
	Set_Temper_Display_Val=Temperature_Set_Val/10;
	Set_Temper_Display_Val_Dec=Temperature_Set_Val%10;
	write_dgus_vp(SET_TEMP_VALUE,(u8*)&Set_Temper_Display_Val,1);
	write_dgus_vp(SET_TEMP_VAL_DEC,(u8*)&Set_Temper_Display_Val_Dec,1);		//写温度设置值
	if((Temperature_Precision!=0)&&(Temperature_Precision!=1))
	{
		Temperature_Precision=1;
	}
	if(Temperature_Precision==0)
	{
		Temp_Pre=5;		//方便测试，后续修改为5
	}
	else if(Temperature_Precision==1)
	{
		Temp_Pre=10;
	}
	read_dgus_vp(TEMPERATURE_UPPER,(u8*)&Temperature_Upper,1);				//读温度设置上下限
	read_dgus_vp(TEMPERATURE_LOWER,(u8*)&Temperature_Lower,1);
	if((Temperature_Upper==0)||(Temperature_Lower==0)||(Temperature_Upper<=Temperature_Lower))
	{
		Temperature_Upper=320;
		Temperature_Lower=160;
	}
	read_dgus_vp(TEMPERATURE_COEFFICIENT,(u8*)Temp_Coef,3);					//读温度系数
	if((Temp_Coef[0]<=0)||(Temp_Coef[1]<=0)||(Temp_Coef[0]!=Temp_Coef[1]+10))
	{
		Temp_Coef[0]=10;
		Temp_Coef[1]=0;
		Temp_Coef[2]=-600;
	}
	read_dgus_vp(CHANGE_PAGE,(u8*)Page_Change,10);			//读取切换页面参数
	read_dgus_vp(SCREEN_SAVER,(u8*)Screen_Saver_Parm,3);	//读取屏保页面参数
	read_dgus_vp(ALS,(u8*)&ALS_Function,1);				//是否有人体接近感应功能
	Message_Val=0xFFFF;
	write_dgus_vp(MESSAGE_DATA,(u8*)&Message_Val,1);
	read_dgus_vp(TIMER1_WEEK,(u8*)&tim_week_c[0],1);
	read_dgus_vp(TIMER1_TIME_SEED_MODE,(u8*)&tim_per_val[0][0],16);	
	Week_Val(0);
	read_dgus_vp(TIMER2_WEEK,(u8*)&tim_week_c[1],1);
	read_dgus_vp(TIMER2_TIME_SEED_MODE,(u8*)&tim_per_val[1][0],16);	
	Week_Val(1);	
	read_dgus_vp(TIMER3_WEEK,(u8*)&tim_week_c[2],1);
	read_dgus_vp(TIMER3_TIME_SEED_MODE,(u8*)&tim_per_val[2][0],16);
	Week_Val(2);
	RTC_init();
// 	P8563_init();							//时间初始化，防止出现不正确的时间
	Temperature_To_Set_Flag=0;			//初始化值
	Wind_Speed_Config_Flag=0;			//风速设置
	Mode_Config_Flag=0;					//模式设置
	Close_Valve_Flag=0;					//开继电器
	Power_Flag=0;
	TC_Status=2;
	Temp_time=200;
	Temp_Freq=10;
	Temperature_Real=260;
	write_dgus_vp(TEMP_CURRENT,(u8*)&Temperature_Real,1);
	Temperatrue_Real_Old=Temperature_Real;
	read_dgus_vp(PIC_NOW,(u8*)&PageNow,1);
	if(PageNow==27)
	{
		Return_Main_page();
	}
	
	movie_icon = 1;
	write_dgus_vp(0x2702,(u8*)&movie_icon,1);			//首页动图
}






/*****************************************************************************
 函 数 名  : void Read_Nor_Flash(void)
 功能描述  : 读nor flash系统参数，固定为25E0-26FF的值
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Read_Nor_Flash(void)
{	
	write_dgus_vp(NOR_FLASH,(u8*)read_flash,4);
	do
	{
		delay_ms(5);
		read_dgus_vp(NOR_FLASH,(u8*)read_flash_status,4);
	}while(read_flash_status[0]!=0);
}


/*****************************************************************************
 函 数 名  : void Write_Nor_Flash(void)
 功能描述  : 写nor flash系统参数，固定为25E0-26FF的值
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Write_Nor_Flash(void)
{
	write_dgus_vp(NOR_FLASH,(u8*)write_flash,4);
	do
	{
		delay_ms(5);
		read_dgus_vp(NOR_FLASH,(u8*)read_flash_status,4);		
	}while(read_flash_status[0]!=0);
}


/*****************************************************************************
 函 数 名  : void Save_Data_Handler(void)
 功能描述  : 保存nor flash的值，参数设置完30s后保存，避免短时间重复保存
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Save_Data_Handler(void)
{
	if((Parm_Change_Flag&0x01)==0x01)
	{
		if((Parm_Change_Flag&0x10)==0x10)
		{
			if(Parm_Change_Count>5000)
			{
				Parm_Change_Count_Val++;
				if(Parm_Change_Count_Val>6)
				{
					Write_Nor_Flash();
					Parm_Change_Flag=0;
					Parm_Change_Count_Val=0;
				}
			}
		}
		else
		{
			Parm_Change_Flag |= 0x10;
			Parm_Change_Count=0;
			Parm_Change_Count_Val=0;
		}
	}
}





/*****************************************************************************
 函 数 名  : void Key_Handler(void)
 功能描述  : 电源开关处理函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月17日
    作    者   : chengjing
    修改内容   : 物理按键驱动，往按键驱动值里面写值
				2000：D1:5A:按键有效		D0:01：单次，10：长按（3S）
				2001:0101：KEY1,0102:KEY2:0103:KEY3...
*****************************************************************************/ 
void Key_Handler(void)
{
	if(Power_Flag==1)
	{
		if(KEY1_Vin_F==0)
		{
			delay_ms(10);			//消抖
			if(KEY1_Vin_F==0)
			{
				Key_Count=0;
				while(1)
				{
					if(KEY1_Vin_F==1)
					{
						break;
					}
					delay_ms(1);
				}
				if(Key_Count<3000)
				{
					Key_Status=0x5A01;
				}
				else
				{
					Key_Status=0x5A10;					
				}
				Key_Value=0x0101;
				write_dgus_vp(KEY_STATUS,(u8*)&Key_Status,1);
				write_dgus_vp(KEY_VALUE,(u8*)&Key_Value,1);
				Key_Scheduling_Event_Handler();
			}
		}
	}
	else
	{
		if(KEY1_Vin_F==1)
		{
			delay_ms(10);
			if(KEY1_Vin_F==1)
			{
				Power_Flag=1;
			}
		}
	}
}

/*****************************************************************************
 函 数 名  : void Key_Scheduling_Event_Handler(void)
 功能描述  : 物理按键调度时间处理，对物理按键键值和当前页面处理实际功能
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Key_Scheduling_Event_Handler(void)
{
	read_dgus_vp(KEY_STATUS,(u8*)&Key_Status,1);
	if((Key_Status&0xFF00)==0x5A00)
	{
		read_dgus_vp(KEY_VALUE,(u8*)&Key_Value,1);
		if(((Key_Status&0xFF)==0x01)&&(Key_Value==0x0101))
		{
			Touch_Event_Flag=0x0010;
			write_dgus_vp(TOUCH_EVENT_FLAG,(u8*)&Touch_Event_Flag,1);
		}
		else if(((Key_Status&0xFF)==0x10)&&(Key_Value==0x0101))
		{
			Touch_Event_Flag=0x0011;
			write_dgus_vp(TOUCH_EVENT_FLAG,(u8*)&Touch_Event_Flag,1);
		}
	}
}


/*****************************************************************************
 函 数 名  : void Enter_Main_Page(void)
 功能描述  : 进入主页面，回复亮度，开继电器
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Enter_Main_Page(void)
{
// 	page_set[0]=0x5A;
// 	page_set[1]=0x01;
// 	page_set[2]=0x00;
// 	switch (Mode)
// 	{
// 		case 1: page_set[3]= 19;break;		//制冷
// 		case 2: page_set[3]= 10;break;		//制热
// 		case 3: page_set[3]= 1;break;			//送风
// 	}
// 	write_dgus_vp(PIC_SET,page_set,2);
	Return_Main_page();
// 	write_dgus_vp(PIC_SET,page_set_to_0,2);		//进入页面0
	read_dgus_vp(SCREEN_BRIGHTNESS,(u8*)&led_new,1);
	led_on[0]=led_new&0xFF;
	write_dgus_vp(LED_CONFIG,led_on,2);			//恢复亮度
	Close_Valve_Flag=0;							//开继电器	
}


/*****************************************************************************
 函 数 名  : void Standby_Handler(void)
 功能描述  : 待机处理，熄屏，关继电器
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Standby_Handler(void)
{
	write_dgus_vp(LED_CONFIG,led_off,2);
	Close_Valve_Flag=0x01;
	Valve_Config_Flag=0;
	TC_Status=2;
	TC_Status_Old=TC_Status;
	write_dgus_vp(SWITCH,(u8*)&TC_Status,1);	//温控器状态休眠
}

/*****************************************************************************
 函 数 名  : void Boot_Handler(void)
 功能描述  : 从待机到开机，进入首页，开继电器
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Boot_Handler(void)
{	
	Enter_Main_Page();
	Sleep_Flag=0;
	Sleep_Count=0;				//休眠计数清零
	Sleep_Count_M=0;
	Screen_Flag=0;				//首次进入屏保标志	
	Wind_Speed_Old_Val=0;
	Mode_Old_Val=0;
	TC_Status=1;
	TC_Status_Old=TC_Status;
	write_dgus_vp(SWITCH,(u8*)&TC_Status,1);
}



/*****************************************************************************
 函 数 名  : void Close_Valve(void)
 功能描述  : 关闭继电器函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void Close_Valve(void)
{
	if((Close_Valve_Flag&0x01)==0x01)
	{
		if(Valve_Config_Flag!=1)
		{
			if((Close_Valve_Flag&0x10)==0x10)
			{
				if(Wait_Count>1000)
				{
					Low_Speed_Valve=0;
					Medium_Speed_Valve=0;
					High_Speed_Valve=0;
					Speed_Valve_Status=0;
					write_dgus_vp(SPEED_VALVE,(u8*)&Speed_Valve_Status,1);
				}
				if(Wait_Count>2000)
				{
					Water_Valve=0;
					Water_Valve_Status=0;
					write_dgus_vp(WATER_VALVE,(u8*)&Water_Valve_Status,1);
					Valve_Config_Flag=1;
				}
			}
			else
			{
				Wait_Count=0;
				Close_Valve_Flag|=0x10;
			}
		}
	}
}

/*****************************************************************************
 函 数 名  : void Sleep_Handler(void)
 功能描述  : 休眠处理函数
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void Sleep_Handler(void)
{
	char temp=0;
	read_dgus_vp(TP_STATUS,TP_Status_New,4);	
	temp=strcmp(TP_Status_New,TP_Status_Old);
	if(temp==0)		//无触摸
	{
		if(Sleep_Flag==0)
		{	
			read_dgus_vp(PIC_NOW,(u8*)&PageNow,1);
			if(PageNow == 30)
			{
				if(M4G6_ct==0)
				{
					Return_Main_page();
				}
			}
			read_dgus_vp(SCREEN_SAVER_TIME,(u8*)&sleep_time,1);
			if(sleep_time==1)				//时间超过1min	
			{
				if(Sleep_Count>=60000)		
				{
					Sleep_Flag=1;
				}
			}
			else if(sleep_time==2)			//时间超过5min
			{
				if(Sleep_Count_M>=5)	
				{					
					Sleep_Flag=1;
				}
				else
				{
					if(Sleep_Count>=60000)			
					{
						Sleep_Count_M++;
						Sleep_Count=0;
					}
				}
			}
			else if(sleep_time==3)
			{
				//永不休眠
			}
			if(Sleep_Flag==1)
			{
				read_dgus_vp(SCREEN_SAVER_TYPE,(u8*)&Sleep_Type,1);
				Parm_Upload_Handler();
			}
		}
		if(Sleep_Flag==1)
		{			
			Screen_Saver_Handler(Sleep_Type);			//进入屏保
		}
	}
	else		//有触摸
	{
			read_dgus_vp(PIC_NOW,(u8*)&PageNow,1);
			if(PageNow == 30)
			{
				M4G6_ct = 10000;
			}
		memcpy(TP_Status_Old,TP_Status_New,sizeof(TP_Status_New));		
		if(Sleep_Flag==1)	//休眠模式或者屏保模式
		{
			Enter_Main_Page();													
		}
		Sleep_Flag=0;
		Sleep_Count=0;				//休眠计数清零
		Sleep_Count_M=0;
		Screen_Flag=0;				//首次进入屏保标志	
		Close_Valve_Flag=0;	
		TC_Status=1;
		write_dgus_vp(SWITCH,(u8*)&TC_Status,1);	//温控器正常状态
	}
}



/*****************************************************************************
 函 数 名  : void Parm_Set_Error_Handler(void)
 功能描述  : 参数设置错误处理函数
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月26日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void Parm_Set_Error_Handler(void)
{
	u8 i=0;
	read_dgus_vp(PARM_SET_ERROR,(u8*)&Parm_Set_Error_Val,1);
	if(Parm_Set_Error_Val>0)
	{
		
		switch(Parm_Set_Error_Val)
		{
			case	0x1001:
				i=0;
				break;
			case	0x1002:
				i=1;
				break;
			case	0x1003:
				i=2;
				
				break;
			case	0x1004:	
				i=3;
				break;
			case	0x1005:	
				i=4;
				break;
			case	0x1006:
				i=5;
				break;
			case	0x1007:
				i=6;				
				break;
			default:
				break;
		}
		Page_Change_Handler(i);
		Parm_Set_Error_Val=0;
		write_dgus_vp(PARM_SET_ERROR,(u8*)&Parm_Set_Error_Val,1);
	}
}

/*****************************************************************************
 函 数 名  : void Warning_Event_Handler(void)
 功能描述  : 告警事件处理
 输入参数  :	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月26日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void Warning_Event_Handler(void)
{
	u8 i=0;
	read_dgus_vp(WARNING_EVENT,(u8*)&Warning_Val,1);
	if(Warning_Val>0)
	{
		switch(Warning_Val)
		{
			case	0x1001:
				i=6;				
				break;
			case	0x1002:
				i=7;
				break;
			default:
				break;
		}
		Page_Change_Handler(i);
		Warning_Val=0;
		write_dgus_vp(WARNING_EVENT,(u8*)&Warning_Val,1);
	}		
}


/*****************************************************************************
 函 数 名  : void Page_Change_Handler(u8 n)
 功能描述  : 页面切换函数
 输入参数  :	  n:需切换的页面数
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月26日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void Page_Change_Handler(u8 n)
{
	page_set[0]=0x5A;
	page_set[1]=0x01;
	page_set[2]=0x00;
	page_set[3]=Page_Change[n];
	write_dgus_vp(PIC_SET,page_set,2);
}




/*****************************************************************************
 函 数 名  : void Screen_Saver_Handler(u16 type)
 功能描述  : 屏保处理函数
 输入参数  :	 type：屏保类型	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月26日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/ 
void Screen_Saver_Handler(u16 type)
{
	switch(type)
	{
		case	1:		//传统	63~70图片
			if(Screen_Flag==1)
			{
				//保持页面，不用进行操作
			}
			else
			{
				page_set[0]=0x5A;
				page_set[1]=0x01;
				page_set[2]=0x00;
				page_set[3]=(u8)Screen_Saver_Parm[0];
				write_dgus_vp(PIC_SET,page_set,2);
				Screen_Flag=1;
			}
			break;
		case	2:		//相册	
			if(Screen_Flag==1)
			{
				//保持页面，不用进行操作
			}
			else
			{
				page_set[0]=0x5A;
				page_set[1]=0x01;
				page_set[2]=0x00;
				page_set[3]=(u8)Screen_Saver_Parm[1];
				write_dgus_vp(PIC_SET,page_set,2);
				Screen_Flag=1;
				Page_Change_Val=1;
				write_dgus_vp(0x2700,(u8*)&Page_Change_Val,1);
			}
			break;
		case	3:		//时钟
			if(Screen_Flag==1)
			{
				//保持页面，不用进行操作
			}
			else
			{
				page_set[0]=0x5A;
				page_set[1]=0x01;
				page_set[2]=0x00;
				page_set[3]=(u8)Screen_Saver_Parm[2];
				write_dgus_vp(PIC_SET,page_set,2);
				Screen_Flag=1;
			}
			break;
		default:
			break;
	}
}








/*****************************************************************************
 函 数 名  : void Brightness_Handler(void)
 功能描述  : 亮度调节处理函数
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月12日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Brightness_Handler(void)
{
	read_dgus_vp(SCREEN_BRIGHTNESS,(u8*)&led_new,1);
	if(led_new!=led_old)
	{
		led_old=led_new;
		led_on[0]=led_new&0xFF;
		write_dgus_vp(LED_CONFIG,led_on,2);
	}
	if(led_new==10) led_icon = 0;
	else
	{
		led_icon = led_new/10;
	}
		write_dgus_vp(LED_ICON,(u8*)&led_icon,1);
}




/*****************************************************************************
 函 数 名  : void Parm_Reset_Init(void)
 功能描述  : 参数设置初始化
 输入参数  :	 	
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月26日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Parm_Reset_Init(void)
{
	Wind_Speed=4;
	write_dgus_vp(SPEED,(u8*)&Wind_Speed,1);	//风速
	Mode=3;
	write_dgus_vp(MODE,(u8*)&Mode,1);			//模式
	movie_icon = 1;
	write_dgus_vp(0x2702,(u8*)&movie_icon,1);			//首页动图
	Alarm_Key[0]=0;
	Alarm_Key[1]=0;
	write_dgus_vp(ALARM_KEY,(u8*)Alarm_Key,2);				//闹钟开关
	timer_key[0]=0;
	timer_key[1]=0;
	timer_key[2]=0;
	write_dgus_vp(TIMER_KEY,(u8*)timer_key,3);				//定时器开关
	Temperature_Set_Val=260;
	write_dgus_vp(TEMP_SET,(u8*)&Temperature_Set_Val,1);		//设定温度						
	led_new=0x50;
	write_dgus_vp(SCREEN_BRIGHTNESS,(u8*)&led_new,1);			//屏幕亮度
	alarm_val.alarm1_time=0;
	alarm_val.alarm2_time=0;
	alarm_val.alarm1_ring=1;
	alarm_val.alarm2_ring=1;
	write_dgus_vp(ALARM_VAL,(u8*)&alarm_val,4);					//闹钟时间铃声
	Sleep_Type=1;
	write_dgus_vp(SCREEN_SAVER_TYPE,(u8*)&Sleep_Type,1);
	sleep_time=1;	
	write_dgus_vp(SCREEN_SAVER_TIME,(u8*)&sleep_time,1);
	memset(&tim_week_c[0],0,21);
	tim_per_val[0][0].timer_time_start=0x081E;
	tim_per_val[0][0].timer_time_end=0x0C00;
	tim_per_val[0][0].timer_speed=1;
	tim_per_val[0][0].time_temperture=26;
	tim_per_val[0][1].timer_time_start=0x0C00;
	tim_per_val[0][1].timer_time_end=0x0D00;
	tim_per_val[0][1].timer_speed=1;
	tim_per_val[0][1].time_temperture=26;
	tim_per_val[0][2].timer_time_start=0x0D00;
	tim_per_val[0][2].timer_time_end=0x1200;
	tim_per_val[0][2].timer_speed=1;
	tim_per_val[0][2].time_temperture=26;
	tim_per_val[0][3].timer_time_start=0x1200;
	tim_per_val[0][3].timer_time_end=0x0C00;
	tim_per_val[0][3].timer_speed=1;
	tim_per_val[0][3].time_temperture=26;
	write_dgus_vp(TIMER1_WEEK,(u8*)&tim_week_c[0],1);
	write_dgus_vp(TIMER1_TIME_SEED_MODE,(u8*)&tim_per_val[0][0],16);
	tim_per_val[1][0].timer_time_start=0x081E;
	tim_per_val[1][0].timer_time_end=0x0C00;
	tim_per_val[1][0].timer_speed=1;
	tim_per_val[1][0].time_temperture=26;
	tim_per_val[1][1].timer_time_start=0x0C00;
	tim_per_val[1][1].timer_time_end=0x0D00;
	tim_per_val[1][1].timer_speed=1;
	tim_per_val[1][1].time_temperture=26;
	tim_per_val[1][2].timer_time_start=0x0D00;
	tim_per_val[1][2].timer_time_end=0x1200;
	tim_per_val[1][2].timer_speed=1;
	tim_per_val[1][2].time_temperture=26;
	tim_per_val[1][3].timer_time_start=0x1200;
	tim_per_val[1][3].timer_time_end=0x0C00;
	tim_per_val[1][3].timer_speed=1;
	tim_per_val[1][3].time_temperture=26;
	write_dgus_vp(TIMER2_WEEK,(u8*)&tim_week_c[1],1);
	write_dgus_vp(TIMER2_TIME_SEED_MODE,(u8*)&tim_per_val[1][0],16);
	tim_per_val[2][0].timer_time_start=0x081E;
	tim_per_val[2][0].timer_time_end=0x0C00;
	tim_per_val[2][0].timer_speed=1;
	tim_per_val[2][0].time_temperture=26;
	tim_per_val[2][1].timer_time_start=0x0C00;
	tim_per_val[2][1].timer_time_end=0x0D00;
	tim_per_val[2][1].timer_speed=1;
	tim_per_val[2][1].time_temperture=26;
	tim_per_val[2][2].timer_time_start=0x0D00;
	tim_per_val[2][2].timer_time_end=0x1200;
	tim_per_val[2][2].timer_speed=1;
	tim_per_val[2][2].time_temperture=26;
	tim_per_val[2][3].timer_time_start=0x1200;
	tim_per_val[2][3].timer_time_end=0x0C00;
	tim_per_val[2][3].timer_speed=1;
	tim_per_val[2][3].time_temperture=26;
	write_dgus_vp(TIMER3_WEEK,(u8*)&tim_week_c[2],1);
	write_dgus_vp(TIMER3_TIME_SEED_MODE,(u8*)&tim_per_val[2][0],16);
}





/*****************************************************************************
 函 数 名  : void T0_Init(void)
 功能描述  : 定时器0初始化	定时间隔1ms
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void T0_Init(void)
{
    TMOD|=0x01;
    TH0=T10uS>>8;        //1ms定时器
    TL0=T10uS;
    ET0=1;              //开启定时器0中断
    EA=1;               //开总中断
    TR0=1;              //开启定时器
}


/*****************************************************************************
 函 数 名  : void T1_Init(void)
 功能描述  : 定时器1初始化	定时间隔1ms
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void T1_Init(void)
{
    TMOD|=0x01;
    TH1=T1MS>>8;        //1ms定时器
    TL1=T1MS;
    ET1=1;              //开启定时器0中断
    EA=1;               //开总中断
    TR1=1;              //开启定时器
}



/*****************************************************************************
 函 数 名  : void T2_Init(void)
 功能描述  : 定时器2初始化	定时间隔1ms
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void T2_Init(void)
{
    T2CON=0x70;
    TH2=0x00;
    TL2=0x00;
    TRL2H=0xBC;
    TRL2L=0xCD;        //1ms的定时器
    IEN0|=0x20;     //开启定时器2
    TR2=0x01;
    EA=1;
}






/*****************************************************************************
 函 数 名  : void read_dgus_vp(u32 addr,u8* buf,u16 len)
 功能描述  : 读dgus地址的值
 输入参数  :	 addr：dgus地址值  len：读数据长度
 输出参数  : buf：数据保存缓存区
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void read_dgus_vp(u32 addr,u8* buf,u16 len)
{
    u32 OS_addr=0,OS_addr_offset=0;
    u16 OS_len=0,OS_len_offset=0;
    OS_addr=addr/2;
    OS_addr_offset=addr%2;
    ADR_H=(u8)(OS_addr>>16)&0xFF;
    ADR_M=(u8)(OS_addr>>8)&0xFF;
    ADR_L=(u8)OS_addr&0xFF;
    ADR_INC=1;                 //DGUS  OS存储器地址在读写后自动加1
    RAMMODE=0xAF;               //启动读模式
    if(OS_addr_offset==1)       //首地址有偏移，修正
    {
        while(APP_ACK==0);      //等待
        APP_EN=1;
        while(APP_EN==1); 
        *buf++=DATA1;
        *buf++=DATA0;              
        len--;
        OS_addr_offset=0;
    }
    OS_len=len/2;
    OS_len_offset=len%2;
    if(OS_len_offset==1)
    {
         OS_len++;
    }
    while(OS_len--)
    {
        if((OS_len_offset==1)&&(OS_len==0))
        {          
            while(APP_ACK==0);
            APP_EN=1;
            while(APP_EN==1);       //读写执行结束
            *buf++=DATA3;
            *buf++=DATA2;           
            break;    
        } 
        while(APP_ACK==0);
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束 
        *buf++=DATA3;
        *buf++=DATA2;
        *buf++=DATA1;
        *buf++=DATA0;                          
    }   
    RAMMODE=0x00;           //读写完成后RAMMODE必须置零
	return;	
}




/*****************************************************************************
 函 数 名  : void write_dgus_vp(u32 addr,u8* buf,u16 len)
 功能描述  : 写dgus地址数据
输入参数  :	 addr：写地址值	buf：写入的数据保存缓存区 len：字长度
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void write_dgus_vp(u32 addr,u8* buf,u16 len)
{
    u32 OS_addr=0,OS_addr_offset=0;
    u16 OS_len=0,OS_len_offset=0;
    EA=0;
    OS_addr=addr/2;
    OS_addr_offset=addr%2; 
    ADR_H=(u8)(OS_addr>>16)&0xFF;
    ADR_M=(u8)(OS_addr>>8)&0xFF;
    ADR_L=(u8)OS_addr&0xFF;
    ADR_INC=0x01;                 //DGUS  OS存储器地址在读写后自动加1
    RAMMODE=0x8F;               //启动写模式 
    if(OS_addr_offset==1)
    {
        ADR_INC=0;
        RAMMODE=0xAF;
        while(APP_ACK==0);
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束
        ADR_INC=0x01; 
        RAMMODE=0x8F;
        while(APP_ACK==0);      
        DATA1=*buf++;
        DATA0=*buf++;
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束
        len--;
        OS_addr_offset=0;
    }
    OS_len=len/2;
    OS_len_offset=len%2; 
    if(OS_len_offset==1)
    {
         OS_len++;
    }
    while(OS_len--)
    {
        if((OS_len_offset==1)&&(OS_len==0))
        {
            ADR_INC=0;
            RAMMODE=0xAF;
            while(APP_ACK==0);
            APP_EN=1;                //增加一个读过程，以免写单字时会将另一个单字写0
            while(APP_EN==1);       //读写执行结束
            ADR_INC=0x01;
            RAMMODE=0x8F;
            while(APP_ACK==0);           
            DATA3=*buf++;
            DATA2=*buf++;
            APP_EN=1;
            while(APP_EN==1);       //读写执行结束
            break;
        }
        while(APP_ACK==0);        
        DATA3=*buf++;
        DATA2=*buf++;
        DATA1=*buf++;
        DATA0=*buf++;
        APP_EN=1;
        while(APP_EN==1);       //读写执行结束
    } 
    RAMMODE=0x00;       //读写完成后RAMMODE必须置零
    EA=1;  
	return;
}  





/*****************************************************************************
 函 数 名  : void Get_ADC_Value(void)
 功能描述  : 得到adc采样的值
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Get_ADC_Value(void)
{
	static u16 temp[8]={0};
	read_dgus_vp(AD_VALUE,(u8*)temp,8);
	adc_val[0]+=temp[0];
	adc_val[6]+=temp[6];
	adc_val[7]+=temp[7];
}



 /**
    * @brief 得到NTC电阻值
    *        先浮点型运算，在乘10000得到实际值
    * @param [in] 
    *      无
    * @param [out]
    *      无
    * @retval 
    *      无
    */
/*****************************************************************************
 函 数 名  : void Get_R_Value(void)
 功能描述  : 得到电阻值
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Get_R_Value(u16 n)
{
	float R1_temp=0.0,R2_temp=0.0;
	adc_value[0]=adc_val[0]/n;
	adc_value[6]=adc_val[6]/n;
	adc_value[7]=adc_val[7]/n;
	R1_temp=adc_value[7]*1.0/(adc_value[6]-adc_value[7]);
	R2_temp=adc_value[0]*1.0/(adc_value[6]-adc_value[0]);
	R1_value=R1_temp*10000;
	R2_value=R2_temp*10000;	
}


/*****************************************************************************
 函 数 名  : u8 FindTab(u16 *pTab,u8 Tablong,u16 dat)
 功能描述  : 二分法查表发，得到温度值，表格数据大到小
 输入参数  :	 pTab：查表指针地址		Tablong：查表长度   dat：查表的数据
 输出参数  : 返回查表的温度值
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
u8 FindTab(u16 *pTab,u8 Tablong,u16 dat)
{
	u8 st=0,ed=0,m=0;
	u8 i=0;
	ed=TempSize-1;
	if(dat>=pTab[st])
		return st;
	else if(dat<=pTab[ed])
		return ed;
	while(st<ed)
	{
		m=(st+ed)/2;
		if(dat==pTab[m])
			break;
		if((dat<pTab[m])&&(dat>pTab[m+1]))
			break;
		if(dat>pTab[m])
			ed=m;
		else
			st=m;
		if(i++>Tablong)
			break;		
	}
	if(st>ed)
		return 0;
	return m;
}


/*****************************************************************************
 函 数 名  : void Get_Temperature(void)
 功能描述  : 得到温度值，经过得到AD值计算得到温度值，采用通过10次采样求平均值来计算温度
 输入参数  :	tim: 时间 n:采样次数
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Get_Temperature(u16 tim,u16 n)
{
	static u16 i=0,temp_t=0;
	float temp1=0,temp2=0;
	if(AD_Count>tim)
	{
		Get_ADC_Value();
		i++;
		if(i>=n)
		{			
			Get_R_Value(n);
			R1_Temperature=FindTab(TabNTC_10k,TempSize,R1_value);
			R2_Temperature=FindTab(TabNTC2_10k,TempSize,R2_value);
			temp1=(TabNTC_10k[R1_Temperature]-R1_value)*1.0/(TabNTC_10k[R1_Temperature]-TabNTC_10k[R1_Temperature+1]);
			temp2=(TabNTC2_10k[R2_Temperature]-R2_value)*1.0/(TabNTC2_10k[R2_Temperature]-TabNTC2_10k[R2_Temperature+1]);
			R1_Temperature=R1_Temperature*10+temp1*10;
			R2_Temperature=R2_Temperature*10+temp2*10;
			write_dgus_vp(0x1002,(u8*)&R1_Temperature,1);
			write_dgus_vp(0x1004,(u8*)&R2_Temperature,1);
			Temperature_Real=(R1_Temperature*Temp_Coef[0]-R2_Temperature*Temp_Coef[1]+Temp_Coef[2]);
			Temperature_Real=(Temperature_Real/(10*Temp_Pre))*10;
			if(abs(Temperature_Real-Temperatrue_Real_Old)==Temp_Pre)
			{
				temp_t++;
				if(temp_t<5)
				{
					Temperature_Real=Temperatrue_Real_Old;
				}
				else
				{
					temp_t=0;
					Temperatrue_Real_Old=Temperature_Real;
				}
			}
			else if(abs(Temperature_Real-Temperatrue_Real_Old)>=Temp_Pre*2)
			{
				Temperatrue_Real_Old=Temperature_Real;
				temp_t=0;
			}
// // 			Temperature_Real=Temperature_Real/10;
			write_dgus_vp(TEMP_CURRENT,(u8*)&Temperature_Real,1);
			memset(adc_val,0,sizeof(adc_val));
			i=0;
		}
		AD_Count=0;
	}
}










/*****************************************************************************
 函 数 名  : u8 Alarm_Handler(void)
 功能描述  : 闹钟处理函数
 输入参数  :	 
 输出参数  : 0：闹钟处理函数成功
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
u8 Alarm_Handler(void)
{
	read_dgus_vp(ALARM_KEY,(u8*)Alarm_Key,2);
	if((Alarm_Key[0]>0)||(Alarm_Key[1]>0))
	{
		read_dgus_vp(ALARM_VAL,(u8*)&alarm_val,4);
		Real_Hour_Min=(u16)real_time.hour*256+real_time.min;
		if(Alarm_Key[0]==1)
		{
			if(Real_Hour_Min==alarm_val.alarm1_time)
			{
				//开启闹钟，gui闹钟还没有调通，后续增加
			}
			else
			{
				//flag清零
			}
		}
		else if(Alarm_Key[1]==1)
		{
			if(Real_Hour_Min==alarm_val.alarm2_time)
			{
				//开启闹钟，gui闹钟还没有调通，后续增加
			}
			else
			{
				//flag清零
			}
		}
		Alarm_Status=1;
		write_dgus_vp(ALARM_STATUS,(u8*)&Alarm_Status,1);
	}
	else
	{
		Alarm_Status=0;
		write_dgus_vp(ALARM_STATUS,(u8*)&Alarm_Status,1);
	}
	return 0;
}













/*****************************************************************************
 函 数 名  : void T0_ISR_PC(void)    interrupt 1
 功能描述  : 定时器0处理函数，毫秒增加
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void T0_ISR_PC(void)    interrupt 1
{
// 	u16   sxi = 1;
    EA=0;
    TH0=T1MS>>8;
    TL0=T1MS;
//  		TP_drawin();
	Wait_Count++;
	if(Key_Count<65500)
	{
		Key_Count++;
	}		
	if(M4G6_ct)
	{
		M4G6_ct--;
	}
	screen_count++;	
	Sleep_Count++;	
	Parm_Change_Count++;
	AD_Count++;
    EA=1;
}

/*****************************************************************************
 函 数 名  : void Time_Calibration(void)
 功能描述  : 时间校准，读取的为RTC时间设置的值
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年5月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void Time_Calibration(void)
{
	read_dgus_vp(RTC_Set,(u8*)time_calibra,4);
	if((time_calibra[0]==0x5A)&&(time_calibra[1]==0xA5))
	{
		real_time.year=time_calibra[2];
		real_time.month=time_calibra[3];
		real_time.day=time_calibra[4];
		real_time.hour=time_calibra[5];
		real_time.min=time_calibra[6];
		real_time.sec=time_calibra[7];
		write_dgus_vp(RTC,(u8*)&real_time.year,4);
		time_calibra[0]=0;
		time_calibra[1]=0;
		write_dgus_vp(RTC_Set,(u8*)time_calibra,4);
	}
}





u8 Is_Leap_Year(u16 year)
{		
	if(year%4==0) //必须能被4整除
	{
		if(year%100==0) 
		{
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}



u8 RTC_Get_Week(u8 year,u8 month,u8 day)
{	
	u16 temp;
	year_real=(u16)year+2000;
	yearH=year_real/100;	
	yearL=year_real%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp=yearL+yearL/4;
	temp=temp%7; 
	temp=temp+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp--;
	temp%=7;
	if(temp==0)
		return 6;
	else
		return temp-1;
}




/*****************************************************************************
 函 数 名  : void Time_Update(void)
 功能描述  : 更新时间
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
// void Time_Update(void)
// {		
// 	if(Second_Updata_Flag==1)
// 	{
// 		real_time.week=RTC_Get_Week(real_time.year,real_time.month,real_time.day);	
// 		write_dgus_vp(RTC,(u8*)&real_time.year,4);	
// 		time_display[0]=real_time.year+2000;
// 		time_display[1]=real_time.month+100;;
// 		time_display[2]=real_time.day+100;
// 		time_display[3]=real_time.week;		//0-6表示星期一到星期日
// 		time_display[4]=real_time.hour+100;
// 		time_display[5]=real_time.min+100;
// 		time_display[6]=real_time.sec+100;
// 		write_dgus_vp(TIME_DISPLAY,(u8*)time_display,7);		
// 		Second_Updata_Flag=0;
// 		Time_Calibration();	
// 		read_dgus_vp(RTC,(u8*)&real_time.year,4);
// 	}
// }



/*****************************************************************************
 函 数 名  : void T1_ISR_PC(void)    interrupt 3
 功能描述  : 定时器1处理函数，提供RTC计时和处理
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void T1_ISR_PC(void)    interrupt 3
{
    EA=0;
    TH1=T1MS>>8;
    TL1=T1MS;
	////////////////////////////
	////////////////////////////////
	SysTick_RTC++;
	if(SysTick_RTC==1000)
	{		
		real_time.sec++;
		if(real_time.sec>59)
		{
			real_time.sec=0;
			real_time.min++;
			if(real_time.min>59)
			{
				real_time.min=0;
				real_time.hour++;
				if(real_time.hour>23)
				{
					real_time.hour=0;
					real_time.day++;
					if((real_time.month==2)&&(Is_Leap_Year((u16)real_time.year+2000)))
					{
						if(real_time.day>(mon_table[real_time.month-1]+1))
						{
							real_time.day=1;
							real_time.month++;
						}
					}
					else
					{
						if(real_time.day>mon_table[real_time.month-1])
						{
							real_time.day=1;
							real_time.month++;
						}
					}
					if(real_time.month>12)
					{
						real_time.month=1;
						real_time.year++;
					}					
				}				
			}			
		}
		SysTick_RTC=0;
		Second_Updata_Flag=1;		
	}
	
    EA=1;
}



/*****************************************************************************
 函 数 名  : void delay_ms(u16 n)
 功能描述  : 延时函数，使用定时器2硬件延时
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void delay_ms(u16 n)
{
    SysTick=n;
    while(SysTick);   
}

/*****************************************************************************
 函 数 名  : void T2_ISR_PC(void)    interrupt 5
 功能描述  : 定时器2中断处理函数，提供延时函数计数和喂狗
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年4月2日
    作    者   : chengjing
    修改内容   : 创建
*****************************************************************************/
void T2_ISR_PC(void)    interrupt 5
{
	    EA=0;

    TF2=0;    
    SysTick--; 
			if(Uart2RxCt==0)
			{
				Uart2_TTL_Status = 0;
			}
			else
			{
				Uart2RxCt--;
				
			}
			if(Uart3RxCt==0)
			{
				Uart3_TTL_Status = 0;
			}
			else
			{
				Uart3RxCt--;
				
			}
// 		Uart3RxCt--;
		Uart4RxCt--;
		Uart5RxCt--;
    WDT_RST();  
    EA=1;
			
}










