#include "timer.h"
#include "sys.h"

// rtc_time	real_time;
u8 RTC_fault;
u8 data date_time[7]={0}; 		//存放RTC时钟，date_time[7]={年，月，日，周，时，分，秒}
/**************************************************************
                hextoBCD

函数功能:	把给定字节按16进制转换成BCD码格式
参数:		number-待转换的字节
返回值:		data-转换后的字节
**************************************************************/
u8 hextobcd(u8 number)
{
	u8 hex;	

	hex = number / 10;
	hex = hex << 4;  
	number = number % 10;
	hex = hex | number; 
	return(hex);                
}

/**************************************************************
*                  BCDtohex

函数功能:	把给定字节按BCD码转换成16进制格式
参数:		number-待转换的字节
返回值:		data-转换后的字节

**************************************************************/
u8 bcdtohex(u8 number)
{
   u8 bcd;	
   
   bcd = number & 0xf0;
   bcd = bcd >> 4;                                                        

   bcd = bcd * 0x0a;
   number = number & 0x0f;
   bcd = bcd + number;
   return(bcd); 
} 
/***********************************************************************************************************************************
							I2C驱动函数定义区
************************************************************************************************************************************/

/**************************************************************
			I2C串口开始条件
**************************************************************/


void RTC_Start(void)
{   
	RTC_SDA_OUT
	RTC_SDA_OUTPUTL

		
	RTC_SCL_OUT
	bus_delay
	RTC_SCL_OUTPUTL 
	

	RTC_SDA_OUTPUTL  
	RTC_SDA_OUTPUTH

	RTC_SCL_OUTPUTH
	RTC_SDA_OUTPUTL
	RTC_SCL_OUTPUTL
}

/**************************************************************
			I2C串口结束条件
**************************************************************/
void RTC_Stop(void)
{ 

	RTC_SCL_OUTPUTL
	RTC_SDA_OUTPUTL

	RTC_SCL_OUTPUTH    
	RTC_SDA_OUTPUTH
	RTC_SCL_OUTPUTL   
	RTC_SDA_OUTPUTL 
  
}

/**************************************************************
			I2C串口发送一个字节

函数功能:	I2C串口发送一个字节
参数:		Byte-发送的字节
返回值:		无
**************************************************************/
void RTC_Send_Byte(u8 Byte)
{
	u8 i;
  
	RTC_SCL_OUTPUTL   
	RTC_SDA_OUTPUTL 
	for (i=0; i<8; i++)
	{
		if(Byte & 0x80)  // first MSB
		{
			RTC_SDA_OUTPUTH
		}
		else
		{ 
			RTC_SDA_OUTPUTL
		}    
		RTC_SCL_OUTPUTH
		RTC_SCL_OUTPUTL    
		Byte = Byte << 1;
	} 
}

/**************************************************************
			I2C串口接收等待回复

函数功能:	I2C串口接收等待回复
参数:		无
返回值:		无
**************************************************************/
void RTC_Ack(void) 
{ 
	u16 timeout = 0;   	  
	u8 temp; 

	RTC_SCL_OUTPUTL
	RTC_SDA_IN

	while(timeout < 255)
	{
		timeout++;
		temp = RTC_SDADATA_PORT;
		if(temp == 0)
			break;
		else  
			timeout++;
	}  

	RTC_SCL_OUTPUTH       
	RTC_SCL_OUTPUTL  

	bus_delay   
	RTC_SDA_OUTPUTL 
}

/**************************************************************
			I2C串口发送回复

函数功能:	I2C串口发送回复
参数:		无
返回值:		无
**************************************************************/
void RTC_Send_Ack(void) 
{      
	RTC_SCL_OUTPUTL  
	bus_delay 
	RTC_SDA_OUTPUTL 

	RTC_SCL_OUTPUTH  
	RTC_SCL_OUTPUTL  
	RTC_SDA_IN
}


/**************************************************************
			I2C串口发送无回复

函数功能:	I2C串口发送无回复
参数:		无
返回值:		无
**************************************************************/
void RTC_NOAck(void)
{
	RTC_SDA_IN  
	RTC_SCL_OUTPUTH
	RTC_SCL_OUTPUTL

	bus_delay
	RTC_SDA_OUTPUTL 
}

/**************************************************************
			I2C串口接收一个字节

函数功能:	I2C串口接收一个字节
参数:		无
返回值:		R_word-接收到的字节

**************************************************************/
u8  RTC_Receive_Byte(void)
{
	u8 R_word; 
	u8 j;
	u8 temp;

	R_word = 0x00;
	RTC_SDA_IN

	for(j=0; j<8; j++)
	{	      
		
		RTC_SCL_OUTPUTH
		temp = RTC_SDADATA_PORT;

		R_word = R_word << 1;
		if(temp != 0)	// first MSB
		{ 
			R_word |= 0x01; 
		}
		else
		{ 
			R_word &= 0xfe; 
		}
		Delay_20ticks;

		RTC_SCL_OUTPUTL            
	}  

	return(R_word);
}
/**************************************************************
			RTC时钟写入函数

函数功能:	RTC时钟写入
参数:		W_addr-写入地址，默认2，无特殊要求不要修改
			*word-写入的时钟数组，即date_time[7]={年，月，日，周，时，分，秒}
返回值:		无

**************************************************************/
u8 last_hour = 0;			//记录小时计数发生变化时的上一次小时数
u8 urtc_hourcnt = 0;			//允许校准时，对小时进行计数

void I2C_RTC_Write(u8 W_addr, u8 *word)
{   
		u8 number;
		u8 i;
		u8 rtc_temp; 
	
		switch(W_addr)
		{
				case 0:
				case 1: 
				case 6: 
				case 7: number = 1; break;
				case 2: number = 7; break;
				case 3: number = 3; break;
				case 4:
				case 5: number = 4; break;
		}
	
		RTC_Start();
	
		RTC_Send_Byte(0xA2);
		RTC_Ack();
		
		RTC_Send_Byte(W_addr);
		RTC_Ack();  
	
		for(i=0; i<number; i++)
		{
				rtc_temp = hextobcd(*(word++));
				RTC_Send_Byte(rtc_temp);
				RTC_Ack();
		}  
		RTC_Stop();
	
	
		last_hour = date_time[4];
		urtc_hourcnt = 0;
}
/**************************************************************
			RTC时钟读取函数

函数功能:	RTC时钟读出
参数:		R_addr-写入地址，默认2，无特殊要求不要修改
			*RTC_buffer-读取的时钟数组，即date_time[7]={年，月，日，周，时，分，秒}
返回值:		无

**************************************************************/
void I2C_RTC_Read(u8 R_addr, u8 *RTC_buffer)
{ 
		u8 rtc_temp; 
		u8 number;
		u8 i;
		
		switch(R_addr)
		{
				case 0:
				case 1: 
				case 6: 
				case 7: number=1; break;
				case 2: number=7; break;
				case 3: number=3; break;
				case 4:
				case 5: number=4; break;
		}
	
	
		RTC_Start();   
	
		RTC_Send_Byte(0xA2);
		RTC_Ack();
		
		RTC_Send_Byte(R_addr);
		RTC_Ack();
	
		RTC_Start();   
		
		RTC_Send_Byte(0xA3);
		RTC_Ack();
		
		for(i=0; i<(number-1); i++)
		{
				rtc_temp = RTC_Receive_Byte(); 
			   
				if((i == 0)||(i == 1))    
				{              
					rtc_temp = rtc_temp & 0x7f;
				}
				if((i == 2)||(i == 3))    
				{              
					rtc_temp = rtc_temp & 0x3f;
				}
				if(i == 4)   
				{              
					rtc_temp = rtc_temp & 0x07;
				}
				if(i == 5)    
				{              
					rtc_temp = rtc_temp & 0x1f;
				}
				rtc_temp = bcdtohex(rtc_temp);  
				RTC_buffer[i] = rtc_temp;
		
				RTC_Send_Ack();
		}
	
		rtc_temp = RTC_Receive_Byte();
		rtc_temp = rtc_temp&0x7f;
		rtc_temp = bcdtohex(rtc_temp);  
		RTC_buffer[i] = rtc_temp;
		RTC_NOAck();
	
		RTC_Stop();  
}
/**************************************************************
			RTC时钟初始化函数

函数功能:	RTC时钟状态初始化
参数:		无
返回值:		无

*************************************************************/
void RTC_init(void)
{
	u8 dat;    

	dat = 0x20;
	I2C_RTC_Write(0x00,&dat);
	delay_ms(10);
	
	dat = 0x00;
	I2C_RTC_Write(0x00,&dat);
	delay_ms(10);
} 

/**************************************************************
			RTC时钟运行函数

函数功能:	RTC时钟运行时调用此函数，必须放在1秒任务中
参数:		srtc_caltype-时钟校准类型:	RTCCAL_ADDTIME	1	//加时间校准，即时间走的慢
										RTCCAL_NONEED	0	//不需要时间校准
										RTCCAL_MINTIME	-1	//减时间校准，即时间走的快
			urtc_calval-校准值，即走快(慢)1秒所需要的时间(小时)	数						
返回值:		无

**************************************************************/
void RTC_RunFunction(void)
{
		I2C_RTC_Read(0x02, date_time); 
		if(date_time[1] > 60)		//
		{
				date_time[6] = 20;//年
				date_time[5] = 5;	//月
				date_time[4] = 5;//星期
				date_time[3] = 31;//日
				date_time[2] = 16;//时
				date_time[1] = 14;//分
				date_time[0] = 14;//秒
				I2C_RTC_Write(0x02, date_time);
			delay_ms(10);
				I2C_RTC_Read(0x02, date_time); 
		}
	

		if(date_time[5] == 0 || date_time[3] == 0 || date_time[5] > 12 || date_time[3] > 31)
			RTC_fault = 1;
		else
			RTC_fault = 0;
}
void RTC_Reset(u8* buf)
{
	u8  datetime_set[7]={0};
	if((*buf++==0x5A)&&(*buf++==0xA5))
	{
		datetime_set[6]=*buf++;
		datetime_set[5]=*buf++;
		datetime_set[3]=*buf++;
		datetime_set[4] = -35 + datetime_set[6] + (datetime_set[6]/4) + (26*(datetime_set[5]+1)/10)+datetime_set[3]-1;
		datetime_set[4] = datetime_set[4]%7;
		if(datetime_set[4] < 0) datetime_set[4]+=7;
		datetime_set[2]=*buf++;
		datetime_set[1]=*buf++;
		datetime_set[0]=0;
		I2C_RTC_Write(0x02, datetime_set);
		
	}
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
void Time_Update(void)
{
		u16 RTC_Hour_D,RTC_Min_D;
		RTC_RunFunction();
		real_time.year=date_time[6];	
		real_time.month=date_time[5];	
		real_time.week=date_time[4];	
		real_time.day=date_time[3];	
		real_time.hour=date_time[2];	
		real_time.min=date_time[1];	
		real_time.sec=date_time[0];	
		
// 		real_time.year=19;	
// 		real_time.month=6;	
// 		real_time.week=13;	
// 		real_time.day=4;	
// 		real_time.hour=16;	
// 		real_time.min=10;	
// 		real_time.sec=0;	
	
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
	
		real_time.week=RTC_Get_Week(real_time.year,real_time.month,real_time.day);	
		write_dgus_vp(RTC,(u8*)&real_time.year,4);
		RTC_Hour_D=real_time.hour+100;
		RTC_Min_D=real_time.min+100;
		write_dgus_vp(TIME_HOUR_DISPLAY,(u8*)&RTC_Hour_D,1);
		write_dgus_vp(TIME_MIN_DISPLAY,(u8*)&RTC_Min_D,1);
		
		time_display[0]=real_time.year+2000;
		time_display[1]=real_time.month+100;;
		time_display[2]=real_time.day+100;
		time_display[3]=real_time.week;		//0-6表示星期一到星期日
		time_display[4]=real_time.hour+100;
		time_display[5]=real_time.min+100;
		time_display[6]=real_time.sec+100;
		write_dgus_vp(TIME_DISPLAY,(u8*)time_display,7);	
}

