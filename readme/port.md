1. 移植到其他目标平台主要是实现如下接口。具体请见 `hal/hal_mcu.h` 文件。
  ```c
  #define HDK_ENTER_CRITICAL_SECTION(s)
  #define HDK_ENTER_CRITICAL_SECTION(s)
  #define HDK_CRITICAL_STATEMENTS(statements)
  
  void haddock_hal_init(void);
  void haddock_hal_sleep(uint16 sleep_ms);
  
  os_uint32 systick_Get(void);
  
  void haddock_send_to_host(const os_uint8 buf[], os_uint16 len);
  os_uint16 haddock_recv_from_host_callback(os_uint8 buf[], os_uint16 len);
  ```

2. todo

P.S.
我对移植并不在行，因此请行家参与，contribute together to make haddock better。
