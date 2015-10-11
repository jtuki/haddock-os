# Haddock-OS
A lightweight, energy-awareness, non-preemptive, event-driven, easy-to-use embedded OS for micro processors.

#### 轻量级 - 非抢占 - 事件驱动

对小型的 MCU 处理器来说，任何无必要的 RAM/ROM 开销都要尽量避免。      
Haddock-OS 里，所有任务都是事件驱动，任务间通过 IPC signal 相互通信，协同工作。     
没有 preemptive system 里任务切换带来的开销，也不需要对共享资源加锁（*note*: ISR 中断服务处理程序要求尽量高效简洁，且不允许修改共享资源）。    

#### 细粒度能量管理

* 细粒度能量管理（模块级）：    
每个任务都可以单独设定休眠唤醒的回调函数。

* 整体调度（系统级）：    
除了任务级别的休眠唤醒之外，OS scheduler 会根据目前系统（全部任务）的状态（休眠中 / 有 pending signals 待处理 / 下一个 timer timeout 事件即将到达）来选择是否将 CPU 休眠。

#### 简单易用

* 移植方便：    
pure C 实现，没有汇编，方便采取 IAR / Keil 等集成开发环境。理论上，只需要完成硬件抽象层的 timer system 和 UART 即可。   
（*note*: 我对移植不在行，仅限于通过 datasheet 掌握相关参数的程度；还望驱动大神们体验补充，一起完善。）   
Haddock-OS 整体实现也比较轻巧简洁，容易理解。   

* 使用简单：
虽然不是开箱即用，但也比较简单。    
用户定义任务，定义任务 signal，init function，entry point，再按照系统核心组件（memory pool，timer，IPC）的使用接口，就可以开始 MCU lightweight multi-tasking 之旅了。   

#### 资源可裁剪   

由于 MCU 的 RAM/ROM 资源很宝贵，因此 Haddock-OS 可裁剪资源以满足不同系统的需求。    
最大任务数，最大任务优先级数量，最大 timer 数量，可分配的内存池资源，都可以配置。

#### 商用友好

Commercial Use Friendly: [Mozilla Public License, version 2.0](https://www.mozilla.org/en-US/MPL/2.0/)

#### Documents

1. [Tutorial](readme/tutorial.md) 
2. [API reference](readme/api/haddock_api.md)
3. [Port Howto](readme/port.md)
