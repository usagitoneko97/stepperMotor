/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int initialValue;
int delayValue;
int loopFlag = 0;
int period;
int initialDelay;

MotorInfo rightMotorInfo, leftMotorInfo;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void rampMotorTo(int delayValue);
void rampMotorExp(int delayValue, int initialValue);
void rampMotorExpIt(int delayValue, int initialValue);
void pulsePin(int pin);
void pulseMotorOnTimeout(MotorInfo *motor);
//void Calculation(AngleSpeed *MainInfo , MotorInfo *leftMotor , MotorInfo *rightMotor);
void calculation(AngleSpeed *MainInfo, MotorInfo *leftMotor,
		MotorInfo *rightMotor);
int getSlowRatio(int angle);
int getQuadrant(int angle);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void delay(volatile unsigned int delay) {
	while (delay--)
		;
}

/* USER CODE END 0 */

int main(void) {

	/* USER CODE BEGIN 1 */
	volatile int i = 1;
	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();

	/* USER CODE BEGIN 2 */
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
//  rampMotorExp(750, 20000);
//  rampMotorTo(750);
	/*leftMotorInfo.reloadPeriod = 800;
	 leftMotorInfo.stepPeriod = 20000;
	 leftMotorInfo.prevStepPeriod= 20000;
	 leftMotorInfo.expDelay = 72000000 / (ACCELERATION * leftMotorInfo.stepPeriod);

	 rightMotorInfo.reloadPeriod = 2500;
	 rightMotorInfo.stepPeriod = 15000;
	 rightMotorInfo.prevStepPeriod= 15000;
	 rightMotorInfo.expDelay = 72000000 / (ACCELERATION * rightMotorInfo.stepPeriod);

	 htim2.Init.Period = rightMotorInfo.stepPeriod;
	 HAL_TIM_Base_Init(&htim2);
	 HAL_TIM_Base_Start_IT(&htim2);*/

	leftMotorInfo.stepPeriod = 67500;
	leftMotorInfo.prevStepPeriod = 67500;

	rightMotorInfo.stepPeriod = 67500;
	rightMotorInfo.prevStepPeriod = 67500;

	AngleSpeed angleSpeed = { .Speed = 50, .Angle = 0 };
	calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);
	htim2.Init.Period = rightMotorInfo.stepPeriod;
	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_Base_Start_IT(&htim2);
	while (1) {
		HAL_Delay(5000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 180;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);

		HAL_Delay(5000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 0;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);
		/*HAL_Delay(3000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 90;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);

		HAL_Delay(3000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 135;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);

		HAL_Delay(3000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 180;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);

		HAL_Delay(3000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 225;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);

		HAL_Delay(3000);
		angleSpeed.Speed = 100;
		angleSpeed.Angle = 270;
		calculation(&angleSpeed, &leftMotorInfo, &rightMotorInfo);*/

		//few things to update when android command comes:
		// 1.
		/*HAL_Delay(3000);
		 leftMotorInfo.reloadPeriod = 20000;
		 rightMotorInfo.reloadPeriod = 20000;
		 rightMotorInfo.expDelay = 72000000 / (ACCELERATION * rightMotorInfo.prevStepPeriod);
		 leftMotorInfo.expDelay = 72000000 / (ACCELERATION * leftMotorInfo.prevStepPeriod);

		 HAL_Delay(1000);
		 leftMotorInfo.reloadPeriod = 2500;
		 rightMotorInfo.reloadPeriod = 750;
		 rightMotorInfo.expDelay = 72000000 / (ACCELERATION * rightMotorInfo.prevStepPeriod);
		 leftMotorInfo.expDelay = 72000000 / (ACCELERATION * leftMotorInfo.prevStepPeriod);
		 HAL_Delay(3000);*/

		/*if(loopFlag){
		 HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
		 delay(750);
		 }*/
//	  HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
//	  HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
//	  delay(15000);
//	  rampMotorExpIt(750, 20000);
//	  HAL_Delay(500);
//	  rampMotorExpIt(20000, 750);
//	  HAL_Delay(500);
//	  rampMotorExpIt(750, 2500);
//	  HAL_Delay(500);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
//	  HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
//	  HAL_GPIO_WritePin(GPIOA, motorEnable_Pin, GPIO_PIN_SET);
	}

	HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, motorEnable_Pin, GPIO_PIN_SET);
	while (1)
		;
	/* USER CODE END 3 */

}

/** System Clock Configuration
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM2 init function */
static void MX_TIM2_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 9;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 10000;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
			leftMotor_Pin | leftMotorDir_Pin | motorEnable_Pin | rightMotor_Pin
					| rightMotorDir_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : led1_Pin */
	GPIO_InitStruct.Pin = led1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(led1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : leftMotor_Pin leftMotorDir_Pin motorEnable_Pin */
	GPIO_InitStruct.Pin = leftMotor_Pin | leftMotorDir_Pin | motorEnable_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : rightMotor_Pin rightMotorDir_Pin */
	GPIO_InitStruct.Pin = rightMotor_Pin | rightMotorDir_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void xHAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	pulseMotorOnTimeout(&leftMotorInfo);
	pulseMotorOnTimeout(&rightMotorInfo);

	int numOfActive = 0;
	unsigned long timeout;

	timeout = leftMotorInfo.stepPeriod;
	timeout =
			timeout <= rightMotorInfo.stepPeriod ?
					timeout : rightMotorInfo.stepPeriod;
	//  printf("timeout = %d\n", timeout);
	if (IS_MOTOR_ACTIVE(leftMotorInfo)) {

		leftMotorInfo.stepPeriod -= timeout;
		numOfActive++;
	}
	if (IS_MOTOR_ACTIVE(rightMotorInfo)) {

		rightMotorInfo.stepPeriod -= timeout;
		numOfActive++;
	}
	if (numOfActive > 0)
		htim2.Instance->ARR = timeout;
//     timer0_write(ESP.getCycleCount() + timeout);
	else {
//     printf("detach called");
//     timer0_detachInterrupt();
//     isTimerOn = 0;
	}

	/*int i;
	 if(period > delayValue){

	 HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
	 htim2.Instance->ARR = period;
	 initialDelay += RAMP_RATE;
	 period = 72000000 / (ACCELERATION * initialDelay);
	 }
	 else{
	 loopFlag = 1;
	 }*/
}

void pulseMotorOnTimeout(MotorInfo *motor) {
	if (motor->stepPeriod < 2000 && motor->reloadPeriod != 0) {
		if (motor == &leftMotorInfo) {
			pulsePin(leftMotor_Pin);
			HAL_GPIO_WritePin(leftMotorDir_GPIO_Port, leftMotorDir_Pin,
					leftMotorInfo.curDir);
		} else {
			pulsePin(rightMotor_Pin);
			HAL_GPIO_WritePin(rightMotorDir_GPIO_Port, rightMotorDir_Pin,
					rightMotorInfo.curDir);
		}

		if (motor->prevStepPeriod > 56000) {
			motor->curDir = motor->dir;
		}
		if (motor->curDir != motor->dir) {
			//ramp to zero
			motor->expDelay -= RAMP_RATE * 2; //ramp faster
			motor->prevStepPeriod = 72000000 / (ACCELERATION * motor->expDelay);
			motor->stepPeriod += motor->prevStepPeriod;
		} else {
			if (motor->prevStepPeriod > (motor->reloadPeriod + THRESHOLD_GAP)) {
				motor->expDelay += RAMP_RATE;
				motor->prevStepPeriod = 72000000
						/ (ACCELERATION * motor->expDelay);
				motor->stepPeriod += motor->prevStepPeriod;
			} else if (motor->prevStepPeriod
					< (motor->reloadPeriod - THRESHOLD_GAP)) {
				motor->expDelay -= RAMP_RATE;
				motor->prevStepPeriod = 72000000
						/ (ACCELERATION * motor->expDelay);
				motor->stepPeriod += motor->prevStepPeriod;
			} else {
				motor->stepPeriod += motor->reloadPeriod;
				motor->prevStepPeriod = motor->stepPeriod;
				//calculate expDelay so that the when next speed request is fasterm expDelay can be used straight away
			}
		}
	}
}

void pulsePin(int pin) {
	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_RESET);
}

// 1. android update speed and angle
// 2. calculate related leftmotor and right motor reloadPeriod
// 3. accelerate/deaccelerate both motor at the same acceleration to reach the reload period

// TEST1.

void rampMotorTo(int delayValue) {
	uint32_t i, currentDelay = INITIAL_DELAY;
	while (currentDelay > delayValue) {
		for (i = 0; i < NO_RAMP_CYCLE; i++) {
			HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
			delay(currentDelay);
			HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
			delay(currentDelay);
		}
		currentDelay -= RAMP_RATE;
	}
	while (1) {
		HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
		delay(delayValue);
		HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
		delay(delayValue);
	}
}

void rampMotorExp(int delayValue, int initialValue) {
	int i;
	uint32_t initialDelay = 72000000 / (ACCELERATION * initialValue);

	uint32_t period = initialValue;
	while (period > delayValue) {
		for (i = 0; i < NO_RAMP_CYCLE; i++) {
			HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
			delay(period);
		}
		initialDelay += RAMP_RATE;
		period = 72000000 / (ACCELERATION * initialDelay);

	}
	while (1) {
		HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, motorStep_Pin, GPIO_PIN_RESET);
		delay(period);
	}
}

void rampMotorExpIt(int delayValuei, int initialValuei) {
	initialDelay = 72000000 / (ACCELERATION * initialValuei);
	delayValue = delayValuei;
	initialValue = initialValuei;
	period = initialValue;
	htim2.Init.Period = initialValuei;
	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_Base_Start_IT(&htim2);
}

void Calculation(AngleSpeed *MainInfo, MotorInfo *leftMotor,
		MotorInfo *rightMotor) {

	if (MainInfo->previousSpeed != MainInfo->Speed
			|| MainInfo->previousAngle != MainInfo->Angle) {
		// Serial.println("new speed and angle");
		//Serial.println("new speed and angle");
		//Serial.println(MainInfo->Speed);
		//Serial.println(MainInfo->Angle);
//        enableMotor();

		MainInfo->previousSpeed = MainInfo->Speed;
		MainInfo->previousAngle = MainInfo->Angle;
		int max_period;
		float tempAngle;

		if (MainInfo->Speed == 0.0) {
//      ForceStop(leftMotor ,rightMotor);
		} else {
//        enableMotor();
		}

		if (MainInfo->Angle >= 180) {
			tempAngle = 360 - MainInfo->Angle;
			leftMotor->dir = MOTOR_LEFT_BACKWARD;
			rightMotor->dir = MOTOR_RIGHT_BACKWARD;
		} else {
			tempAngle = MainInfo->Angle;
			leftMotor->dir = MOTOR_LEFT_FOWARD;
			rightMotor->dir = MOTOR_RIGHT_FOWARD;
		}

		max_period = (80000 / MainInfo->Speed);
		leftMotor->reloadPeriod = (max_period * (tempAngle / 180)) + 200;
		rightMotor->reloadPeriod = (max_period
				- (max_period * (tempAngle / 180))) + 200;
	}
}

void calculation(AngleSpeed *MainInfo, MotorInfo *leftMotor,
		MotorInfo *rightMotor) {
	int maxPeriod = MOTOR_MAX_PERIOD / (MainInfo->Speed / 100.00);

	int acute, ratio;
	int quadrant = getQuadrant(MainInfo->Angle);
	switch (quadrant) {
	case 1:
		acute = 90 - (MainInfo->Angle);
		acute = acute == 0 ? 1 : acute;
		ratio = getSlowRatio(acute);
		//left wheel will be fast, right wheel will be slow
		leftMotor->reloadPeriod = maxPeriod;
		rightMotor->reloadPeriod = ratio * maxPeriod;
		leftMotor->dir = 1;
		rightMotor->dir = 1;
		break;
	case 2:
		acute = 90 - (180 - MainInfo->Angle);
		acute = acute == 0 ? MINIMUM_ANGLE : acute;
		ratio = getSlowRatio(acute);
		leftMotor->reloadPeriod = maxPeriod;
		rightMotor->reloadPeriod = ratio * maxPeriod;
		leftMotor->dir = 0;
		rightMotor->dir = 0;
		break;
	case 3:
		acute = 90 - (MainInfo->Angle - 180);
		acute = acute == 0 ? 1 : acute;
		ratio = getSlowRatio(acute);
		rightMotor->reloadPeriod = maxPeriod;
		leftMotor->reloadPeriod = ratio * maxPeriod;
		leftMotor->dir = 0;
		rightMotor->dir = 0;
		break;
	case 4:
		acute = 90 - (360 - MainInfo->Angle);
		acute = acute == 0 ? 1 : acute;
		ratio = getSlowRatio(acute);
		rightMotor->reloadPeriod = maxPeriod;
		leftMotor->reloadPeriod = ratio * maxPeriod;
		leftMotor->dir = 1;
		rightMotor->dir = 1;
		break;
	}
	//expDelay needs to be reload here for some reason that needs investigate
	leftMotor->expDelay = 72000000 / (ACCELERATION * leftMotor->prevStepPeriod);
	rightMotor->expDelay = 72000000
			/ (ACCELERATION * rightMotor->prevStepPeriod);
}

int getSlowRatio(int angle) {
	return 1 / (angle / 90.000);
}

int getQuadrant(int angle) {
	return (angle / 90) + 1;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
