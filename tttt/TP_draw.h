#ifndef __TP_DRAW_H__
#define __TP_DRAW_H__

#include "sys.h"

extern u8   sx[];
extern u16 	sxi;
extern u8   Gbk_XY[16][16];

extern	data s16 tese_p[4];


void TP_drawin(void);
void read_ram_vp(u32 addr,u8 *buf,u32 len);
void  coordinate_transf(s16* buf,u8 pixeX,u8 pixeY);
void GbkDatadeal(u8* buf);
void   Read_Gbkdata(u16* buf);
#endif
