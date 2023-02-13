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
osTimerId myTimer01Handle;
osTimerId myTimer02Handle;
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
void Callback01(void const * argument);
void Callback02(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

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

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

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

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer02 */
  osTimerDef(myTimer02, Callback02);
  myTimer02Handle = osTimerCreate(osTimer(myTimer02), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  xTimerChangePeriod(myTimer01Handle, 500, 200); // 修改定时器1周期：500ms, 发送队列等待时间200ms（任填）
  xTimerChangePeriod(myTimer02Handle, 1000, 200);
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
* @note Task1 -> 任务通知.模拟消息队列.接收消息:  V1.0.11
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTask */
void LedTask(void const * argument)
{
  /* USER CODE BEGIN LedTask */
	BaseType_t xReturn;
	uint32_t r_addr; /* 定义一个事件接收变量 */
  /* Infinite loop */
  for(;;)
  {

	  //获取任务通知 ,没获取到则一直等待
	  xReturn = xTaskNotifyWait(0x0, //进入函数的时候不清除任务
			   0xffffffff, //退出函数的时候清除所有的bitR
			   &r_addr, //保存任务通知值
			   0); //阻塞时间, 无限阻塞时需要考虑任务优先级问题

	  if( pdTRUE == xReturn )
	  {
		  printf ( "Receive Task: %s\r\n", (uint8_t *)r_addr); // 邮箱消息
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	  }
	  vTaskDelay(20);

  }
  /* USER CODE END LedTask */
}

/* USER CODE BEGIN Header_LedTaskCo */
/**
* @brief Function implementing the LED_Co thread.
* @note Task2 -> 任务通知.模拟事件组.倒计时:  V1.0.10
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LedTaskCo */
void LedTaskCo(void const * argument)
{
  /* USER CODE BEGIN LedTaskCo */
	uint32_t n=10;
	TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {

	  n--;
	  if(n==1){
		  printf("Task2 Ready...\r\n");
		  xTaskNotify(LEDHandle, 0x0001, eSetBits); /* 触发一个事件 1 */
		  n=10;
	  }
	  else printf("CountDown: %ld\r\n", n);

	  vTaskDelayUntil(&xLastWakeTime,100000); //绝对延时10s
  }
  /* USER CODE END LedTaskCo */
}

/* USER CODE BEGIN Header_KeyTask */
/**
* @brief Function implementing the KEY thread.
* @note Task3 -> 定时器.控制定时器开关: V1.0.12
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KeyTask */
void KeyTask(void const * argument)
{
  /* USER CODE BEGIN KeyTask */
	static BaseType_t xPress = pdFALSE;
  /* Infinite loop */
  for(;;)
  {
	  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
	  {
		  osDelay(10);
		  if(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
		  {
			  if(xPress == pdFALSE)
			  {
				 xTimerStart(myTimer01Handle, 100); // 启动定时器1: 发送队列超时100ms
				 xPress = pdTRUE;
			  }
			  else{
				  xTimerStop(myTimer01Handle, 100); // 关闭定时器1: 发送队列超时100ms
				  xTimerStop(myTimer02Handle, 100); // 关闭定时器2
				  printf("Timers Stop!\r\n");
				  xPress = pdFALSE;
			  }

		  }
		  while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET); //检测松开
	  }
    osDelay(1);
  }
  /* USER CODE END KeyTask */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */
	printf("Timer1\r\n");
  /* USER CODE END Callback01 */
}

/* Callback02 function */
void Callback02(void const * argument)
{
  /* USER CODE BEGIN Callback02 */
	printf("Timer2\r\n");
  /* USER CODE END Callback02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

