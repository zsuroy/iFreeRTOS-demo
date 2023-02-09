/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "retarget.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId LEDHandle;
osThreadId LED_CoHandle;
osThreadId KEYHandle;
osMessageQId myQueue01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void LedTask(void const * argument);
void LedTaskCo(void const * argument);
void KeyTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
  osMessageQDef(myQueue01, 16, uint16_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LED */
  osThreadDef(LED, LedTask, osPriorityNormal, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);

  /* definition and creation of LED_Co */
  osThreadDef(LED_Co, LedTaskCo, osPriorityLow, 0, 128);
  LED_CoHandle = osThreadCreate(osThread(LED_Co), NULL);

  /* definition and creation of KEY */
  osThreadDef(KEY, KeyTask, osPriorityNormal, 0, 128);
  KEYHandle = osThreadCreate(osThread(KEY), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_LedTask */
/**
* @brief Function implementing the LED thread.
* @note LED闪烁: 点亮程序
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTask */
void LedTask(void const * argument)
{
  /* USER CODE BEGIN LedTask */
  /* Infinite loop */
  for(;;)
  {

	  printf("Task1: ");
	  printf(argument);
	  printf("\r\n");
	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	  osDelay(200);
	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	  osDelay(800);

//	  printf("Task1: %s\r\n", (char *)argument); // 这句可能会导致输出异常
  }
  /* USER CODE END LedTask */
}

/* USER CODE BEGIN Header_LedTaskCo */
/**
* @brief Function implementing the LED_Co thread.
* @note 消息队列读取(出队)
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTaskCo */
void LedTaskCo(void const * argument)
{
  /* USER CODE BEGIN LedTaskCo */
  BaseType_t xStatus; //接收消息队列状态
  uint32_t Buf;

  // V1.0.0
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {

	  printf("Task2: ");
	  printf(argument);
	  printf("\r\n");

	  // 消息队列读取(出队)
	  printf("Now Buf(Out): %ld\r\n", Buf);
//	  xStatus = xQueueReceive(myQueue01Handle, &Buf, 0); //接收任务等待时间: 0, 不会阻塞；出队会读取并删除元素
//	  xStatus = xQueueReceive(myQueue01Handle, &Buf, portMAX_DELAY); //接收任务等待时间: 阻塞 portMAX_DELAY，队空时不会运行
	  xStatus = xQueuePeek(myQueue01Handle, &Buf, 0); // 偷窥队列: 读取但不删除元素

	  if(xStatus == pdPASS)
	  {
		 printf("Read Buf: %ld Ok!\r\n", Buf);
	  } else {
		 printf("Queue Out Failed!\r\n");
	  }
	  vTaskDelayUntil(&xLastWakeTime,1000); //绝对延时500ms
  }
  /* USER CODE END LedTaskCo */
}

/* USER CODE BEGIN Header_KeyTask */
/**
* @brief Function implementing the KEY thread.
* @note 消息队列发送(入队)，按键按下入队
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyTask */
void KeyTask(void const * argument)
{
  /* USER CODE BEGIN KeyTask */
  BaseType_t xStatus; //发送
  uint32_t Buf=2023;
  /* Infinite loop */
  for(;;)
  {
	  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	  {
		  osDelay(10);
		  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
		  {
			  printf("Now Buf(In): %ld\r\n", Buf);
			  xStatus = xQueueSendToBack(myQueue01Handle, &Buf, 0); //入队
			  if(xStatus == pdPASS)
			  {
				  printf("Write Buf: %ld Ok!\r\n", Buf);
			  } else {
				 printf("Queue In Failed!\r\n");
			  }
		  }
		  while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET); //检测松开
	  }
    osDelay(1);
  }
  /* USER CODE END KeyTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

