/**
 * hal_host_if.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "hal_config_pc_test.h"
#include "hal_pc_test.h"

#include "lib/assert.h"

#include <stdio.h>

/**
 * On PC, we send buf to stdout.
 * \sa print_log()
 */
void __haddock_send_to_host_pc(const os_uint8 c_buf[], os_uint16 len)
{
    haddock_assert(len >= 2);

    // change "\r\n" to "\n\0"
    char *buf = (void *) c_buf;
    buf[len-2] = '\n';
    buf[len-1] = '\0';

    printf(buf);
    fflush(stdout);
}

os_uint16 __haddock_recv_from_host_callback_pc(os_uint8 buf[], os_uint16 len)
{
    return 0;
}
