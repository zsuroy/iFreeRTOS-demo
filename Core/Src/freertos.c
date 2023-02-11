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
osMutexId myMutex01Handle;
osSemaphoreId myBinarySem01Handle;
osSemaphoreId myCountingSem01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

EventGroupHandle_t Event_Handle =NULL; //事件组句柄

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
  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

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
  /* 创建 Event_Handle */
  Event_Handle = xEventGroupCreate();
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LED */
  osThreadDef(LED, LedTask, osPriorityNormal, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);

  /* definition and creation of LED_Co */
  osThreadDef(LED_Co, LedTaskCo, osPriorityBelowNormal, 0, 128);
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
  * @note nop: V1.0.7
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	  printf("Program Running\n");
	  vTaskDelay(5000);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_LedTask */
/**
* @brief Function implementing the LED thread.
* @note Task1 -> 任务通知.模拟二值信号量.接收任务1:  V1.0.8
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

	  //获取任务通知 ,没获取到则一直等待
	  ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
	  printf("Receive1_Task Got!\n\n");
	  vTaskDelay(20);

  }
  /* USER CODE END LedTask */
}

/* USER CODE BEGIN Header_LedTaskCo */
/**
* @brief Function implementing the LED_Co thread.
* @note Task2 -> 任务通知.模拟二值信号量.接收任务2:  V1.0.8
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTaskCo */
void LedTaskCo(void const * argument)
{
  /* USER CODE BEGIN LedTaskCo */
	uint32_t take_num = pdTRUE;
	TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {

//	  printf("Task2: ");
//	  printf(argument);
//	  printf("\r\n");

	  //模拟二值信号量: 获取任务通知 ,没获取到则一直等待
//	  ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
//	  printf("Receive2_Task Got!\n\n");
	  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	  	  {
	  		  osDelay(10);
	  		  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	  		  {
	  //模拟计数信号量: 获取任务通知 ,没获取到则不等待
	  take_num = ulTaskNotifyTake(pdFALSE,0);
	  if(take_num > 0)printf("Now cars: %ld\r\n", take_num-1);
	  		  }
			  while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET); //检测松开

	  	  }

	  vTaskDelayUntil(&xLastWakeTime,1000); //绝对延时10s
  }
  /* USER CODE END LedTaskCo */
}

/* USER CODE BEGIN Header_KeyTask */
/**
* @brief Function implementing the KEY thread.
* @note Task3 -> 按键按下(模拟点火): V1.0.7
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyTask */
void KeyTask(void const * argument)
{
  /* USER CODE BEGIN KeyTask */
	BaseType_t xReturn;
  /* Infinite loop */
  for(;;)
  {
	  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	  {
		  osDelay(10);
		  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
		  {
			  osDelay(500);
			  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
			  { //长按停车
					  xReturn = xTaskNotifyGive(LED_CoHandle);
					  if( xReturn == pdTRUE )
					  printf("Receive1_Task_Handle Send!\r\n");
			  }
			  while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET); //检测松开

		  }
//		  while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET); //检测松开
	  }
    osDelay(1);
  }
  /* USER CODE END KeyTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

