/**
 * process.h
 *
 * \date    
 * \author  jtuki@foxmail.com
 */

#ifndef HADDOCK_PROCESS_H_
#define HADDOCK_PROCESS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"
#include "lib/linked_list.h"
#include "lib/hdk_utilities.h"
#include "kernel/kernel_config.h"

/**
 * The process id. (at most 255 processes can be created)
 * 0 is reserved.
 */
typedef os_uint8 os_pid_t;
#define PROCESS_ID_RESERVED     0

typedef os_uint32 signal_bv_t; // signal bit-vector type

/**
 * Each process has a "process_func_t" function as entry.
 * 
 * \remark @pid will not be used in the process function actually. We need to
 *         make sure that haddock_assert(this->_pid == pid).
 */
typedef signal_bv_t (*process_func_t)(os_pid_t pid, signal_bv_t signal);

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
/** A process _should_ make sure that it could wake up when the system
 *  alarm it. */
typedef void (*proc_wakeup_init_t)(void);
/** Return 0 if successfully perform the sleep-cleanup.
 *  Else return -1 (or other negative value defined in each process). */
typedef os_int8 (*proc_sleep_cleanup_t)(void);
#endif // HDK_CFG_POWER_SAVING_ENABLE

/**
 * The main abstraction for process.
 */
struct process {
    struct list_head hdr;
    os_pid_t _pid;      /**< range between [1, HDK_CFG_PROC_MAX_NUM] */
    os_uint8 priority;   /**< range between [0, HDK_CFG_PROC_PRIORITY_NUM).
                           The higher @priority value, the nicer the process is
                           (which means lower priority). */
    process_func_t entry;
#if (defined HDK_CFG_PROC_WITH_NAME) && HDK_CFG_PROC_WITH_NAME == OS_TRUE
    const char *name;
#endif
    /**
     * Bit-vectored value. 0x80000000 is reserved for IPC message.
     * All other 31 bits are for signal(s).
     * 
     * \note signal_t
     */
    signal_bv_t signal;
    
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
    proc_wakeup_init_t    wakeup_init_callback;
    proc_sleep_cleanup_t  sleep_cleanup_callback;
    /**< \sa macro process_sleep() process_wakeup() */
    os_boolean is_wakeup;
#endif
};

/**
 * The process list for struct process(es) with the same priority.
 */
struct process_list
{
    os_uint32 ready; /**< bit-vector */
    struct list_head list;
};

__HADDOCK extern struct process_list process_list_table[HDK_CFG_PROC_PRIORITY_NUM];
__HADDOCK extern os_pid_t cur_process_id;

void haddock_process_module_init(void);

#define haddock_process(proc_name) \
    static const char *_proc_name = proc_name; \
    static struct process *this = NULL

#define process_create(f, priority) \
    __haddock_process_create(& this, _proc_name, (f), (priority), 0)

#define process_destroy(proc_name) \
    haddock_assert(is_equal_string(_proc_name, proc_name)); \
    __haddock_process_destroy(& this)

#define process_is_ready_hdr(p_hdr) \
    ((list_entry((p_hdr), struct process, hdr))->signal > 0)

#define process_is_ready(p_proc) ((p_proc)->signal > 0)

struct process *get_struct_process(os_pid_t pid);

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
#define process_set_wakeup_init_callback(f) \
    __haddock_process_set_wakeup_callback(this, f)
#else
#define process_set_wakeup_init_callback(f)
#endif

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
#define process_set_sleep_cleanup_callback(f) \
    __haddock_process_set_sleep_cleanup_callback(this, f)
#else
#define process_set_sleep_cleanup_callback(f)
#endif

/**< system internal function declarations. */
struct process *__haddock_process_create(struct process **this,
                                         const char *name,
                                         process_func_t f,
                                         os_uint8 priority, 
                                         os_size_t msg_queue_size);
os_int8 __haddock_process_destroy(struct process **this);

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
void __haddock_process_set_wakeup_callback(struct process *proc,
                                           proc_wakeup_init_t f);
void __haddock_process_set_sleep_cleanup_callback(struct process *proc,
                                                  proc_sleep_cleanup_t f);
#endif
/**<  */

#ifdef __cplusplus
}
#endif

#endif /* HADDOCK_PROCESS_H_ */
