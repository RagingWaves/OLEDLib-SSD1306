#include "SPI.h"

void SPI2_Init(u8 SPI_Speed)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,	ENABLE);//PORTBʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,		ENABLE);//SPI2ʱ��ʹ��

	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;							//PB13/14/15�����������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);										//��ʼ��GPIOB

	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);					//PB13/14/15����

	SPI_InitStructure.SPI_Direction			= SPI_Direction_2Lines_FullDuplex;	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master;					//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize			= SPI_DataSize_8b;					//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL				= SPI_CPOL_Low;					//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA				= SPI_CPHA_1Edge;					//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS				= SPI_NSS_Soft;						//NSS�ź���Ӳ����NSS�ܽţ�����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_16;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit			= SPI_FirstBit_MSB;					//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial		= 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);		//ʹ��SPI����

	SPI2_ReadWriteByte(0xFF);	//��������
	
	SPI2_SetSpeed(SPI_Speed);	//����SPI�ٶ�
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ
//SPI_BaudRatePrescaler_4   4��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ
//SPI_BaudRatePrescaler_16  16��Ƶ
// 32 64 128
//SPI_BaudRatePrescaler_256 256��Ƶ
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ�
	SPI_Cmd(SPI2,ENABLE);
} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
//SPIͨ��Ϊ�����������ͽ�����ͬʱ����
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry = 0;
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)	//���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry>200)
			return 0;
	}
	SPI_I2S_SendData(SPI2, TxData);									//ͨ������SPIx����һ������
	retry = 0;

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)	//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry>200)
			return 0;
	}
	
	return SPI_I2S_ReceiveData(SPI2);								//����ͨ��SPIx������յ�����
}
