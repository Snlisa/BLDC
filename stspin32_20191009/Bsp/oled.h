#ifndef __OLED_H
#define __OLED_H


#include "main.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define Max_Column	128
#define Max_Row		64

extern const unsigned char image_bat_0[];
extern const unsigned char image_bat_1[];
extern const unsigned char image_bat_2[];
extern const unsigned char image_bat_3[];
extern const unsigned char image_bat_4[];
extern const unsigned char image_bat_5[];
extern const unsigned char image_bat_charge[];

extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];

void OLED_Init(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_On(void);
void OLED_Clear(void);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);



#endif


