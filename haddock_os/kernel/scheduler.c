/**
 * scheduler.c
 *
 * \date   
 * \author jtuki@foxmail.com
 */

#include "kernel/process.h"
#include "kernel/scheduler.h"

#if (defined HDK_CFG_DEFAULT_SCHEDULER) && HDK_CFG_DEFAULT_SCHEDULER == OS_TRUE

/**
 * Haddock's default scheduler.
 * If you want to implement your own scheduler, MAKE SURE you understand this
 * one first.
 * 
 * @return The next struct process to be scheduled.
 */
struct process *schedule_next(void)
{
    /* iterator to go through \global process_list_table[] */
    os_uint8 i = 0;
    
    /* iterator to go through each process_list_table[i] */
    struct list_head *proc_hdr = NULL;
    
    for (; i < HDK_CFG_PROC_PRIORITY_NUM; i++) {
        list_for_each (proc_hdr, & process_list_table[i].list) {
            if (process_is_ready_hdr(proc_hdr))
                return list_entry(proc_hdr, struct process, hdr);
        }
    }
    
    /* currently no process is ready. */
    return NULL;
}

#endif // end of HDK_CFG_DEFAULT_SCHEDULER
