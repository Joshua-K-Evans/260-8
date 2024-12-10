/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define I2C_ADDR 0x27 // I2C address of the PCF8574
#define RS_BIT 0 // Register select bit
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Backlight bit
#define D4_BIT 4 // Data 4 bit
#define D5_BIT 5 // Data 5 bit
#define D6_BIT 6 // Data 6 bit
#define D7_BIT 7 // Data 7 bit
#define LCD_ROWS 2 // Number of rows on the LCD
#define LCD_COLS 16 // Number of columns on the LCD
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
uint8_t backlight_state = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
uint8_t keypad_decode();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
	uint8_t data = nibble << D4_BIT;
	data |= rs << RS_BIT;
	data |= backlight_state << BL_BIT; // Include backlight state in data
	data |= 1 << EN_BIT;
	HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
	HAL_Delay(1);
	data &= ~(1 << EN_BIT);
	HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
}
void lcd_send_cmd(uint8_t cmd) {
	uint8_t upper_nibble = cmd >> 4;
	uint8_t lower_nibble = cmd & 0x0F;
	lcd_write_nibble(upper_nibble, 0);
	lcd_write_nibble(lower_nibble, 0);
	if (cmd == 0x01 || cmd == 0x02) {
		HAL_Delay(2);
	}
}
void lcd_send_data(uint8_t data) {
	uint8_t upper_nibble = data >> 4;
	uint8_t lower_nibble = data & 0x0F;
	lcd_write_nibble(upper_nibble, 1);
	lcd_write_nibble(lower_nibble, 1);
}
void lcd_init() {
	HAL_Delay(50);
	lcd_write_nibble(0x03, 0);
	HAL_Delay(5);
	lcd_write_nibble(0x03, 0);
	HAL_Delay(1);
	lcd_write_nibble(0x03, 0);
	HAL_Delay(1);
	lcd_write_nibble(0x02, 0);
	lcd_send_cmd(0x28);
	lcd_send_cmd(0x0C);
	lcd_send_cmd(0x06);
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}
void lcd_write_string(char *str) {
	while (*str) {
		lcd_send_data(*str++);
	}
}
void lcd_set_cursor(uint8_t row, uint8_t column) {
	uint8_t address;
	switch (row) {
	case 0:
		address = 0x00;
		break;
	case 1:
		address = 0x40;
		break;
	default:
		address = 0x00;
	}
	address += column;
	lcd_send_cmd(0x80 | address);
}
void lcd_clear(void) {
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}
void lcd_backlight(uint8_t state) {
	if (state) {
		backlight_state = 1;
	} else {
		backlight_state = 0;
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
	/* USER CODE BEGIN 2 */
	// I2C pull-up resistors
	GPIOB->PUPDR |= 0b01 << (8 * 2);
	GPIOB->PUPDR |= 0b01 << (9 * 2);
	// Initialize the LCD
	lcd_init();
	lcd_backlight(1); // Turn on backlight
	// Write a string to the LCD

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
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
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x10D19CE4;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : DATA_AVAILABLE_Pin */
	GPIO_InitStruct.Pin = DATA_AVAILABLE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DATA_AVAILABLE_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DATA0_Pin DATA1_Pin DATA2_Pin */
	GPIO_InitStruct.Pin = DATA0_Pin | DATA1_Pin | DATA2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : DATA3_Pin */
	GPIO_InitStruct.Pin = DATA3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DATA3_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// This function reads the four data pins from the keypad encoder and maps them to the key value
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (HAL_GPIO_ReadPin(DATA_AVAILABLE_GPIO_Port, DATA_AVAILABLE_Pin) != 0) {
		// Read data
		uint8_t key = keypad_decode();  // determine which key was pressed
		// Based on what key was pressed, do something (each case)
		switch (key) {

		case 0xA:
			lcd_write_string("A");
		case 0xB:
			lcd_write_string("B");
		case 0xC:
			lcd_write_string("C");
		case 0xD:
			lcd_write_string("D");
		case 0xE: //*
			lcd_write_string("*");
		case 0xF: //#
			lcd_write_string("#");
		case 0x0:
			lcd_write_string("0");
		case 0x1:
			lcd_write_string("1");
		case 0x2:
			lcd_write_string("2");
		case 0x3:
			lcd_write_string("3");
		case 0x4:
			lcd_write_string("4");
		case 0x5:
			lcd_write_string("5");
		case 0x6:
			lcd_write_string("6");
		case 0x7:
			lcd_write_string("7");
		case 0x8:
			lcd_write_string("8");
		case 0x9:
			lcd_write_string("9");

		}
	}
}

// This function reads the four data pins from the keypad encoder and maps them to the key value
unsigned char keypad_decode() {
	unsigned char key = 0x0;
	unsigned char data = 0b0000;

	// read the data pins and combine into the 4-bit value: D3_D2_D1_D0
	data = HAL_GPIO_ReadPin(DATA0_GPIO_Port, DATA0_Pin)<<0 |
		   HAL_GPIO_ReadPin(DATA1_GPIO_Port, DATA1_Pin)<<1 |
		   HAL_GPIO_ReadPin(DATA2_GPIO_Port, DATA2_Pin)<<2 |
		   HAL_GPIO_ReadPin(DATA3_GPIO_Port, DATA3_Pin)<<3;//  reads D3, D2, D1, and D0 and then combine them into a 4-bit number: D3_D2_D1_D0

	// The key encoder gives the following "data" values:
	// 0 1 2 3
	// 4 5 6 7
	// 8 9 A B
	// C D E F

	// The following switch statement re-maps it to these "key" names:
	// 1 2 3 A
	// 4 5 6 B
	// 7 8 9 C
	// E 0 F D, where E is * and F is #

   // Finish this switch statement to remap the "data" to the correct "key"
	switch (data) {
      case 0x0:
         key = 0x1;
         break;
      case 0x1:
         key = 0x2;
         break;
      case 0x2:
         key = 0x3;
         break;
      case 0x3:
         key = 0xA;
         break;
      case 0x4:
         key = 0x4;
         break;
      case 0x5:
         key = 0x5;
         break;
      case 0x6:
         key = 0x6;
         break;
      case 0x7:
         key = 0xB;
         break;
      case 0x8:
         key = 0x7;
         break;
      case 0x9:
         key = 0x8;
         break;
      case 0xA:
         key = 0x9;
         break;
      case 0xB:
         key = 0xC;
         break;
      case 0xC:
         key = 0xE;
         break;
      case 0xD:
         key = 0x0;
         break;
      case 0xE:
         key = 0xF;
         break;
      case 0xF:
         key = 0xD;
         break;
	}

	return key;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
/* USER CODE BEGIN Error_Handler_Debug */
/* User can add his own implementation to report the HAL error return state */
__disable_irq();
while (1) {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
