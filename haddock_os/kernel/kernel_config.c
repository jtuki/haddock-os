/**
 * kernel_config.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "kernel_config.h"

/**
 * Different classes's capacity.
 */
const os_size_t haddock_ipc_msg_classes_capacity[] = {
    3,
    3, 
    2, 
    1,
};

/**
 * As for each class, the block size should increase monotonically as the
 * class number increase.
 */
const os_size_t haddock_ipc_msg_classes_blk_size[] = {
    20,
    30,
    60,
    120,
};
