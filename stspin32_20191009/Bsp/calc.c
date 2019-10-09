#include "calc.h"



/* 涵盖基础算法 */

float pid_controller(pid_parameters * ptr)
{
    ptr->err_new = ptr->target - ptr->feedback;
    ptr->outp = ptr->p * ptr->err_new;

    if(ptr->output_old > ptr->output_limit)  	/* 累计负积分 */
    {
        if(ptr->err_new <= 0)
        {
            ptr->err_sum += ptr->err_new;
        }
    }
    else if(ptr->output_old < 0)  	/* 累计正积分 */
    {
        if(ptr->err_new >= 0)
        {
            ptr->err_sum += ptr->err_new;
        }
    }
    else     //正负误差都累积
    {
        ptr->err_sum += ptr->err_new;
    }

    if(ptr->err_sum > ptr->err_sum_max)
    {
        ptr->err_sum = ptr->err_sum_max;
    }
    else if(ptr->err_sum <= ptr->err_sum_min)
    {
        ptr->err_sum = ptr->err_sum_min;
    }

    ptr->outi = ptr->i * ptr->err_sum;	/* 计算i项 */
	ptr->err_old = ptr->err_new;
	ptr->output_new = ptr->outp + ptr->outi;
	
    ptr->output_old = ptr->output_new;

    if(ptr->output_new < ptr->output_min)
    {
        ptr->output_new = ptr->output_min;
    }
    else if(ptr->output_new > ptr->output_max)
    {
        ptr->output_new = ptr->output_max;
    }

    return ptr->output_new;
}

uint16_t calc_recent_speed(uint16_t time_counter)
{
	uint16_t recent_speed;
	
	return recent_speed;
}


