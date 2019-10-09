#include "bsp.h"
#include "oled.h"

#if 0
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM3_Init(void);
void Error_Handler(void);


ADC_HandleTypeDef hadc;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_adc;



void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSI14CalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}


/* ADC初始化 */
static void MX_ADC_Init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.LowPowerAutoPowerOff = DISABLE;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.DMAContinuousRequests = ENABLE;
	hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	
	if (HAL_ADC_Init(&hadc) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;

	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_4;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfig.Channel = ADC_CHANNEL_5;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

/* 初始化I2C */
static void MX_I2C1_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x0000020B;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
	{
		Error_Handler();
	}
}


/* 定时器1及PWM初始化 */
static void MX_TIM1_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 1199;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV2;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 100;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.Pulse = 200;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.Pulse = 300;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_TIM_MspPostInit(&htim1);
}

static void MX_TIM3_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	
	__HAL_RCC_TIM3_CLK_ENABLE();	/* 开启时钟 */

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 47;	/* 定时1ms */
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 999;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

/* IO口设置 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* 使能时钟 */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* 设置电源开关控制为低 */
	HAL_GPIO_WritePin(PWR_CTL_PIN_GPIO_Port, PWR_CTL_PIN_Pin, GPIO_PIN_RESET);

	/* 设置下桥为低 */
	HAL_GPIO_WritePin(GPIOB, W_LOW_PIN_Pin|V_LOW_PIN_Pin|U_LOW_PIN_Pin, GPIO_PIN_RESET);

	/* 电位器开关状态 */
	GPIO_InitStruct.Pin = PWR_SW_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PWR_SW_PIN_GPIO_Port, &GPIO_InitStruct);

	/* 电源开关控制 */
	GPIO_InitStruct.Pin = PWR_CTL_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PWR_CTL_PIN_GPIO_Port, &GPIO_InitStruct);

	/* BEMF反电动势中断输入 */
	GPIO_InitStruct.Pin = U_BEMF_PIN_Pin|V_BEMF_PIN_Pin|W_BEMF_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;	/* 双边沿 */
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* 电位器输入/母线电压输入/电流输入 */
	GPIO_InitStruct.Pin = SPEED_AD_PIN_Pin|V_BUS_AD_PIN_Pin|C_BUS_AD_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* BLDC下桥 */
	GPIO_InitStruct.Pin = W_LOW_PIN_Pin|V_LOW_PIN_Pin|U_LOW_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}


static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

}

static void MX_NVIC_Init(void)
{
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* TIM3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}


/* 硬件初始化错误函数 */
void Error_Handler(void)
{
	while(1);
}


/* bsp_init */
void bsp_init(void)
{
	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	MX_DMA_Init();
	MX_ADC_Init();
	MX_NVIC_Init();
	
	
	OLED_Init();
	HAL_TIM_Base_Start_IT(&htim3);
}

#endif

