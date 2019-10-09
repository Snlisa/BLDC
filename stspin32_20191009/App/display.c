#include "display.h"
#include "image.h"
#include "oled.h"


/* 定义图片显示坐标以及长度 */
/* 电池 */
image_disp_params bat = {100, 0, 29, 2, 1};
image_disp_params speed_1 = {0, 0, 30, 8, 1};
image_disp_params speed_2 = {30, 0, 30, 8, 1};
image_disp_params speed_3 = {60, 0, 30, 8, 1};
image_disp_params diretion = {92, 2, 35, 6, 1};

/* 更新电池图标 */
void dis_refresh_battary(uint8_t bat_level)
{
	const uint8_t * image_bat_ptr = image_bat_array[bat_level];
	OLED_DrawBMP(bat.x_base, bat.y_base, bat.x_base+bat.x_lenth, bat.y_base+bat.y_lenth, (uint8_t *)image_bat_ptr);
}


/* 更新转速显示 */
void dis_refresh_speed(uint8_t speed_in)
{
	uint8_t unit = speed_in%10;	/* 取个位 */
	uint8_t ten = speed_in/10;	/* 取十位 */
	
	const uint8_t * posi_1_ptr = image_num_array[ten];
	const uint8_t * posi_2_ptr = image_num_array[unit];
	const uint8_t * posi_3_ptr = image_num_array[11];

	
	OLED_DrawBMP(speed_1.x_base, speed_1.y_base, speed_1.x_base+speed_1.x_lenth, speed_1.y_base+speed_1.y_lenth, (uint8_t *)posi_1_ptr);
	OLED_DrawBMP(speed_2.x_base, speed_2.y_base, speed_2.x_base+speed_2.x_lenth, speed_2.y_base+speed_2.y_lenth, (uint8_t *)posi_2_ptr);
	OLED_DrawBMP(speed_3.x_base, speed_3.y_base, speed_3.x_base+speed_3.x_lenth, speed_3.y_base+speed_3.y_lenth, (uint8_t *)posi_3_ptr);
}


/* 更新故障显示 */
void dis_refresh_error(uint8_t error)
{
	uint8_t unit = error%10;/* 取个位 */
	uint8_t ten = error/10;	/* 取十位 */
	
	const uint8_t * posi_1_ptr = image_num_array[ten];
	const uint8_t * posi_2_ptr = image_num_array[unit];
	const uint8_t * posi_0_ptr = image_num_array[10];

	
	OLED_DrawBMP(speed_1.x_base, speed_1.y_base, speed_1.x_base+speed_1.x_lenth, speed_1.y_base+speed_1.y_lenth, (uint8_t *)posi_0_ptr);
	OLED_DrawBMP(speed_2.x_base, speed_2.y_base, speed_2.x_base+speed_2.x_lenth, speed_2.y_base+speed_2.y_lenth, (uint8_t *)posi_1_ptr);
	OLED_DrawBMP(speed_3.x_base, speed_3.y_base, speed_3.x_base+speed_3.x_lenth, speed_3.y_base+speed_3.y_lenth, (uint8_t *)posi_2_ptr);	

}


/* 更新转向显示 */
void dis_refresh_roll_direction(uint8_t dir)
{

	const uint8_t * dir_image_ptr = image_dir_array[dir];
	if((dir != 0)&&(dir != 1))
		return;
	OLED_DrawBMP(diretion.x_base, diretion.y_base, diretion.x_base+diretion.x_lenth, diretion.y_base+diretion.y_lenth, (uint8_t *)dir_image_ptr);
}



