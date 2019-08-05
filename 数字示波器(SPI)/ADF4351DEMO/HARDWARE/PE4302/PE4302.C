#include "PE4302.H"

/**************************************************************** 
函数名称: PE_GPIO_Init 
功    能: 管脚初始化 
参    数: 无
返回值  : 无 
*****************************************************************/ 

void PE_LE_Switch(unsigned char num, unsigned char order)
{
	if(order>0){
		switch(num){
			case 0: PE_LE_0_EN; break;
			case 1: PE_LE_1_EN; break;
			default: break;
		}

	}else{
		switch(num){
			case 0: PE_LE_0_DIS; break;
			case 1: PE_LE_1_DIS; break;
			default: break;
		}
	}
	

}

void PE_GPIO_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_PE4302_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_PE4302_DAT, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_PE4302_LE, ENABLE);

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 
	
	GPIO_InitStruct.GPIO_Pin=PE4302_CLK_Pin;
	GPIO_Init(PE4302_CLK_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=PE4302_DAT_Pin;
	GPIO_Init(PE4302_DAT_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	GPIO_InitStruct.GPIO_Pin=PE4302_LE_0_Pin;
	GPIO_Init(PE4302_LE_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=PE4302_LE_1_Pin;
	GPIO_Init(PE4302_LE_GPIO, &GPIO_InitStruct);
	PE_LE_0_DIS;
	PE_LE_1_DIS;
	PE_CLK_0;
	PE_DAT_0;
	
	
}
/**************************************************************** 
函数名称: PE4302_0_Set 
功    能: 衰减值设置 
参    数: db，范围：0~63，对应衰减0~31.5dB
返回值  : 无 
*****************************************************************/ 
void PE4302_Set(unsigned char db)
{
  unsigned int i;
  unsigned int W_DB;
	W_DB = db;
	PE_LE_0_DIS;
	PE_CLK_0;
	for(i = 0; i < 6;i++)
	{
		if (0x20 == (W_DB & 0X20))
		{	
		  PE_DAT_1;
		}
		else
		{
			PE_DAT_0;
		}
		  delay_us(500);
		  PE_CLK_1;
			delay_us(500);
			PE_CLK_0;
			delay_us(500);
		  W_DB = W_DB << 1;
     		
  }
	   PE_LE_0_EN;
     delay_us(500);
	   PE_LE_0_DIS;
     PE_DAT_0;
}

void PE4302_x_Set(unsigned char db, unsigned char num)
{
  unsigned int i;
  unsigned int W_DB;
	W_DB = db;
	PE_LE_Switch(num, 0);
	delay_ms(1);
	PE_CLK_0;
	for(i = 0; i < 6;i++)
	{
		if (0x20 == (W_DB & 0X20))
		{	
			PE_DAT_1;
		}
		else
		{
			PE_DAT_0;
		}
		delay_us(500);
		PE_CLK_1;
		delay_us(500);
		PE_CLK_0;
		delay_us(500);
		W_DB = W_DB << 1;
		
	}
	PE_LE_Switch(num, 1);
	delay_us(500);
	PE_LE_Switch(num, 0);
	PE_DAT_0;
}

