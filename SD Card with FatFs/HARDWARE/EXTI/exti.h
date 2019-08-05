#ifndef __EXTI_H
#define __EXTI_H	 
#include "sys.h"  	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

#define EQUIVALENT 0
#define CONTINUOUS 1

#define MAXDATA 20

extern u8 new_signal_flag;
extern uint64_t data_f;
extern uint64_t Freq;
extern uint32_t Freq_cnt_1;
extern uint32_t Freq_cnt_2;
extern uint16_t data[25][28];
extern uint16_t data_snd[MAXDATA];
extern u8 sample_mode;

void EXTIX_Init(void);	//外部中断初始化		 					    
#endif

























