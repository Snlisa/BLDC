#ifndef __CALC_H
#define __CALC_H

#include "main.h"

typedef void(*_params_init)(void);
typedef void(*_pid_handler)(void);

typedef struct
{
    float target;
    float feedback;

    float p;
    float i;
    float d;

    float outp;
    float outi;
    float outd;

    float err_sum;
    float err_new;
    float err_old;
    float err_sum_max;
    float err_sum_min;

    float output_new;
    float output_old;
    float output_max;
    float output_min;

    float output_limit;		/* 输出最大值 */
    uint16_t period;		/* 运行周期 */
	
	_pid_handler pid_handler;	/* 闭环函数 */
	_params_init params_init;	/* 参数初始化 */

}pid_parameters;


float pid_controller(pid_parameters * ptr);

#endif




