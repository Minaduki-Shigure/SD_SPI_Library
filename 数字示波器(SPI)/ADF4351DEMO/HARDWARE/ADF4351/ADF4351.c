/*------------------------------------------------------
Modify Author   : yusnows
Modify Date     : 2017.8.2
File            : ADF4351.c
version         : v0.0
-------------------------------------------------------*/

#include "ADF4351.h"
#include "delay.h"

//#define 
#define ADF4351_R0			((u32)0X2C8018)
#define ADF4351_R1			((u32)0X8029)
#define ADF4351_R2			((u32)0X10E42)
#define ADF4351_R3			((u32)0X4B3)
#define ADF4351_R4			((u32)0XEC803C)
#define ADF4351_R5			((u32)0X580005)

#define ADF4351_R1_Base		((u32)0X8001)
#define ADF4351_R4_Base		((u32)0X8C803C)
#define ADF4351_R4_ON	  	((u32)0X8C803C)	//0x8C803C
#define ADF4351_R4_OFF		((u32)0X8C883C)

#define ADF4351_RF_OFF		((u32)0XEC801C)

#define ADF4351_PD_ON		((u32)0X10E42)
#define ADF4351_PD_OFF		((u32)0X10E02)


ADF4351MSG ADF4351;

void ADF_Output_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_ADF4351_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_ADF4351_DOUT, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_ADF4351_DIN, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_ADF4351_LE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_ADF4351_CE, ENABLE);

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 
	
	GPIO_InitStruct.GPIO_Pin=ADF4351_CLK_Pin;
	GPIO_Init(ADF4351_CLK_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=ADF4351_DIN_Pin;
	GPIO_Init(ADF4351_DIN_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=ADF4351_CE_Pin;
	GPIO_Init(ADF4351_CE_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin=ADF4351_LE_Pin;
	GPIO_Init(ADF4351_LE_GPIO, &GPIO_InitStruct);
}

void ADF_DOUT_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_ADF4351_DOUT, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin=ADF4351_DOUT_Pin;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(ADF4351_DOUT_GPIO, &GPIO_InitStruct);
}

void delay (int length)
{
	while (length >0)
    	length--;
}

void WriteToADF4351(u8 count, u8 *buf)
{
	u8 ValueToWrite = 0;
	u8 i = 0;
	u8 j = 0;
	
//	ADF_Output_GPIOInit();
	
	ADF4351_CE = 1;
	delay_us(1);
	ADF4351_CLK = 0;
	ADF4351_LE = 0;
	delay_us(1);
	
	for(i = count; i>0; i--)
	{
		ValueToWrite = *(buf+i-1);
		for(j=0; j<8; j++)
		{
			if(0x80 == (ValueToWrite & 0x80))
			{
				ADF4351_DIN = 1;
			}
			else
			{
				ADF4351_DIN = 0;
			}
			delay_us(1);
			ADF4351_CLK = 1;
			delay_us(1);
			ValueToWrite <<= 1;
			ADF4351_CLK = 0;	
		}
	}
	ADF4351_DIN = 0;
	delay_us(1);
	ADF4351_LE = 1;
	delay_us(1);
	ADF4351_LE = 0;
}


void ReadToADF4351(u8 count, u8 *buf)
{
	u8 i = 0;
	u8 j = 0;
	u8 iTemp = 0;
	u8 RotateData = 0;
	
	ADF_DOUT_GPIOInit();
	ADF4351_CE = 1;
	delay_us(1);
	ADF4351_CLK = 0;
	ADF4351_LE = 0;
	delay_us(1);
	
	for(i = count; i>0; i--)
	{
		for(j = 0; j<8; j++)
		{
			RotateData <<=1;
			delay_us(1);
			iTemp = ADF4351_DOUT;
			ADF4351_CLK = 1;
			if(0x01 == (iTemp&0x01))
			{
				RotateData |= 1;
			}
			delay_us(1);
			ADF4351_CLK = 0;
		}
		*(buf+i-1) = RotateData;
	}
	delay_us(1);
	ADF4351_LE = 1;
	delay_us(1);
	ADF4351_LE = 0;
}
void ADF4351MEGInit(void)
{
	ADF4351.R0=0x7D00000;
	ADF4351.R1=0x8011;
	ADF4351.R2=0x10C8E42;
	ADF4351.R3=0x404B3;
	ADF4351.R4=0xB0A0B4;
	ADF4351.R5=0x580005;
    	
	ADF4351.RFDiv=8;
    ADF4351.INT=4000;
    ADF4351.MOD=2;
    ADF4351.FRAC=0;
	ADF4351.BandCLKSelDiv=10;
	ADF4351.CurrentFreq=500*MHz;
	ADF4351.Fpfd=1*MHz;
}

void ADF4351Init(void)
{
	u8 buf[4] = {0,0,0,0};
	ADF4351MEGInit();
	ADF_Output_GPIOInit();
	
	TransformInt32ToU8((uint32_t)ADF4351.R5,buf);
	WriteToADF4351(4,buf);		
	TransformInt32ToU8((uint32_t)ADF4351.R4,buf);
	WriteToADF4351(4,buf);		

	TransformInt32ToU8((uint32_t)ADF4351.R3,buf);
	WriteToADF4351(4,buf);	

	TransformInt32ToU8((uint32_t)ADF4351.R2,buf);
	WriteToADF4351(4,buf);		

	TransformInt32ToU8((uint32_t)ADF4351.R1,buf);
	WriteToADF4351(4,buf);
							   
	TransformInt32ToU8((uint32_t)ADF4351.R0,buf);
	WriteToADF4351(4,buf);	
}

///////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////
void ADF4351RangeConstInit(void)
{
    
}
void WriteOneRegToADF4351(u32 Regster)
{
	u8 buf[4] = {0,0,0,0};
	buf[3] = (u8)((Regster>>24)&(0X000000FF));
	buf[2] = (u8)((Regster>>16)&(0X000000FF));
	buf[1] = (u8)((Regster>>8) &(0X000000FF));
	buf[0] = (u8)((Regster)&(0X000000FF));
	WriteToADF4351(4,buf);	
}
void ADF4351_Init_some(void)
{
	WriteOneRegToADF4351(ADF4351_R2);
	WriteOneRegToADF4351(ADF4351_R3);
	WriteOneRegToADF4351(ADF4351_R5);
}


void ADF4351WriteFreq(double Fre)		//	(xx.x) M Hz
{
	u16 Fre_temp, N_Mul = 1, Mul_Core = 0;
	u16 INT_Fre, Frac_temp, Mod_temp, i;
	double multiple;
	
	if(Fre < 35.0)
		Fre = 35.0;
	if(Fre > 4400.0)
		Fre = 4400.0;
	Mod_temp = 1000;
	Fre = ((double)((u32)(Fre*1000)))/1000;
	ADF4351.CurrentFreq=Fre*MHz;        //ADF4351.CurrentFreq 的单位为 Hz
	Fre_temp = Fre;
	for(i = 0; i < 10; i++)
	{
		if(((Fre_temp*N_Mul) >= 2199.9) && ((Fre_temp*N_Mul) <= 4400.1))
			break;
		Mul_Core++;
		N_Mul = N_Mul*2;
	}
	
	multiple = (Fre*N_Mul)/1;		//带宽出已固定为参考1M，若改参考频率，则应修改R4低字节
	INT_Fre = (u16)multiple;
//	if(((u32)(multiple*10000)%10)> 5)
//		multiple += 0.001;
	Frac_temp = ((u32)(multiple*1000))%1000;
	while(((Frac_temp%5) == 0) && ((Mod_temp%5) == 0))
	{
		Frac_temp = Frac_temp/5;
		Mod_temp = Mod_temp/5;
	}
	while(((Frac_temp%2) == 0)&&((Mod_temp%2) == 0))
	{
		Frac_temp = Frac_temp/2;
		Mod_temp = Mod_temp/2;
	}
	
	Mul_Core %= 7;
    ADF4351.INT=INT_Fre;
    ADF4351.FRAC=Frac_temp;
    ADF4351.RFDiv=1<<Mul_Core;
    ADF4351.MOD=Mod_temp;
	ADF4351.R0 = (INT_Fre<<15)+(Frac_temp<<3);
	ADF4351.R1 = ADF4351_R1_Base + (Mod_temp<<3);
	ADF4351.R4 = ADF4351_R4_ON + (Mul_Core<<20);
    
    ADF4351.CurrentVCOFreq=Fre*ADF4351.RFDiv;
	
//	WriteOneRegToADF4351(ADF4351_PD_OFF); //ADF4351_RF_OFF
//	WriteOneRegToADF4351((u32)(ADF4351_R4_OFF + (6<<20)));
	WriteOneRegToADF4351(ADF4351_RF_OFF);
	WriteOneRegToADF4351(ADF4351.R1);
	WriteOneRegToADF4351(ADF4351.R0);
	WriteOneRegToADF4351(ADF4351.R4);
//	WriteOneRegToADF4351(ADF4351_PD_ON);
	
//	WriteOneRegToADF4351((u32)(ADF4351_R4_ON + (Mul_Core<<20)));
}

void TransformInt32ToU8(uint32_t value,u8 buf[4])
{
	buf[0]=value&0x000000FF;
	buf[1]=(value>>8)&0x000000FF;
	buf[2]=(value>>16)&0x000000FF;
	buf[3]=(value>>24)&0x000000FF;
}


