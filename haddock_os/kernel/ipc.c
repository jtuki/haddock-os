/**
 * ipc.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "kernel_config.h"

#include "lib/assert.h"
#include "lib/mem_pool.h"
#include "lib/ringbuffer.h"

#include "hal/hal_mcu.h"
#include "sys_signal_defs.h"
#include "ipc.h"

/**
 * Return OS_TRUE if only one bit is set in @sig.
 */
static os_boolean is_signal(signal_t sig)
{
    for (os_size_t i=0; i < 32; i++) {
        if (sig == (((os_uint32)1)<<(i)))
            return OS_TRUE;
    }
    return OS_FALSE;
}

/**
 * \sa macro os_ipc_set_signal
 */
void __haddock_ipc_set_signal(struct process *proc, signal_t sig)
{
    haddock_assert(proc);
    haddock_assert(is_signal(sig));
    
    HDK_CRITICAL_STATEMENTS(proc->signal |= sig);
}

/**
 * Only the process itself can clear a triggered signal.
 * \sa macro os_ipc_clear_signal
 */
void __haddock_ipc_clear_signal(struct process *this, signal_t sig)
{
    haddock_assert(this);
    haddock_assert(is_signal(sig));

    HDK_CRITICAL_STATEMENTS(this->signal &= ~sig);
}

