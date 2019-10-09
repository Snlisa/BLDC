#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define ON 1
#define OFF 0


/* 读开关状态 */
#define PWR_SWITCH_LEVEL HAL_GPIO_ReadPin(PWR_SEN_GPIO_Port, PWR_SEN_Pin)

/* 读正反转状态 */
#define DIR_SWITCH_LEVEL HAL_GPIO_ReadPin(DIR_GPIO_Port, DIR_Pin)

/* 设置电源开关 */
#define BUS_PWR_ON HAL_GPIO_WritePin(PWR_CTL_GPIO_Port, PWR_CTL_Pin, GPIO_PIN_SET);
#define BUS_PWR_OFF HAL_GPIO_WritePin(PWR_CTL_GPIO_Port, PWR_CTL_Pin, GPIO_PIN_RESET);



#endif

