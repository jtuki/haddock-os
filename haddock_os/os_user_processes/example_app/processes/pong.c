/**
 * pong.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "kernel/process.h"
#include "kernel/timer.h"
#include "kernel/ipc.h"
#include "kernel/power_manager.h"
#include "kernel/sys_signal_defs.h"
#include "lib/assert.h"

#include "ping.h"
#include "pong.h"

static signal_bv_t proc_pong_entry(os_pid_t pid, signal_bv_t signal);

os_pid_t gl_proc_pong_pid;
haddock_process("proc_ping");

static struct timer *gl_timeout_timer = NULL;

static os_int8 pong_sleep_callback(void);
static void pong_wakup_callback(void);

void proc_pong_init(os_uint8 priority)
{
    struct process *proc_pong = process_create(proc_pong_entry, priority);
    gl_proc_pong_pid = proc_pong->_pid;

    proc_pong->wakeup_init_callback = pong_wakup_callback;
    proc_pong->sleep_cleanup_callback = pong_sleep_callback;

    gl_timeout_timer = os_timer_alloc();
    haddock_assert(gl_timeout_timer);

    os_ipc_set_signal(this->_pid, SIGNAL_PONG_INITED);
}

static signal_bv_t proc_pong_entry(os_pid_t pid, signal_bv_t signal)
{
    haddock_assert(pid == this->_pid);
    
    if (signal & SIGNAL_PONG_INITED) {
        print_log(LOG_INFO_COOL, "pong: inited, wait for ping.");
        process_sleep();
        return signal ^ SIGNAL_PONG_INITED;
    }

    if (signal & SIGNAL_DO_PONG) {
        os_ipc_set_signal(gl_proc_ping_pid, SIGNAL_PONG_IS_HERE);

        process_sleep();
        return signal ^ SIGNAL_DO_PONG;
    }

    if (signal & SIGNAL_PING_IS_HERE) {
        print_log(LOG_INFO_COOL, "pong: ping received");

        os_timer_reconfig(gl_timeout_timer, this->_pid, SIGNAL_DO_PONG, 2000);
        os_timer_start(gl_timeout_timer);

        process_sleep();
        return signal ^ SIGNAL_PING_IS_HERE;
    }

    // unkown signal? discard.
    return 0;
}

static os_int8 pong_sleep_callback(void)
{
    print_log(LOG_INFO, "pong: sleep");
    return 0;
}

static void pong_wakup_callback(void)
{
    print_log(LOG_INFO, "pong: wakeup");
}
