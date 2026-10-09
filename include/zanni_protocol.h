#ifndef ZANNI_PROTOCOL_H
#define ZANNI_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/* Total size of the protocol message: 8 (tag) + 4 (device_type) + 4 (status) = 16 bytes */
#define ZANNI_MSG_SIZE 16
#define ZANNI_TAG_SIZE 8

typedef struct 
{
    char tag[ZANNI_TAG_SIZE]; /* 8 bytes text tag. Not necessarily null-terminated if it takes all 8 bytes */
    uint32_t device_type;     /* 4 bytes device type */
    uint32_t status;          /* 4 bytes status */
} zanni_message_t;

/**
 * Encodes a zanni_message_t into a 16-byte binary buffer.
 * 
 * @param msg Pointer to the message to encode.
 * @param buffer Pointer to the destination buffer (must be at least 16 bytes).
 * @param buffer_size Size of the buffer to prevent overflow.
 * @return 0 on success, -1 on invalid arguments or insufficient buffer size.
 */
int zanni_message_encode(const zanni_message_t* msg, uint8_t* buffer, size_t buffer_size);

/**
 * Decodes a 16-byte binary buffer into a zanni_message_t.
 * 
 * @param buffer Pointer to the source buffer containing the encoded data.
 * @param buffer_size Size of the buffer (must be at least 16 bytes).
 * @param msg Pointer to the destination message structure.
 * @return 0 on success, -1 on invalid arguments or insufficient buffer size.
 */
int zanni_message_decode(const uint8_t* buffer, size_t buffer_size, zanni_message_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* ZANNI_PROTOCOL_H */
