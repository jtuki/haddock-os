/**
 * ping.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef PING_H_
#define PING_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "lib/hdk_utilities.h"
#include "kernel/process.h"

extern os_pid_t gl_proc_ping_pid;
extern void proc_ping_init(os_uint8 priority);

#define SIGNAL_PING_INITED              BV(0)
#define SIGNAL_DO_PING                  BV(1)
#define SIGNAL_PONG_IS_HERE             BV(2)

#ifdef __cplusplus
}
#endif

#endif /* PING_H_ */
