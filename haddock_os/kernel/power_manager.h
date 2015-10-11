/**
 * power_manager.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef POWER_MANAGER_H_
#define POWER_MANAGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "kernel/process.h"

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE

typedef enum {
    POWER_STATUS_RUNNING = 0,
    /** The interrupts can only change @haddock_power_status to
     *  POWER_STATUS_ISR_SET_SIGNAL. */
    POWER_STATUS_ISR_SET_SIGNAL,
    /**< about to run next process.
     * \sa HDK_CFG_POWER_SAVING_NEXT_TIMEOUT_THRESHOLD */
    POWER_STATUS_READY_TO_RUN_NEXT,
    POWER_STATUS_READY_TO_SLEEP,
    POWER_STATUS_SLEEPING,
} power_manager_status_t;

extern power_manager_status_t haddock_power_status;

/** \sa HDK_CFG_PROC_MAX_NUM */
typedef os_uint32 haddock_allow_power_conserve_bv_t;

void haddock_power_manager_module_init(void);
void haddock_power_conserve_routine(void);

#endif // HDK_CFG_POWER_SAVING_ENABLE

/** The processes can use these interfaces to sleep and wakeup.
 *  When the process sleep, the @process::sleep_cleanup_callback will be evoked.
 *  Also, the @process::wakeup_init_callback will be evoked when wakeup.
 *  
 *  \sa struct process::is_wakeup
 *  @{ 
 */
#define process_sleep() \
    haddock_enable_power_conserve(this)

#define process_wakeup() \
    haddock_disable_power_conserve(this)
/** @} */

os_int8 haddock_enable_power_conserve(struct process *proc);
void haddock_disable_power_conserve(struct process *proc);


#ifdef __cplusplus
}
#endif

#endif /* POWER_MANAGER_H_ */
