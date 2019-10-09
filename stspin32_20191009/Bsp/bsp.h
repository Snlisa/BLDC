#ifndef __BSP_H
#define __BSP_H

#include "main.h"


void bsp_init(void);

extern ADC_HandleTypeDef hadc;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;


#endif


