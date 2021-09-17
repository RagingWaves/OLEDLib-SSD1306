#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include "SPI.h"

#define OLED_RES_GPIO_PIN	GPIO_Pin_12
#define OLED_RES_GPIO_PORT	GPIOB
#define OLED_RES_GPIO_CLK	RCC_APB2Periph_GPIOB

#define OLED_DC_GPIO_PIN	GPIO_Pin_8
#define OLED_DC_GPIO_PORT	GPIOA
#define OLED_DC_GPIO_CLK	RCC_APB2Periph_GPIOA

#define OLED_CS_GPIO_PIN	GPIO_Pin_9
#define OLED_CS_GPIO_PORT	GPIOA
#define OLED_CS_GPIO_CLK	RCC_APB2Periph_GPIOA

void OLED_DBMP(int x0, int y0, int bmpx, int bmpy, const char *bmp);	//��ʾλͼ
void OLED_FSize(int size);												//���������С
void OLED_PStr(int x0, int y0, const char *str);						//��ӡ�ַ���
void OLED_PCh(int x0, int y0, char ch);									//��ӡ�����ַ�
void OLED_printf(int x0, int y0, const char *str, ...);					//��ӡ����
void OLED_Display(void);												//��ʾ��������
void OLED_Clean(void);													//��ջ���
void OLED_Init(void);													//��ʼ��

#endif

