#ifndef __BLDC_H
#define __BLDC_H

#include "main.h"

extern TIM_HandleTypeDef htim1;

typedef struct{
	uint16_t userspeed;
	uint16_t actualspeed;
	uint16_t act_current;
	uint16_t avr_current;
	uint8_t dir;
	uint8_t error;		/* 错误 */
	uint8_t pid_loop;	/* 闭环标志 */
	uint8_t phase;
	uint8_t function;
}motor_func_params;

#define PHASE_ZERO 	0
#define PHASE_ONE  	1
#define PHASE_TWO  	2
#define PHASE_THREE 3
#define PHASE_FOUR  4
#define PHASE_FIVE  5

#define MOTOR_DIR_FWD 0
#define MOTOR_DIR_REV 1

#define HIGH_U_OFF HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1)
#define HIGH_V_OFF HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2)
#define HIGH_W_OFF HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3)

#define HIGH_U_ON {HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);}
#define HIGH_V_ON {HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);}
#define HIGH_W_ON {HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);}

#define LOW_U_OFF HAL_GPIO_WritePin(U_LOW_GPIO_Port, U_LOW_Pin, GPIO_PIN_RESET)
#define LOW_V_OFF HAL_GPIO_WritePin(V_LOW_GPIO_Port, V_LOW_Pin, GPIO_PIN_RESET)
#define LOW_W_OFF HAL_GPIO_WritePin(W_LOW_GPIO_Port, W_LOW_Pin, GPIO_PIN_RESET)

#define LOW_U_ON {HAL_GPIO_WritePin(U_LOW_GPIO_Port, U_LOW_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(W_LOW_GPIO_Port, V_LOW_Pin|W_LOW_Pin, GPIO_PIN_RESET);}
#define LOW_V_ON {HAL_GPIO_WritePin(V_LOW_GPIO_Port, V_LOW_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(W_LOW_GPIO_Port, U_LOW_Pin|W_LOW_Pin, GPIO_PIN_RESET);}
#define LOW_W_ON {HAL_GPIO_WritePin(W_LOW_GPIO_Port, W_LOW_Pin, GPIO_PIN_SET);HAL_GPIO_WritePin(W_LOW_GPIO_Port, U_LOW_Pin|V_LOW_Pin, GPIO_PIN_RESET);}


#endif


