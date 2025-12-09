#include "vcnl4000.h" 

#define I2C_SCL_SETOUT      {P1MDOUT|=0x80;} 
#define I2C_SDA_SETOUT  	   {P1MDOUT|= 0x20;}  
#define I2C_SDA_SETIN       {P1MDOUT&= ~0x20;}  
#define SCL_H         I2C_SCLDATA_PORT = 1;
#define SCL_L         I2C_SCLDATA_PORT = 0;
   
#define SDA_H         {P1MDOUT|= 0x20;I2C_SDADATA_PORT = 1;}
#define SDA_L         {P1MDOUT|= 0x20;I2C_SDADATA_PORT = 0;}

#define SDA_read      (I2C_SDADATA_PORT)

u16 VCNL4200_ALS_Result = 0;
u16 VCNL4200_PROX_Result = 0;
u8 VCNL4200_Register[12] ={0};

//|---------------------------------------------------|
//|Function		:I2C_Init
//|Description	:initialize I2C GPIO,SCL and SDA
//|Input		:None
//|Output		:None
//|Return		:None
//|---------------------------------------------------|
void I2C_Initial(void)
{
	I2C_SCL_SETOUT;    
	I2C_SDA_SETOUT;  	
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void I2C_delay(void)
{	
	u8 i=30; //这里可以优化速度	，经测试最低到5还能写入
	while(i) 
	{ 
	i--; 
	} 
// 	delay_us(30);
}

void delay5ms(void)
{	
	u16 i=5000;  
	while(i) 
	{ 
	i--; 
	}
// 	delay_ms(5);
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
u8 I2C_Start(void)
{
	I2C_SDA_SETOUT;
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return 0;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return 0;	//SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	SCL_L;
	return 1;
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	I2C_SDA_SETOUT;
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge I2C
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}   

/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge I2C
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
} 

/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge I2C
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge I2C
****************************************************************************** */
u8 I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	I2C_SDA_SETIN;
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
		  I2C_SDA_SETOUT;
	    I2C_delay();
      return 0;
	}
	SCL_L;
	I2C_SDA_SETOUT;
	I2C_delay();
	return 1;
}

/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte( u8 SendByte) //数据从高位到低位//
{
	u8 i=8;
	while(i--)
	{
		SCL_L;
		I2C_delay();
		if(SendByte&0x80)	{SDA_H;}  
		else 	{SDA_L;}   
		SendByte<<=1;
		I2C_delay();
		SCL_H;
		I2C_delay();
	}
	SCL_L;
}  

/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave 
****************************************************************************** */
u8 I2C_ReadByte(void)  //数据从高位到低位//
{ 
    u8 i=8;
    u8 ReceiveByte=0;
    I2C_SDA_SETIN;
	while(i--)
	{
		ReceiveByte<<=1;      
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();	
		if(SDA_read)
		{
			ReceiveByte|=0x01;
		}
	}
    SCL_L;
    return ReceiveByte;
}

//|---------------------------------------------------|
//|Function		:I2C_Write
//|Description	:Write data to SlaveDeive's register
//|Input		:
//|					SlaveAddress:Slave Device's Address
//|					REG_Address :Slave Device's Register Address
//|					REG_data	:Data to be wrote into register
//|Output		:None
//|Return		:u8 data of slave device's register
//|---------------------------------------------------|
u8 I2C_Write(u8 SlaveAddress, u8 REG_Address, u8 REG_data)		     //void
{
	if(!I2C_Start())return 0;
	I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址 
	I2C_WaitAck();	
	I2C_SendByte(REG_Address);    //设置低起始地址      
	I2C_WaitAck();	
	I2C_SendByte(REG_data);
	I2C_WaitAck();   
	I2C_Stop(); 
	delay5ms();
	return 1;
}

//|---------------------------------------------------------------------|
//|Function		:I2C_Read
//|Description	:Read I2C_Device's Register
//|Input		:
//|					SlaveAddress:Slave Device's Address
//|					REG_Address :Slave Device's Register Address
//|Output		:None
//|Return		:u8  value of Register
//|---------------------------------------------------------------------|
u8 I2C_Read(u8 SlaveAddress,u8 REG_Address)
{   
	u8 REG_data;     	
	I2C_Start();
	I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址 
	I2C_WaitAck();
	I2C_SendByte((u8) REG_Address);   //设置低起始地址      
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(SlaveAddress+1);
	I2C_WaitAck();
	
	REG_data= I2C_ReadByte();
	I2C_NoAck();
	I2C_Stop();
	return REG_data;
}


void I2C_ReadRegister(u8 startaddr, u8 *regbuf, u8 num)
{
	u8 i=0;
	for(i=0;i<num;i++,regbuf++,startaddr++)
	{
		*regbuf = I2C_Read(0x26,startaddr);
	}
}

void VCNL4200_ReadOneWords(u8 ReadAddr,u8* databuf)
{				  
// 	u16 temp;		  	    																 
    I2C_Start();
	I2C_SendByte(VCNL4200_Write);  	//发送写命令
	I2C_WaitAck();	    
	I2C_SendByte(ReadAddr);   	 	//发送需要写的目的地址
	I2C_WaitAck();
// 	I2C_Stop();						//产生一个停止条件

	I2C_Start();  	 	   						   
	I2C_SendByte(VCNL4200_Read);   	//写入 写命令,进入接收模式
	I2C_WaitAck();	 				
    databuf[0]=I2C_ReadByte();		    //接收数据
	I2C_Ack();	 				
    databuf[1]=I2C_ReadByte();		    //接收数据
    I2C_Stop();						//停止
		    
// 	return temp;					//返回读出的数据
}

void VCNL4200_WriteOneWords(u8 WriteAddr,u8 *DataToWrite)
{		
// 	u8 dataW[2] = {0};	
// 	dataW = DataToWrite;
    I2C_Start();					//开始
	I2C_SendByte(VCNL4200_Write);	//发送写命令
	I2C_WaitAck();					//等待VCNL返回一个ACK
    I2C_SendByte(WriteAddr);   	 	//发送需要写的目的地址
	I2C_WaitAck();					//等待VCNL返回一个ACK
	I2C_SendByte(DataToWrite[0]);      //发送要写入的地址							   
	I2C_WaitAck();  		    	//等待VCNL返回一个ACK  
	I2C_SendByte(DataToWrite[1]);      //发送要写入的地址							   
	I2C_WaitAck();  		    	//等待VCNL返回一个ACK  
    I2C_Stop();						//产生一个停止条件 
	delay_ms(10);	 
}

//----------初始化----------
void VCNL4200_Init(void)
{
	u8  conf_data[2];
	
	I2C_Initial();										//初始化I2C引脚
	conf_data[0] =0xFA;			//占空比1/640，中断持续性2，积分时间9T，PS电源打开
	conf_data[1] =0x03;			//PS输出16bits，接近离开都触发中断
	VCNL4200_WriteOneWords(VCNL4200_PSCONF,conf_data);			
 	conf_data[0] =0x6B;			//脉冲重复8，智能持续性打开，循环发送，普通背景光消除
	conf_data[1] =0x0F;			//普通接近操作模式，普通光照能力，LED输出200mA
	VCNL4200_WriteOneWords(VCNL4200_PSCONF2,conf_data);	
	conf_data[0] =0x4F;
	conf_data[1] =0x00;
	VCNL4200_WriteOneWords(VCNL4200_PSCANC,conf_data);	
	conf_data[0] =5;
	conf_data[1] =0x00;
	VCNL4200_WriteOneWords(VCNL4200_PSTHDL,conf_data);	
	conf_data[0] =10;
	conf_data[1] =0x00;
	VCNL4200_WriteOneWords(VCNL4200_PSTHDH,conf_data);	
// 	VCNL4200_WriteOneByte(VCNL4200_ALS_Para,0x0d);		//AMBIENT LIGHT PARAMETER:1)bit7 = 0 连续转换模式选择;2)bit3=1,自动偏移补偿;3)bit2-bit0=5,每转换一次测量的次数
}

//----------------------------------------------测量函数----------------------------------------------------|
/*void VCNL4200_Measurement(u16 *ProxResult,u16 *ALSResult)
{
	VCNL4200_Init();								//初始化VCNL4200
	VCNL4200_WriteOneByte(VCNL4200_CMD,0x98);		//写命令寄存器,开始测量
	delay_ms(10);
	while((VCNL4200_ReadOneByte(VCNL4200_CMD))&0x20 == 0);	//检测命令寄存器中Prox_data_rdy位，为1则是已转换完成，数据可读
	*ProxResult = ((u16)VCNL4200_ReadOneByte(VCNL4200_PROX_HSB)<<8) | VCNL4200_ReadOneByte(VCNL4200_PROX_LSB);

	while((VCNL4200_ReadOneByte(VCNL4200_CMD))&0x40 == 0);	//检测命令寄存器中Als_data_rdy位，为1则是已转换完成，数据可读
	*ALSResult = ((u16)VCNL4200_ReadOneByte(VCNL4200_ALS_HSB)<<8) | VCNL4200_ReadOneByte(VCNL4200_ALS_LSB);

} */



// void VCNL4200_ReadAllRegisters(u8 *RegisterBuf)
// {
// 	u8 i=0;
// 	for(i=0;i<12;i++)
// 	{
// 		RegisterBuf[i]=VCNL4200_ReadOneByte(0x80+i);
// 	}
// }

void VCNL4200_MeasurementResult(u8 *ProxResult)	    //获取测量结果
{
	u8  conf_data[2];
// 	u16  TC_Status=0;
// 	VCNL4200_Init();									//初始化VCNL4200
// 	VCNL4200_WriteOneByte(VCNL4200_CMD,0x98);			//写命令寄存器,开始测量
	delay_ms(100);
	
// 	VCNL4200_ReadOneWords(VCNL4200_PSCONF2,conf_data);
// 	if(conf_data[0]&0x04)
	if(TC_Status==2)
	{
		conf_data[0] =0x6F;			//脉冲重复8，智能持续性打开，循环发送，普通背景光消除
		conf_data[1] =0x0F;			//普通接近操作模式，普通光照能力，LED输出200mA
		VCNL4200_WriteOneWords(VCNL4200_PSCONF2,conf_data);	
		VCNL4200_ReadOneWords(VCNL4200_PSDATA,ProxResult);	
		if((ProxResult[0]!=0)||(ProxResult[1]!=0))
		{
// 			conf_data[0] =0x6B;			//脉冲重复8，智能持续性打开，循环发送，普通背景光消除
// 			conf_data[1] =0x0F;			//普通接近操作模式，普通光照能力，LED输出200mA
// 			VCNL4200_WriteOneWords(VCNL4200_PSCONF2,conf_data);	
		
			TC_Status=1;
			TC_Status_Old=TC_Status;
			write_dgus_vp(SWITCH,(u8*)&TC_Status,1);
			Boot_Handler();
		}
	}
	write_dgus_vp(0x1110,ProxResult,1);
// 	VCNL4200_ReadOneWords(VCNL4200_PSDATA+1,ProxResult);	
// 	VCNL4200_ReadOneWords(VCNL4200_PSDATA+2,ProxResult);	
// 	VCNL4200_ReadOneWords(VCNL4200_PSDATA+5,ProxResult);	
// 	VCNL4200_ReadOneWords(VCNL4200_PSDATA+6,ProxResult);	
// 	while((VCNL4200_ReadOneWords(0x80)&0x60 != 0x60));	//判断两个测量是否都完成
// 	*AlsResult = (u16)VCNL4200_ReadOneByte(VCNL4200_ALS_HSB)<<8 | VCNL4200_ReadOneByte(VCNL4200_ALS_LSB);
// 	*ProxResult = (u16)VCNL4200_ReadOneByte(VCNL4200_PROX_HSB)<<8 | VCNL4200_ReadOneByte(VCNL4200_PROX_LSB);
}

