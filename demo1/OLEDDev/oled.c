////////////////////////////////////////////////////////////////////////////////////	 
////本程序只供学习使用，未经作者许可，不得用于其它任何用途
////中景园电子
////店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
////
////  文 件 名   : main.c
////  版 本 号   : v2.0
////  作    者   : Evk123
////  生成日期   : 2014-0101
////  最近修改   : 
////  功能描述   : 0.69寸OLED 接口演示例程(STM32F103ZE系列IIC)
////              说明: 
////              ----------------------------------------------------------------
////              GND   电源地
////              VCC   接5V或3.3v电源
////              SCL   接PD6（SCL）
////              SDA   接PD7（SDA）            
////              ----------------------------------------------------------------
////Copyright(C) 中景园电子2014/3/16
////All rights reserved
////////////////////////////////////////////////////////////////////////////////////?

#include "oled.h"
#include "oledfont.h"  	 
#include "i2c.h"
#include "main.h"

#include <stdio.h>
#include <stdint.h>

char OLED_GRAMbuf[8][128];
char OLED_CMDbuf[8][4];

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	

/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	HAL_I2C_Mem_Write(&hi2c3 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&IIC_Command,1,0xff);
}

/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	HAL_I2C_Mem_Write(&hi2c3 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&IIC_Data,1,0xff);
}

void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd){
		Write_IIC_Data(dat); 
	}
	else {
   		Write_IIC_Command(dat);	
	}
}

//写指令 向cmdAdd地址写cmdDat
void OLED_Write_CMD(uint8_t cmdAdd, uint8_t cmdDat, uint8_t datLen)
{
	HAL_I2C_Mem_Write(&hi2c3 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmdAdd,1,0xff);

	for(int i = 0; i < datLen; i++){
		HAL_I2C_Mem_Write(	&hi2c3 , 
							0x78, 
							0x40,
						  	I2C_MEMADD_SIZE_8BIT,
							cmdDat + i,
							1,
						  	0xff);
	}
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  {  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>Max_Column-1){
		x=0;y=y+2;
	}
	if(Char_Size ==16){
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else {	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
			OLED_WR_Byte(F6x8[c][i],OLED_DATA);
	}
}

//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0'){	
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120){
			x=0;y+=2;
		}
		j++;
	}
}


//初始化SSD1306					    
void OLED_Init(void)
{ 	
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  设置起始行地址
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	
	OLED_WR_Byte(0x81,OLED_CMD); // contract control 	设置分辨率
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 集段重映射
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	//OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	OLED_Write_CMD(0xAF, NULL, 0);
	printf("OLED init over\r\n");
}  
