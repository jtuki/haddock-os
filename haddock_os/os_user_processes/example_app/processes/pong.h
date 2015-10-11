/**
 * pong.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef PONG_H_
#define PONG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "lib/hdk_utilities.h"
#include "kernel/process.h"

extern os_pid_t gl_proc_pong_pid;
extern void proc_pong_init(os_uint8 priority);

#define SIGNAL_PONG_INITED              BV(0)
#define SIGNAL_DO_PONG                  BV(1)
#define SIGNAL_PING_IS_HERE             BV(2)

#ifdef __cplusplus
}
#endif

#endif /* PONG_H_ */
