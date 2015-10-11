/**
 * process.c
 *
 * \date    
 * \author  jtuki@foxmail.com
 */

#include "haddock_types.h"

#include "kernel_config.h"
#include "kernel/hdk_memory.h"
#include "kernel/sys_signal_defs.h"
#include "kernel/power_manager.h"

#include "lib/assert.h"
#include "lib/linked_list.h"
#include "lib/ringbuffer.h"

#include "process.h"

struct process_pool
{
    os_size_t _min_available; /**< range between [0, HDK_CFG_PROC_MAX_NUM]
                                \sa __process_alloc() */
    struct process pool[HDK_CFG_PROC_MAX_NUM];
};

/**
 * Allocate struct process from this pool.
 * 
 * \note pid's range is between [1, CONFIG_MAX_PROCESS_NUM).
 * \note
 *      Leave it as un-initialized (.bss sector), reference:
 *      http://stackoverflow.com/a/3532366/428467.
 */
static struct process_pool __process_pool;

/**
 * [0].list -> proc5_hdr -> proc7_hdr => highest priority.
 * [1].list -> proc2_hdr
 * [2].list -> proc6_hdr -> proc8_hdr
 * ...
 * [HDK_CFG_PROC_PRIORITY_NUM-1].list -> proc3_hdr
 */
__HADDOCK struct process_list process_list_table[HDK_CFG_PROC_PRIORITY_NUM];
__HADDOCK os_pid_t cur_process_id;

static struct process *__process_alloc(void);
static void __process_free(struct process *proc);

/**
 * MAKE SURE to initialize process module before any operation with processes.
 */
void haddock_process_module_init(void)
{
    for (os_size_t i=0; i < HDK_CFG_PROC_PRIORITY_NUM; i++) {
        list_head_init(& process_list_table[i].list);
        process_list_table[i].ready = 0x0;
    }
    
    haddock_memset(&__process_pool, 0, sizeof(__process_pool));
}

struct process *get_struct_process(os_pid_t pid)
{
    haddock_assert(pid <= HDK_CFG_PROC_MAX_NUM);
    return & (__process_pool.pool[pid-1]);
}

#if defined HDK_CFG_PROC_WITH_NAME && HDK_CFG_PROC_WITH_NAME == OS_TRUE
void __haddock_process_set_name(struct process *proc, const char *name)
{
    haddock_assert(proc && name);
    proc->name = name;
}
#else
void __haddock_process_set_name(struct process *proc, const char *name)
{
    (void) proc; (void) name;
}
#endif

/**
 * Set msg_queue_size to 0 if a process don't use message queue.
 * @return NULL if allocation failed.
 */
struct process *__haddock_process_create(struct process **this,
                                         const char *name,
                                         process_func_t f,
                                         os_uint8 priority, 
                                         os_size_t msg_queue_size)
{
    haddock_assert(f && name && *this == NULL
                   && priority < HDK_CFG_PROC_PRIORITY_NUM
                   && msg_queue_size < HDK_CFG_PROC_MAX_MSG_QUEUE_SIZE);
    
    struct process *proc = __process_alloc();
    if (! proc)
        return NULL;
    
    proc->entry = f;
    proc->signal = 0;
    proc->priority = priority;
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
    proc->is_wakeup = OS_FALSE;
#endif
    
    list_add_tail(& proc->hdr, & process_list_table[priority].list);
    
    *this = proc;
    __haddock_process_set_name(proc, name);
    
    return proc;
}

/**
 * Destroy the current struct process.
 * \note We actually don't encourage process destroy!
 */
os_int8 __haddock_process_destroy(struct process **this)
{
    haddock_assert(this && *this);
    
#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
    haddock_enable_power_conserve(*this);
#endif
    
    list_del(& (*this)->hdr);
    __process_free(*this);
    *this = NULL;
    
    return 0;
}

#if defined HDK_CFG_POWER_SAVING_ENABLE && HDK_CFG_POWER_SAVING_ENABLE == OS_TRUE
void __haddock_process_set_wakeup_callback(struct process *proc,
                                           proc_wakeup_init_t f)
{
    proc->wakeup_init_callback = f;
}

void __haddock_process_set_sleep_cleanup_callback(struct process *proc,
                                                  proc_sleep_cleanup_t f)
{
    proc->sleep_cleanup_callback = f;
}
#endif

#define process_avail(proc) ((proc)->_pid == 0)

static struct process *__process_alloc(void)
{
    struct process_pool *p = &__process_pool;
    os_size_t i = p->_min_available;
    
    for (; i < HDK_CFG_PROC_MAX_NUM; i++) {
        if (process_avail(& p->pool[i])) {
            p->pool[i]._pid = i+1;
            list_head_init(& p->pool[i].hdr);
            
            /* We don't find next available free struct process here, but
             * simply incease __process_pool._min_available by 1.
             */
            p->_min_available = i+1;
            return & p->pool[i];
        }
    }

    __process_pool._min_available = HDK_CFG_PROC_MAX_NUM;
    return NULL;
}

static void __process_free(struct process *proc)
{
    haddock_assert(proc && proc->_pid > 0);
    
    if (proc->_pid < __process_pool._min_available)
        __process_pool._min_available = proc->_pid;
    proc->_pid = 0;
}
