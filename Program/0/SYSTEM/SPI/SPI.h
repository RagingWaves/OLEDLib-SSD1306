#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

//	SCK		PB13
//	MISO	PB14
//	MOSI	PB15

void SPI2_Init(u8 SPI_Speed);		//初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet);	//设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);	//SPI总线读写一个字节

#endif

