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
osSemaphoreId myBinarySem01Handle;
osSemaphoreId myCountingSem01Handle;

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

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* definition and creation of myCountingSem01 */
  osSemaphoreDef(myCountingSem01);
  myCountingSem01Handle = osSemaphoreCreate(osSemaphore(myCountingSem01), 2);

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

  /* definition and creation of LED_Co */
  osThreadDef(LED_Co, LedTaskCo, osPriorityAboveNormal, 0, 128);
  LED_CoHandle = osThreadCreate(osThread(LED_Co), NULL);

  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LED */
  osThreadDef(LED, LedTask, osPriorityBelowNormal, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);


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
  * @note Task2 -> 模拟优先级翻转 - 中优先级任务: V1.0.5
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	  printf("MidPriority_Task Running\n");
	  vTaskDelay(500);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_LedTask */
/**
* @brief Function implementing the LED thread.
* @note Task3 -> 模拟优先级翻转 - 低优先级任务:  V1.0.5
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTask */
void LedTask(void const * argument)
{
  /* USER CODE BEGIN LedTask */
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为 pdPASS */
	long i;
  /* Infinite loop */
  for(;;)
  {

//	  printf("Task1: ");
//	  printf(argument);
//	  printf("\r\n");
//	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//	  osDelay(200);
//	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//	  osDelay(800);

	  printf("LowPriority_Task Take Sem\n");
	  //获取二值信号量 xSemaphore,没获取到则一直等待
	  xReturn = xSemaphoreTake(myBinarySem01Handle,/* 二值信号量句柄 */
	  portMAX_DELAY); /* 等待时间 */
	  if( xReturn == pdTRUE )
		  printf("LowPriority_Task Running\n\n");
	  for(i=0;i<2000000;i++)//模拟低优先级任务占用信号量
	  {
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);//发起任务调度
	  }
	  printf("LowPriority_Task Give Sem!\r\n");
	  xReturn = xSemaphoreGive( myBinarySem01Handle );//给出二值信号量
	  vTaskDelay(500);

  }
  /* USER CODE END LedTask */
}

/* USER CODE BEGIN Header_LedTaskCo */
/**
* @brief Function implementing the LED_Co thread.
* @note Task1 -> 模拟优先级翻转 - 高优先级: V1.0.5
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTaskCo */
void LedTaskCo(void const * argument)
{
  /* USER CODE BEGIN LedTaskCo */
	BaseType_t xReturn;
	TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {

//	  printf("Task2: ");
//	  printf(argument);
//	  printf("\r\n");


	  printf("HighPriority_Task Take Sem\n");
	  //获取二值信号量 xSemaphore,没获取到则一直等待
	  xReturn = xSemaphoreTake(myBinarySem01Handle,/* 二值信号量句柄 */
	  portMAX_DELAY); /* 等待时间 */
	  if(pdTRUE == xReturn)
		  printf("HighPriority_Task Running\n");

	  xReturn = xSemaphoreGive( myBinarySem01Handle );//给出二值信号量
//	  vTaskDelayUntil(&xLastWakeTime,1000); //绝对延时500ms
	  vTaskDelay(500);
  }
  /* USER CODE END LedTaskCo */
}

/* USER CODE BEGIN Header_KeyTask */
/**
* @brief Function implementing the KEY thread.
* @note 计数信号量(Give): V1.0.4
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyTask */
void KeyTask(void const * argument)
{
  /* USER CODE BEGIN KeyTask */
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为 pdPASS */
  /* Infinite loop */
  for(;;)
  {
	  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	  {
		  osDelay(10);
		  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
		  {
//			  xReturn = xSemaphoreGive( myCountingSem01Handle );//给出二值信号量: 模拟停车(需要改计数信号量初始化中的初值为0)
//			  if( xReturn == pdTRUE )
//				  printf("Car stop!\r\n");
//			  else printf("No Space!\r\n");

			  xReturn = xSemaphoreTake(myCountingSem01Handle, 0);//给出二值信号量: 模拟开走
			  if( xReturn == pdTRUE )
				  printf("Car left!\r\n");
			  else printf("No cars!\r\n");
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

