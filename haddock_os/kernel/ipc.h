/**
 * ipc.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef IPC_MSG_H_
#define IPC_MSG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"
#include "kernel/kernel_config.h"
#include "kernel/process.h"
#include "kernel/power_manager.h"

/**
 * Only one bit is set in this signal_t.
 * \sa struct process::signal
 * \sa signal_bv_t
 */
typedef os_uint32 signal_t;

/**
 * _Should_ only be called in interrupt service routine.
 */
#define os_interrupt_ipc_set_signal(pid, sig) \
    haddock_power_status = POWER_STATUS_ISR_SET_SIGNAL; \
    __haddock_ipc_set_signal(get_struct_process(pid), sig)

#define os_ipc_set_signal(pid, sig) \
    __haddock_ipc_set_signal(get_struct_process(pid), sig)

#define os_ipc_clear_signal(sig) \
    __haddock_ipc_clear_signal(this, signal_t sig)

void __haddock_ipc_set_signal(struct process *proc, signal_t sig);
void __haddock_ipc_clear_signal(struct process *this, signal_t sig);


#ifdef __cplusplus
}
#endif

#endif /* IPC_MSG_H_ */
