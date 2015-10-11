/**
 * simple_log.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "kernel/timer.h"
#include "lib/assert.h"
#include <stdio.h>

#include "hal/hal_mcu.h"

#include "simple_log.h"

#if defined SIMPLE_LOG_ENABLE && (SIMPLE_LOG_ENABLE == OS_TRUE)
static const char *log_type_string[5] = {
    " ", "*", "W", "E", "DEBUG_ONLY_NO_PRINT",
};
static char log_buffer[HOST_IF_TX_BUFFER_LEN];
#endif


#define xPRINT_LOG_DUTY_CYCLE_ENABLED // only for debug purpose

/** Refer to:
 * http://www.velocityreviews.com/forums/t437343-printf-wrapper.html
 * http://www.cplusplus.com/reference/cstdio/vprintf/
 *
 * \note print_log() may invoke IO operation (UART interruption etc.) which will
 * seemingly block some time, which will in thus will cause some delay.
 * \note So remember to put print_log() to the end of a block of operations/statements,
 * to avoid breaking the timing needs.
 */
void print_log(enum log_type type, const char *log, ...)
{
#if defined SIMPLE_LOG_ENABLE && (SIMPLE_LOG_ENABLE == OS_TRUE)
    static struct time cur_time;
    haddock_get_time_tick_now(& cur_time);

#ifdef PRINT_LOG_DUTY_CYCLE_ENABLED
#define PRINT_LOG_DUTY_CYCLE_TIMES      5
#define PRINT_LOG_DUTY_CYCLE_SECONDS    10  // wait 10 seconds for next print_log()

    static os_uint8 duty_cycle_cnter = 0; /**< allow several  */
    static os_boolean run_first_time = OS_TRUE;
    static struct time prev_time;

    if (run_first_time) {
        haddock_get_time_tick_now(& prev_time);
        run_first_time = OS_FALSE;
    }

    if (++duty_cycle_cnter > PRINT_LOG_DUTY_CYCLE_TIMES) {
        if (cur_time.s <= prev_time.s)
            return;
        else if (cur_time.s - prev_time.s <= PRINT_LOG_DUTY_CYCLE_SECONDS)
            return;
        else {
            duty_cycle_cnter = 0;
            prev_time = cur_time;
        }
    }
#endif

    static va_list args;
    va_start(args, log);

    os_int8 len = 0;
    os_int8 ret = 0;

    ret = (os_int8) sprintf(log_buffer, "%lX.%03ld\t\t%s: ",
                                         (unsigned long int) cur_time.s,
                                         (unsigned long int) cur_time.ms,
                                         log_type_string[(int) type]);
    haddock_assert(ret > 0);
    len += ret;

    haddock_assert(len > 0 && len < HOST_IF_TX_BUFFER_LEN-2);

    ret = (os_int8) vsprintf(log_buffer + len, log, args);
    haddock_assert(ret > 0);
    len += ret;
    haddock_assert(len > 0 && len < HOST_IF_TX_BUFFER_LEN-2);

    log_buffer[len++] = '\r';
    log_buffer[len++] = '\n';

    if (type < LOG_NO_PRINT) {
        haddock_send_to_host((void *) log_buffer, len);
    }

    va_end(args);
#endif
}
