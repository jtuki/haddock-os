/**
 * timer.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"
#include "lib/linked_list.h"
#include "kernel/ipc.h"
#include "kernel/process.h"

#define timer_started(timer)      (! list_empty(& (timer)->hdr))
#define timer_not_started(timer)  (list_empty(& (timer)->hdr))

#include "time.h"

struct time {
    os_uint32 s;
    os_uint32 ms;
    /** Does the timer is an absolute timer?
     *  (absolute timers will not shift during sync after first sync)
     *  \sa haddock_timer_sync() */
    os_boolean is_absolute;
};

struct timer {
    struct list_head hdr;
    struct time timeout_value; /**< \sa haddock_time_tick_now */
    signal_t signal;
    os_pid_t pid;
    os_boolean is_one_shot;
};

int haddock_timer_module_init(void);
void haddock_timer_update_routine(void);
void haddock_timer_sync(const struct time *sync_time);

void haddock_get_time_tick_now(struct time *t);
void haddock_get_time_tick_now_cached(struct time *t);

typedef void (*timer_out_of_sync_callback_t) (os_uint32 delta_s, os_uint16 delta_ms); 
void haddock_timer_set_out_of_sync_callback(timer_out_of_sync_callback_t f);

struct time *haddock_check_next_timeout(void);

#define os_timer_alloc()    __haddock_timer_alloc(OS_FALSE, OS_FALSE)

/** create a normal timer. */
#define os_timer_create(pid, signal, delta_ms) \
    __haddock_timer_create((pid), OS_FALSE, OS_FALSE, (signal), (delta_ms))

/** create an timer which @timeout_value is absolute. */
#define os_atimer_create(pid, signal, delta_ms) \
    __haddock_timer_create((pid), OS_TRUE, OS_FALSE, (signal), (delta_ms))

#define os_timer_create_one_shot(pid, signal, delta_ms) \
    __haddock_timer_create((pid), OS_FALSE, OS_TRUE, (signal), (delta_ms))

#define os_timer_reconfig(timer, pid, signal, delta_ms) \
     __os_timer_reconfig("", __LINE__, (timer), (pid), (signal), (delta_ms))

void __os_timer_reconfig(const char* _cur_file, os_uint32 _cur_line,
                         struct timer *timer, os_pid_t pid,
                         signal_t signal, os_uint32 delta_ms);
    
int os_timer_start(struct timer *timer);
void os_timer_stop(struct timer *timer);
void os_timer_destroy(struct timer *timer);

os_boolean os_no_more_timers(void);

/**
 * Called in the interrupt service routine each several ms if possible!
 * \ref \file systick.c systick_Get()
 */
void __haddock_increment_time_tick_now(os_uint32 delta_ms);

struct timer *__haddock_timer_alloc(os_boolean is_one_shot, os_boolean is_absolute);
struct timer *__haddock_timer_create(os_pid_t pid,
                                     os_boolean is_absolute, os_boolean is_one_shot,
                                     signal_t signal, os_uint32 delta_ms);

struct timer *haddock_get_next_timer(void);
struct timer *haddock_get_next_atimer(void);

os_int8 haddock_time_calc_delta(const struct time *t1, const struct time *t2,
                                struct time *delta);
os_int8 haddock_time_calc_delta_till_now(const struct time *t,
                                         struct time *delta);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
