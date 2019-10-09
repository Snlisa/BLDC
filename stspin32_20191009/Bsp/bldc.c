#include "bldc.h"
#include "ctrl.h"

/* 中断入口函数，及换向程序 */
/*	
		   0	1	 2	  3	   4	5
	正转： UW - VW - VU - WU - WV - UV
		  W↓   V↑	U↓   W↑	  V↓   U↑
	反转： WV - WU - VU - VW - UW - UV
		  W↑   U↓	V↑	 W↓	  U↑   V↓   		
*/


static uint8_t bemf_commutate_enable = 0;
static uint8_t bemf_int_corrent_counter = 0;	//外部中断正确计数

uint8_t get_current_dir(void);
uint8_t get_current_phase(void);
void set_current_phase(uint8_t phase);
uint16_t calc_recent_speed(uint16_t time_counter);
void clean_force_comm_counter(void);	/* 清强制换向标志 */

/* W相 */
void W_INTERRUPT(uint8_t int_type);
void V_INTERRUPT(uint8_t int_type);
void U_INTERRUPT(uint8_t int_type);

/* 运行中断换相位 */
void enable_bemf_commutate(void)
{
	bemf_commutate_enable = ENABLE;
}

/* 禁止中断换向 */
void disable_bemf_commutate(void)
{
	bemf_commutate_enable = DISABLE;
}

/* 检查U相电平 */
static uint8_t get_bemf_U_stat(void)
{
	if(HAL_GPIO_ReadPin(U_BEMF_GPIO_Port, U_BEMF_Pin) == 0)
		return 0;
	else return 1;
}

/* 检查V相电平 */
static uint8_t get_bemf_V_stat(void)
{
	if(HAL_GPIO_ReadPin(V_BEMF_GPIO_Port, V_BEMF_Pin) == 0)
		return 0;
	else return 1;
}

/* 检查W相电平 */
static uint8_t get_bemf_W_stat(void)
{
	if(HAL_GPIO_ReadPin(W_BEMF_GPIO_Port, W_BEMF_Pin) == 0)
		return 0;
	else return 1;
}

/* 获取换向计数值 */
uint16_t get_commutation_counter(void)
{
	uint16_t counter;
	
	return counter;
}



/* U相 */
static void _U_commutation(void)
{
	uint8_t sense = get_bemf_U_stat();	/* 读取电平状态 */
	uint8_t dir = get_current_dir();	/* 读取当前转向 */
	uint8_t phase = get_current_phase();/* 获取当前相位 */
	uint16_t commutate_counter = 0;
	
	/* 检测进入中断是否正确 */
	//if()

	
	/* 判断电平正负 */
	if(bemf_commutate_enable == ENABLE)
	{
		if(dir == ROLL_DIR_F)
		{
			do{
				if(get_bemf_U_stat()) 
					sense = 1;
				else sense = 0;	
				
				switch(phase)
				{
					case PHASE_TWO:	//VU
					{			
						HIGH_V_ON;	//维持V相导通
						if(!sense)
						{
							LOW_U_ON;
							W_INTERRUPT(1);
							set_current_phase(PHASE_THREE);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();	//清除堵转计数
						}
						else 
						{
							LOW_W_ON;
						}
					}break;
					
					case PHASE_FIVE: //UV
					{
						LOW_V_ON;
						if(sense)
						{
							HIGH_U_ON;
							W_INTERRUPT(0);
							set_current_phase(PHASE_ZERO);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();	//清除堵转计数						
						}
						else
						{
							HIGH_W_ON;
						}
					}
				}
			}while((get_bemf_U_stat() && !sense) || (!get_bemf_U_stat() && sense));	
		}
		else if(dir == ROLL_DIR_R)
		{
			do{
				if(get_bemf_U_stat()) 
					sense = 1;
				else sense = 0;	

				switch(phase)
				{
					case PHASE_ONE:	//WU
					{		
						HIGH_W_ON;
						if(sense)
						{
							LOW_U_ON;
							V_INTERRUPT(1);
							set_current_phase(PHASE_TWO);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();	//清除堵转计数												
						}
						else 
						{
							LOW_V_ON;
						}
					}break;
					
					case PHASE_FOUR:	//UW
					{
						LOW_W_ON;
						if(!sense)
						{
							HIGH_U_ON;
							V_INTERRUPT(0);
							set_current_phase(PHASE_FIVE);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();	//清除堵转计数						
						}
						else
						{
							HIGH_V_ON;
						}
					}
				}					
			}while((get_bemf_U_stat() && !sense) || (!get_bemf_U_stat() && sense));					
		}
	}
}




/* B相 */
static void _V_commutation(void)
{
	uint8_t sense = get_bemf_V_stat();	/* 读取电平状态 */
	uint8_t dir = get_current_dir();	/* 读取当前转向 */
	uint8_t phase = get_current_phase();/* 获取当前相位 */
	uint16_t commutate_counter = 0;
	
	/* 判断电平正负 */
	if(bemf_commutate_enable == ENABLE)
	{
		if(dir == ROLL_DIR_F)
		{
			do{
				if(get_bemf_V_stat()) 
					sense = 1;
				else sense = 0;	
				
				switch(phase)
				{
					case PHASE_ONE:	//VW
					{		
						LOW_W_ON;
						if(sense)	// V↑
						{
							HIGH_V_ON;
							U_INTERRUPT(0);
							set_current_phase(PHASE_TWO);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数							
						}
						else 
						{
							HIGH_U_ON;
						}
					}break;
					
					case PHASE_FOUR: //WV
					{
						HIGH_W_ON;
						if(!sense)	//V↓
						{
							LOW_V_ON;
							U_INTERRUPT(1);
							set_current_phase(PHASE_FIVE);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数						
						}
						else
						{
							LOW_U_ON;
						}
					}
				}
			}while((get_bemf_V_stat() && !sense) || (!get_bemf_V_stat() && sense));	
		}
		else if(dir == ROLL_DIR_R)
		{
			do{
				if(get_bemf_V_stat()) 
					sense = 1;
				else sense = 0;	

				switch(phase)
				{
					case PHASE_TWO:	//VU
					{		
						LOW_U_ON;
						if(sense)	//V↑
						{
							HIGH_V_ON;
							W_INTERRUPT(0);
							set_current_phase(PHASE_THREE);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数	
						}
						else 
						{
							HIGH_W_ON;
						}
					}break;
					
					case PHASE_FIVE:	//UV
					{
						HIGH_U_ON;
						if(!sense)		//V↓
						{
							LOW_V_ON;
							W_INTERRUPT(1);
							set_current_phase(PHASE_ZERO);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数							
						}
						else
						{
							LOW_W_ON;
						}
					}
				}					
			}while((get_bemf_V_stat() && !sense) || (!get_bemf_V_stat() && sense));					
		}
	}	
}




/* C相 */
static void _W_commutation(void)
{
	uint8_t sense = get_bemf_W_stat();	/* 读取电平状态 */
	uint8_t dir = get_current_dir();	/* 读取当前转向 */
	uint8_t phase = get_current_phase();/* 获取当前相位 */
	uint16_t commutate_counter = 0;
	/* 判断电平正负 */
	if(bemf_commutate_enable == ENABLE)
	{
		if(dir == ROLL_DIR_F)
		{
			do{
				if(get_bemf_W_stat()) 
					sense = 1;
				else sense = 0;	
				
				switch(phase)
				{
					case PHASE_ZERO:	//UW
					{		
						HIGH_U_ON;
						if(!sense)	//W↓
						{
							LOW_W_ON;
							V_INTERRUPT(1);
							set_current_phase(PHASE_ONE);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数								
						}
						else 
						{
							LOW_V_ON;
						}
					}break;
					
					case PHASE_THREE: //WU
					{
						LOW_U_ON;
						if(sense)	//W↑
						{
							HIGH_W_ON;
							V_INTERRUPT(0);
							set_current_phase(PHASE_FOUR);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数							
						}
						else
						{
							HIGH_V_ON;
						}
					}
				}
			}while((get_bemf_W_stat() && !sense) || (!get_bemf_W_stat() && sense));	
		}
		else if(dir == ROLL_DIR_R)
		{
			do{
				if(get_bemf_W_stat()) 
					sense = 1;
				else sense = 0;	

				switch(phase)
				{
					case PHASE_ZERO:	//WV
					{	
						LOW_V_ON;
						if(sense)	//W↑
						{
							HIGH_W_ON;
							U_INTERRUPT(0);
							set_current_phase(PHASE_ONE);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数	
						}
						else 
						{
							HIGH_U_ON;
						}
					}break;
					
					case PHASE_THREE:	//VW
					{
						HIGH_V_ON;
						if(!sense)	//W↓
						{
							LOW_W_ON;
							U_INTERRUPT(1);
							set_current_phase(PHASE_FOUR);	//更改相序
							commutate_counter = get_commutation_counter();//获取计数值
							clean_force_comm_counter();		//清除堵转计数							
						}
						else
						{
							LOW_U_ON;
						}
					}
				}					
			}while((get_bemf_W_stat() && !sense) || (!get_bemf_W_stat() && sense));					
		}
	}
}



/* 中断入口函数，及换向程序 */
typedef void(*p_func_commutation)(void);

p_func_commutation p_irq_array[5] = {
	0,
	_U_commutation,	//U相中断
	_V_commutation,	//V相中断
	0,
	_W_commutation, //W相中断
};

/* 中断函数入口 */
void do_exti_handler(uint16_t gpio_pin)
{
	uint8_t irq_src = (uint8_t)(gpio_pin);
	p_irq_array[irq_src]();		/* 执行相应中断程序 */
}




/* 设置电压 */
void set_pwm_duty(uint16_t duty)
{
	/* 设定占空比 */
	if(duty>1180) 
		duty = 1180;
	TIM1->CCR1 = duty; 
	TIM1->CCR2 = duty; 
	TIM1->CCR3 = duty; 
}

/* 启动成功检测 */
uint8_t isStartSuccess(void)
{
	if(bemf_int_corrent_counter > 5)
		return 1;		//正确换向次数 大于5次
	else 
		return 0;
}


/* 获取目前电机PWM占空比 */
uint16_t get_lastet_pwm_duty(void)
{
	uint16_t duty_ret = TIM1->CCR1;
	return duty_ret;
}



/* W相 */
void W_INTERRUPT(uint8_t int_type)    //PA2
{
	if(int_type == 1)	/* 上升沿 */
	{
		EXTI->FTSR &= ~(0X0001 << 2);
		EXTI->RTSR |=  (0X0001 << 2);
	}
	else if(int_type == 0)
	{
		EXTI->RTSR &= ~(0X0001 << 2);
		EXTI->FTSR |=  (0X0001 << 2);
	}
}


/* U相 */
void V_INTERRUPT(uint8_t int_type)
{
	if(int_type == 1)	/* 上升沿 */
	{
		EXTI->FTSR &= ~(0X0001 << 1);
		EXTI->RTSR |=  (0X0001 << 1);
	}
	else if(int_type == 0)	/* 下降沿 */
	{
		EXTI->RTSR &= ~(0X0001 << 1);
		EXTI->FTSR |=  (0X0001 << 1);
	}
}


/* U相 */
void U_INTERRUPT(uint8_t int_type)   //PA0
{
	if(int_type == 1)	/* 上升沿 */
	{
		EXTI->FTSR &= ~(0X0001 << 0);
		EXTI->RTSR |=  (0X0001 << 0);
	}
	else if(int_type == 0)	/* 下降沿 */
	{
		EXTI->RTSR &= ~(0X0001 << 0);
		EXTI->FTSR |=  (0X0001 << 0);
	}
}

/* 失能所有中断 */
void EXTI_DISABLE(void)
{
	EXTI->IMR &= ~(1 << 2);
	EXTI->IMR &= ~(1 << 1);
	EXTI->IMR &= ~(1 << 0);
}


/* 使能所有中断 */
void EXTI_ENABLE(void)
{
	EXTI->IMR |= (1 << 2);
	EXTI->IMR |= (1 << 1);
	EXTI->IMR |= (1 << 0);
}

/* 转动函数 */
/* 外同步转动/强制切换 */
void Motor_SyncRun(uint8_t DIR, uint8_t PHASE)
{
	if(DIR == MOTOR_DIR_FWD)	/* 正装 */
	{
		switch(PHASE)
		{
		case PHASE_ZERO:
		{
			HIGH_U_ON;
			LOW_W_ON;
			V_INTERRUPT(1);
			PHASE = PHASE_ONE;
		}
		break;

		case PHASE_ONE:
		{
			HIGH_V_ON;
			LOW_W_ON;
			U_INTERRUPT(0);
			PHASE = PHASE_TWO;
		}
		break;

		case PHASE_TWO:
		{
			HIGH_V_ON;
			LOW_U_ON;
			W_INTERRUPT(1);
			PHASE = PHASE_THREE;
		}
		break;

		case PHASE_THREE:
		{
			HIGH_W_ON;
			LOW_U_ON;
			V_INTERRUPT(0);
			PHASE = PHASE_FOUR;
		}
		break;

		case PHASE_FOUR:
		{
			HIGH_W_ON;
			LOW_V_ON;
			U_INTERRUPT(1);
			PHASE = PHASE_FIVE;
		}
		break;

		case PHASE_FIVE:
		{
			HIGH_U_ON;
			LOW_V_ON;
			W_INTERRUPT(0);
			PHASE = PHASE_ZERO;
		}
		break;
		}
	}
	else if(DIR == MOTOR_DIR_REV)	/* 反转 */
	{
		switch(PHASE)
		{
		case PHASE_ZERO:
		{
			HIGH_W_ON;
			LOW_V_ON;
			U_INTERRUPT(0);
			PHASE = PHASE_ONE;
		}
		break;

		case PHASE_ONE:
		{
			HIGH_W_ON;
			LOW_U_ON;
			V_INTERRUPT(1);
			PHASE = PHASE_TWO;
		}
		break;

		case PHASE_TWO:
		{
			HIGH_V_ON;
			LOW_U_ON;
			W_INTERRUPT(0);
			PHASE = PHASE_THREE;
		}
		break;

		case PHASE_THREE:
		{
			HIGH_V_ON;
			LOW_W_ON;
			U_INTERRUPT(1);
			PHASE = PHASE_FOUR;
		}
		break;

		case PHASE_FOUR:
		{
			HIGH_U_ON;
			LOW_W_ON;
			V_INTERRUPT(0);
			PHASE = PHASE_FIVE;
		}
		break;

		case PHASE_FIVE:
		{
			HIGH_U_ON;
			LOW_V_ON;
			W_INTERRUPT(1);
			PHASE = PHASE_ZERO;
		}
		break;
		}
	}
}



/* DO NOT CHANGE--------------------------- */
/* system callback function here */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	do_exti_handler(GPIO_Pin);	/* goto irq handler */
}




