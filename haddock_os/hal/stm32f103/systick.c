/**
 * systick.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "hal_config_stm32f103.h"
#include "hal_stm32f103.h"
#include "haddock_types.h"

#define SYSTICK_INCR_CYCLE      100

/**
 * \return current systick in ms
 * \note We only simulate the systick of MCU here.
 */
os_uint32 __systick_Get_stm32f103(void)
{
    // todo
    static os_uint32 incr_cycle = 0;
    static os_uint32 systick = 0;

    if (incr_cycle++ == SYSTICK_INCR_CYCLE) {
        systick += 1;
        incr_cycle = 0;
    }

    return systick;
}
