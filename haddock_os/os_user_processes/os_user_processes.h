/**
 * os_user_processes.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef OS_USER_PROCESSES_H_
#define OS_USER_PROCESSES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "hdk_user_config.h"
#include "haddock_types.h"
    
#if defined HDK_USER_CFG_MAIN_PC_TEST && HDK_USER_CFG_MAIN_PC_TEST == OS_TRUE
#include "pc_test/app_entry.h"
#define os_user_processes_init() \
    os_processes_init_pc_test()
#elif (defined HDK_USER_CFG_MAIN_EXAMPLE_APP) && (HDK_USER_CFG_MAIN_EXAMPLE_APP == OS_TRUE)
#include "example_app/app_entry.h"
#define os_user_processes_init() \
    os_processes_init_example_app()
#endif


#ifdef __cplusplus
}
#endif

#endif /* OS_USER_PROCESSES_H_ */
