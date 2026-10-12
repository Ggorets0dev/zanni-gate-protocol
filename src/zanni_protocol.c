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
    memcpy(buffer, msg->tag, ZANNI_TAG_SIZE - 1);
    buffer[ZANNI_TAG_SIZE - 1] = '\0';

    const int offset = ZANNI_TAG_SIZE;

    /* 2. Encode device_type (4 bytes). 
     * Using Big Endian (Network Byte Order) for cross-platform compatibility. 
     */
    buffer[offset]  = (uint8_t)((msg->device_type >> 24) & 0xFF);
    buffer[offset + 1]  = (uint8_t)((msg->device_type >> 16) & 0xFF);
    buffer[offset + 2] = (uint8_t)((msg->device_type >> 8) & 0xFF);
    buffer[offset + 3] = (uint8_t)(msg->device_type & 0xFF);

    /* 3. Encode status (4 bytes). 
     * Using Big Endian (Network Byte Order). 
     */
    buffer[offset + 4] = (uint8_t)((msg->status >> 24) & 0xFF);
    buffer[offset + 5] = (uint8_t)((msg->status >> 16) & 0xFF);
    buffer[offset + 6] = (uint8_t)((msg->status >> 8) & 0xFF);
    buffer[offset + 7] = (uint8_t)(msg->status & 0xFF);

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

    const int offset = ZANNI_TAG_SIZE;

    /* 2. Decode device_type (4 bytes, Big Endian) */
    msg->device_type = ((uint32_t)buffer[offset] << 24) |
                       ((uint32_t)buffer[offset + 1] << 16) |
                       ((uint32_t)buffer[offset + 2] << 8) |
                       ((uint32_t)buffer[offset + 3]);

    /* 3. Decode status (4 bytes, Big Endian) */
    msg->status = ((uint32_t)buffer[offset + 4] << 24) |
                  ((uint32_t)buffer[offset + 5] << 16) |
                  ((uint32_t)buffer[offset + 6] << 8) |
                  ((uint32_t)buffer[offset + 7]);

    return 0;
}
