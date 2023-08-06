#ifndef __OLED_H
#define __OLED_H

#include "main.h"

//OLED ����
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	  
				   
#define I2C_OLED    hi2c3

#define OLED_CMD    0	//д����
#define OLED_DATA   1	//д����

//OLED�����ú���
void OLED_Clear(void);
void OLED_Init(void);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size);
/*��ʱ*/
#define delay_ms(delay) HAL_Delay(delay)
#endif  
	 



