#include <string.h>
#include "zanni_protocol.h"

/**
 * @brief Reads a 32-bit unsigned integer from a buffer in Big-Endian (Network Byte Order).
 * 
 * @param[in] buffer Pointer to the source byte buffer.
 * @param[in] offset Byte offset in the buffer from which to read 4 bytes.
 * @return 32-bit unsigned integer in host byte order.
 */
static inline uint32_t read_u32_be(const uint8_t *buffer, size_t offset)
{
    return ((uint32_t)buffer[offset] << 24)     |
           ((uint32_t)buffer[offset + 1] << 16) |
           ((uint32_t)buffer[offset + 2] << 8)  |
           ((uint32_t)buffer[offset + 3]);
}

/**
 * @brief Writes a 32-bit unsigned integer to a buffer in Big-Endian (Network Byte Order).
 * 
 * @param[out] buffer Pointer to the destination byte buffer.
 * @param[in] offset Byte offset in the buffer where 4 bytes will be written.
 * @param[in] value 32-bit unsigned integer to encode.
 */
static inline void write_u32_be(uint8_t *buffer, size_t offset, uint32_t value)
{
    buffer[offset]     = (uint8_t)(value >> 24);
    buffer[offset + 1] = (uint8_t)(value >> 16);
    buffer[offset + 2] = (uint8_t)(value >> 8);
    buffer[offset + 3] = (uint8_t)value;
}

const char* zanni_protocol_version(void) 
{
    return ZANNI_PROTOCOL_VERSION_STRING;
}

zanni_encode_status_e zanni_message_encode(const zanni_message_t* msg, uint8_t* buffer, size_t buffer_size) 
{
    if (!msg || !buffer || buffer_size < ZANNI_MSG_SIZE) 
    {
        return ZANNI_ECD_STATUS_ERROR;
    }

    /* 1. Copy text tag (8 bytes) */
    memcpy(buffer, msg->tag, ZANNI_TAG_SIZE - 1);
    buffer[ZANNI_TAG_SIZE - 1] = '\0';

    const int offset = ZANNI_TAG_SIZE;

    /* 2. Encode major_proto_ver (4 bytes).
     * Using Big Endian (Network Byte Order) for cross-platform compatibility.
     */
    write_u32_be(buffer, offset, ZANNI_PROTOCOL_VERSION_MAJOR);

    /* 3. Encode device_type (4 bytes).
     * Using Big Endian (Network Byte Order) for cross-platform compatibility.
     */
    write_u32_be(buffer, offset + 4, (uint32_t)msg->device_type);

    /* 4. Encode status (4 bytes).
     * Using Big Endian (Network Byte Order).
     */
    write_u32_be(buffer, offset + 8, (uint32_t)msg->payload.dev_status);

    return ZANNI_ECD_STATUS_OK;
}

zanni_decode_status_e zanni_message_decode(const uint8_t* buffer, size_t buffer_size, zanni_message_t* msg) 
{
    if (!buffer || !msg || buffer_size < ZANNI_MSG_SIZE) 
    {
        return ZANNI_DCD_STATUS_ERROR;
    }

    /* 1. Decode text tag (8 bytes) */
    memcpy(msg->tag, buffer, ZANNI_TAG_SIZE);

    const int offset = ZANNI_TAG_SIZE;

    /* 2. Decode device_type (4 bytes, Big Endian) */
    msg->major_proto_ver = read_u32_be(buffer, offset);

    if (msg->major_proto_ver != ZANNI_PROTOCOL_VERSION_MAJOR)
    {
        return ZANNI_DCD_STATUS_WRONG_VER;
    }

    /* 3. Decode device_type (4 bytes, Big Endian) */
    msg->device_type = read_u32_be(buffer, offset + 4);

    /* 4. Decode status (4 bytes, Big Endian) */
    msg->payload.dev_status = read_u32_be(buffer, offset + 8);

    return ZANNI_DCD_STATUS_OK;
}
