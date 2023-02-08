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

