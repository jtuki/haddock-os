/**
 * ping.c
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

static signal_bv_t proc_ping_entry(os_pid_t pid, signal_bv_t signal);

os_pid_t gl_proc_ping_pid;
haddock_process("proc_ping");

static struct timer *gl_timeout_timer = NULL;

static os_int8 ping_sleep_callback(void);
static void ping_wakup_callback(void);

#define MAX_PING_TIME       5
static os_uint8 gl_ping_times = 0;

void proc_ping_init(os_uint8 priority)
{
    struct process *proc_ping = process_create(proc_ping_entry, priority);
    gl_proc_ping_pid = proc_ping->_pid;

    gl_timeout_timer = os_timer_alloc();
    haddock_assert(gl_timeout_timer);

    proc_ping->wakeup_init_callback = ping_wakup_callback;
    proc_ping->sleep_cleanup_callback = ping_sleep_callback;

    os_ipc_set_signal(this->_pid, SIGNAL_PING_INITED);
}

static signal_bv_t proc_ping_entry(os_pid_t pid, signal_bv_t signal)
{
    haddock_assert(pid == this->_pid);
    
    if (signal & SIGNAL_PING_INITED) {
        print_log(LOG_INFO_COOL, "ping: inited");

        os_timer_reconfig(gl_timeout_timer, this->_pid, SIGNAL_DO_PING, 2000);
        os_timer_start(gl_timeout_timer);

        process_sleep();
        return signal ^ SIGNAL_PING_INITED;
    }

    if (signal & SIGNAL_DO_PING) {
        if (gl_ping_times++ < MAX_PING_TIME) {
            os_ipc_set_signal(gl_proc_pong_pid, SIGNAL_PING_IS_HERE);
        }
        process_sleep();
        return signal ^ SIGNAL_DO_PING;
    }

    if (signal & SIGNAL_PONG_IS_HERE) {
        print_log(LOG_INFO_COOL, "ping: pong received");

        os_timer_reconfig(gl_timeout_timer, this->_pid, SIGNAL_DO_PING, 2000);
        os_timer_start(gl_timeout_timer);

        process_sleep();
        return signal ^ SIGNAL_PONG_IS_HERE;
    }

    // unkown signal? discard.
    return 0;
}

static os_int8 ping_sleep_callback(void)
{
    print_log(LOG_INFO, "pong: sleep");
    return 0;
}

static void ping_wakup_callback(void)
{
    print_log(LOG_INFO, "pong: wakeup");
}
