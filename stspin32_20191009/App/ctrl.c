#include "ctrl.h"
#include "display.h"

/* 电池电量 */
enum {bat_chr = 0, bat_0, bat_1, bat_2, bat_3, bat_4, bat_5, bat_6};

/* 错误值 */
enum {err_null = 0, err_disconnect = 11, err_stall = 12};


/* main.c*/
extern ADC_HandleTypeDef hadc;

/* adc参数 */
static uint16_t adc_array[3] = {0};


/* from display.c*/
void dis_refresh_battary(uint8_t bat_level);
void dis_refresh_speed(uint8_t speed_in);
void dis_refresh_error(uint8_t error);
void dis_refresh_roll_direction(uint8_t dir);

/* from bldc.c */
void set_pwm_duty(uint16_t duty);
uint8_t isStartSuccess(void);
void enable_bemf_commutate(void);
void disable_bemf_commutate(void);

/* from local */
void start_to_get_adc(void);
uint8_t isPowerSwitchOFF(void);
uint8_t calc_given_speed(uint16_t);
uint8_t calc_bat_quantity(uint16_t);
uint16_t calc_bus_current(uint16_t);
uint8_t get_dir_set(void);
uint16_t get_lastet_pwm_duty(void);
void speed_controller_params_init(void);
void power_off(void);
uint8_t isForceCommutateErr(void);

	
typedef void (*do_case)(void);
static enum {idle = 0, start, work, stop, reversal, err}motor_func_case;
static motor_func_params motor_params;	/* 运行参数 */
static pid_parameters motor_pid;	/* 闭环参数 */
static uint8_t err_num = err_null;


/* 变量 */
static struct screen_area_refresh screen_refresh;	/* 刷新显示的数据 */
static uint8_t battery_percentage = 0;	/* 电量百分比 */


/* 判断过流 */
static uint8_t isOverCurrent(void)
{
	#define current_max 1000
	if(motor_params.avr_current > current_max)
		return 1;
	else return 0;
}

/* 判断解除过流 */
static uint8_t isReleaseCurrent(void)
{
	#define current_release 800
	if(motor_params.avr_current < current_release)
		return 1;
	else return 0;
}

/* 判断电流过小 */
static uint8_t isDisconnectCurrent(void)
{
	#define current_disconnect 50
	if(motor_params.avr_current < current_disconnect)
		return 1;
	else return 0;
}


/* 清除故障 */
static void clear_err(void)
{
	err_num = err_null;
}

/* 清除故障 */
static uint8_t get_err(void)
{
	return err_num;
}


/* 控制逻辑 */
/* 空闲状态 */
static void case_idle(void)
{
	/* 检测电位器 */
	uint8_t power_off_flag = isPowerSwitchOFF();	//检测关机
	uint8_t mtr_userspeed = motor_params.userspeed;	/* 设定速度 */
	uint8_t mtr_dir = get_dir_set();	/* 获取转向设定 */
	
	motor_params.dir = mtr_dir;		/* 当前转向设定 */
	
	if(mtr_userspeed > 0)			/* 设定转速 > 0 */
	{
		motor_func_case = start;	/* 跳转到启动态 */
		motor_params.function = ENABLE;
	}
	else
	{
		motor_params.function = DISABLE;	/* 停止转动 */
	}
	
	/* 判断断电 */
	if(power_off_flag == 1) /* 电位器断开 */
	{
		power_off();	/* 关断电源 */
	}
}



/* 启动状态 */
static void case_start(void)
{
	static uint16_t start_up_pwm = 0;
	static uint8_t restart_times = 0;
	
	uint8_t power_off_flag = isPowerSwitchOFF();	//检测关机	

	/* 升压 */
	start_up_pwm++;
	set_pwm_duty(start_up_pwm);
	if(start_up_pwm > 100)
	{
		if(isDisconnectCurrent())	/* 判断电流过小 */	
		{	
			err_num = err_disconnect;	/* 显示未连接: 禁止再次启动 */
			motor_func_case = err;		/* 跳转到错误态 */
			start_up_pwm = 0;
			restart_times = 0;
		}
		else if(isOverCurrent())	/* 判断电流过大 */
		{
			motor_func_case = err;		/* 跳转到错误态 */
			err_num = err_stall;		/* 显示堵转 */		
			start_up_pwm = 0;
			restart_times = 0;
		}
		else if(!isStartSuccess())	/* 启动未成功 */
		{
			start_up_pwm = 0;	/* 重新启动 */	
			restart_times ++;
			if(restart_times > 3)
			{
				motor_func_case = err;		/* 跳转到错误态 */
				err_num = err_stall;		/* 显示堵转 */
				restart_times = 0;
			}		
		}
	}
	
	if(isStartSuccess())	//启动成功：通过正确进入外部中断
	{
		motor_func_case = work;			/* 跳转到错误态 */
		motor_params.pid_loop = ENABLE;	/* 切入闭环 */
		speed_controller_params_init(); /* 初始化PID参数 */
		start_up_pwm = 0;
		restart_times = 0;
	}
	
	if(power_off_flag == ENABLE)
	{
		motor_func_case = stop;			/* 跳转到错误态 */
		motor_params.pid_loop = DISABLE;	/* 切入闭环 */
	}
}


/* 运行状态 */
static void case_work(void)
{
	uint8_t power_off_flag = isPowerSwitchOFF();	//检测关机
	uint8_t mtr_dir = get_dir_set();				//检测转向
	uint8_t mtr_userspeed = motor_params.userspeed; //当前速度
	
	static uint8_t overcurrent_counter = 0;	/* 过流计算值 */
	
	/* 判断电源按钮，进行关机 */
	if(power_off_flag == 1)	
	{
		/* 进行关机 */
		motor_params.pid_loop = DISABLE;	/* 切出闭环 */	
		
		/* 切换为stop状态 */
		motor_func_case = stop;
	}
	
	/* 判断正反切换按键，切换正反转 */
	if(mtr_dir != motor_params.dir)
	{
		motor_params.pid_loop = DISABLE;	/* 切出闭环 */
		
		motor_func_case = stop;		/* 跳转到停止态 */
	}
	
	if(mtr_userspeed == 0)	//设定转速为0
	{
		/* 停车 */
		motor_params.pid_loop = DISABLE;	/* 切出闭环 */	
		
		/* 切换为stop状态 */
		motor_func_case = stop;	
	}
	
	if(isForceCommutateErr())	//强制换向进入次数太多
	{
		overcurrent_counter = 0;
		motor_func_case = err;	/* 跳转到错误态 */
		err_num = err_stall;		
	}
	

	/* 读取电流值，进行过载判断 */
	if(isOverCurrent())
	{
		if(++overcurrent_counter == 100)	/* 过载保护 */
		{
			overcurrent_counter = 0;
			motor_func_case = err;	/* 跳转到错误态 */
			err_num = err_stall;
		}
	}
	else
	{
		if(isReleaseCurrent() == 1)	/* 释放电流 */
		{
			overcurrent_counter = 0;
		}
		else
		{
			overcurrent_counter = 0;
		}
	}
}


/* 停止状态 */
static void case_stop(void)
{
	static uint16_t current_pwm = 0;
	static uint8_t stop_init_flag = DISABLE;
	
	if(!stop_init_flag)	//第一次进入获取当前PWM值	
	{
		current_pwm = get_lastet_pwm_duty();
		stop_init_flag = ENABLE;
	}
	else	//开始减速
	{
		if(current_pwm > 10) current_pwm -= 10; //10递减
		else current_pwm = 0;
		set_pwm_duty(current_pwm);
		
		if(current_pwm == 0)
		{
			motor_func_case = idle;	/* 跳转到idle */
				//若是停止，则停止，若是反转，则再次启动
			
			motor_params.function = DISABLE;	/* 停止转动 */
		} 
	}
}



/* 故障状态 */
static void case_err(void)
{
	static uint16_t err_wait_counter = 0;
	
	/* 等待 */
	if(++err_wait_counter == 1000)		/* 等待10s */
	{
		err_wait_counter = 0;
		clear_err();
		motor_func_case = idle;	/* 跳转到idle */
	}
}


static do_case case_array[] = {
	case_idle,
	case_start,
	case_work,
	case_stop,
	case_err,
};


/* 电机启动切换状态机切换 */
void do_motor_fucn_case(void)
{
	case_array[motor_func_case]();
}

/* pid参数初始化 */
static void speed_controller_params_init(void)
{
	motor_pid.p = 0.01;
	motor_pid.i = 0.0;
	
	motor_pid.outp = 0;
	motor_pid.outi = 0;
	motor_pid.outd = 0;
	
	motor_pid.err_sum_max = 1000000;	/* 积分饱和值 */
	motor_pid.err_sum_min = 0;			/* 积分最小值 */
	
	motor_pid.output_new = 0;
	motor_pid.output_old = 0;
	
	motor_pid.output_limit = 1180;	/* 最大运行的占空比 */
	motor_pid.output_max = 1180;	/* 最大运行的占空比 */
	motor_pid.output_min = 50;		/* 最小占空比 */
	
	motor_pid.feedback = 0;
	motor_pid.err_sum = 0;
	motor_pid.err_new = 0;
	motor_pid.err_old = 0;
	
	motor_pid.target = 2000;	/* 最小启动转速 */
}


/* 闭环控制 */
void speed_controller(void)
{
	uint16_t speed_set = motor_params.userspeed*1000;
	uint16_t pwm = 0;
	
	if(motor_params.pid_loop == ENABLE)
	{
		/* 速度平滑控制 */
		if(speed_set > motor_pid.target)
		{
			motor_pid.target += 100;
			if(motor_pid.target > speed_set) 
				motor_pid.target = speed_set;	
		}
		else if(speed_set < motor_pid.target)
		{
			motor_pid.target -= 100;	
			if(motor_pid.target < speed_set) 
				motor_pid.target = speed_set;			
		}
		
		pwm = (uint16_t)pid_controller(&motor_pid);	/* pid计算pwm */
		
		set_pwm_duty(pwm);	/* 赋值pwm */

	}
	else
	{
		motor_pid.target = 0;
	}
}


/* 显示数据刷新 */
void do_refresh_oled(void)
{
	/* 刷新 */	
	if(screen_refresh.full == ENABLE)	/* 全屏刷新 */
	{
		dis_refresh_battary(battery_percentage);
		dis_refresh_roll_direction(motor_params.dir);
		if(get_err() == err_null)
			dis_refresh_speed(motor_params.userspeed);	/* 显示设定转速 */			
		else if(get_err() == err_stall)
			dis_refresh_error(ERR_STALL);	/* 显示错误代码：堵转 */
		else if(get_err() == err_disconnect)
			dis_refresh_error(ERR_DISCONNECT);	/* 显示错误代码：未连接 */
	}
	if(screen_refresh.battary == ENABLE)	/* 刷新电池区域 */
	{
		dis_refresh_battary(battery_percentage);
	}
	if(screen_refresh.diretion == ENABLE)	/* 刷新方向区域 */
	{
		dis_refresh_roll_direction(motor_params.dir);
	}
	if(screen_refresh.speed == ENABLE)	/* 刷新转速区域 */
	{
		dis_refresh_speed(motor_params.userspeed);
	}
}


/* 输入更新 */
void do_input_sync(void)
{
	/* 电位器刷新 */
	motor_params.userspeed = calc_given_speed(adc_array[0]);	/* 转速赋值 */
	
	/* ADC电流滤波值 */
 	motor_params.avr_current = calc_bus_current(adc_array[2]);	/* 母线电流 */
	
	/* adc电压值获取，计算当前电量 */
	battery_percentage = calc_bat_quantity(adc_array[1]);		/* 电池电压 */

	start_to_get_adc();			/* 获取AD值 */
	
	screen_refresh.full = ENABLE;	/* 刷新全屏 */
	
}



/* 主循环程序 */
void main_thread(void)
{
	do_input_sync();		/* 更新给定 */
	
	do_motor_fucn_case();	/* 电机状态机 */
	
	do_refresh_oled();		/* 显示刷新 */
}




/* 开通电源 */
static void power_on(void)
{
	HAL_GPIO_WritePin(PWR_CTL_GPIO_Port, PWR_CTL_Pin, GPIO_PIN_SET);	/* 1为导通 */
}


/* 关闭电源 */
static void power_off(void)
{
	HAL_GPIO_WritePin(PWR_CTL_GPIO_Port, PWR_CTL_Pin, GPIO_PIN_RESET);	/* 0为关闭 */
}

/* 检测是否关闭开关 */
static uint8_t isPowerSwitchOFF(void)
{
	if(HAL_GPIO_ReadPin(PWR_SEN_GPIO_Port, PWR_SEN_Pin) == GPIO_PIN_RESET)
		return 0;
	else 
		return 1;
}

/* 检测设定转向 */
static uint8_t get_dir_set(void)
{
	if(HAL_GPIO_ReadPin(PWR_SEN_GPIO_Port, PWR_SEN_Pin) == GPIO_PIN_RESET)
		return ROLL_DIR_F;
	else 
		return ROLL_DIR_R;
}

/* 计算给定转速 */
uint8_t calc_given_speed(uint16_t ad_val)
{
	static uint16_t last_val = 0;
	static uint8_t last_duty = 0;
	
	uint8_t duty = 0;
	
	if(ad_val >= SPEED_VAL_MAX) 
	{
		last_duty = MAX_SPEED/1000;
		last_val = SPEED_VAL_MAX;
		return MAX_SPEED/1000;	/* 最大速度 */
	}
	if(ad_val < SPEED_VAL_MIN)
	{
		last_duty = 0;
		last_val = 0;
		return 0;	/* 最小速度 */
	}
	else
	{
		duty = (ad_val-SPEED_VAL_MIN)/SPEED_STEP;	/* 计算当前赋予速度 */
		
		if(duty != last_duty)
		{
			if(duty > last_duty) /* 速度增加 */
			{
				if((ad_val - last_val) > (SPEED_STEP>>1))
				{
					last_duty = duty;
					last_val = ad_val;
				}
			}
			else if(last_duty > duty)
			{
				if((last_val - ad_val) > (SPEED_STEP>>1))
				{
					last_duty = duty;
					last_val = ad_val;
				}				
			}
		}	
	}
	
	if(last_duty >  MAX_SPEED/1000 - 1)
		last_duty = MAX_SPEED/1000 - 1;
	else if(last_duty < MIN_SPEED/1000 - 1)
		last_duty = MIN_SPEED/1000 - 1;
	
	return (last_duty + 1);
}



/* 计算电池电量 */
uint8_t calc_bat_quantity(uint16_t ad_val)
{
	static uint16_t last_val = 0;
	static uint8_t last_battary_quantity = bat_6;	//上一次电量值
	uint8_t battary_quantity = bat_6;	//本次电量值
	
	if(ad_val >= BUS_VOLT_MAX) 	//最大电量
	{
		last_battary_quantity = bat_6;
		last_val = SPEED_VAL_MAX;
		return bat_6;
	}
	if(ad_val < SPEED_VAL_MIN)	//最小电量
	{
		last_battary_quantity = 0;
		last_val = 0;
		return 0;
	}
	else	//正常计算
	{
		battary_quantity = (ad_val-SPEED_VAL_MIN)/SPEED_STEP;	/* 计算当前赋予速度 */
		
		if(battary_quantity != last_battary_quantity)
		{
			if(battary_quantity > last_battary_quantity) /* 速度增加 */
			{
				if((ad_val - last_val) > (BAT_STEP>>1))
				{
					last_battary_quantity = battary_quantity;
					last_val = ad_val;
				}
			}
			else if(last_battary_quantity > battary_quantity)
			{
				if((last_val - ad_val) > (BAT_STEP>>1))
				{
					last_battary_quantity = battary_quantity;
					last_val = ad_val;
				}				
			}
		}	
	}
	
	if(last_battary_quantity >  6)	/* 限制电量大小 */
		last_battary_quantity = 6;	
	
	return last_battary_quantity;
}

/* 滑动均值滤波 计算当前母线电流 */
uint16_t calc_bus_current(uint16_t ad_val)
{
	#define FILTER_DEEP 10
	
	static uint16_t average_array[FILTER_DEEP] = {0};
	uint8_t i = 0;
	uint32_t sum = 0;
	for(i=0;i<FILTER_DEEP-1;i++)
	{
		average_array[i+1] = average_array[i];
	}
	average_array[0] = ad_val;
	
	for(i=0;i<FILTER_DEEP;i++)
	{
		sum += average_array[i];
	}
	
	ad_val = sum & 0xffff;
	
	return ad_val;
}


/* 启动adc */
void start_to_get_adc(void)
{
	HAL_ADC_Start_DMA(&hadc ,(uint32_t*)adc_array, 3);
}

void lock_power(void)
{
	if(isPowerSwitchOFF() == 0)
		power_on();	
	else
		power_off();
}

uint8_t get_current_dir(void)
{
	return motor_params.dir;
}

uint8_t get_current_phase(void)
{
	return motor_params.phase;
}

void set_current_phase(uint8_t phase)
{
	motor_params.phase = phase;
}




static uint16_t commutate_time_counter = 0;
static uint16_t commu_counter = 0;
static uint16_t force_stall_flag = 0;

void clean_force_comm_counter(void)
{
	commutate_time_counter = 0;
}

static uint8_t isForceCommutateErr(void)
{
	return force_stall_flag;
}


/* 强制换向 */
void force_commutate(void)
{
	if(++ commutate_time_counter == 20)	/* 20ms */
	{
		/* 连续进入20次，判断为堵转 */
		
		/* 进行换向 */
		//Motor_SyncRun();
		
		/* 累加换向次数 */
		commu_counter ++;
	
	}
	
	if(commu_counter == 20)
	{
		commu_counter = 0;
		force_stall_flag = 1;	//通知堵转，进行停止
	}

}
	



