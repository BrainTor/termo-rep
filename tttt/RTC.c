// #include "reg51.h"
#include "RTC.h"
#include "sys.h"
#include <intrins.h>
sbit SDA=P3^1;                          // 将p3.1口模拟数据口
sbit SCL=P3^0;	
// 将p3.0口模拟时钟口

#define bus_delay 			{u8 bus_ticks = 15; while(bus_ticks--);}
#define Delay_20ticks		{u8 bus_ticks = 20; while(bus_ticks--);}
#define RTC_SCL_OUT      {P3MDOUT|=0x01;  bus_delay;}  
#define RTC_SDA_OUT  	 {P3MDOUT|= 0x02; bus_delay;}  
#define RTC_SDA_IN       {P3MDOUT&= 0xFD; bus_delay;}  
#define MIN    0x02 //秒寄存器
#define SEC    0x03 //分寄存器
#define HOUR   0x04 //时寄存器
#define DAY    0x05 //日寄存器
#define WEEK   0x06 //周寄存器
#define MONTH 0x07 //月寄存器
#define YEAR   0x08 //年寄存器
#define read_ADD 0xA3 //写器件地址
#define write_ADD 0xA2 //读器件地址
#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};                  
unsigned char g8563_Store[4]; /*时间交换区,全局变量声明*/
unsigned char code c8563_Store[4]={0x00,0x59,0x07,0x01}; /*写入时间初值：星期一 07:59:00*/
 
	u8  ackcheck;
bit   bdata SystemError;                // 从机错误标志位
//--------------------------------------------------------------------------------------------------
// 函数名称： RTC_iic_start()
// 函数功能： 启动I2C总线子程序
//--------------------------------------------------------------------------------------------------
void RTC_iic_start(void)
{ RTC_SDA_OUT           //时钟保持高，数据线从高到低一次跳变，I2C通信开始
	RTC_SCL_OUT
SDA = 1;         
SCL = 1;
bus_delay      // 延时5us 
SDA = 0;
bus_delay
SCL = 0;
}
//--------------------------------------------------------------------------------------------------
// 函数名称： RTC_iic_stop()
// 函数功能： 停止I2C总线数据传送子程序
//--------------------------------------------------------------------------------------------------
void RTC_iic_stop(void)
{ RTC_SDA_OUT
SDA = 0;       //时钟保持高，数据线从低到高一次跳变，I2C通信停止
SCL = 1;
bus_delay
SDA = 1;
bus_delay
SCL = 0;
}
//--------------------------------------------------------------------------------------------------
// 函数名称： slave_ACK
// 函数功能： 从机发送应答位子程序
//--------------------------------------------------------------------------------------------------
void slave_ACK(void)
{RTC_SDA_OUT
SDA = 0;    
SCL = 1;
bus_delay   
SDA = 1;
SCL = 0;
}
//--------------------------------------------------------------------------------------------------
// 函数名称： slave_NOACK
// 函数功能： 从机发送非应答位子程序，迫使数据传输过程结束
//--------------------------------------------------------------------------------------------------
void slave_NOACK(void)
{ RTC_SDA_OUT
SDA = 1;   
SCL = 1;
bus_delay
SDA = 0;
SCL = 0;
}
//--------------------------------------------------------------------------------------------------
// 函数名称： check_ACK
// 函数功能： 主机应答位检查子程序，迫使数据传输过程结束
//--------------------------------------------------------------------------------------------------
void check_ACK(void)
{ 
	u16 timeout = 0;   	  
	u8 temp; 
	SCL = 0;
	RTC_SDA_IN

			ackcheck = 1;
	while(timeout < 255)
	{
		timeout++;
		if(SDA == 0)
		{ackcheck = 0;
			break;
		}
		else  
			timeout++;
	}  
	SCL = 1;
	bus_delay
	SCL = 0;
	
	RTC_SDA_OUT
	SDA = 0;
// 	bus_delay   
// 	RTC_SDA_OUTPUTL 
// SDA = 1;      // 将p1.0设置成输入，必须先向端口写1
// SCL = 1;
// RTC_SDA_IN
// 	for(loopct=0;loopct<20;loopct++)
// 	{
// 		if(SDA == 0)    // 若SDA=1表明非应答，置位非应答标志F0
// 		{

// 		SCL = 0;
// 		return 0;
// 		}
// 	}
// 		return 1;
}
 
//--------------------------------------------------------------------------------------------------
// 函数名称： IICSendByte
// 入口参数： ch
// 函数功能： 发送一个字节
//--------------------------------------------------------------------------------------------------
void IICSendByte(unsigned char ch)
{ 
unsigned char idata n=8;     // 向SDA上发送一位数据字节，共八位
	RTC_SDA_OUT
while(n--)
{ 
    SCL = 0;
   if((ch&0x80) == 0x80)    // 若要发送的数据最高位为1则发送位1
   {
    SDA = 1;    // 传送位1
    bus_delay
    SCL = 1;
   }
   else
   { 
    SDA = 0;    // 否则传送位0
    bus_delay
    SCL = 1;
   }
   ch = ch<<1;    // 数据左移一位
}
    SCL = 0;    
SDA = 1;
}
//--------------------------------------------------------------------------------------------------
// 函数名称： IICreceiveByte
// 返回接收的数据
// 函数功能： 接收一字节子程序
//--------------------------------------------------------------------------------------------------
unsigned char IICreceiveByte(void)
{
unsigned char idata n=8;    // 从SDA线上读取一上数据字节，共八位
unsigned char tdata;
	
   SDA = 1;
   SCL = 0;
	RTC_SDA_IN
while(n--)
{
   SCL = 1;
   tdata = tdata<<1;    // 左移一位，或_crol_(temp,1)
   if(SDA == 1)
    tdata = tdata|0x01;    // 若接收到的位为1，则数据的最后一位置1
   else 
    tdata = tdata&0xfe;    // 否则数据的最后一位置0
   SCL=0;
}

   SDA = 0;
   SCL = 1;
return(tdata);
}
//--------------------------------------------------------------------------------------------------
// 用户调用子程序
// 函数名称： write_CFGbyte
// 入口参数： CFG_add寄存器地址,CFG_data要写入寄存器的数值
// 函数功能： 发送n位数据子程序
//--------------------------------------------------------------------------------------------------
void write_CFGbyte(unsigned char CFG_add,unsigned char CFG_data)
{          
//unsigned char idata send_da,i=0;
RTC_iic_start();                // 启动I2C
IICSendByte(write_ADD);     // 发送器件写地址
check_ACK();                // 检查应答位
    if(ackcheck == 1)
{ 
   SystemError = 1;
   return;    // 若非应答表明器件错误或已坏，置错误标志位SystemError
}
IICSendByte(CFG_add);       // 发送寄存器地址
check_ACK();                // 检查应答位
    if(ackcheck == 1)
{ 
   SystemError = 1;
   return;    // 若非应答表明器件错误或已坏，置错误标志位SystemError
}
    IICSendByte(CFG_data);       // 发送寄存器数据
check_ACK();                // 检查应答位
    if(ackcheck == 1)
{ 
   SystemError = 1;
   return;    // 若非应答表明器件错误或已坏，置错误标志位SystemError
}
RTC_iic_stop();         // 全部发完则停止
}
//--------------------------------------------------------------------------------------------------
// 用户调用子程序
// 函数名称： receiveNbyte
// 入口参数： CFG_add寄存器地址地址
// 出口参数： receive_da
// 函数功能： 接收某个寄存器数据子程序
//--------------------------------------------------------------------------------------------------
unsigned char receive_CFGbyte(unsigned char idata CFG_add)
{ 
unsigned char idata receive_da;
// 	u8  ackcheck;
RTC_iic_start();
    IICSendByte(write_ADD); //器件写地址
check_ACK(); 
if(ackcheck == 1)
{
   SystemError = 1;
   return(0);
}
IICSendByte(CFG_add); //寄存器地址
check_ACK(); 
if(ackcheck == 1)
{
   SystemError = 1;
   return(0);
}
     RTC_iic_start();
    IICSendByte(read_ADD); //器件读地址
   if(ackcheck == 1)
{
   SystemError = 1;
   return(0);
}
    receive_da=IICreceiveByte();
   slave_NOACK();       // 收到最后一个字节后发送一个非应答位
RTC_iic_stop();
return(receive_da);
}
//--------------------------------------------------------------------------------------------------
// 用户调用函数
// 函数名称： receive_CFGNbyte
// 入口参数： CFG_add寄存器地址地址，n连续读数位，* buff存储区地址
// 函数功能： 接收n个寄存器数据子程序
//--------------------------------------------------------------------------------------------------
void receive_CFGNbyte(unsigned char CFG_add, unsigned char n,unsigned char * buff)
{ 
unsigned char receive_da,i=0;
// 	u8  ackcheck;
RTC_iic_start();
    IICSendByte(write_ADD); //器件写地址
check_ACK();
if(ackcheck == 1)
{
   SystemError = 1;
	RTC_iic_stop();
   return;
}
IICSendByte(CFG_add); //寄存器地址
check_ACK();
if(ackcheck == 1)
{
   SystemError = 1;
	RTC_iic_stop();
   return;
}
   RTC_iic_start();
    IICSendByte(read_ADD); //器件读地址
   if(ackcheck == 1)
{
   SystemError = 1;
	RTC_iic_stop();
   return;
}
while(n--)
{ 
   receive_da=IICreceiveByte();
   buff[i++]=receive_da;
   slave_ACK();    // 收到一个字节后发送一个应答位
}
slave_NOACK();    // 收到最后一个字节后发送一个非应答位
RTC_iic_stop();
}
//--------------------------------------------------------------------------------------------------
// 用户调用函数
// 函数名称： P8563_Readtime
// 函数功能： 读出时间信息
//--------------------------------------------------------------------------------------------------
void P8563_Readtime()
{   unsigned char time[7];
    receive_CFGNbyte(MIN,0x07,time);
    g8563_Store[0]=time[0]&0x7f; /*秒*/
    g8563_Store[1]=time[1]&0x7f; /*分*/
    g8563_Store[2]=time[2]&0x3f; /*小时*/
    g8563_Store[3]=time[4]&0x07; /*星期*/
}
//--------------------------------------------------------------------------------------------------
// 用户调用函数
// 函数名称： P8563_settime
// 函数功能： 写时间修改值
//--------------------------------------------------------------------------------------------------
void P8563_settime()
{
     unsigned char i;
     for(i=2;i<=4;i++) { write_CFGbyte(i,g8563_Store[i-2]); }
     write_CFGbyte(6,g8563_Store[3]);
}
//--------------------------------------------------------------------------------------------------
// 用户调用函数
// 函数名称： P8563_init
// 函数功能： 初始设置
//--------------------------------------------------------------------------------------------------
void P8563_init()
{
    unsigned char i;
	RTC_SCL_OUT
	RTC_SDA_OUT
    if((receive_CFGbyte(0x0a))!=0x8) /*检查是否第一次启动，是则初始化时间*/
    {
        for(i=0;i<=3;i++) 
   g8563_Store[i]=c8563_Store[i]; /*初始化时间*/
        P8563_settime();
        write_CFGbyte(0x0,0x00);
        write_CFGbyte(0xa,0x8); /*8:00报警*/
        write_CFGbyte(0x01,0x12); /*报警有效*/
        write_CFGbyte(0xd,0xf0);
    }
}
