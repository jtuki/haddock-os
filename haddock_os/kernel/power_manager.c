/**
 * power_manager.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "kernel/kernel_config.h"
#include "kernel/timer.h"
#include "kernel/process.h"
#include "lib/assert.h"
#include "hal/hal_mcu.h"

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE

/**
 * When @__allow_power_conserve is 0, the power conservation is allowed.
 */
static haddock_allow_power_conserve_bv_t __allow_power_conserve_bv;

power_manager_status_t haddock_power_status;

void haddock_power_manager_module_init(void)
{
    // we enable power conservation by default.
    __allow_power_conserve_bv = 0;
}

/**
 * \sa The main loop in \file kernel_main.c
 * \sa haddock_check_next_timeout()
 * \sa HDK_CFG_POWER_SAVING_NEXT_TIMEOUT_THRESHOLD
 */
void haddock_power_conserve_routine(void)
{
    static struct time *next_timeout;
    static os_uint32 next_timeout_ms;
    static interrupt_state_t _state;
#if defined HADDOCK_DEBUG_OS_POWER_MANAGER && (HADDOCK_DEBUG_OS_POWER_MANAGER == OS_TRUE)
    static struct timer *_next_timer;
    static struct timer *_next_atimer;
#endif

    if (__allow_power_conserve_bv != 0
        || haddock_power_status == POWER_STATUS_READY_TO_RUN_NEXT
        || haddock_power_status == POWER_STATUS_ISR_SET_SIGNAL)
        goto __power_conserve_not_sleep;
    else {
        next_timeout = haddock_check_next_timeout();
#if defined HADDOCK_DEBUG_OS_POWER_MANAGER && (HADDOCK_DEBUG_OS_POWER_MANAGER == OS_TRUE)
        _next_timer  = haddock_get_next_timer();
        _next_atimer = haddock_get_next_atimer();
#endif
        if (!next_timeout ||
            next_timeout->s >= 1 ||
            ((next_timeout->s == 0)
            && next_timeout->ms > HDK_CFG_POWER_SAVING_NEXT_TIMEOUT_THRESHOLD))
        {
            HDK_ENTER_CRITICAL_SECTION(_state);
            if (haddock_power_status == POWER_STATUS_RUNNING) {
                // not POWER_STATUS_ISR_SET_SIGNAL
                haddock_power_status = POWER_STATUS_READY_TO_SLEEP;
            }
            HDK_EXIT_CRITICAL_SECTION(_state);
            goto __power_conserve_sleep;
        } else {
            haddock_power_status = POWER_STATUS_READY_TO_RUN_NEXT;
            goto __power_conserve_not_sleep;
        }
    }
    
__power_conserve_sleep:
    if (!next_timeout) {
        next_timeout_ms = HDK_CFG_POWER_SAVING_SLEEP_MAX_MS;
    } else {
        next_timeout_ms = 1000*next_timeout->s + next_timeout->ms;
        next_timeout_ms = next_timeout_ms > HDK_CFG_POWER_SAVING_SLEEP_MAX_MS ? 
                          HDK_CFG_POWER_SAVING_SLEEP_MAX_MS : next_timeout_ms;
    }
    if (haddock_power_status != POWER_STATUS_READY_TO_SLEEP) {
        // POWER_STATUS_ISR_SET_SIGNAL
        goto __power_conserve_not_sleep;
    } else {
        haddock_hal_sleep((os_uint16) next_timeout_ms); // STOP here when sleeping.
    }
    
    // has woken up from haddock_hal_sleep()
    haddock_power_status = POWER_STATUS_RUNNING;
    return;
    
__power_conserve_not_sleep:
    return;
}

#endif // HDK_CFG_POWER_SAVING_ENABLE

/**
 * Return 0 if successful, else return a specific negative value as the
 * process's @sleep_cleanup_callback.
 */
os_int8 haddock_enable_power_conserve(struct process *proc)
{
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
    haddock_assert(proc && proc->_pid >= 1
                   && proc->_pid <= HDK_CFG_PROC_MAX_NUM);
    
    os_int8 ret = 0;
    if (proc->is_wakeup == OS_FALSE) // no need to sleep again
        return 0;
    
    if (proc->sleep_cleanup_callback) {
        if ((ret = proc->sleep_cleanup_callback()) == 0) {
            proc->is_wakeup = OS_FALSE;
            __allow_power_conserve_bv &= ~(((os_uint32)1)<<(proc->_pid - 1));
        }
    } else {
        /*
         * If the callback function is not set, we will assume that
         * the process knows what itself is doing, and set the global
         * power conservation bit.
         */ 
        __allow_power_conserve_bv &= ~(((os_uint32)1)<<(proc->_pid -1));
    }
    
    return ret;
#else
    (void) proc;
    return 0;
#endif
}

/**
 * \sa proc_wakeup_init_t
 */
void haddock_disable_power_conserve(struct process *proc)
{
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
    haddock_assert(proc && proc->_pid >= 1
                   && proc->_pid <= HDK_CFG_PROC_MAX_NUM);
    
    if (proc->is_wakeup == OS_TRUE) // no need to wakeup again
        return;
    
    if (proc->wakeup_init_callback) {
        proc->wakeup_init_callback();
    }
    
    proc->is_wakeup = OS_TRUE;
    __allow_power_conserve_bv |= (((os_uint32)1)<<(proc->_pid - 1));
#else
    (void) proc;
#endif
}
