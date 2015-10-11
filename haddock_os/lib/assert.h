/**
 * assert.h
 *
 * \date   
 * \author jtuki@foxmail.com
 */
#ifndef HADDOCK_ASSERT_H_
#define HADDOCK_ASSERT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "kernel/kernel_config.h"
#include "hdk_user_config.h"
#include "simple_log.h"

#define __should_never_fall_here() haddock_assert(OS_FALSE)

#if defined HDK_USER_CFG_HAL_TARGET && HDK_USER_CFG_HAL_TARGET == HAL_TARGET_PC
#include <stdlib.h>
#define __system_halt() do {exit(-1);} while (0)
#else
// other MCU platforms
#define __system_halt() do {while(1) {;}} while(0)
#endif

#define haddock_assert(condition) do { \
    if (!(condition)) { \
        print_log(LOG_ERROR, "assert fail: %d", __LINE__); \
        __system_halt(); \
    } \
} while (0)

extern os_uint32   __haddock_call_function_line;
extern const char *__haddock_call_function_file;

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H_ */
