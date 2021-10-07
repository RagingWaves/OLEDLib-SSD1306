#include "OLED.h"
#include "Font.h"
#include "stdarg.h"

char GRAM[1024]={0};

void WriteCmd(u8 Cmd)		//��������
{
	GPIO_ResetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);	//CS������0
	GPIO_ResetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);	//DC������0
	SPI2_ReadWriteByte(Cmd);
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);		//CS������1
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);		//DC������1
}

void WriteDat(u8 Data)		//��������
{
	GPIO_ResetBits(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN);		//CS������0
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);		//DC������1
	SPI2_ReadWriteByte(Data);
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);		//CS������1
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);		//DC������1
}

void OLED_SetPos(unsigned char page, unsigned char seg)			//�����С�ҳ
{ 
	WriteCmd(0xB0+page);
	WriteCmd((seg&0x0F)|0x00);
	WriteCmd(((seg&0xF0)>>4)|0x10);
}

void OLED_DBMP(int x0, int y0, int bmpx, int bmpy, const char *bmp)
{
	int dely, delly;
	int gcx, gcy, gcn;
	int gx, gy=0;
	
	gcn = (y0/8)*128+x0;				//��ʼ����
	gcx = (x0+bmpx)>127?127-x0:bmpx;	//x��Ч�ߴ�
	gcy = (y0+bmpy)>63?63-y0:bmpy;		//y��Ч�ߴ�
	dely = y0%8;						//yƫ����
	delly = 8-dely;
	
	if(gcx <= 0)
		return;							//xԽ��
	while(gcy > 0)
	{
		for(gx=0; gx<gcx; gx++)
			GRAM[gcn+gx] |= (bmp[gx+gy] << dely);
		gcn+=128;
		for(gx=0; gx<gcx; gx++)
			GRAM[gcn+gx] |= (bmp[gx+gy] >> delly);
		gy+=bmpx;
		gcy-=8;
	}
}

int OLED_FSizeSta(int set, int size)	//���������С״̬
{
	static int sta=0;
	if(set)
		sta = size;
	return sta;
}

void OLED_FSize(int size)
{
	OLED_FSizeSta(1, size);
}

void OLED_PStr(int x0, int y0, const char *str)
{
	int ch;
	int i=0;
	
	if(OLED_FSizeSta(0,0))
	{
		for(i=0; str[i]!='\0'; i++)
		{
			if(str[i] == '\n' || str[i] == '\r')
			{
				x0 = 0;
				y0 += 16;
				continue;
			}
			if(x0 >= 120)
			{
				x0 = 0;
				y0 += 16;
			}
			ch = str[i] - 32;	//ASCII���������±����
			OLED_DBMP(x0, y0, 8, 16, &ASCII8x16[ch*16]);
			x0 += 8;
		}
	}
	else
	{
		for(i=0; str[i]!='\0'; i++)
		{
			if(str[i] == '\n' || str[i] == '\r')
			{
				x0 = 0;
				y0 += 8;
				continue;
			}
			if(x0 >= 122)
			{
				x0 = 0;
				y0 += 8;
			}
			ch = str[i] - 32;
			OLED_DBMP(x0, y0, 6, 8, &ASCII6x8[ch*6]);
			x0 += 6;
		}
	}
}

void OLED_PCh(int x0, int y0, char ch)
{
	if(OLED_FSizeSta(0,0))
	{
		ch -= 32;
		OLED_DBMP(x0, y0, 8, 16, &ASCII8x16[ch*16]);
		x0 += 8;
	}
	else
	{
		ch -= 32;
		OLED_DBMP(x0, y0, 6, 8, &ASCII6x8[ch*6]);
		x0 += 6;
	}
}

void OLED_printf(int x0, int y0, const char *str, ...)
{
	char buf[128]={0};
	int i=0, n=0;
	
	//%s
	char *ptstr;
	char ptstri;
	
	//%d %f
	int ptint;
	int numn;
	float ptfloat;
	int ptfint;
	int ptfflt;
	
	va_list ap;
	__va_start(ap, str);
	
	while((str[i] != '\0')&&n<128)
	{
		if(str[i] == '%')
		{
			i++;
			switch(str[i])
			{
				case 'c':
					buf[n++] = __va_arg(ap, int);
					i++;
					continue;
				case 's':
					ptstr = __va_arg(ap, char*);
					ptstri = 0;
					while(ptstr[ptstri] != '\0')
						buf[n++] = ptstr[ptstri++];
					i++;
					continue;
				case 'd':
					ptint = __va_arg(ap,int);
					if(ptint == 0)
					{
						buf[n++] = '0';
						i++;
						continue;
					}
					if(ptint < 0)	//����
					{
						buf[n++] = '-';
						ptint = -ptint;
					}
					numn = 1000000000;	//int���2^32
					while(numn != 0)
					{
						if(ptint/numn)
							buf[n++] = (char)(ptint/numn+48);			//ASCII���������±����
						if(ptint == 0)
							buf[n++] = '0';
						ptint %= numn;
						numn /= 10;
					}
					i++;
					continue;
				case 'f':
					ptfloat = __va_arg(ap,double);
					if(ptfloat < 0)
					{
						buf[n++] = '-';
						ptfloat = -ptfloat;
					}
					ptfint = (int)ptfloat;
					ptfflt = (int)(100000*(ptfloat-ptfint));
					if(ptfint == 0)										//��ӡ����
						buf[n++] = '0';
					else
					{
						numn = 1000000;
						while(numn != 0)
						{
							if(ptfint/numn)
								buf[n++] = (char)(ptfint/numn+48);
							if(ptfint == 0)
								buf[n++] = '0';
							ptfint %= numn;
							numn /= 10;
						}
					}
					buf[n++] = '.';
					if(ptfflt == 0)										//��ӡС��
						buf[n++] = '0';
					else
					{
						numn = 1000000;
						while(numn != 0)
						{
							if(ptfflt/numn)
								buf[n++] = (char)(ptfflt/numn+48);
							if(ptfflt == 0)
								buf[n++] = '0';
							ptfflt %= numn;
							numn /= 10;
						}
					}
					if(ptfloat < 0)
					{
						buf[n++] = '-';
						ptfloat = -ptfloat;
					}
					i++;
					continue;
			}
		}
		buf[n++] = str[i++];	//��ӡ�����ַ�
	}
	
	OLED_PStr(x0, y0, buf);
}

void OLED_DClean(void)	//��ջ���
{
	int i;
	
	for(i=0; i<1024; i++)
		GRAM[i] = 0x00;
}

void OLED_Display(void)
{
	unsigned char page, seg;
	
	for(page = 0; page < 8; page++)
	{
		OLED_SetPos(page, 0x00);
		for(seg = 0; seg < 128; seg++)
			WriteDat(GRAM[page*128+seg]);
	}
}

void OLED_Clean(void)//����
{
	unsigned char page, seg;
	
	for(page = 0; page < 8; page++)
	{
		OLED_SetPos(page, 0x00);
		for(seg = 0; seg < 128; seg++)
			WriteDat(0xFF);
	}
}

void OLED_GPIO_Init(void)	//���ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(OLED_RES_GPIO_CLK	, ENABLE);
	RCC_APB2PeriphClockCmd(OLED_DC_GPIO_CLK		, ENABLE);
	RCC_APB2PeriphClockCmd(OLED_CS_GPIO_CLK		, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin			= OLED_RES_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(OLED_RES_GPIO_PORT	, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin			= OLED_DC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(OLED_DC_GPIO_PORT		, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin			= OLED_CS_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(OLED_CS_GPIO_PORT		, &GPIO_InitStructure);
	
	GPIO_SetBits(OLED_RES_GPIO_PORT	, OLED_RES_GPIO_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);
}

void OLED_Init_Setting(void)	//���ó�ʼ��
{
	WriteCmd(0xAE); //�ر���ʾ
	
	WriteCmd(0x20); //�����ڴ��ַģʽ    
	WriteCmd(0x02); //[1:0]��00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	
	WriteCmd(0xB0); //ҳ��ַģʽ��������ʼҳ0~7
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //�Աȶ�����
	WriteCmd(0xFF); //���ȵ��� 0x00~0xFF
	
	WriteCmd(0xA1); //���ض�������,bit0:0,0->0;1,0->127;
	WriteCmd(0xC8); //Set COM Output Scan Direction
	
	WriteCmd(0xA6); //--set normal display
	WriteCmd(0xA8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xD3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xD5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xF0); //--set divide ratio
	WriteCmd(0xD9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xDA); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xDB); //����VCOMH ��ѹ����
	WriteCmd(0x20); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	WriteCmd(0x8D); //���õ�ɱ�
	WriteCmd(0x14); //bit2������/�ر�
	WriteCmd(0xAF); //������ʾ
}

void OLED_Init(void)
{
	//SPI
	SPI2_Init(SPI_BaudRatePrescaler_16);
	OLED_GPIO_Init();
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);
	
	//OLED����ʼ����ʱ
	delay_ms(500);

	//OLED���ó�ʼ��
	OLED_Init_Setting();
}

