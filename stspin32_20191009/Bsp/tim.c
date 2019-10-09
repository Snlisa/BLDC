#include "tim.h"
#include "bsp.h"
#include "ctrl.h"

void speed_controller(void);
void force_commutate(void);

static uint8_t tim_5ms_counter = 0;
static uint8_t tim_10ms_counter = 0;
static uint8_t tim_1s_counter = 0;


static uint8_t _10ms_flag = 0;
static uint8_t _1s_flag = 0;


/* timer irq callback */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim3.Instance)
	{
		/* add code here */
		if(++tim_10ms_counter == 10)
		{
			tim_10ms_counter = 0;
			_10ms_flag = 1;
			
			tim_1s_counter++;
			if(tim_1s_counter == 100)
			{
				tim_1s_counter = 0;
				_1s_flag = 1;
			}	
		}
		if(++tim_5ms_counter == 5)
		{
			tim_5ms_counter = 0;
			speed_controller();
		}
	
		force_commutate();	/* 强制换向 */
	}
}


uint8_t isCountTo10ms(void)
{
	uint8_t ret = _10ms_flag;
	_10ms_flag = 0;
	return ret;
}


uint8_t isCountTo1s(void)
{
	uint8_t ret = _1s_flag;
	_1s_flag = 0;
	return ret;
}



