todo

1. 在 `hdk_user_config.h` 里可以配置目标运行时，以及应用程序入口函数。

2. 在 `hdk_user_config.h` 里，如果选择了 `HAL_TARGET_PC` 以及 `HDK_USER_CFG_MAIN_EXAMPLE_APP` 那么默认编译后是在 PC 环境下（windows MinGW / *nix）的一个例子程序，简单的 ping-pong print。

3. 在 `kernel/kernel_config.h` 里可以对系统资源进行裁剪。

4. 可以通过 `lib/simple_log.h` 里的 `print_log()` 函数进行日志输出。比如上面的 ping-pong 程序就会大致输出如下。      
  ```
  0.000		*: Welcome to emaple app!
  0.000		 : pong: wakeup
  0.000		*: ping: inited
  0.000		 : pong: sleep
  0.000		 : pong: wakeup
  0.000		*: pong: inited, wait for ping.
  0.000		 : pong: sleep
  2.000		 : pong: wakeup
  2.000		 : pong: sleep
  2.000		 : pong: wakeup
  ... (omitted)
  12.000		 : pong: sleep
  14.000		 : pong: wakeup
  14.000		 : pong: sleep
  14.000		 : pong: wakeup
  14.000		*: ping: pong received
  14.000		 : pong: sleep
  16.000		 : pong: wakeup
  16.000		 : pong: sleep
  ```

5. MCU 外部通信支持。默认情况下，MCU 和外部的通信，都采用 network endianness，即大端模式。Haddock 提供 utility 函数来供 ntoh / hton 转换。请参考 `lib/hdk_utilities.c` 文件：
  ```c
  void hdk_init_platform_endian(void);
  os_uint32 os_hton_u32(os_uint32 i);
  os_uint16 os_hton_u16(os_uint16 i);
  os_uint32 os_ntoh_u32(os_uint32 i);
  os_uint16 os_ntoh_u16(os_uint16 i);
  ```

6. todo
