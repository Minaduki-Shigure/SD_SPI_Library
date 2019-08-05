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

//definations -->
#define XaxisHeight 12
#define YaxisHeight XaxisHeight

#define Workspace_x_max 500
#define Workspace_y_max 400

#define MODE_CHANGE_FLAG 0

#define Y_MAXIMUM 20

//variables defination -->
extern int Enter_flag;
int menu_flag;
int Draw_finish_flag=1;
int i;
int j,a;
int X_pos=0;
int hori_div;
float Y_to_draw=100;
float Y_last=100;

uint64_t data_f=0;
u8 new_signal_flag=0;
uint32_t Freq_cnt_1=0;
uint32_t Freq_cnt_2=0;
uint64_t Freq=0;
//uint16_t data[28]={0,23,48,89,120,170,210,230,0,0,240,200,197,145,123,110,97,78,32,0, 1,2,3,4,0xA,0xB,0xC,0xD};
uint16_t data[25][28]={0,23,48,89,120,170,210,230,0,0,240,200,197,145,123,110,97,78,32,0, 1,2,3,4,0xA,0xB,0xC,0xD};
u8 sample_mode = EQUIVALENT;

//function defination -->
u8 touchandgive(void);
u8	GT9147_Init(void);
void Display_Wave(void);
void Filter_Menu(void);

int main(void)
{
	char mode, key_char;
	double input=0.0;
	unsigned char* input_s ;
	//char* menuitem[4]={"pattern1","pattern2"};
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
	reset_init();
	LCD_Init();
	LCD_Display_Dir(1);
	LCD_DisplayOn();
	Adc_Init();
	//Init_KeyBoard_Port();
	//KeyBoard_Interrupt_Init();
  GT9147_Init();
	LCD_Clear(DARKBLUE);
	POINT_COLOR=RED;
	BACK_COLOR=DARKBLUE;
	SPI1_Init();	
	SPI1_SetSpeed(SPI_BaudRatePrescaler_64);
	EXTIX_Init();
	
	POINT_COLOR=WHITE;
//	menu_flag = 1;
//	mode=1;
		
					
						Filter_Menu();
						while(1)
						{								
						//设置x轴挡位
							i=4;
							while(tp_dev.x[i]>370&&tp_dev.x[i]<470&&tp_dev.y[i]>10&&tp_dev.y[i]<140)
							{
								POINT_COLOR=WHITE;	
								LCD_ShowxString(690,60,100,24,24,(u8*)"20ms/div",0);
								LCD_ShowxString(670,180,100,24,24,(u8*)"200us/div",0);
								LCD_ShowxString(690,300,100,24,24,(u8*)"2us/div",0);
								GT9147_Scan(0);
								hori_div=100;
								Display_Wave();
								POINT_COLOR=RED;	
								LCD_ShowxString(670,420,100,24,24,(u8*)"100ns/div",0);
							}
							
							while(tp_dev.x[i]>250&&tp_dev.x[i]<350&&tp_dev.y[i]>10&&tp_dev.y[i]<140)
							{
								POINT_COLOR=WHITE;	
								LCD_ShowxString(690,60,100,24,24,(u8*)"20ms/div",0);
								LCD_ShowxString(670,180,100,24,24,(u8*)"200us/div",0);
								LCD_ShowxString(670,420,100,24,24,(u8*)"100ns/div",0);
								GT9147_Scan(0);
								hori_div=20000000;
								Display_Wave();
								POINT_COLOR=RED;	
								LCD_ShowxString(690,300,100,24,24,(u8*)"2us/div",0);
							}	
							while(tp_dev.x[i]>130&&tp_dev.x[i]<230&&tp_dev.y[i]>10&&tp_dev.y[i]<140)
							{
								POINT_COLOR=WHITE;	
								LCD_ShowxString(690,60,100,24,24,(u8*)"20ms/div",0);
								LCD_ShowxString(690,300,100,24,24,(u8*)"2us/div",0);
								LCD_ShowxString(670,420,100,24,24,(u8*)"100ns/div",0);
								GT9147_Scan(0);
								hori_div=190000;
								Display_Wave();
								POINT_COLOR=RED;	
								LCD_ShowxString(670,180,100,24,24,(u8*)"200us/div",0);
							}	
							while(tp_dev.x[i]>10&&tp_dev.x[i]<110&&tp_dev.y[i]>10&&tp_dev.y[i]<140)
							{
								POINT_COLOR=WHITE;	
								LCD_ShowxString(670,180,100,24,24,(u8*)"200us/div",0);
								LCD_ShowxString(690,300,100,24,24,(u8*)"2us/div",0);
								LCD_ShowxString(670,420,100,24,24,(u8*)"100ns/div",0);
								GT9147_Scan(0);
								hori_div=1000;
								Display_Wave();
								POINT_COLOR=RED;	
								LCD_ShowxString(690,60,100,24,24,(u8*)"20ms/div",0);
							}	
							GT9147_Scan(0);
						}		
}
void Display_Wave(void)
{
	int i,j,k;
	int b;
	int c;
	u16 y1,y2,y3,y4;
	u32 cnt1,cnt2;
	double pixel_step,display[20];
	cnt1=((u32)data[0][20])|((u32)data[0][21]<<8)|((u32)data[0][22]<<16)|((u32)data[0][23]<<24);
	cnt2=((u32)data[0][24])|((u32)data[0][25]<<8)|((u32)data[0][26]<<16)|((u32)data[0][27]<<24);
	
	//display frequency
	POINT_COLOR=RED;
	//LCD_ShowxNum(30,400,cnt1,20,28,1);
	//LCD_ShowxNum(140,400,cnt2,20,28,1);
	//LCD_ShowxNum(140,380,cnt2*10000.0/cnt1,20,28,1);
	
	if(sample_mode==0)
	{
		for(i=0; i<20; i++)display[i]=data[0][i]*10/256.0-5;								//change to voltage	
//	//display voltage
//	LCD_Fill(30,70,90,100,BLACK);
//	LCD_Fill(30,400,770,460,BLACK);
//	for(i=0;i<10;i++)LCD_ShowxFloat(30+i*75,400,display[i],1,5,32,1);
//	for(i=0;i<10;i++)LCD_ShowxFloat(30+i*75,430,display[i+10],1,5,32,1);
	
	//display wave
	POINT_COLOR=GREEN;
	for(i=0; i<20; i++)display[i]=(10-display[i])*20;								//change to coordinate
	pixel_step=25*100000000.0/(cnt2*hori_div);
	LCD_ShowxFloat(140,360,pixel_step,3,7,28,1);
	y1=display[0];	
	//for(k=0;k<3;k++)
	//{
		for(j=0;;j++)
		{
			for(i=0;(i<20)&&((20*pixel_step*j+pixel_step*(i))<500); i++)
			{ 
				y2=display[i];//+k
				if(!(i==0&&j==0&&k==0))
					LCD_DrawLine((int)(pixel_step*(20*j+(i))),y1,(int)(pixel_step*(20*j+(i+1))),y2);
				y1=y2;
				GT9147_Scan(0);
			}
			if((20*pixel_step*j+pixel_step*(i))>=500)break;
						X_pos=pixel_step*(20*j+(i));
			for(;X_pos<pixel_step*(20*j+(i))+30*pixel_step+20;X_pos++)
			{
						if(X_pos<Workspace_x_max-1)
						{
							GT9147_Scan(0);			
							LCD_Fill(X_pos+1, 0, X_pos+1, Workspace_y_max, BLACK);
							if(!((X_pos+1)%50))
							{
								for(b=1;b<41;b++)
								{
									LCD_DrawPoint(X_pos+1,10*b);
									GT9147_Scan(0);			
								}
							}
							else if(!((X_pos+1)%10))
							{
								for(a=0; a<9; a++)
								{
									LCD_DrawPoint(X_pos+1, 50*a);
									GT9147_Scan(0);			
								}	
							}
						}  
						else
						{
							LCD_Fill(X_pos+1-Workspace_x_max, 0, X_pos+1-Workspace_x_max, Workspace_y_max, BLACK);
							if(!((X_pos+1-Workspace_x_max)%50))
							{
								for(b=1;b<41;b++)
								{
									LCD_DrawPoint(X_pos+1-Workspace_x_max,10*b);
									GT9147_Scan(0);			
								}
							}
							else if(!((X_pos+1-Workspace_x_max)%10))
							{
								for(a=0; a<9; a++)
								{
									LCD_DrawPoint(X_pos+1-Workspace_x_max, 50*a);
									GT9147_Scan(0);			
								}	
							}	
						}
			}
		}
	}
	if(sample_mode==1)
	{
		
		LCD_Fill(0,0,500,400,BLACK);
		POINT_COLOR=WHITE;
		for(i=1;i<10;i++)
		{
			for(j=1;j<41;j++)LCD_DrawPoint(50*i,10*j);
		}
		for(i=1;i<8;i++)
		{
			for(j=1;j<51;j++)LCD_DrawPoint(10*j,50*i);
		}																				//画虚线
		
		LCD_DrawLine(0, 0, 500, 0);
		LCD_DrawLine(0, 400, 500, 400);					//横线
		
		LCD_DrawLine(0, 0, 0, 400);
		LCD_DrawLine(500, 0, 500  , 400);					//竖线		
		
		for(j=0;j<25;j++)
			{
			POINT_COLOR=GREEN;
			for(i=0; i<20; i++)display[i]=data[j][i]*10/256.0-5;
			for(i=0; i<20; i++)display[i]=(10-display[i])*20;	
			y1=display[0];
			for(c=0;c<20;c++)
				{
					y2=display[c];
					if(!(c==0&&j==0))
						LCD_DrawLine((int)(20*j+c),y1,(int)(20*j+c+1),y2);
						y1=y2;
						GT9147_Scan(0);	
				}
			y3=(10-(data[j][19]*10/256.0-5))*20;
			y4=(10-(data[j+1][0]*10/256.0-5))*20;
			LCD_DrawLine((int)(20*j+20),y3,(int)(20*j+21),y4);
			if((20*j+c)>=500)break;
			//描点画图
				
							
	//			POINT_COLOR=WHITE;
	//				LCD_DrawPoint(X_pos++, Y_to_draw*Workspace_y_max);
	//				if(X_pos<Workspace_x_max-30){
	//					LCD_Fill(X_pos+30, 0, X_pos+30, Workspace_y_max, DARKBLUE);
	//					for(i=0; i<9; i++){
	//						LCD_DrawPoint(X_pos+30, 50*i);
	//					}		
	//				}
	//				else{
	//					LCD_Fill(X_pos+30-Workspace_x_max, 0, X_pos+30-Workspace_x_max, Workspace_y_max, DARKBLUE);
	//					for(i=0; i<9; i++){
	//						LCD_DrawPoint(X_pos+30-Workspace_x_max, 50*i);
	//					}		
	//				}
	//				if(X_pos==Workspace_x_max)
	//					X_pos=0;
				
			}
		mydelay(1000);
		}	
	}
//}

void Filter_Menu(void)
{
//画矩形框
						LCD_DrawRectangle(510, 10, 640, 110 );
						LCD_DrawRectangle(660, 10, 790, 110 );
						LCD_DrawRectangle(510,130, 640, 230 );
						LCD_DrawRectangle(660,130, 790, 230 );
						LCD_DrawRectangle(510,250, 640, 350 );
						LCD_DrawRectangle(660,250, 790, 350 );
						LCD_DrawRectangle(510,370, 640, 470 );
						LCD_DrawRectangle(660,370, 790, 470 );//右两列
						
						LCD_DrawRectangle(30,420, 160, 470 );
						LCD_DrawRectangle(190,420, 320, 470 );
						LCD_DrawRectangle(350,420, 480, 470 );//下一行
						
						//示波器按键显示
						LCD_ShowxString(530,60,100,24,24,(u8*)"1V/div",0);
						LCD_ShowxString(690,60,100,24,24,(u8*)"20ms/div",0);
						LCD_ShowxString(530,180,100,24,24,(u8*)"0.1V/div",0);
						LCD_ShowxString(670,180,100,24,24,(u8*)"200us/div",0);
						LCD_ShowxString(530,300,100,24,24,(u8*)"2mV/div",0);
						LCD_ShowxString(690,300,100,24,24,(u8*)"2us/div",0);
						LCD_ShowxString(530,420,100,24,24,(u8*)"auto",0);
						LCD_ShowxString(670,420,100,24,24,(u8*)"100ns/div",0);
						
						LCD_ShowxString(60,445,100,24,24,(u8*)"trigger",0);
						LCD_ShowxString(220,445,100,24,24,(u8*)"display",0);
						LCD_ShowxString(380,445,100,24,24,(u8*)"storage",0);
						
						//示波器界面画格
		  			LCD_Fill(0,0,500,400,BLACK);
						POINT_COLOR=WHITE;
						for(i=1;i<10;i++)
						{
							for(j=1;j<41;j++)LCD_DrawPoint(50*i,10*j);
						}
						for(i=1;i<8;i++)
						{
							for(j=1;j<51;j++)LCD_DrawPoint(10*j,50*i);
						}																				//画虚线
						
						LCD_DrawLine(0, 0, 500, 0);
						LCD_DrawLine(0, 400, 500, 400);					//横线
						
						LCD_DrawLine(0, 0, 0, 400);
						LCD_DrawLine(500, 0, 500  , 400);					//竖线					
						
}

