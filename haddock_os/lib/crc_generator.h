/**
 * crc_generator.h
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */
#ifndef CRC_GENERATOR_H_
#define CRC_GENERATOR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "haddock_types.h"
    
os_uint8  crc8_generator(const os_uint8 *buf, os_uint32 len);
os_uint16 crc16_generator(const os_uint8 *buf, os_uint32 len);
os_uint32 crc32_generator(const os_uint8 *buf, os_uint32 len);

#ifdef __cplusplus
}
#endif

#endif /* CRC_GENERATOR_H_ */
