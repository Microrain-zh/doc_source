# 1. 目的

​	本篇是介绍在`5_4_Serialport_RingBuffer_GPIO_IRQ`工程基础上创建一个带有RTOS的工程模板，使用的工具是STM32CubeMX.

# 2. 步骤

​	首先将`5_4_Serialport_RingBuffer_GPIO_IRQ`复制改名为`6_FreeRTOS_Template`，随后打开里面的STM32CubeMX配置文件`F103_Moduel.ioc`，打开这个文件有可能会因为STM32CubeMX版本不一致弹出如下信息：

| ![](pic\1_version_migrate.png) |
| :----------------------------: |

选择`Migrate`。

## 2.1 选择CMSIS的RTOS接口版本

​	`CMSIS`封装的RTOS接口有两个版本`RTOS V1`和`RTOS V2`，V2兼容V1，支持更多的cortex内核，我们的课程使用的就是V2版本的接口。

​	在STM32CubeMX配置界面的`Middleware`栏选择***FREERTOS***，在`Interface`那里选择`CMSIS_V2`，其它的参数我们暂且使用默认值，如图所示：

| <img src="pic\2_rtos_v2.png" style="zoom:67%;" /> |
| :-----------------------------------------------: |

## 2.2 改变HAL库使用的时基源

​	首先说明下为什么要改变HAL库使用的时基源。在默认情况下，ST的HAL库使用的时钟基准输入源也就是时基源，利用的是内核的滴答定时器，如果没有使用RTOS的话没有什么问题；但是如果要使用一个RTOS比如FreeRTOS或者RT-Thread，这些RTOS的内核时钟通常也是利用的内核的滴答定时器，为了不影响内核的运行，HAL库的时基源就最好换一个，比如我们这里就选择STM32F103的定时器TIM8作为HAL库的时基源：

| <img src="pic\3_timebase_src.png" style="zoom:67%;" /> |
| :----------------------------------------------------: |

在实际设计中，选择哪一个定时器作为HAL库的时基源应该需要谨慎考虑，为了不要互相影响，最好不要选择与控制其它外设的定时器相同的定时器，比如我需要使用TIM3来输出一个PWM波，那么这里就最好不要再选择TIM3作为HAL库的时基源。

## 2.3 确认IDE的版本

​	在生成工程前，我们提供的cubemx选择的IDE版本可能和自己实际使用的IDE版本不同，因而就需要注意一下IDE的版本问题。比如这个cubemx配置工程我们一开始选择的是MDK，版本是`5.27`，但是电脑上使用的MDK实际已经更新到更高等级的版本了，那么这里最好也要改变：

| <img src="pic\4_ide_version.png" style="zoom:67%;" /> |
| :---------------------------------------------------: |

## 2.4 生成工程

​	经过前面3步的配置后，就可以点击`Generate Code`生成工程了。

## 2.5 MDK工程勾选MicroLIB

​	因为我们是在`5_4_Serialport_RingBuffer_GPIO_IRQ`基础上修改的，源工程的MDK是需要勾选MicroLIB来支撑运行的，而使用STM32CubeMX配置文件改变配置重新生成后的工程默认是不会勾选此项的，所以我们需要打开重新生成的工程，把这里勾选上：

| <img src="pic\5_use_microlib.png" style="zoom:67%;" /> |
| :----------------------------------------------------: |

## 2.6 简单修改freertos.c

​	在STM32CubeMX中选择好RTOS的接口版本后，默认参数中是有一个默认的任务的：

| <img src="pic\6_default_task.png" style="zoom:67%;" /> |
| :----------------------------------------------------: |

为了验证我们配置的RTOS是否能够正常运行，我们可以在freertos.c中添加测试打印(5_4工程已经实现了printf功能)：

|   <img src="pic\7_include_stdio.png" style="zoom:67%;" />    |
| :----------------------------------------------------------: |
| <img src="pic\8_start_default_task.png" style="zoom:67%;" /> |

然后我们编译工程，烧写到开发板上看下结果：

| ![](pic\9_result.png) |
| :-------------------: |

可以看到能够正常打印，说明RTOS内核已经运行起来了，这样就得到了我们的FreeRTOS的工程模板。