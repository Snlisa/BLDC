#ifndef __CTRL_H
#define __CTRL_H

#include "bldc.h"
#include "calc.h"


/* 转动方向 */
#define ROLL_DIR_F	0
#define ROLL_DIR_R	1

/* 最大速度 */
#define MAX_SPEED 35000
#define MIN_SPEED 2000

/* 电位器值 */
#define MAX_BAT 7
#define MIN_BAT 0

#define BUS_VOLT_MIN 100	/* 母线电压 */
#define BUS_VOLT_MAX 600
#define BUS_VOLT_ERR BUS_VOLT_MAX-BUS_VOLT_MIN
#define BAT_STEP (uint8_t)((BUS_VOLT_MAX - BUS_VOLT_MIN)/(MAX_BAT-MIN_BAT))


#define SPEED_VAL_MIN 50	/* 电位器电压 */
#define SPEED_VAL_MAX 990
#define SPEED_VAL_ERR SPEED_VAL_MAX-SPEED_VAL_MIN
#define SPEED_STEP (uint8_t)((SPEED_VAL_MAX - SPEED_VAL_MIN)/((MAX_SPEED-MIN_SPEED)/1000))


/* 定义故障代码 */
#define ERR_STALL 10
#define ERR_DISCONNECT 11


/* 显示刷新位 */
struct screen_area_refresh{
	uint8_t battary;
	uint8_t speed;
	uint8_t diretion;
	uint8_t full;	/* 全屏刷新 */
};

#endif

