/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : TP_draw.c
  版 本 号   : V1.0
  作    者   : ad
  生成日期   : 2019年6月19日
  功能描述   : 手写输入法
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "sys.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "config.h"
#include "control.h"
#include "TP_draw.h"

u8 sx[] = {0};
u16   sxi = 1;
u16 xdata  *Coord_in;
u8   Coord_XY[16][16] = {0};
u8   Gbk_XY[16][16] = {0};
u8   ramdata_rd[40];

	data s16 tese_p[4]={0};
u8  code  Sx_Cmd_Code[16] = {0x5A,0x01,0x00,0x00,0x05,0xFF,0x00,0x00,0x00,0x10,0x00,0x10,0x00,0xc8,0x00,0xc8};
u8  code  Gbk_san[32] = {0,0,0,0,0x7F,0xFC,0,0,0,0,0,0,0,0,0x3F,0xF8,
											0,0,0,0,0,0,0,0,0,0,0x7F,0xFE,0,0,0,0};
void write_dgus_vp_tp(u32 addr,u8* buf,u16 len)
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

void read_dgus_vp_tp(u32 addr,u8* buf,u16 len)
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

// void read_ram_fun(u32 addr,u8* buf,u16 len)
// {
// 	
// }
void read_ram_vp(u32 addr,u8 *buf,u32 len)
{   
//     addr=addr/2;
    ADR_H=0x00;
    ADR_M=(unsigned char)(addr>>8);
    ADR_L=(unsigned char)(addr);
    ADR_INC=0x01;
    RAMMODE=0xAF;
	while(APP_ACK==0);
	while(len>0)
{   APP_EN=1;
    while(APP_EN==1);
    if(len>0)   
    {   *buf++=DATA0;
        *buf++=DATA1;   
				*buf++=DATA2;
        *buf++=DATA3; 		
		len--;	}
 	}
	RAMMODE=0x00;}
// void  coord_rank(u16 num, u8* buf_insert)
// {
// // 	u16  *P;
// 	u16  pre,pre2,aft,aft2,i,j,num1;
// // 	*P = *buf;
// 	num1 = num*2;
// 	for(i=0;i<(num-1);i=i+1)
// 	{
// 		if((Coord_in[i][0]<=buf_insert[0])&&(buf_insert[0]<=Coord_in[i+1][0]))
// 		{
// 			if(((Coord_in[i+1][0]==buf_insert[0])&&(buf_insert[1]<=Coord_in[i+1][1]))||(buf_insert[0]<Coord_in[i+1][0]))
// 			{
// 				pre = Coord_in[i+1][0];
// 				pre2 = Coord_in[i+1][1];
// 				Coord_in[i+1][0] = buf_insert[0];
// 				Coord_in[i+1][1] = buf_insert[1];
// 				i=i+2;
// 				break;
// 			}
// 			
// 		}
// 		else if(Coord_in[i][0]>buf_insert[0])
// 		{
// 			pre = Coord_in[i][0];
// 			pre2 = Coord_in[i][1];
// 			Coord_in[i][0] = buf_insert[0];
// 			Coord_in[i][1] = buf_insert[1];
// 			break;
// 		}
// 	}
// 	for(j = i;j<(num-1) ;j=j+1)
// 	{
// 		aft = Coord_in[i+1][0];
// 		aft2 =  Coord_in[i+1][1];
// 		 Coord_in[i+1][0] = pre;
// 		 Coord_in[i+1][1]= pre2;
// 		pre = aft;
// 		pre2 = aft2;
// 	}
// }
u16  CosCalcu(u8* buf,u8* buf2)
{
	u16  i,j,k;
	u16  sumXY,sumXX,sumYY,Cos;
	sumXY = 0;
	sumXX = 0;
	sumYY = 0;
	for(i = 0;i<256;i++)
	{
		sumXY+= buf[i]*buf2[i];
	}
	for(j = 0;j<256;j++)
	{
		sumXX+= buf[i]*buf[i];
	}
	for(k = 0;k<256;k++)
	{
		sumYY+= buf2[i]*buf2[i];
	}
	Cos = sumXY/(sqrt(sumXX)*sqrt(sumYY));
	return Cos;
}
void GbkDatadeal(u8* buf)
{
	u8 xx,yy,ct;
	data	u16 bufdata;
	for(ct = 0;ct<32;ct=ct+2)
	{
		
		for(xx = 0;xx<16;xx++)
		{
			bufdata = (u16)(buf[xx*2])<<8|buf[xx*2+1];
			for(yy = 0;yy<16;yy++)
			{
				if(bufdata&0x8000)
				{
					Gbk_XY[xx][yy]=1;
				}
				else
				{
					Gbk_XY[xx][yy]=0;
				}
					bufdata = bufdata<<1;
			}
		}
	}
	
}
void TP_drawin(void)
{
		
	u16 page,j,k;
	u8  Tp_ss[8] = {0};
// 	data u16 tese_p[4]={0};
	data u16 tese_g[4]={0};
// 	u8  *p;
	u8  XX[2]= {0};

		read_dgus_vp_tp(SX_START,(u8*)&page,1);
		if(page == 1)
		{
			page =2;
			write_dgus_vp_tp(SX_START,(u8*)&page,1);
			write_dgus_vp_tp(0x00F4,(u8*)&Sx_Cmd_Code,8);
		}
		else if (page == 2)
		{
			read_dgus_vp_tp(TP_STATUS,Tp_ss,4);
			if(Tp_ss[0]==0x5A)
			{
				Tp_ss[0]= 0 ;
				write_dgus_vp_tp(TP_STATUS,Tp_ss,1);
// 					delay_ms(5);
				if((Tp_ss[1]==0x03))
				{
					
					XX[0] = (u8)((((u16)Tp_ss[2]<<8)|Tp_ss[3])*16/480);

					XX[1]= (u8)((((u16)Tp_ss[4]<<8)|Tp_ss[5])*16/480);

					Coord_XY[XX[1]][XX[0]] = 1;
// 					pin = &Coord_XY[0][0];
// 					write_dgus_vp_tp(TEST_VP1,XX,1);
// 					write_dgus_vp_tp(TEST_VP3,Coord_XY[0],3);
				}
			}
			
		}
		
		else if (page == 3)
		{
			page =0;
			write_dgus_vp_tp(SX_START,(u8*)&page,1);
			GbkDatadeal(Gbk_san);
// 			cosnum = CosCalcu(Coord_XY[0],Gbk_XY[0])*100;
// 			
// 					write_dgus_vp_tp(TEST_VP2,(u8*)&cosnum,1);
			coordinate_transf(tese_p,200,200);
					write_dgus_vp_tp(TEST_VP1,(u8*)&tese_p[0],4);
			Read_Gbkdata(tese_g);
		}
		else
		{
			sxi = 1;
			for(k=0;k<16;k++)
			{
					for(j = 0;j<16;j++)
				{
					Coord_XY[k][j] = 0;
				}
			}
		}
// 		read_ram_vp(0x8003,ramdata_rd,10);
// 		write_dgus_vp_tp(TEST_VP1,ramdata_rd,5);
}

