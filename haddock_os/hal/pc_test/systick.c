/**
 * systick.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "hal_config_pc_test.h"
#include "hal_pc_test.h"
#include "haddock_types.h"

#define SYSTICK_INCR_CYCLE      100

/**
 * \return current systick in ms
 * \note We only simulate the systick of MCU here.
 */
os_uint32 __systick_Get_pc(void)
{
    static os_uint32 incr_cycle = 0;
    static os_uint32 systick = 0;

    if (incr_cycle++ == SYSTICK_INCR_CYCLE) {
        systick += 1;
        incr_cycle = 0;
    }

    return systick;
}
