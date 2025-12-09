#ifndef __VCNL4000_H
#define __VCNL4000_H

#include "sys.h"
// #include "i2c.h" 
// #include "delay.h"  

//定义端口
sbit I2C_SCLDATA_PORT=P1^7;	//SCL端口数值引脚定义，注意引脚的数字IO口属性修改
sbit I2C_SDADATA_PORT=P1^5;	//SDA端口数值引脚定义，注意引脚的数字IO口属性修改
sbit I2C_INT_PORT=P1^6;	//INT端口数值引脚定义，注意引脚的数字IO口属性修改

//-----------寄存器------------共有12个，下面为常用-------------------------------------------------------|
#define	VCNL4200_PSCONF		0x03	//PS_CONFIG(重要)
#define	VCNL4200_PSCONF2			0x04	//PS_CONFIG2											
#define	VCNL4200_PSCANC		0x05	//PS cancellation level setting
#define	VCNL4200_PSTHDL   0x06	// PS low interrupt threshold setting							
#define	VCNL4200_PSTHDH		0x07	// PS high interrupt threshold setting
#define	VCNL4200_PSDATA		0x08	//PS output data 
// #define	VCNL4200_PROX_HSB	0x87	//VCNL proximity measurement result HSB		 **			
// #define	VCNL4200_PROX_LSB	0x88	//VCNL proximity measurement result LSB		 **

//-----------命令------------------------------------------------------------------------------------------|
#define VCNL4200_Write		0xA2 	//写命令   	
#define VCNL4200_Read		0xA3 	//读命令   	
// #define VCNL4200_MODE_PROX	0x88	//PROX_OD=1开始近距离测量（1-200mm）
// #define	VCNL4200_MODE_ALS	0x90	//ALS(Ambient Light Sensor),ALS_OD=1，开始环境光测试

extern u16 VCNL4200_ALS_Result;			//ALS的测量结果
extern u16 VCNL4200_PROX_Result;			//PROX的测量结果
extern u8 VCNL4200_Register[12];			//VCNL4200的12个寄存器的值

void VCNL4200_Init(void);									//初始化VCNL的引脚
u8 VCNL4200_ReadID(void);
void VCNL4200_StartMeasure(u8 mode);
void VCNL4200_ReadOneWords(u8 ReadAddr,u8* databuf);						//读出一个字节从VCNL4200中指定的地址
void VCNL4200_ReadAllRegisters(u8 *RegisterBuf);			//读出12个寄存器的值
void VCNL4200_WriteOneWords(u8 WriteAddr,u8 *DataToWrite);	//写入一个字节到VCNL4200中指定的地址

void VCNL4200_Measurement(u16 ProxResult,u16 ALSResult);

void VCNL4200_StartMeasurement(void);
void VCNL4200_MeasurementResult(u8 *ProxResult);	    //获取测量结果
#endif

