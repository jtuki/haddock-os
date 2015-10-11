/**
 * timer.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "kernel_config.h"
#include "hal/hal_mcu.h"
#include "lib/assert.h"
#include "lib/mem_pool.h"
#include "timer.h"

/**
 * The timer-related variable.
 * @haddock_timer_list  - linked all the timers.
 * @haddock_atimer_list - linked all the absolute timers.
 * @haddock_time_tick_now - current time tick.
 */
static struct list_head haddock_timer_list;
static struct list_head haddock_atimer_list;

#define HADDOCK_TIMER_LIST_NUM  2 // as above timer list
static const struct list_head *_timer_list[HADDOCK_TIMER_LIST_NUM] = {
    & haddock_timer_list, & haddock_atimer_list,
};

static struct time haddock_time_tick_now;

static struct mem_pool_hdr *timer_pool;
static os_boolean is_haddock_timer_synchronized; /** \sa haddock_timer_sync() */

/** \sa haddock_timer_set_out_of_sync_callback() */
timer_out_of_sync_callback_t haddock_timer_out_of_sync_callback = NULL;

/** Helper function declarations. @{ */
static os_boolean is_signal(signal_t sig);
static inline void __haddock_calc_timeout_value(os_uint32 delta_ms, struct time *t);

static void _haddock_time_shift(struct time *t,
                                const struct time *delta, os_boolean is_add);
/** @} */

static os_boolean haddock_debug_os_timer_list_have_loop(os_boolean is_absolute_timer_list,
                                                        os_size_t max_len);

/**
 * For system resource profiling (how many timers are used).
 */
static os_uint8 __haddock_timer_max_num = 0;

int haddock_timer_module_init(void)
{
    list_head_init(& haddock_timer_list);
    list_head_init(& haddock_atimer_list);
    haddock_time_tick_now.s = haddock_time_tick_now.ms = 0;
    
    timer_pool = mem_pool_create(HDK_CFG_TIMER_MAX_NUM, sizeof(struct timer));
    haddock_assert(timer_pool);
    
    return 0;
}

/**
 * Called in the interrupt service routine each several ms if possible!
 * \ref \file systick.c systick_Get()
 */
void __haddock_increment_time_tick_now(os_uint32 delta_ms)
{
    /**
     * we suppose that os_uint32 will not overflow.
     */
    haddock_time_tick_now.ms += delta_ms;
    
    if (haddock_time_tick_now.ms >= 1000) {
        haddock_time_tick_now.s += (haddock_time_tick_now.ms) / 1000;
        haddock_time_tick_now.ms = (haddock_time_tick_now.ms) % 1000;
    }
}

void haddock_timer_set_out_of_sync_callback(timer_out_of_sync_callback_t f)
{
    haddock_timer_out_of_sync_callback = f;
}

struct timer *__haddock_timer_alloc(os_boolean is_one_shot, os_boolean is_absolute)
{
    struct mem_pool_blk *_blk = \
            mem_pool_alloc_blk(timer_pool, sizeof(struct timer));
    if (!_blk)
        return NULL;

    struct timer *timer = (struct timer *) _blk->blk;
    list_head_init(& timer->hdr);

    timer->is_one_shot = is_one_shot;
    timer->timeout_value.is_absolute = is_absolute;

    __haddock_timer_max_num += 1;
    return timer;
}

/**
 * Create a struct timer.
 * 
 * \remark @signal should be valid; and @timeout_value is a positive number.
 * \sa is_signal(signal)
 * 
 * \sa macro os_timer_create()
 * \sa macro os_atimer_create()
 * 
 * \remark Once the timer is created, either absolute or not, the @is_absolute
 *         property _should_ not be modified.
 * \sa __os_timer_reconfig()
 *
 * \remark if set @is_one_shot, the timer will be automatically destroyed after
 *      the timer has been evoked.
 */
struct timer *__haddock_timer_create(os_pid_t pid,
                                     os_boolean is_absolute, os_boolean is_one_shot,
                                     signal_t signal, os_uint32 delta_ms)
{
    haddock_assert(is_signal(signal));
    haddock_assert(delta_ms > 0);
    
    struct timer *timer;
    if ((timer = __haddock_timer_alloc(is_one_shot, is_absolute)) == NULL)
        return NULL;

    timer->pid = pid;
    timer->signal = signal;
    __haddock_calc_timeout_value(delta_ms, & timer->timeout_value);
    
    return timer;
}

/**
 * \note make sure the @timer has been stopped but not destroyed.
 * 
 * \remark This interface cannot modify the timer->timeout_value.is_absolute.
 * \sa __haddock_timer_create()
 */
void __os_timer_reconfig(const char* _cur_file, os_uint32 _cur_line,
                         struct timer *timer, os_pid_t pid,
                         signal_t signal, os_uint32 delta_ms)
{
    __haddock_call_function_file = _cur_file;
    __haddock_call_function_line = _cur_line;

    haddock_assert(timer);
    haddock_assert(is_signal(signal));
    haddock_assert(delta_ms > 0);
    haddock_assert(timer_not_started(timer)); // is stopped
    
    timer->signal = signal;
    timer->pid = pid;
    __haddock_calc_timeout_value(delta_ms, & timer->timeout_value);
}

/**
 * Utility function for struct time comparison.
 */
#define time_tick_less_than(t1, t2) \
    (((t1).s < (t2).s) || (((t1).s == (t2).s) && ((t1).ms < (t2).ms)))

#define time_tick_equal(t1, t2) \
    ((t1).s == (t2).s && (t1).ms == (t2).ms)

#define time_tick_less_or_equal(t1, t2) \
    (((t1).s < (t2).s) || (((t1).s == (t2).s) && ((t1).ms <= (t2).ms)))

#define time_tick_greater_than(t1, t2) \
    time_tick_less_than(t2, t1)

#define time_tick_greater_or_equal(t1, t2) \
    time_tick_less_or_equal(t2, t1)

/** now is slightly greater than t? */
#define time_tick_within_threshold(t_s, t_ms, now_s, now_ms) \
    ((t_s) == (now_s) && ((now_ms)-(t_ms) <= HDK_CFG_TIMER_SCHEDULE_THRESHOLD)) \
    || \
    (((t_s)+1) == (now_s) && \
     ((now_ms) + (1000 - (t_ms))) <= HDK_CFG_TIMER_SCHEDULE_THRESHOLD)
    
/**
 * The @timer should have been stopped or have not yet started.
 */
int os_timer_start(struct timer *timer)
{
    haddock_assert(timer && timer->signal != 0
                   && (timer->timeout_value.s > 0 || timer->timeout_value.ms > 0)
                   && timer_not_started(timer));
    
    struct list_head *timer_list = (timer->timeout_value.is_absolute) ?
                                    & haddock_atimer_list : & haddock_timer_list;
    struct list_head *i;
    os_boolean added = OS_FALSE;
    
    os_uint8 _loop_cnter = 0;

    struct time *t1 = & timer->timeout_value;
    struct time *t2;

    haddock_assert(! haddock_debug_os_timer_list_have_loop(OS_FALSE, HDK_CFG_TIMER_MAX_NUM));
    list_for_each(i, timer_list) {
        haddock_assert(_loop_cnter++ < HDK_CFG_TIMER_MAX_NUM);
        t2 = & (list_entry(i, struct timer, hdr))->timeout_value;
        if (time_tick_less_than(*t1, *t2)) {
            list_add_tail(& timer->hdr, i); // i.prev -> timer -> i -> i.next
            added = OS_TRUE;
            haddock_assert(! haddock_debug_os_timer_list_have_loop(OS_FALSE, HDK_CFG_TIMER_MAX_NUM));
            break;
        }
    }
    
    if (!added) {
        list_add_tail(& timer->hdr, timer_list);
    }

    return 0;
}

void os_timer_stop(struct timer *timer)
{
    haddock_assert(timer);
    if (timer_not_started(timer)) {
        return;
    }
    
    list_del_init(& timer->hdr);
}

/**
 * Try to stop the timer first, then destroy the timer.
 */
void os_timer_destroy(struct timer *timer)
{
    os_timer_stop(timer);
    
    struct mem_pool_blk *blk = find_mem_pool_blk(timer);
    mem_pool_free_blk(blk);
}

/**
 * Return the next timeout value.
 * \sa haddock_power_conserve_routine()
 */
struct time *haddock_check_next_timeout(void)
{
    static struct time _now;
    static struct timer *t;
    static struct time delta1;
    static struct time delta2;
    os_int8 delta_comparison;
    os_boolean delta1_exist = OS_FALSE;
    os_boolean delta2_exist = OS_FALSE;
    
    HDK_CRITICAL_STATEMENTS(
        _now.s = haddock_time_tick_now.s;
        _now.ms = haddock_time_tick_now.ms;
    );
    
    if (! list_empty(& haddock_timer_list)) {
        delta1_exist = OS_TRUE;
        t = list_entry(haddock_timer_list.next, struct timer, hdr);
        delta_comparison = haddock_time_calc_delta(& t->timeout_value,
                                                    & _now, & delta1); 
        if (delta_comparison == -1) {
            /* next timeout _should_ be 0, because the task is about to run. */
            delta1.s = 0;
            delta2.ms = 0;
            return & delta1;
        }
    }
    
    if (! list_empty(& haddock_atimer_list)) {
        delta2_exist = OS_TRUE;
        t = list_entry(haddock_atimer_list.next, struct timer, hdr);
        delta_comparison = haddock_time_calc_delta(& t->timeout_value,
                                                    & _now, & delta2);
        if (delta_comparison == -1) {
            delta2.s = 0;
            delta2.ms = 0;
            return & delta2;
        }
    }
    
    if (!delta1_exist && !delta2_exist)
        return NULL;
    else if (delta1_exist && !delta2_exist)
        return & delta1;
    else if (!delta1_exist && delta2_exist)
        return & delta2;
    else { // both exists
        if (time_tick_less_or_equal(delta1, delta2))
            return & delta1;
        else
            return & delta2;
    }
}

os_boolean os_no_more_timers(void)
{
    if (list_empty(& haddock_timer_list) && list_empty(& haddock_atimer_list))
        return OS_TRUE;
    else
        return OS_FALSE;
}

/**
 * Update the timers (running in the kernel main loop).
 * i.e.
 * Check @haddock_time_tick_now and timers in @haddock_timer_list, update
 * the status of timers.
 * 
 * \sa HDK_CFG_TIMER_SCHEDULE_THRESHOLD
 */
void haddock_timer_update_routine(void)
{
    static struct time _now = {
        .s  = 0xFFFFFFFF,
        .ms = 0xFFFFFFFF,
    };
    
    if (list_empty(& haddock_timer_list) && list_empty(& haddock_atimer_list))
        return;

    /*
     * @haddock_time_tick_now has not changed yet.
     */
    if (_now.ms == haddock_time_tick_now.ms &&
        _now.s == haddock_time_tick_now.s)
        return;
    
    /*
     * To avoid the update of @haddock_time_tick_now during the execution of 
     * this function.
     * eg. from 1s999ms to 2s0ms.
     */
    HDK_CRITICAL_STATEMENTS(
        _now.s = haddock_time_tick_now.s;
        _now.ms = haddock_time_tick_now.ms;
    );
    
    struct list_head *pos;
    struct list_head *n;
    struct timer *t;
    
    for (os_size_t i=0; i < HADDOCK_TIMER_LIST_NUM; ++i) {
        list_for_each_safe(pos, n, _timer_list[i]) {
            t = list_entry(pos, struct timer, hdr);
            
            if (time_tick_equal(_now, t->timeout_value)) {
                // the same 
                os_ipc_set_signal(t->pid, t->signal);
                if (t->is_one_shot) {
                    os_timer_destroy(t);
                } else {
                    os_timer_stop(t);
                }
            } else if (time_tick_greater_than(_now, t->timeout_value)) {
                if (time_tick_within_threshold(t->timeout_value.s, 
                                               t->timeout_value.ms,
                                               _now.s, _now.ms)) {
                    // _now is slightly greater than t
                    os_ipc_set_signal(t->pid, t->signal);
                    if (t->is_one_shot) {
                        os_timer_destroy(t);
                    } else {
                        os_timer_stop(t);
                    }
                } else {
                    /**
                     * @_now is much greater than @t; we still evoke the timer
                     * instead of deleting it. #mark#
                     * todo
                     */ 
                    os_ipc_set_signal(t->pid, t->signal);
                    if (t->is_one_shot) {
                        os_timer_destroy(t);
                    } else {
                        os_timer_stop(t);
                    }
                }
            } else {
                break;
            }
        }
    }
}

/**
 * @sync is the struct time value from external clock source.
 * If timer sync is enabled, this function should be called periodically.
 * 
 * \sa HDK_CFG_TIMER_SYNC_THRESHOLD
 * \sa is_haddock_timer_synchronized
 */
void haddock_timer_sync(const struct time *sync)
{
#if defined HDK_CFG_TIMER_ENABLE_SYNC && HDK_CFG_TIMER_ENABLE_SYNC == OS_TRUE
    haddock_assert(sync);
    
    struct time _now;
    HDK_CRITICAL_STATEMENTS(
        _now.s = haddock_time_tick_now.s;
        _now.ms = haddock_time_tick_now.ms;
        haddock_time_tick_now.s = sync->s;
        haddock_time_tick_now.ms = sync->ms;
    );
    
    struct time delta;
    os_int8 is_greater_than = haddock_time_calc_delta(sync, &_now, &delta);
    
    if (is_greater_than == 0)
        return;
    
    os_boolean is_add = (is_greater_than == 1) ? OS_TRUE:OS_FALSE;
    struct list_head *pos;
    struct timer *t;
    
    if (!is_haddock_timer_synchronized ||
        delta.s > HDK_CFG_TIMER_SYNC_THRESHOLD)
    {
        if (is_haddock_timer_synchronized) {
            /**
             * Call the handler of timer-out-of-sync-handler.
             * \sa HDK_CFG_TIMER_SYNC_THRESHOLD
             */
            if (haddock_timer_out_of_sync_callback)
                haddock_timer_out_of_sync_callback(delta.s, delta.ms);
        }
        
        for (os_size_t i=0; i < sizeof(_timer_list); ++i) {
            list_for_each(pos, _timer_list[i]) {
                t = list_entry(pos, struct timer, hdr);
                _haddock_time_shift(& t->timeout_value, & delta, is_add);
            }
        }
        
        is_haddock_timer_synchronized = OS_TRUE;
    } else {
        list_for_each(pos, & haddock_timer_list) {
            t = list_entry(pos, struct timer, hdr);
            _haddock_time_shift(& t->timeout_value, & delta, is_add);
        }
    }
    
#else
    // to avoid compiling warnings.
    (void) sync;
    (void) is_haddock_timer_synchronized;
    (void) haddock_time_calc_delta;
    (void) _haddock_time_shift;
#endif
}

static os_boolean is_signal(signal_t sig)
{
    for (os_size_t i=0; i < 32; i++) {
        if (sig == (((os_uint32)1)<<(i)))
            return OS_TRUE;
    }
    return OS_FALSE;
}

static inline void __haddock_calc_timeout_value(os_uint32 delta_ms, struct time *t)
{
    static os_uint32 cur_s;
    static os_uint32 cur_ms;
    HDK_CRITICAL_STATEMENTS(
        cur_s  = haddock_time_tick_now.s;
        cur_ms = haddock_time_tick_now.ms;
    );
    
    t->s = cur_s + (delta_ms + cur_ms) / 1000;
    t->ms = (cur_ms + delta_ms) % 1000;
}

/**
 * Calculate the delta value between @t1 and @t2.
 * Return 0 if t1 == t2, 1 if t1 > t2, else (t1 < t2) return -1.
 */
os_int8 haddock_time_calc_delta(const struct time *t1,
                                const struct time *t2,
                                struct time *delta)
{
    if (t1->s > t2->s) {
        delta->s = t1->s - t2->s;
        if (t1->ms >= t2->ms)
            delta->ms = t1->ms - t2->ms;
        else {
            delta->ms = (1000 - t2->ms) + t1->ms;
            delta->s -= 1;
        }
        return 1;
    } else if (t1->s == t2->s) {
        if (t1->ms > t2->ms) {
            delta->s = 0;
            delta->ms = t1->ms - t2->ms;
            return 1;
        } else if (t1->ms == t2->ms) {
            delta->s = 0;
            delta->ms = 0;
            return 0;
        } else {
            delta->s = 0;
            delta->ms = t2->ms - t1->ms;
            return -1;
        }
    } else { // t1->s < t2->s
        delta->s = t2->s - t1->s;
        if (t2->ms >= t1->ms) {
            delta->ms = t2->ms - t1->ms;
        } else {
            delta->ms = (1000 - t1->ms) + t2->ms;
            delta->s -= 1;
        }
        return -1;
    }
}

os_int8 haddock_time_calc_delta_till_now(const struct time *t,
                                         struct time *delta)
{
    haddock_get_time_tick_now(& haddock_time_tick_now);
    return haddock_time_calc_delta(& haddock_time_tick_now, t, delta);
}

/**
 * Shift the @t according to @delta and @is_add.
 * if is_add:
 *      t = t + delta
 * if not @is_add:
 *      t = t - delta
 */
static void _haddock_time_shift(struct time *t,
                                const struct time *delta, os_boolean is_add)
{
    if (is_add) {
        t->ms += delta->ms;
        if (t->ms >= 1000) {
            t->ms -= 1000;
            t->s += 1;
        }
        t->s += delta->s;
    } else {
        haddock_assert(time_tick_greater_or_equal(*t, *delta));
        if (t->ms >= delta->ms) {
            t->ms -= delta->ms;
            t->s -= delta->s;
        } else {
            t->ms += (1000 - delta->ms);
            t->s -= (delta->s + 1);
        }
    }
}

/**
 * Get current system time tick, update @haddock_time_tick_now, and assign to @t.
 * \sa haddock_get_time_tick_now_cached()
 */
void haddock_get_time_tick_now(struct time *t)
{
    haddock_assert(t);

    static os_uint32 _systick_prev = 0;
    static os_uint32 _systick_now = 0;

    _systick_now = (os_uint32) systick_Get();

    if (_systick_now != _systick_prev) {
        __haddock_increment_time_tick_now(
            (_systick_now >= _systick_prev) ? (_systick_now - _systick_prev)
                                            : (0xFFFFFFFF - _systick_prev + _systick_now)
        ); // 0xFFFFFFFF is MAX_UINT32
        _systick_prev = _systick_now;
    }

    if (t != &haddock_time_tick_now) {
        HDK_CRITICAL_STATEMENTS(
            t->s = haddock_time_tick_now.s;
            t->ms = haddock_time_tick_now.ms;
        );
    }
}

/**
 * Don't update @haddock_time_tick_now, simply copy to @t.
 */
void haddock_get_time_tick_now_cached(struct time *t)
{
    haddock_assert(t);
    if (t != & haddock_time_tick_now) {
        HDK_CRITICAL_STATEMENTS(
            t->s = haddock_time_tick_now.s;
            t->ms = haddock_time_tick_now.ms;
        );
    }
}

struct timer *haddock_get_next_timer(void)
{
    if (! list_empty(& haddock_timer_list)) {
        return list_entry(haddock_timer_list.next, struct timer, hdr);
    } else {
        return NULL;
    }
}

struct timer *haddock_get_next_atimer(void)
{
    if (! list_empty(& haddock_atimer_list)) {
        return list_entry(haddock_atimer_list.next, struct timer, hdr);
    } else {
        return NULL;
    }
}

static os_boolean haddock_debug_os_timer_list_have_loop(os_boolean is_absolute_timer_list,
                                                        os_size_t max_len)
{
#if defined(HADDOCK_DEBUG_OS_TIMER_CHECK_LOOP) && HADDOCK_DEBUG_OS_TIMER_CHECK_LOOP == OS_TRUE
    struct list_head *pos;
    struct list_head *timer_list = is_absolute_timer_list ?
                                   & haddock_atimer_list : & haddock_timer_list;
    os_size_t i = 0;

    list_for_each(pos, timer_list) {
        ++i;
        if (i > max_len)
            return OS_TRUE;
    }
    return OS_FALSE;
#else
    (void) is_absolute_timer_list;
    (void) max_len;
    return OS_FALSE;
#endif
}

