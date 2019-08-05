/*------------------------------------------------------
Modify Author: yusnows
Modify Date  : 2017.8.2
File         : ADF4351.h
version      : v0.0
-------------------------------------------------------*/


#ifndef _ADF4351_H_
#define _ADF4351_H_

#include "sys.h" 


#define RCC_ADF4351_CLK		RCC_AHB1Periph_GPIOF
#define RCC_ADF4351_DOUT	RCC_AHB1Periph_GPIOF
#define RCC_ADF4351_DIN		RCC_AHB1Periph_GPIOF
#define RCC_ADF4351_LE		RCC_AHB1Periph_GPIOF
#define RCC_ADF4351_CE		RCC_AHB1Periph_GPIOF

#define ADF4351_CLK_GPIO	GPIOF
#define ADF4351_DIN_GPIO	GPIOF
#define ADF4351_DOUT_GPIO	GPIOF
#define ADF4351_LE_GPIO		GPIOF
#define ADF4351_CE_GPIO		GPIOF

#define ADF4351_CLK_Pin		GPIO_Pin_1
#define ADF4351_DOUT_Pin	GPIO_Pin_3
#define ADF4351_DIN_Pin		GPIO_Pin_3
#define ADF4351_LE_Pin		GPIO_Pin_5
#define ADF4351_CE_Pin		GPIO_Pin_7

#define ADF4351_CLK         PFout(3)
#define ADF4351_DIN         PFout(5)
#define ADF4351_LE          PFout(7)
#define ADF4351_CE          PFout(9)

#define ADF4351_DOUT        PFin(5)

#ifndef kHz
#define kHz		            1000
#endif
#ifndef MHz
#define MHz		            1000*kHz
#endif

#define FREQREFIN	        100*MHz

#define R0ADDR		        0
#define R1ADDR		        1
#define R2ADDR		        2
#define R3ADDR	`	        3
#define R4ADDR		        4
#define R5ADDR		        5

typedef struct
{
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R4;
	uint32_t R5;
	double Fpfd;		//鉴相器频率;Ffpd=FREQREFIN*((1+D)/(R*(1+T))),一般设置：D=0，T=1
	uint32_t RFDiv;	//RF输出分频
    uint32_t INT;
    uint32_t MOD;
    uint32_t FRAC;
	uint32_t BandCLKSelDiv;	//锁定定时器时钟分频
	double CurrentFreq;	    //当前ADF4351的输出频率
    double CurrentVCOFreq;      //当前AF4351VCO频率,单位 MHz
	
}ADF4351MSG;

extern ADF4351MSG ADF4351;

void ADF4351Init(void);
void ReadToADF4351(u8 count, u8 *buf);
void WriteToADF4351(u8 count, u8 *buf);
void WriteOneRegToADF4351(u32 Regster);
void ADF4351_Init_some(void);
void ADF4351WriteFreq(double Fre);		//	(xx.x) M Hz

void ADF4351MEGInit(void);
void ADF4351RangeConstInit(void);
void TransformInt32ToU8(uint32_t value,u8 buf[4]);

#endif

