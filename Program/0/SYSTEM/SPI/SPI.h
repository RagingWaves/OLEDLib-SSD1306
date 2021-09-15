#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

//	SCK		PB13
//	MISO	PB14
//	MOSI	PB15

void SPI2_Init(u8 SPI_Speed);		//��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet);	//����SPI�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);	//SPI���߶�дһ���ֽ�

#endif

