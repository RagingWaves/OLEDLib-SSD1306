#include "OLED.h"
#include "Font.h"

char GRAM[1024]={0};

void WriteCmd(u8 Cmd)
{
	GPIO_ResetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);
	GPIO_ResetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);//命令
	SPI2_ReadWriteByte(Cmd);
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);
}

void WriteDat(u8 Data)
{
	GPIO_ResetBits(OLED_CS_GPIO_PORT, OLED_CS_GPIO_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);
	SPI2_ReadWriteByte(Data);
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);
}

void OLED_SetPos(unsigned char page, unsigned char seg) //设置起始点坐标
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
	
	gcn = (y0/8)*128+x0;				//起始坐标
	gcx = (x0+bmpx)>127?127-x0:bmpx;	//x有效尺寸
	gcy = (y0+bmpy)>63?63-y0:bmpy;		//y有效尺寸
	dely = y0%8;						//y偏移量
	delly = 8-dely;
	
	if(gcx <= 0)
		return;							//x越界
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

void OLED_PStr(int x0, int y0, char *str, int FSize)
{
	int ch;
	int i=0;
	
	if(FSize)
	{
		for(i=0; str[i]!='\0'; i++)
		{
			ch = str[i] - 32;
			OLED_DBMP(x0, y0, 8, 16, &ASCII8x16[ch*16]);
			x0 += 8;
			if(x0 >= 120)
			{
				x0 = 0;
				y0 += 16;
			}
		}
	}
	else
	{
		for(i=0; str[i]!='\0'; i++)
		{
			ch = str[i] - 32;
			OLED_DBMP(x0, y0, 6, 8, &ASCII6x8[ch*6]);
			x0 += 6;
			if(x0 >= 122)
			{
				x0 = 0;
				y0 += 8;
			}
		}
	}
}

void OLED_DClean(void)
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

void OLED_Clean(void)//清屏
{
	unsigned char page, seg;
	
	for(page = 0; page < 8; page++)
	{
		OLED_SetPos(page, 0x00);
		for(seg = 0; seg < 128; seg++)
			WriteDat(0xFF);
	}
}

void OLED_GPIO_Init(void)
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

void OLED_Init_Setting(void)
{
	WriteCmd(0xAE); //关闭显示
	
	WriteCmd(0x20); //设置内存地址模式    
	WriteCmd(0x02); //[1:0]，00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	
	WriteCmd(0xB0); //页地址模式，设置起始页0~7
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //对比度设置
	WriteCmd(0xFF); //亮度调节 0x00~0xFF
	
	WriteCmd(0xA1); //段重定义设置,bit0:0,0->0;1,0->127;
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
	WriteCmd(0xDB); //设置VCOMH 电压倍率
	WriteCmd(0x20); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
	
	WriteCmd(0x8D); //设置电荷泵
	WriteCmd(0x14); //bit2，开启/关闭
	WriteCmd(0xAF); //开启显示
}

void OLED_Init(void)
{
	//SPI协议
	SPI2_Init(SPI_BaudRatePrescaler_16);
	OLED_GPIO_Init();
	GPIO_SetBits(OLED_CS_GPIO_PORT	, OLED_CS_GPIO_PIN);
	GPIO_SetBits(OLED_DC_GPIO_PORT	, OLED_DC_GPIO_PIN);
	
	//OLED屏初始化延时
	delay_ms(500);

	//OLED配置初始化
	OLED_Init_Setting();
}

