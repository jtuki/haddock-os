/**
 * hal_stm32f103.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef HAL_STM32F103_H_
#define HAL_STM32F103_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

typedef unsigned int interrupt_state_t;

/**
 * todo
 */
#define HDK_ENTER_CRITICAL_SECTION(s) {(void)s;}
#define HDK_EXIT_CRITICAL_SECTION(s) {(void)s;}

#define HDK_CRITICAL_STATEMENTS(statements) \
    do {interrupt_state_t _state; HDK_ENTER_CRITICAL_SECTION(_state); \
        statements; HDK_EXIT_CRITICAL_SECTION(_state);} while (0)

#define haddock_hal_init() \
    __haddock_hal_stm32f103_init()

#define haddock_hal_sleep(delta_ms) \
    __haddock_hal_stm32f103_sleep(delta_ms)

#define haddock_send_to_host(buf, len)  \
    __haddock_send_to_host_stm3f103(buf, len)

#define haddock_recv_from_host_callback(buf, len) \
    __haddock_recv_from_host_callback_stm32f103(buf, len)

#define systick_Get() __systick_Get_stm32f103()

extern void __haddock_hal_stm32f103_init(void);
extern void __haddock_hal_stm32f103_sleep(os_uint16 delta_ms);
extern void __haddock_send_to_host_stm3f103(const os_uint8 buf[], os_uint16 len);
extern os_uint16 __haddock_recv_from_host_callback_stm32f103(os_uint8 buf[], os_uint16 len);
extern os_uint32 __systick_Get_stm32f103(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_STM32F103_H_ */
