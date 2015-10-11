/**
 * hdk_user_config.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef HDK_USER_CONFIG_H_
#define HDK_USER_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"

/**
 * Specify the main() function.
 * \note _ONLY_ one is available for main(). Comment all the others.
 */
// #define HDK_USER_CFG_MAIN_PC_TEST       OS_TRUE
#define HDK_USER_CFG_MAIN_EXAMPLE_APP  OS_TRUE

/*---------------------------------------------------------------------------*/
#define HAL_TARGET_PC               0
#define HAL_TARGET_STM32F103        1
#define HAL_TARGET_STM32L051        2
// add you new target here ...

/**
 * Specify the HAL (hardware platform)'s target.
 * \note Only one target _should_ be set.
 */
#define HDK_USER_CFG_HAL_TARGET             HAL_TARGET_PC
//#define HDK_USER_CFG_HAL_TARGET             HAL_TARGET_STM32F103
//#define HDK_USER_CFG_HAL_TARGET             HAL_TARGET_STM32L051

/*---------------------------------------------------------------------------*/
#define HOST_IF_TX_BUFFER_LEN       128
#define HOST_IF_RX_BUFFER_LEN       128

#ifdef __cplusplus
}
#endif

#endif /* HDK_USER_CONFIG_H_ */
