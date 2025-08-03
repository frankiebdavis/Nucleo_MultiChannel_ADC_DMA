/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : STM32 project using ADC to read an LDR sensor and control an LED
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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
// ADC channel readings
uint16_t lux = 0;                 // LDR value from PA0
uint16_t pot = 0;                 // Potentiometer value from PA1
uint16_t rawValues[2];           // DMA buffer for ADC1 (2 channels)

// UART message buffer
char msg[20];

// Flag set when ADC conversion completes
uint8_t convCompleted = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Triggered automatically when DMA-based ADC conversion is done
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	convCompleted = 1;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  // Start ADC1 in DMA mode to read two analog channels continuously
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) rawValues, 2);
  /* USER CODE END 2 */

  /* Infinite loop */
  while (1)
  {
    /* USER CODE BEGIN 3 */

    // Wait for DMA to finish transferring both ADC results
    while(!convCompleted);

    // Copy raw ADC readings from DMA buffer
    lux = rawValues[0];  // LDR sensor (PA0)
    pot = rawValues[1];  // Potentiometer (PA1)

    // Reset flag so next DMA cycle can be handled
    convCompleted = 0;

    // Format and send LDR reading over UART
    sprintf(msg, "Light: %hu \r\t\t", lux);
    HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);

    // Format and send potentiometer reading over UART
    sprintf(msg, "POT: %hu \r\n", pot);
    HAL_UART_Transmit(&huart2, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);

    // Delay to make output readable (prints once per second)
    HAL_Delay(1000);

    /* USER CODE END 3 */
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
    // Stay here if something goes wrong
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
  // Optional: implement logging here
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
