/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <math.h>
#include "st7735\st7735.h"
#include <time.h>
#include "stdbool.h"
#define gravity 9.81f
#define SCREEN_W 160
#define SCREEN_H 80

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    float posX_inicio;
    float posY_inicio;
    int alvoX;
    int alvoRaio;
    float ventoForca;
    float ventoDirecao;
    float tempoEntrega;
    int sucesso; // 1 = acertou, 0 = errou
} Tentativa;

// Array global para armazenar até 3 tentativas
Tentativa tentativas[3];
int tentativasCount = 0; // contador de tentativas


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void InicializarSimulacao(float *altura, float *vel, float *vento);
void ConfigurarParametros(void);
int DefinirAlvo(void);
void DefinirPacote(int tipo, float *massa, float *Cd, float *area);
float CalcularPosicaoLancamento(float alvoPixelF, float alturaQueda, float velAviao, float vento, float massa, float Cd, float area);
void ExecutarLancamento(float alturaAviao, float velAviao, float vento, float posicaoLancamentoPixelF, int posicaoAlvoPixel, float massa, float Cd, float area);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  srand(time(NULL));
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init();
  ST7735_FillScreen(BLACK);
  srand(HAL_GetTick());
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
   {
	  float alturaAviao, velocidadeAviao, xVento;
    InicializarSimulacao(&alturaAviao, &velocidadeAviao, &xVento);
    ConfigurarParametros();

    int posicaoAlvoPixel = DefinirAlvo();

    float massa, Cd, area;
    DefinirPacote(rand() % 2, &massa, &Cd, &area);

    float alturaQueda = (SCREEN_H - 2) - alturaAviao;
    float posicaoLancamento = CalcularPosicaoLancamento(
        posicaoAlvoPixel, alturaQueda, velocidadeAviao, xVento, massa, Cd, area);

    ExecutarLancamento(alturaAviao, velocidadeAviao, xVento,
    posicaoLancamento, posicaoAlvoPixel, massa, Cd, area);

       /* USER CODE END WHILE */

       /* USER CODE BEGIN 3 */
     }
     /* USER CODE END 3 */
   }


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST7735_DC_Pin|ST7735_RES_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ST7735_CS_Pin */
  GPIO_InitStruct.Pin = ST7735_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST7735_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7735_DC_Pin ST7735_RES_Pin */
  GPIO_InitStruct.Pin = ST7735_DC_Pin|ST7735_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
	void InicializarSimulacao(float *altura, float *velAviao, float *vento) {
  *altura = (rand() % 60);
  *velAviao   = (rand() % 20);
  *vento  = (rand() % 10);
}
	void ConfigurarParametros(){
  /*ST7735_FillScreen(BLACK);
  ST7735_WriteString(40, 15, "Configure na", Font_7x10, WHITE, BLACK);
  ST7735_WriteString(40, 30, "IDE", Font_7x10, WHITE, BLACK);
  HAL_Delay(2000);
  ST7735_FillScreen(BLACK);
  ST7735_WriteString(40, 0, "Pressione PA10", Font_7x10, WHITE, BLACK);
  ST7735_WriteString(40, 15, "para prosseguir", Font_7x10, WHITE, BLACK);
  ST7735_WriteString(40, 40, "Pressione P11", Font_7x10, WHITE, BLACK);

  ST7735_WriteString(40, 60, "para random", Font_7x10, WHITE, BLACK);
  */
  ST7735_FillScreen(BLACK);

}

  int DefinirAlvo(void) {
    float pct = ((rand() % 21) + 70) / 100.0f;
    float alvoPixelF = pct * SCREEN_W;
    int posicaoAlvoPixel = (int)roundf(alvoPixelF);
    ST7735_FillRectangle(posicaoAlvoPixel, SCREEN_H - 2, 5, 2, RED);
    return posicaoAlvoPixel;
}
  void DefinirPacote(int tipo, float *massa, float *Cd, float *area) {
    if (tipo == 0) {
        *massa = 2.0f;
        *Cd = 0.8f;
        *area = 20.0f;
    } else {
        *massa = 3.0f;
        *Cd = 0.8f;
        *area = 30.0f;
    }
}
  float CalcularPosicaoLancamento(
          float alvoPixelF, float alturaQueda,
          float velAviao, float vento,
          float massa, float Cd, float area) {
      float tempoVoo = sqrtf(2.0f * alturaQueda / gravity);
      float alcancePx = (velAviao + vento) * tempoVoo;  // simplificado
      return alvoPixelF - alcancePx;
  }
  void ExecutarLancamento(
        float alturaAviao, float velAviao, float vento,
        float posicaoLancamentoPixelF, int posicaoAlvoPixel,
        float massa, float Cd, float area) {

    float posicaoAviaoPixelF = 0.0f;
    float posicaoQuedaPixelXf = posicaoLancamentoPixelF;
    bool cargaJaSolta = false;

    int aviaoXprev = 0;
    int aviaoY = (int)roundf(alturaAviao);
    int cargaXprev = (int)roundf(posicaoQuedaPixelXf);
    int cargaYprev = aviaoY;

    uint32_t tickPrev = HAL_GetTick();
    float tempoQueda = 0.0f;

    while (1) {
        uint32_t tickNow = HAL_GetTick();
        float dt = (tickNow - tickPrev) / 1000.0f;
        tickPrev = tickNow;

        posicaoAviaoPixelF += (velAviao + vento) * dt;
        int aviaoX = (int)roundf(posicaoAviaoPixelF);
        if (aviaoX != aviaoXprev) {
            ST7735_FillRectangle(aviaoXprev, aviaoY, 15, 5, BLACK);
            aviaoXprev = aviaoX;
        }
        ST7735_FillRectangle(aviaoX, aviaoY, 15, 5, WHITE);

        if (!cargaJaSolta && posicaoAviaoPixelF >= posicaoLancamentoPixelF) {
            cargaJaSolta = true;
        }

        if (cargaJaSolta) {
            tempoQueda += dt;

            // deslocamento horizontal
              posicaoQuedaPixelXf += (velAviao + vento) * dt;

              // queda vertical (eixo Y cresce pra baixo na tela)
              float posicaoQuedaPixelYf = alturaAviao + 0.5f * gravity * tempoQueda * tempoQueda;

            int cargaX = (int)roundf(posicaoQuedaPixelXf)+5;
            int cargaY = (int)roundf(posicaoQuedaPixelYf)+5;

            if (cargaX != cargaXprev || cargaY != cargaYprev) {
                ST7735_FillRectangleFast(cargaXprev, cargaYprev, 2, 2, BLACK);
                cargaXprev = cargaX;
                cargaYprev = cargaY;
            }
            ST7735_FillRectangleFast(cargaX, cargaY, 2, 2, WHITE);

            // impacto com tolerância de 5px no X
              if (abs(cargaX - posicaoAlvoPixel) <= 5 && cargaY >= SCREEN_H - 2) {
                  break;
              }
        }

        HAL_Delay(16);
    }
}

  int AvaliarTentativa(float cargaX, float cargaY, int alvoX, int alvoRaio) {
      // verifica se a carga está dentro do alvo
      if (cargaX >= (alvoX - alvoRaio) && cargaX <= (alvoX + alvoRaio) &&
          cargaY >= (SCREEN_H - 2 - alvoRaio) && cargaY <= (SCREEN_H - 2 + alvoRaio)) {
          return 1; // acerto
      }
      return 0; // erro
  }

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
