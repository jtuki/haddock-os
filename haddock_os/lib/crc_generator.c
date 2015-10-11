/**
 * crc_generator.c
 *
 * \date   
 * \author jtuki@foxmail.com
 * \author 
 */

#include "hdk_utilities.h"
#include "crc_generator.h"

/**
 * Currently we simply return constant CRC value, later we will refer to the
 * library below to generate CRC codes for both C and Python.
 * todo
 * \ref http://www.tty1.net/pycrc/
 */

os_uint8 crc8_generator(const os_uint8 *buf, os_uint32 len)
{
    (void) buf;
    (void) len;

    return 0xAA;
}

os_uint16 crc16_generator(const os_uint8 *buf, os_uint32 len)
{
    (void) buf;
    (void) len;

    return construct_u16_2(0x55, 0xAA);
}

os_uint32 crc32_generator(const os_uint8 *buf, os_uint32 len)
{
    (void) buf;
    (void) len;

    return construct_u32_4(0x55, 0xAA, 0x55, 0xAA);
}
