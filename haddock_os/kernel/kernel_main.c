/**
 * kernel_main.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "hal/hal_mcu.h"

#include "kernel/kernel_config.h"
#include "kernel/process.h"
#include "kernel/timer.h"
#include "kernel/ipc.h"
#include "kernel/hdk_memory.h"
#include "kernel/scheduler.h"
#include "kernel/power_manager.h"

#include "lib/assert.h"

#include "os_user_processes/os_user_processes.h"

#if defined (HDK_USER_CFG_HAL_TARGET) && HDK_USER_CFG_HAL_TARGET == HAL_TARGET_PC
#include <stdlib.h>
#endif

#if defined HDK_DEBUG_GENERAL_INFO_ENABLE && HDK_DEBUG_GENERAL_INFO_ENABLE == OS_TRUE
os_uint32 __kernel_main_loop_counter = 0;
struct time __kernel_current_timetick;

#define __increment_main_loop_counter() {__kernel_main_loop_counter += 1;}
#define __get_kernel_timetick_now() {haddock_get_time_tick_now(&__kernel_current_timetick);}
#else
#define __increment_main_loop_counter()
#define __get_kernel_timetick_now()
#endif

int main ()
{
    hdk_init_platform_endian();

    // hardware related initialization.
    haddock_hal_init();
    
    // kernel resource initialization.
    haddock_process_module_init();
    haddock_timer_module_init();

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
    haddock_power_manager_module_init();
#endif
    
    os_user_processes_init();
    
    static struct process *proc = NULL;
    /**
     * \remark We use @_signal to backup the process's signal_bv_t before the
     *         execution of @proc->entry to avoid the race conditions between 
     *         interrupts (who may want to set_signal) and @proc->entry (who
     *         may want to clear_signal).
     */
    signal_bv_t _signal = 0;
    interrupt_state_t _state;
    
    while (OS_TRUE) {
        __increment_main_loop_counter();
        
        __get_kernel_timetick_now(); // also updated @haddock_time_tick_now
        haddock_timer_update_routine();

        proc = schedule_next();
        if (proc) {
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
            haddock_power_status = POWER_STATUS_RUNNING;
#endif
            
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
            if (proc->is_wakeup == OS_FALSE) {
                haddock_disable_power_conserve(proc);
            }
#endif

            HDK_ENTER_CRITICAL_SECTION(_state);
            _signal = proc->signal;
            proc->signal = 0;
            HDK_EXIT_CRITICAL_SECTION(_state);

            haddock_assert(proc->entry);
            cur_process_id = proc->_pid;
            
            _signal = proc->entry(proc->_pid, _signal);
            
            HDK_ENTER_CRITICAL_SECTION(_state);
            proc->signal |= _signal;
            HDK_EXIT_CRITICAL_SECTION(_state);
        } else {
#if defined (HDK_USER_CFG_HAL_TARGET) && HDK_USER_CFG_HAL_TARGET == HAL_TARGET_PC
            if (os_no_more_timers()) { // no more timers.
                exit(0);
            }
#endif

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
            haddock_power_conserve_routine();
#endif
        }
    }
    
    return 0;
}
