#ifndef _PE4302_H_
#define _PE4302_H_

#include "sys.h"
#include "delay.h"


#define RCC_PE4302_CLK		RCC_AHB1Periph_GPIOE
#define RCC_PE4302_DAT	  	RCC_AHB1Periph_GPIOE
#define RCC_PE4302_LE		RCC_AHB1Periph_GPIOE


#define PE4302_CLK_GPIO	  	GPIOE
#define PE4302_DAT_GPIO	  	GPIOE
#define PE4302_LE_GPIO		GPIOE


#define PE4302_DAT_Pin	  	GPIO_Pin_0
#define PE4302_CLK_Pin		GPIO_Pin_2
#define PE4302_LE_0_Pin		GPIO_Pin_4
#define PE4302_LE_1_Pin		GPIO_Pin_6


#define PE_DAT         		PEout(0)
#define PE_CLK         		PEout(2)
#define PE_LE_0        		PEout(4)
#define PE_LE_1				PEout(6)


#define PE_LE_0_EN      	PE_LE_0 = 1
#define PE_LE_0_DIS     	PE_LE_0 = 0

#define PE_LE_1_EN      	PE_LE_1 = 1
#define PE_LE_1_DIS     	PE_LE_1 = 0

#define PE_CLK_0        PE_CLK = 0
#define PE_CLK_1        PE_CLK = 1

#define PE_DAT_0        PE_DAT = 0
#define PE_DAT_1        PE_DAT = 1

void PE_GPIO_Init(void);
void PE4302_Set(unsigned char db);
void PE4302_x_Set(unsigned char db, unsigned char num);
#endif
