# 基于 STM32CubeMX + STM32CubeIDE 的 FreeRTOS 移植模版

> 记录 FreeRTOS 学习日志及备份项目工程模版


## 开发环境

+ FreeRTOS: V10.0
+ IDE: STM32CUBEIDE
+ MCU: STM32F103C8T6
+ 工程模版: STM32CubeMX 生成 FreeRTOS 工程

## 功能说明

两个Task任务，实现点亮频闪LED灯～


## 移植说明

> 已经移植上机测试过了，直接编译运行即可。  
> 本文仅记录一下关键步骤，详细移植可以看  
> [使用STM32CubeIDE 进行 FreeRTOS 手动移植F103系列最小系统](https://suroy.cn/embeded/use-stmcubeide-for-freertos-manual-migration-of-f103-series-minimum-system.html)  
> [记基于STM32 的 FreeRTOS 学习的移植日志](https://suroy.cn/embeded/record-the-migration-log-of-freertos-learning-based-on-stm32.html)

## 版本日志

### V1.0.0 2023.2.8

+ Task1、Task2同优先级模拟延时测试
  + 测试相对延时和绝对延时效果  
  + 已经配置 printf 串口重定向
  + 配置 PC13 LED闪烁


### V1.0.1 2023.2.9

+ 按键控制任务创建/删除 - 短按
+ 按键控制任务挂起/恢复 - 长按
+ 按键引脚 PA0
+ 任务中打印输出传入参数

注意： 
+ 检测按键输入的时候一般采用上拉；延时消抖动实测取10ms更佳
+ 本例使用的是同一个板载按键实现的长按/短按功能，不建议这样使用，可能会出现异常。


#### 已知问题： 任务2输出按以下方式会卡在任务2中，导致任务1无法执行或者仅执行第一次

```c
// 任务2
void LedTaskCo(void const * argument)
{
  /* USER CODE BEGIN LedTaskCo */
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  printf(argument);
	  printf("Task2: "); //不要这句，才可以跑
	  printf("\r\n");
	  osDelay(200); // 模拟任务占用时间
	  vTaskDelayUntil(&xLastWakeTime,500); //绝对延时500ms
  }
  /* USER CODE END LedTaskCo */
}
```

玄学，去掉第二个printf可以运行；若改用完全一致的程序效果正常。
问题应该是出在两个任务调用的功能、所耗时间不同导致时间片轮转时产生的task1的printf没有来得及输出所致。

解决： `printf("Task2: %s\n\n", (char *)argument); //传入NULL时会大量输出乱码`

正解： 在任务中增加互斥量进行 printf 函数的控制


### V1.0.2 2023.2.10

+ 队列的基本使用
  + 消息队列(使用CubeMX创建)
  + 按键发送任务: 设置等待时间为0
  + 一个接收任务: 等待时间为最大(portMAX_DELAY)

+ 修复BUG
  + 修改之前版本的按键检测为LED


### V1.0.3 2023.2.10

创建信号量: STM32CubeMX创建默认为1; 手动创建为0

1. FreeRTOS -> Timers and Semaphores -> Binary Semaphores

2. Add

手动创建:

```c
SemaphoreHandle_t BinarySem_Handle =NULL;
/* 创建 BinarySem */
BinarySem_Handle = xSemaphoreCreateBinary();
```

+ 二值信号量
  + 上机会自动Take 1次信号，由于CubeMX初始化二值信号量为最大值1
  + 按键按下Give信号
  + Task2定时1s负责Take信号


### V1.0.4 2023.2.10


+ 修复BUG
  + V1.0.3 手动创建二值信号量的错误

+ 计数信号量

> 模拟车库停车

创建信号量: STM32CubeMX创建默认为最大值; 手动创建为0
1. FreeRTOS -> Config Parameters -> USE_COUNTING_SEMAPHORES [enable]
2. FreeRTOS -> Timers and Semaphores -> Counting Semaphores
3. Add


或手动创建: 

```c
SemaphoreHandle_t CountSem_Handle =NULL;
/* 创建 CountSem */
CountSem_Handle = xSemaphoreCreateCounting(5,5); // MAX_Value, INIT_Value
```

+ 任务: 创建计数信号量大小为2 
  + Task3: (按键)可以将车停入车库/开出车库
  + Task2: 串口每1秒显示一下车库内车辆数据

### V1.0.5 2023.2.11

+ 互斥量引入

+ 任务: 模拟优先级翻转
  > Task1、3所访问的为同一二值信号量
  + Task1: 高优先级（二值信号量）
  + Task2: 中优先级（模拟任务干扰）
  + Task3: 低优先级 (二值信号量)

+ 结果:
  当上电**开机**之后，**系统会调用一次最高优先级的任务**，是由于CubeMX在生成二值信号量时给的初值为1(即已经释放二值信号量)，此时任务1可以获取该信号量(同时获取之后信号量减1为0)；**之后的Task1高优先级任务均会等待 Task2 和 Task3 执行之后才会从阻塞进入就绪到运行状态**。(PS: 可以阅读博文详细分析)


### V1.0.6 2023.2.11

+ 任务: 互斥量的优先级继承
  > Task1、3所访问的为同一互斥量，修改自V1.0.5
  + Task1: 高优先级（二值信号量）
  + Task2: 中优先级（模拟任务干扰）
  + Task3: 低优先级 (二值信号量)

实验结果
    当上电**开机**之后，**系统会依次从高到低调用各优先级的任务**，是由于CubeMX在生成互斥量时给的初值为1(即已经释放互斥量)。最后从此**Task1高优先级任务均会阻塞等待互斥量释放，同时将 低优先级任务Task3提到同级(即高优先级)，待Task3执行之后才会从阻塞进入就绪到运行状态，方运行Task2 中优先级任务**。(PS: 可以看博文解析)


### V1.0.7 2023.2.11

+ 任务: 事件组模拟火箭发射倒计时点火
  > 手动创建事件组
  + Task1: 实时检测点火
  + Task2: 模拟倒计时10秒
  + Task3: 按键确认点火
  + 成功点火: 倒计时完成 + 按键确认

+ 实验结果
    任务2倒数10秒，当任务3按键按下后，且倒计时1秒时🚀发射。