#include "key.h"

static uint16_t speed_adjust_val = 0;


/* 获取当前电源开关状态 */
uint8_t get_pwr_switch_stat(void)
{
	return PWR_SWITCH_LEVEL;
}


/* 获取速度获取AD值 */
uint16_t get_speed_adjust_val(void)
{
	return speed_adjust_val;
}


/* 设置电源开关 */
void set_pwr_stat(uint8_t stat)
{
	if(stat == ON) 
	{
		BUS_PWR_ON;
	}
	else 
	{
		BUS_PWR_OFF;
	}
}




