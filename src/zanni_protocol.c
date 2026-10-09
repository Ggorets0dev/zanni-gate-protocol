#include <string.h>
#include "zanni_protocol.h"

const char* zanni_protocol_version(void) 
{
    return ZANNI_PROTOCOL_VERSION_STRING;
}

int zanni_message_encode(const zanni_message_t* msg, uint8_t* buffer, size_t buffer_size) 
{
    if (!msg || !buffer || buffer_size < ZANNI_MSG_SIZE) 
    {
        return -1;
    }

    /* 1. Copy text tag (8 bytes) */
    memcpy(buffer, msg->tag, ZANNI_TAG_SIZE);

    /* 2. Encode device_type (4 bytes). 
     * Using Big Endian (Network Byte Order) for cross-platform compatibility. 
     */
    buffer[8]  = (uint8_t)((msg->device_type >> 24) & 0xFF);
    buffer[9]  = (uint8_t)((msg->device_type >> 16) & 0xFF);
    buffer[10] = (uint8_t)((msg->device_type >> 8) & 0xFF);
    buffer[11] = (uint8_t)(msg->device_type & 0xFF);

    /* 3. Encode status (4 bytes). 
     * Using Big Endian (Network Byte Order). 
     */
    buffer[12] = (uint8_t)((msg->status >> 24) & 0xFF);
    buffer[13] = (uint8_t)((msg->status >> 16) & 0xFF);
    buffer[14] = (uint8_t)((msg->status >> 8) & 0xFF);
    buffer[15] = (uint8_t)(msg->status & 0xFF);

    return 0;
}

int zanni_message_decode(const uint8_t* buffer, size_t buffer_size, zanni_message_t* msg) 
{
    if (!buffer || !msg || buffer_size < ZANNI_MSG_SIZE) 
    {
        return -1;
    }

    /* 1. Decode text tag (8 bytes) */
    memcpy(msg->tag, buffer, ZANNI_TAG_SIZE);

    /* 2. Decode device_type (4 bytes, Big Endian) */
    msg->device_type = ((uint32_t)buffer[8] << 24) |
                       ((uint32_t)buffer[9] << 16) |
                       ((uint32_t)buffer[10] << 8) |
                       ((uint32_t)buffer[11]);

    /* 3. Decode status (4 bytes, Big Endian) */
    msg->status = ((uint32_t)buffer[12] << 24) |
                  ((uint32_t)buffer[13] << 16) |
                  ((uint32_t)buffer[14] << 8) |
                  ((uint32_t)buffer[15]);

    return 0;
}
