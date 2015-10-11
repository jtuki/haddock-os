/**
 * main_example_user.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "hdk_user_config.h"

#include "lib/simple_log.h"
#include "lib/assert.h"
#include "processes/ping.h"
#include "processes/pong.h"

#include "app_entry.h"

void os_processes_init_example_app(void) {
    haddock_assert(HDK_USER_CFG_HAL_TARGET == HAL_TARGET_PC);
    print_log(LOG_INFO_COOL, "Welcome to emaple app!");

    proc_ping_init(0);
    proc_pong_init(0);

    return;
}
