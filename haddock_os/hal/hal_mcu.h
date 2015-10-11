/**
 * hal_mcu.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef HAL_MCU_H_
#define HAL_MCU_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "hdk_user_config.h"

/**
 * Each HAL (hardware abstraction layer) implementation should implement:
 *
 * \note critical section related
 * #define HDK_ENTER_CRITICAL_SECTION(s)
 * #define HDK_ENTER_CRITICAL_SECTION(s)
 * #define HDK_CRITICAL_STATEMENTS(statements)
 *
 * void haddock_hal_init(void);                 // init HAL
 * void haddock_hal_sleep(uint16 sleep_ms);     // sleep @sleep_ms ms
 *
 * os_uint32 systick_Get(void);                 // get current systick
 *
 * \note As to MCU, the interaction interface with host is commonly UART.
 * void haddock_send_to_host(const os_uint8 buf[], os_uint16 len);
 * os_uint16 haddock_recv_from_host_callback(os_uint8 buf[], os_uint16 len);
 * 
 * \sa pc_test/hal_pc_test.h
 */

#if defined HDK_USER_CFG_HAL_TARGET && HDK_USER_CFG_HAL_TARGET == HAL_TARGET_PC
#include "pc_test/hal_pc_test.h"
#elif defined HDK_USER_CFG_HAL_TARGET && HDK_USER_CFG_HAL_TARGET == HAL_TARGET_STM32F103
#include "stm32f103/hal_stm32f103.h"
#endif


#ifdef __cplusplus
}
#endif

#endif /* HAL_MCU_H_ */
