
#include "math.h"
#include "sys.h"
#include "TP_draw.h"


typFNT_GB16_st   ReadGb;
pixel_coord    S1[32],M1[32],M2[32],E1[32],Pmove1;


u8  absC(s8 xx,s8 yy)
{
	u8 data xxx,yyy,xy;
	xxx = xx;
	yyy = yy;
	if(xx>yy)
	{
		xy = xxx-yyy;
	}
	else
	{
		xy = yyy-xxx;
	}
		return  xy;
}
/////////////////////////////////////////////////////////////////////////
//Rosenfeld细化算法
//功能：对图象进行细化
//参数：image：代表图象的一维数组
//      lx：图象宽度
//      ly：图象高度
//      无返回值

void ThinnerRosenfeld(void *image, unsigned long lx, unsigned long ly)
{
    char *f, *g;
    char n[10];
    char a[5] = {0, -1, 1, 0, 0};
    char b[5] = {0, 0, 0, 1, -1};
    char nrnd, cond, n48, n26, n24, n46, n68, n82, n123, n345, n567, n781;
    short k, shori;
    unsigned long i, j;
    long ii, jj, kk, kk1, kk2, kk3, size;
    size = (long)lx * (long)ly;

//     g = (char *)malloc(size);
//     if(g==NULL)
//     {
//         printf("error in alocating mmeory!/n");
//         return;
//     }

    f = (char *)image;
    for(kk=0l; kk<size; kk++)
    {
        g[kk] = f[kk];
    }

    do
    {
        shori = 0;
        for(k=1; k<=4; k++)
        {
            for(i=1; i<lx-1; i++)
            {
                ii = i + a[k];

                for(j=1; j<ly-1; j++)
                {
                    kk = i*ly + j;

                    if(!f[kk])
                        continue;

                    jj = j + b[k];
                    kk1 = ii*ly + jj;

                    if(f[kk1])
                        continue;

                    kk1 = kk - ly -1;
                    kk2 = kk1 + 1;
                    kk3 = kk2 + 1;
                    n[3] = f[kk1];
                    n[2] = f[kk2];
                    n[1] = f[kk3];
                    kk1 = kk - 1;
                    kk3 = kk + 1;
                    n[4] = f[kk1];
                    n[8] = f[kk3];
                    kk1 = kk + ly - 1;
                    kk2 = kk1 + 1;
                    kk3 = kk2 + 1;
                    n[5] = f[kk1];
                    n[6] = f[kk2];
                    n[7] = f[kk3];

                    nrnd = n[1] + n[2] + n[3] + n[4]
                        +n[5] + n[6] + n[7] + n[8];
                    if(nrnd<=1)
                        continue;

                    cond = 0;
                    n48 = n[4] + n[8];
                    n26 = n[2] + n[6];
                    n24 = n[2] + n[4];
                    n46 = n[4] + n[6];
                    n68 = n[6] + n[8];
                    n82 = n[8] + n[2];
                    n123 = n[1] + n[2] + n[3];
                    n345 = n[3] + n[4] + n[5];
                    n567 = n[5] + n[6] + n[7];
                    n781 = n[7] + n[8] + n[1];

                    if(n[2]==1 && n48==0 && n567>0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[6]==1 && n48==0 && n123>0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[8]==1 && n26==0 && n345>0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[4]==1 && n26==0 && n781>0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[5]==1 && n46==0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[7]==1 && n68==0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[1]==1 && n82==0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    if(n[3]==1 && n24==0)
                    {
                        if(!cond)
                            continue;
                        g[kk] = 0;
                        shori = 1;
                        continue;
                    }

                    cond = 1;
                    if(!cond)
                        continue;
                    g[kk] = 0;
                    shori = 1;
                }
            }

            for(i=0; i<lx; i++)
            {
                for(j=0; j<ly; j++)
                {
                    kk = i*ly + j;
                    f[kk] = g[kk];
                }
            }
        }
    }while(shori);

//     free(g);
}          
//特征值提取，8方向、梯度特征
void  feature_extraction(u8* buf,s16* charact_buf,u8  num)
{
	u16  charact[4]={0};
	u8 		pixels_data[32][32];
	data u8		i,j,see;
	data u16		xx,xy,yx,yy;
	for(i=0;i<num;i++)
	{
		for(j=0;j<num;j++)
		{
			pixels_data[i][j]=*buf++;			//指针赋值到二维数组
			see = pixels_data[i][j];
		}
	}
	xx=xy=yx=yy=0;
	for(i=1;i<num-1;i++)
	{
		for(j=1;j<num-1;j++)
		{
			if(pixels_data[i][j])				//如果该点为1
			{
				if(pixels_data[i-1][j-1])		//左上角
				{
					xy+=5;
				}
				if(pixels_data[i+1][j+1])		//右下角
				{
					xy+=5;
				}
				if(pixels_data[i-1][j+1])		//左下角
				{
					yx+=5;
				}
				if(pixels_data[i+1][j-1])		//右上角
				{
					yx+=5;
				}
				if(pixels_data[i-1][j])		//上侧
				{
					yy+=5;
				}
				if(pixels_data[i+1][j])		//下侧
				{
					yy+=5;
				}
				if(pixels_data[i][j-1])		//左侧
				{
					xx+=5;
				}
				if(pixels_data[i][j+1])		//右侧
				{
					xx+=5;
				}
			}
		}
	}
	*charact_buf++ = xx/10;
	*charact_buf++ = xy/10;
	*charact_buf++ = yx/10;
	*charact_buf++ = yy/10;
	
}
/*像素点转换、细化*/
void  coordinate_transf(s16* buf,u8 pixeX,u8 pixeY)
{
		data u8  ramdata_rd[12];
		u8  pixels_rd[192]={0};
		u8  pixels_cg1[32][32]={0};
		u8  pixels_cg2[32][32]={0};
		u8  pixels_cg3[32][32]={0};
		u8  pixels_cg4[16][16]={0};
		data s16  charact_value[4]={0};
		u32	ramaddr = 0x8000;
		data u16		pixels_ct,pixels_ct2,i,j,nX,nY,ii,jj;
		u8		i1,i2,j1,j2;
		data u8	ifblankX1,ifblankY1,ifblankX2,ifblankY2,squarxx,squaryy;
		data u8  squarxy,seedata,m,n,Mct,Mct2;
	
	nX=pixeX/32;
	nY=pixeY/32;
	pixels_ct=pixels_ct2=0;
	//坐标变换，将像素点信息转换为32*32点阵
	for(j = 0;j<pixeY;j++)	//行信息
	{
		for(i = 0;i<pixeX;i=i+4)	//列信息
		{
			read_ram_vp(0x8000+((j*pixeX/4)+i/4)*3,ramdata_rd,3);	//一个地址4字节，一个像素点信息占3字节，所以每次取3个地址4个像素点信息
			if((ramdata_rd[0]==0)&&(ramdata_rd[1]==0)&&(ramdata_rd[2]==0xff))		//颜色为红，默认为红色笔迹
			{
				pixels_cg1[j/6][i/6]++;
			}
			if((ramdata_rd[3]==0)&&(ramdata_rd[4]==0)&&(ramdata_rd[5]==0xff))
 			{
				pixels_cg1[j/6][(i+1)/6]++;
			}
			if((ramdata_rd[6]==0)&&(ramdata_rd[7]==0)&&(ramdata_rd[8]==0xff))
 			{
				pixels_cg1[j/6][(i+2)/6]++;
			}
			if((ramdata_rd[9]==0)&&(ramdata_rd[10]==0)&&(ramdata_rd[11]==0xff))
 			{
				pixels_cg1[j/6][(i+3)/6]++;
			}
		}
	}
	for(m = 0;m<32;m++)
	{
		for(n = 0;n<32;n++)
		{
			if(pixels_cg1[m][n]<15)
			{
				pixels_cg1[m][n] = 0;
			}
			else 
			{
				seedata = pixels_cg1[m][n];
				pixels_cg1[m][n] = 1;
			}
		}
	}
	//轨迹取直，小于一定角度的值视为直线
	//横向判断
	i = 0;
	for(m = 1;m<31;m++)
	{
		for(n = 1;n<31;n++)
		{
			if(pixels_cg1[m][n]==1)
			{
				if((pixels_cg1[m][n-1]==0)&&(pixels_cg1[m-1][n-1]==0)&&(pixels_cg1[m+1][n-1]==0))//左侧无有效点
				{
					if(((pixels_cg1[m-1][n]==1)&&(pixels_cg1[m-2][n-1]==1))||((pixels_cg1[m+1][n]==1)&&(pixels_cg1[m+2][n-1]==1)))//向上或向下连通
					{
					}
					else
					{
						if((pixels_cg1[m][n+1]==1)||((pixels_cg1[m-1][n+1]==1)&&(pixels_cg1[m-1][n]==0))||((pixels_cg1[m+1][n+1]==1)&&(pixels_cg1[m+1][n]==0))) //右侧有有效点
						{
							S1[i].coordX = m;
							S1[i].coordY = n;
							i++;
							////////记录各个横向起点的坐标
						}
					}
					
				}
			}
		}
	}
	Mct = Mct2=0;
	for(j=0;j<i;j++)		//对于每个横向的起点
	{
	Mct = Mct2=0;
		m=S1[j].coordX;
		n=S1[j].coordY;
		do
		{
			if(pixels_cg1[m][n+1]==1)
			{
//				seedata = pixels_cg1[m+1][n+1];
//				seedata = M1[Mct-1].coordX;
//				seedata = M1[Mct-1].coordY;
//				seedata = M1[Mct-2].coordX;
//				seedata = M1[Mct-2].coordY;
				if((pixels_cg1[m+1][n+1]==1)&&
					(((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY<n))||
				((Mct>2)&&((M1[Mct-2].coordX==m)&&(M1[Mct-2].coordY<n))&&((M1[Mct-3].coordX<m)&&(M1[Mct-3].coordY<n-1)))||
				((Mct>2)&&((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY==n))&&((M1[Mct-3].coordX<m-1)&&(M1[Mct-3].coordY<n)))))
				{
					m+=1;
					n+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
				}
				else if((pixels_cg1[m-1][n+1]==1)&&
					(((M1[Mct-2].coordX>m)&&(M1[Mct-2].coordY<n))||
				((Mct>2)&&((M1[Mct-2].coordX==m)&&(M1[Mct-2].coordY<n))&&((M1[Mct-3].coordX>m)&&(M1[Mct-3].coordY<n-1)))||
				((Mct>2)&&((M1[Mct-2].coordX>m)&&(M1[Mct-2].coordY==n))&&((M1[Mct-3].coordX>m+1)&&(M1[Mct-3].coordY<n)))))
				{
					m-=1;
					n+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
				}
				else
				{
					n+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
					if((pixels_cg1[m-1][n]==1)||(pixels_cg1[m+1][n]==1))
					{
						M2[Mct].coordX = m;
						M2[Mct].coordY = n;
					}
					else
					{
						M2[Mct].coordX = 0;
						M2[Mct].coordY = 0;
					}
				}
				Mct++;
			}
			else if(pixels_cg1[m-1][n+1]==1)
			{
				if(pixels_cg1[m+1][n+1]==1)
				{
					if(((M1[Mct-2].coordX>m)&&(M1[Mct-2].coordY<n))||(((M1[Mct-2].coordX==m)&&(M1[Mct-2].coordY<n))&&((M1[Mct-3].coordX>m)&&(M1[Mct-3].coordY<n-1))&&(Mct>2)))
					{
						m-=1;
						n+=1;
						M1[Mct].coordX = m;
						M1[Mct].coordY = n;
						Mct++;
					}
					else
					{
						m+=1;
						n+=1;
						M1[Mct].coordX = m;
						M1[Mct].coordY = n;
						Mct++;
					}
				}
				else
				{
					m-=1;
					n+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
					Mct++;
				}
			}
			else if(pixels_cg1[m+1][n+1]==1)
			{
				m+=1;
				n+=1;
				M1[Mct].coordX = m;
				M1[Mct].coordY = n;
				Mct++;
			}
			else if((pixels_cg1[m+1][n]==1)&&(pixels_cg1[m+2][n+1]==1))
			{
				m+=2;
				n+=1;
				M1[Mct].coordX = m;
				M1[Mct].coordY = n;
				Mct++;
			}
			else if((pixels_cg1[m-1][n]==1)&&(pixels_cg1[m-2][n+1]==1))
			{
				m-=2;
				n+=1;
				M1[Mct].coordX = m;
				M1[Mct].coordY = n;
				Mct++;
			}
			else
			{
				E1[j].coordX = m;
				E1[j].coordY = n;
				break;
			}
		}while((m>0)&&(m<31)&&(n<31));
		if((m==0)||(m==31)||(n==31))
		{
			E1[j].coordX = m;
			E1[j].coordY = n;
		}
		if(absC(E1[j].coordX,S1[j].coordX)>1)
		{
			nX=(E1[j].coordX+S1[j].coordX)/2;
		}
		else 
		{
			nX=S1[j].coordX;
		}
		seedata = E1[j].coordX;
		seedata = S1[j].coordX;
		seedata = E1[j].coordY;
		seedata = S1[j].coordY;
		seedata = absC(E1[j].coordX,S1[j].coordX)*2;
		seedata = E1[j].coordY-S1[j].coordY;
		if((absC(E1[j].coordX,S1[j].coordX)*2)<(E1[j].coordY-S1[j].coordY))
		{
			for(ii=0;ii<Mct;ii++)
			{
				if((M1[ii].coordX==M2[ii].coordX)&&(M1[ii].coordY==M2[ii].coordY))
				{
					continue;
				}
				else
				{
					pixels_cg1[M1[ii].coordX][M1[ii].coordY] = 0;
				}
			}
			for(ii=S1[j].coordY;ii<(E1[j].coordY);ii++)
			{
				pixels_cg1[nX][ii] = 1;
			}
		}
	}
	
	//纵向判断
	i = 0;
	for(n = 1;n<31;n++)
	{
		for(m = 1;m<31;m++)
		{
			if(pixels_cg1[m][n]==1)
			{
				if((pixels_cg1[m-1][n]==0)&&(pixels_cg1[m-1][n-1]==0)&&(pixels_cg1[m-1][n+1]==0))//上侧无有效点
				{
					if(((pixels_cg1[m][n-1]==1)&&(pixels_cg1[m-1][n-2]==1))||((pixels_cg1[m][n+1]==1)&&(pixels_cg1[m-1][n+2]==1)))//向上或向下连通
					{
					}
					else
					{
						if((pixels_cg1[m+1][n]==1)||((pixels_cg1[m+1][n-1]==1)&&(pixels_cg1[m][n-1]==0))||((pixels_cg1[m+1][n+1]==1)&&(pixels_cg1[m][n+1]==0))) //下侧有有效点
						{
							S1[i].coordX = m;
							S1[i].coordY = n;
							i++;
							////////记录各个横向起点的坐标
						}
					}
				}
			}
		}
	}
	Mct = Mct2=0;
	for(j=0;j<i;j++)		//对于每个横向的起点
	{
	Mct = Mct2=0;
		m=S1[j].coordX;
		n=S1[j].coordY;
		do
		{
			if(pixels_cg1[m+1][n]==1)
			{
				if((pixels_cg1[m+1][n+1]==1)&&
					(((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY<n))||
				((Mct>2)&&((M1[Mct-2].coordX==m)&&(M1[Mct-2].coordY<n))&&((M1[Mct-3].coordX<m)&&(M1[Mct-3].coordY<n-1)))||
				((Mct>2)&&((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY==n))&&((M1[Mct-3].coordX<m-1)&&(M1[Mct-3].coordY<n)))))
				{
					m+=1;
					n+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
				}
				else if((pixels_cg1[m+1][n-1]==1)&&
					(((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY>n))||
				((Mct>2)&&((M1[Mct-2].coordX==m)&&(M1[Mct-2].coordY>n))&&((M1[Mct-3].coordX<m)&&(M1[Mct-3].coordY>n+1)))||
				((Mct>2)&&((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY==n))&&((M1[Mct-3].coordX<m-1)&&(M1[Mct-3].coordY>n)))))
				{
					m+=1;
					n-=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
				}
				else
				{
					m+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
					if((pixels_cg1[m][n-1]==1)||(pixels_cg1[m][n+1]==1))
					{
						M2[Mct].coordX = m;
						M2[Mct].coordY = n;
					}
					else
					{
						M2[Mct].coordX = 0;
						M2[Mct].coordY = 0;
					}
					Mct++;
				}
			}
			else if(pixels_cg1[m+1][n-1]==1)
			{
				if(pixels_cg1[m+1][n+1]==1)
				{
					if(((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY>n))||(((M1[Mct-2].coordX<m)&&(M1[Mct-2].coordY==n))&&((M1[Mct-3].coordX<m-1)&&(M1[Mct-3].coordY>n))&&(Mct>2)))
					{
						n-=1;
						m+=1;
						M1[Mct].coordX = m;
						M1[Mct].coordY = n;
						Mct++;
					}
					else
					{
						m+=1;
						n+=1;
						M1[Mct].coordX = m;
						M1[Mct].coordY = n;
						Mct++;
					}
				}
				else
				{
					n-=1;
					m+=1;
					M1[Mct].coordX = m;
					M1[Mct].coordY = n;
					Mct++;
				}
			}
			else if(pixels_cg1[m+1][n+1]==1)
			{
				m+=1;
				n+=1;
				M1[Mct].coordX = m;
				M1[Mct].coordY = n;
				Mct++;
			}
			else if((pixels_cg1[m][n+1]==1)&&(pixels_cg1[m+1][n+2]==1))
			{
				m+=1;
				n+=2;
				M1[Mct].coordX = m;
				M1[Mct].coordY = n;
				Mct++;
			}
			else if((pixels_cg1[m][n-1]==1)&&(pixels_cg1[m+1][n-2]==1))
			{
				m+=1;
				n-=2;
				M1[Mct].coordX = m;
				M1[Mct].coordY = n;
				Mct++;
			}
			else
			{
				E1[j].coordX = m;
				E1[j].coordY = n;
				break;
			}
		}while((n>0)&&(n<31)&&(m<31));
		if((n==0)||(n==31)||(m==31))
		{
			E1[j].coordX = m;
			E1[j].coordY = n;
		}
		if(absC(E1[j].coordY,S1[j].coordY)>1)
		{
			nY=(E1[j].coordY+S1[j].coordY)/2;
		}
		else 
		{
			nY=S1[j].coordY;
		}
		if((absC(E1[j].coordY,S1[j].coordY)*2)<(E1[j].coordX-S1[j].coordX))
		{
			for(ii=0;ii<Mct;ii++)
			{
				if((M1[ii].coordY=M2[ii].coordY)&&(M1[ii].coordX==M2[ii].coordX))
				{
					continue;
				}
				else
				{
					pixels_cg1[M1[ii].coordX][M1[ii].coordY] = 0;
				}
			}
			for(ii=S1[j].coordX;ii<(E1[j].coordX);ii++)
			{
				pixels_cg1[ii][nY] = 1;
			}
		}
	}
 	for(m = 1;m<31;m++)
 	{
 		for(n = 1;n<31;n++)
 		{
			if(pixels_cg1[m][n])
			{
				jj=1;
			}
		}
	}
	
	
	//根据字迹有效范围，进行二次缩放，至16x16点阵
	ifblankX1=ifblankY1=32;
	ifblankX2=ifblankY2=0;
	for(i=0;i<32;i++)					//自上而下扫描
	{
		for(j=0;j<32;j++)
		{
			if(pixels_cg1[i][j])
			{
				ifblankX1=i;			//使非零
				break;
			}
		}
		if(ifblankX1!=32) break;
	}
	for(i=31;i>0;i--)					//自下而上扫描
	{
		for(j=0;j<32;j++)
		{
			if(pixels_cg1[i][j])
			{
				ifblankX2=i;			//使非零
				break;
			}
		}
		if(ifblankX2) break;
	}
	for(j=0;j<32;j++)					//自左至右扫描
	{
		for(i=0;i<32;i++)
		{
			if(pixels_cg1[i][j])
			{
				ifblankY1=j;			//使非零
				break;
			}
		}
		if(ifblankY1!=32) break;
	}
	for(j=31;j>0;j--)					//自右至左扫描
	{
		for(i=0;i<32;i++)
		{
			if(pixels_cg1[i][j])
			{
				ifblankY2=j;			//使非零
				break;
			}
		}
		if(ifblankY2) break;
	}
	
// 	轨迹缩放，缩放到32点阵中
	squarxx=ifblankX2-ifblankX1+1;			//有效方阵高度
	squaryy=ifblankY2-ifblankY1+1;			//有效方阵宽度
	if(squarxx>squaryy)
	{
		squarxy = squarxx;
		Pmove1.coordX = ifblankX1;
		if(ifblankY1>((squarxx-squaryy)/2))
		{
			Pmove1.coordY = ifblankY1-(squarxx-squaryy)/2;		//实际点阵的左上角坐标
		}
		else Pmove1.coordY = 0;
	}
	else
	{
		squarxy = squaryy;
		if(ifblankX1>((squaryy-squarxx)/2))
		{
			Pmove1.coordX = ifblankX1-(squaryy-squarxx)/2;
		}
		else Pmove1.coordX = 0;
		Pmove1.coordY = ifblankY1;		//实际点阵的左上角坐标
	}
	for(i=0;i<32;i++)
	{
		for(j=0;j<32;j++)
		{
			pixels_cg2[i][j]=0;
			pixels_cg3[i][j]=0;
		}
		
	}
	for(m = 1;m<31;m++)
 	{
 		for(n = 1;n<31;n++)
 		{
			if(pixels_cg1[m][n])
			{
				jj=1;
				pixels_cg2[m-Pmove1.coordX][n-Pmove1.coordY] = 1;
			}
		}
	}
	for(i=0;i<32;i++)
	{
		for(j=0;j<32;j++)
		{
			if(pixels_cg2[i][j])
			{
				seedata = 0;
			}
		}
		
	}
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			pixels_cg4[i][j]=0;
		}
		
	}
	for(m = 1;m<squarxy-1;m++)
	{
		if(pixels_cg2[m][0])
			{
				pixels_cg4[m*16/squarxy][0] = 1;
			}
	}
	for(n = 1;n<squarxy-1;n++)
	{
		if(pixels_cg2[0][n])
			{
				pixels_cg4[0][n*16/squarxy] = 1;
			}
	}
	for(m = 1;m<squarxy-1;m++)
 	{
 		for(n = 1;n<squarxy-2;n++)
 		{
			if(pixels_cg2[m][n])
			{
				pixels_cg4[m*16/squarxy][n*16/squarxy] = 1;
				//点阵信息放大，两点之间插值
				if(pixels_cg2[m-1][n+1])									//右上角有效点插值
				{
					if((m*16/squarxy-(m-1)*16/squarxy)>1)
					{
						for(i=0;i<(m*16/squarxy-(m-1)*16/squarxy);i++)
						{
							pixels_cg4[m*16/squarxy-i][n*16/squarxy+i] = 1;
						}
					}
				}
				if(pixels_cg2[m-1][n])									//正上方有效点插值
				{
					if((m*16/squarxy-(m-1)*16/squarxy)>1)
					{
						for(i=0;i<(m*16/squarxy-(m-1)*16/squarxy);i++)
						{
							pixels_cg4[m*16/squarxy-i][n] = 1;
						}
					}
				}
				if(pixels_cg2[m-1][n-1])									//左上方有效点插值
				{
					if((m*16/squarxy-(m-1)*16/squarxy)>1)
					{
						for(i=0;i<(m*16/squarxy-(m-1)*16/squarxy);i++)
						{
							pixels_cg4[m*16/squarxy-i][n*16/squarxy-i] = 1;
						}
					}
				}
				if(pixels_cg2[m][n-1])									//正左方有效点插值
				{
					if((n*16/squarxy-(n-1)*16/squarxy)>1)
					{
						for(i=0;i<(n*16/squarxy-(n-1)*16/squarxy);i++)
						{
							pixels_cg4[m][n*16/squarxy-i] = 1;
						}
					}
				}
			}
		}
	}
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			if(pixels_cg3[i][j])
			{
				pixels_cg4[i/2][j/2] = 1;
			}
		}
		
	}
////////	for(i=0;i<16;i++)
////////	{
////////		for(j=0;j<16;j++)
////////		{
////////			if(pixels_cg4[i][j])
////////			{
////////			seedata= pixels_cg4[i][j];
////////			}
////////		}
////////		
////////	}
	feature_extraction(pixels_cg4[0],charact_value,16);
	*buf++ = charact_value[0];
	*buf++ = charact_value[1];
	*buf++ = charact_value[2];
	*buf++ = charact_value[3];
// 		read_ram_vp(0x8000,ramdata_rd,52);
// 		write_dgus_vp_tp(TEST_VP1,ramdata_rd,5);
}

void   Read_Gbkdata(u16* buf)
{
	data u32  Word_addr=0;
	 u32  Word_addr_base1,Word_addr_base=0;
	u8		Word_save_bank,i;
	data u8 test[2];
	data u8		readFlash_CMD[12] = {0x5A,0x01,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	data s16  charact_value2[4]={0};
	data s16  charact_value3[4]={0};
	data  s32  calcu;
	data  u32  addrsee;
// 	if(ReadGb.Index[0]<0xB0)
// 	{
// 		ReadGb.Index[0] = 0xB0;
// 		ReadGb.Index[1] = 0xA1;
// 	}
// // // // 	当GBKL<0X7F 时：Hp=((GBKH-0x81)×190+GBKL-0X40)×(sizex2)；

// // // // 	当GBKL>0X80 时：Hp=((GBKH-0x81)×190+GBKL-0X41)×(sizex2)；
	for(ReadGb.Index[0]=0xB0;ReadGb.Index[0]<=0xB2;ReadGb.Index[0]++)
	{
		Word_addr_base1=Word_addr_base = ((u32)(ReadGb.Index[0]-0x81)*190+0xA1-0x41)*16;	
		if(Word_addr_base>0x3FFFF)											//256KB为一个字库
			{
				Word_save_bank = 2;
			}
			else if(Word_addr_base>0x1FFFF)
			{
				Word_save_bank = 1;
			}
			else
			{
				Word_save_bank = 0;
			}
			Word_addr_base = Word_addr_base - 0x20000*Word_save_bank;
			readFlash_CMD[2] = Word_save_bank+0x10;
			readFlash_CMD[3] = (u8)(Word_addr_base>>16);
			Word_addr_base&=0xFFFF;
			readFlash_CMD[4] = (u8)(Word_addr_base>>8);
			readFlash_CMD[5] = (u8)(Word_addr_base&0xff);
			readFlash_CMD[6] = 0x70;
			readFlash_CMD[7] = 0x00;
			readFlash_CMD[8] = 0x06;
			readFlash_CMD[9] = 0x00;
			readFlash_CMD[10] = 0;
			write_dgus_vp(0x00AA,readFlash_CMD,6);
			do
				{
				read_dgus_vp(0x00AA,readFlash_CMD,1);
				}while(readFlash_CMD[0]==0x5A);
		for(ReadGb.Index[1]=0xA1;ReadGb.Index[1]<=0xFE;ReadGb.Index[1]++)
		{
			test[0]=ReadGb.Index[0];
			test[1]=ReadGb.Index[1];
//			Word_addr_base1=Word_addr_base = ((u32)(ReadGb.Index[0]-0x81)*190+0xA1-0x41)*16;	
			Word_addr = ((u32)((ReadGb.Index[0]-0x81)*190+ReadGb.Index[1]-0x41))*16;		//计算GBK码偏移地址
			addrsee = Word_addr -	Word_addr_base1+0x7000;
			 read_dgus_vp(addrsee,ReadGb.Msk,16);
				GbkDatadeal(ReadGb.Msk);
				feature_extraction(Gbk_XY[0],charact_value2,16);
				for(i = 0;i<4;i++)
				{
					charact_value3[i]=tese_p[i]-charact_value2[i];
					if(charact_value3[i]<0)charact_value3[i]=0-charact_value3[i];
				}
				
				calcu=(charact_value3[0]*charact_value3[0]+charact_value3[1]*charact_value3[1]+charact_value3[2]*charact_value3[2]+charact_value3[3]*charact_value3[3]);
				i++;
				//				*buf++ = charact_value2[0];
//				*buf++ = charact_value2[1];
//				*buf++ = charact_value2[2];
//				*buf++ = charact_value2[3];
				
		}
	}
}