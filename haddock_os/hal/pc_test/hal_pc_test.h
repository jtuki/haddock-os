/**
 * hal_pc_test.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef HAL_PC_TEST_H_
#define HAL_PC_TEST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

typedef unsigned int interrupt_state_t;

#define HDK_ENTER_CRITICAL_SECTION(s) {(void)s;}
#define HDK_EXIT_CRITICAL_SECTION(s) {(void)s;}

#define HDK_CRITICAL_STATEMENTS(statements) \
    do {interrupt_state_t _state; HDK_ENTER_CRITICAL_SECTION(_state); \
        statements; HDK_EXIT_CRITICAL_SECTION(_state);} while (0)

#define haddock_hal_init() \
    __haddock_hal_pc_test_init()

#define haddock_hal_sleep(delta_ms) \
    __haddock_hal_pc_test_sleep(delta_ms)

#define haddock_send_to_host(buf, len)  \
    __haddock_send_to_host_pc(buf, len)

#define haddock_recv_from_host_callback(buf, len) \
    __haddock_recv_from_host_callback_pc(buf, len)

#define systick_Get() __systick_Get_pc()

extern void __haddock_hal_pc_test_init(void);
extern void __haddock_hal_pc_test_sleep(os_uint16 delta_ms);
extern void __haddock_send_to_host_pc(const os_uint8 buf[], os_uint16 len);
extern os_uint16 __haddock_recv_from_host_callback_pc(os_uint8 buf[], os_uint16 len);
extern os_uint32 __systick_Get_pc(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_PC_TEST_H_ */
