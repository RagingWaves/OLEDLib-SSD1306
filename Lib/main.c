#include "sys.h"
#include "OLED.h"

void Init(void)
{
	OLED_Init();
	OLED_Clean();
	delay_ms(500);
}

int main(void)
{
	Init();
	while(1)
	{
		OLED_FSize(1);
		OLED_PStr(6,4,"hello hello hello");
		OLED_FSize(0);
		OLED_printf(3,20,"c:%c%c%c\ns:%s\r\nd:%d\n",'j','k','l', "test",1234567890);
		OLED_Display();
		delay_ms(500);
	}
}

