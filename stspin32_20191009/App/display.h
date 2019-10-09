#ifndef __CTRL_H
#define __CTRL_H

#include "oled.h"


typedef struct {
	uint8_t x_base;
	uint8_t y_base;
	uint8_t x_lenth;
	uint8_t y_lenth;
	uint8_t polarity;	/* 显示正负片 */
}image_disp_params;


void dis_refresh_battary(uint8_t bat_level);
void dis_refresh_speed(uint8_t speed_in);
void dis_refresh_error(uint8_t error);
void dis_refresh_roll_direction(uint8_t dir);

#endif


