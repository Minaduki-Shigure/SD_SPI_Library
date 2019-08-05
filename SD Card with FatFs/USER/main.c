#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "adc.h"
#include "spi.h"
#include "key.h"
#include "LCD12864.h"
//#include "KeyBoard.h"
#include "EXTI.h"
#include <math.h>
#include "gt9147.h"
#include "touch.h"
#include "ctiic.h"
#include "string.h" 
#include "exti.h"
#include "dac.h"
#include "timer.h"

#include "malloc.h" 
#include "sdio_sdcard.h"

#include "diskio.h"
#include "ff.h"
#include "ffconf.h"
#include "exfuns.h"

//definations -->


//variables defination -->
int i;
int j;

uint64_t data_f=0;
u8 new_signal_flag=0;
uint16_t data_snd[MAXDATA]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	
FIL* fp;
UINT size;

//function defination -->
u8 touchandgive(void);
u8	GT9147_Init(void);
void Filter_Menu(void);
	
//void show_sdcard_info(void);

int main(void)
{
	u8 key;
	u32 sd_size;
	u8 t=0;
	u8 res=0;
	u32 total,free;
	u8 *buf;
	char msg[50]="Hello from Minaduki Shigure, have a nice day!";
	int i;
	uint16_t *content;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
	reset_init();
	LCD_Init();
	LCD_Display_Dir(1);
	LCD_DisplayOn();
	//Adc_Init();
	//Init_KeyBoard_Port();
	//KeyBoard_Interrupt_Init();
  GT9147_Init();
	LCD_Clear(DARKBLUE);
	POINT_COLOR=RED;
	BACK_COLOR=DARKBLUE;
	SPI1_Init();	
	SPI1_SetSpeed(SPI_BaudRatePrescaler_64);
	EXTIX_Init();
	Dac1_Init();
	
	my_mem_init(SRAMIN);	//We use the ALIENTEK malloc size(100KiB) to avoid mistakes, but it should work properly if you use no more than 192KiB.
	
	LCD_ShowString(30,50,200,16,16,"STM32F407ZG");	
	LCD_ShowString(30,70,200,16,16,"SD CARD & SPI SEND TEST");	
	LCD_ShowString(30,90,200,16,16,"MINADUKI SHIGURE @ NJU");
	LCD_ShowString(30,110,200,16,16,"2019/8/2");
	
	while(SD_Init())
	{
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);					
		LCD_ShowString(30,150,200,16,16,"Please Check!");
		delay_ms(500);
	}
	
	//show_sdcard_info();
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:LCD_ShowString(30,130,200,16,16,"Card Type:SDSC V1.1");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:LCD_ShowString(30,130,200,16,16,"Card Type:SDSC V2.0");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:LCD_ShowString(30,130,200,16,16,"Card Type:SDHC V2.0");break;
		case SDIO_MULTIMEDIA_CARD:LCD_ShowString(30,130,200,16,16,"Card Type:MMC Card");break;
	}	
	LCD_ShowString(30,150,200,16,16,"SD Card OK    ");
	LCD_ShowString(30,170,200,16,16,"SD Card Size:     MB");
	LCD_ShowNum(30+13*8,170,SDCardInfo.CardCapacity>>20,5,16);//显示SD卡容量
	
	LCD_ShowString(330,190,300,16,16,"Now trying to mount the SD card");
	exfuns_init();
	res=f_mount(fs[0],"0:",1);
	if(res==FR_NO_FILESYSTEM)//磁盘,FAT文件系统错误,重新格式化
	{
		LCD_ShowString(330,210,200,16,16,"SD card formatting...");	//格式化
		buf=mymalloc(SRAMIN,512);
		res=f_mkfs("0:",0,4096);
		if(res==0)
		{
			f_setlabel((const TCHAR *)"0:MINADUKI");	//设置磁盘的名字为：MINADUKI
			LCD_ShowString(330,230,300,16,16,"SD card format completed.");	//格式化完成
		}else LCD_ShowString(330,230,300,16,16,"SD card format failed.");	//格式化失败
		delay_ms(1000);
		myfree(SRAMIN,buf);
	}
	else
	{
		LCD_ShowString(330,210,300,16,16,"SD card file system check passed!");
	}
	
	while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		LCD_ShowString(330,250,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(330,250,540,250+16,DARKBLUE);	//清除显示			  
		delay_ms(200);
	}
	LCD_Fill(330,250,540,250+16,DARKBLUE);	//清除显示
	LCD_ShowString(330,250,200,16,16,"FATFS OK!");	 
	LCD_ShowString(330,270,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(330,290,200,16,16,"SD  Free Size:     MB"); 	    
 	LCD_ShowNum(330+8*14,270,total>>10,5,16);				//显示SD卡总容量 MB
 	LCD_ShowNum(330+8*14,290,free>>10,5,16);					//显示SD卡剩余容量 MB	
	
	POINT_COLOR=YELLOW;
	delay_ms(500);
	LCD_ShowString(30,190,300,16,16,"Now trying to read Sector 0");
	delay_ms(500);
	buf=mymalloc(SRAMIN,512);
	LCD_ShowString(30,210,200,16,16,"Reading");
	while(SD_ReadDisk(buf,0,1))
		LCD_ShowString(30,210,200,16,16,"Reading");
	for(sd_size=0;sd_size<512;sd_size++)
	{
		LCD_ShowNum(30,230,sd_size,5,16);
		LCD_ShowNum(180,230,buf[sd_size],8,16);
		//delay_ms(20);
	}
	LCD_ShowString(30,250,200,16,16,"Read completed.");
	
//	for(sd_size=0;sd_size<512;sd_size++)
//	{
//		buf[sd_size]=sd_size%256;
//	}
//	LCD_ShowString(30,270,300,16,16,"Now trying to write Sector 0");
//	SD_WriteDisk(buf,0,1);
//	LCD_ShowString(30,290,200,16,16,"Write completed.");
//	myfree(SRAMIN,buf);
//	delay_ms(500);
	
//	LCD_ShowString(30,190,300,16,16,"Now trying to read Sector 0");
//	delay_ms(500);
//	//buf=mymalloc(SRAMIN,512);
//	LCD_ShowString(30,210,200,16,16,"Reading");
//	while(SD_ReadDisk(buf,0,1))
//		LCD_ShowString(30,210,200,16,16,"Reading");
//	for(sd_size=0;sd_size<512;sd_size++)
//	{
//		LCD_ShowNum(30,230,sd_size,5,16);
//		LCD_ShowNum(180,230,buf[sd_size],8,16);
//		//delay_ms(20);
//	}
//	LCD_ShowString(30,250,200,16,16,"Read completed.");
//	myfree(SRAMIN,buf);
	
	fp=mymalloc(SRAMIN,sizeof(FIL));
	f_open(fp,"0:helloworld",FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	f_write(fp,"Hello from Minaduki Shigure, have a nice day!",sizeof("Hello from Minaduki Shigure, have a nice day!"),&size);
	f_close(fp);
	LCD_ShowString(30,270,200,16,16,"File write completed.");
	
	buf=mymalloc(SRAMIN,512);
	memset(buf,EOF,512);
	f_open(fp,"0:helloworld",FA_READ);
	f_read(fp,buf,50,&size);
	f_close(fp);
	LCD_ShowString(30,290,300,16,16,"File read completed.");
	LCD_ShowString(30,310,400,16,16,buf);
	myfree(SRAMIN,buf);
	
	buf=mymalloc(SRAMIN,512);
	memset(buf,EOF,512);
	f_open(fp,"0:heart16k.wav",FA_READ);
	f_lseek(fp,0x24);
	f_read(fp,buf,512,&size);
	f_lseek(fp,0x2C);
	content=mymalloc(SRAMIN,100000*sizeof(uint16_t));
	memset(content,EOF,100000*sizeof(uint16_t));
	f_read(fp,content,100000*sizeof(uint16_t),&size);
	f_close(fp);
	LCD_ShowString(30,330,300,16,16,"File read completed.");
	LCD_ShowString(30,350,400,16,16,buf);
	myfree(SRAMIN,content);
	
	myfree(SRAMIN,buf);
	myfree(SRAMIN,fp);
	
	for(i=0;i<100000;i++)
	{
		Dac1_Set_Vol((float)content[i]/256*3300);
		//delay_ms(20);
	}
	
	fp=mymalloc(SRAMIN,sizeof(FIL));
	f_open(fp,"0:heart16k.wav",FA_READ);
	f_lseek(fp,0x2C);
	TIM3_Int_Init(5250-1,8400-1);	//定时器时钟84M，分频系数(Disabled)，所以84M的计数频率，计数5000次为16k;
	while(1);
}

