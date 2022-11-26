/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "pos_pid.h"
#include "oled_096.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t mode = 0;
uint16_t target = 200;
uint16_t speed = 700;
uint8_t done = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define PA0_SET	GPIOA->BSRR = GPIO_PIN_0
#define PA0_CLR	GPIOA->BRR = GPIO_PIN_0

/*int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 10);
    return (ch);
}*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    int16_t count = (int16_t)htim3.Instance->CNT;
    int16_t out = pos_pid_update(count);
    if(out == 0) {
        htim2.Instance->CCR1 = 0;
        htim2.Instance->CCR2 = 0;
		HAL_TIM_Base_Stop_IT(&htim17);
		done = 1;
    } else {
        htim2.Instance->CCR1 = 1000 - out;
        htim2.Instance->CCR2 = 1000;
    }
    //printf("%d %d\r\n", count, out);
}

uint8_t read_key() {
    uint8_t ret = 0;
    if((GPIOA->IDR & GPIO_PIN_4) == 0) {
        ret |= 1;
    }
    if((GPIOA->IDR & GPIO_PIN_3) == 0) {
        ret |= 2;
    }
    if((GPIOA->IDR & GPIO_PIN_2) == 0) {
        ret |= 4;
    }
    if((GPIOA->IDR & GPIO_PIN_5) == 0) {
        ret |= 8;
    }
    return ret;
}

void show_target() {
	static char buf[16];
	sprintf(buf, "target: %4d", target);
	oled_paint_string(&hi2c1, 16, 4, buf);
}

void show_speed() {
	static char buf[16];
	sprintf(buf, " speed: %4d", speed);
	oled_paint_string(&hi2c1, 16, 4, buf);
}

void show_mode_info() {
    if(mode == 0) {
        oled_paint_hz(&hi2c1, 32 + 0, 2, 0);
        oled_paint_hz(&hi2c1, 32 + 16, 2, 1);
        oled_paint_hz(&hi2c1, 32 + 32, 2, 2);
        oled_paint_hz(&hi2c1, 32 + 48, 2, 3);
        show_target();
    } else if (mode == 1) {
        oled_paint_hz(&hi2c1, 32 + 0, 2, 4);
        oled_paint_hz(&hi2c1, 32 + 16, 2, 5);
        oled_paint_hz(&hi2c1, 32 + 32, 2, 2);
        oled_paint_hz(&hi2c1, 32 + 48, 2, 3);
        show_speed();
		oled_paint_string(&hi2c1, 72, 6, "     ");
    } else if (mode == 2) {
		oled_paint_hz(&hi2c1, 32 + 0, 2, 6);
        oled_paint_hz(&hi2c1, 32 + 16, 2, 7);
        oled_paint_hz(&hi2c1, 32 + 32, 2, 2);
        oled_paint_hz(&hi2c1, 32 + 48, 2, 3);
        show_speed();
		oled_paint_string(&hi2c1, 72, 6, "     ");
	}
}

void adjust_target_or_speed(uint8_t key) {
	if (mode == 0) {
		if (key == 2) {
			target = target < 2000 ? target + 100 : 2000;
		} else {
			target = target > 100 ? target - 100 : 100;
		}
		show_target();
	} else if (mode == 1 || mode == 2) {
		if (key == 2) {
			speed = speed < 800 ? speed + 50 : 800;
		} else {
			speed = speed > 300 ? speed - 50 : 300;
		}
		show_speed();
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

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
    MX_I2C1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM17_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    //printf("hello solder paste elf\r\n");

    oled_init(&hi2c1);
    oled_clear(&hi2c1);

    oled_paint_string(&hi2c1, 8, 0, "solder paste v1");

    htim2.Instance->CCR1 = 0;
    htim2.Instance->CCR2 = 0;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 | TIM_CHANNEL_2);

    pos_pid_init(1, 0.05, 0.6, 800, 0);
    //pos_pid_set_target(3000);

    //htim2.Instance->CCR1 = 800;
    //htim2.Instance->CCR2 = 1000;

    uint8_t key_flag = 0;
    show_mode_info();
    while(1) {
        //int16_t count = (int16_t)htim3.Instance->CNT;
        //printf("%d\r\n", count);
        //printf("%x %x\r\n", a, b);
        uint8_t key = read_key();
        //oled_paint_char(&hi2c1, 0, 6, '0' + key);
        if(key == 1 && key_flag == 0) {
            key_flag = 10;
            mode = (mode + 1) % 3;
            show_mode_info();
		} else if (key == 2 || key == 4) {
			if (key_flag == 0 || key_flag > 7) {
				adjust_target_or_speed(key);
			}
			key_flag ++;
		} else if (key == 8) {
			if (mode == 0) {
				if (key_flag == 0 && done == 1) {
					key_flag = 10;
					done = 0;
					htim3.Instance->CNT = 0;
					pos_pid_set_target(target);
					HAL_TIM_Base_Start_IT(&htim17);
				}
			} else if (mode == 1) {
				htim2.Instance->CCR1 = 1000 - speed;
				htim2.Instance->CCR2 = 1000;
				while (read_key() == 8) {
					HAL_Delay(10);
				}
				htim2.Instance->CCR1 = 0;
				htim2.Instance->CCR2 = 0;
			} else if (mode == 2) {
				htim2.Instance->CCR1 = 1000;
				htim2.Instance->CCR2 = 1000 - speed;
				while (read_key() == 8) {
					HAL_Delay(10);
				}
				htim2.Instance->CCR1 = 0;
				htim2.Instance->CCR2 = 0;
			}
        } else if(key == 0) {
            key_flag = 0;
        }
		
		char buf[32];
		if (mode == 0) {
			int16_t count = (int16_t)htim3.Instance->CNT;
			sprintf(buf, "%5d", count);
			oled_paint_string(&hi2c1, 72, 6, buf);
		}
			
        HAL_Delay(100);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while(1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

