#ifndef ZANNI_PROTOCOL_H
#define ZANNI_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "zanni_version.h"

#ifdef __cplusplus
extern "C" 
{
#endif
/**
 * @brief Total size of the serialized protocol message in bytes
 */
#define ZANNI_MSG_SIZE 20

/**
 * @brief Size of the message text tag field in bytes.
 */
#define ZANNI_TAG_SIZE 8

/**
 * @brief Status of command execution
 */
typedef enum 
{
    ZANNI_EXEC_STATUS_OK,
    ZANNI_EXEC_STATUS_ERROR,
} zanni_exec_status_e;

/**
 * @brief Status of message decoding
 */
typedef enum 
{
    ZANNI_DCD_STATUS_OK,
    ZANNI_DCD_STATUS_WRONG_VER,
    ZANNI_DCD_STATUS_ERROR,
} zanni_decode_status_e;

/**
 * @brief Status of message encoding
 */
typedef enum 
{
    ZANNI_ECD_STATUS_OK,
    ZANNI_ECD_STATUS_ERROR,
} zanni_encode_status_e;

/**
 * @brief Union type of payload in message
 */
typedef union 
{
    uint32_t dev_status;
    zanni_exec_status_e exec_status;
} zanni_msg_payload_u;

/**
 * @brief Virtual device types supported by the protocol.
 */
typedef enum 
{
    ZANNI_VIRT_DEV_TOGGLE, /**< @brief Toggle virtual device. */
    ZANNI_VIRT_DEV_MAX     /**< @brief Upper bound / count of virtual device types. */
} zanni_virt_dev_e;

/**
 * @brief Protocol message data structure.
 */
typedef struct 
{
    uint32_t major_proto_ver;       /**< @brief 4-byte virtual device type. */
    char tag[ZANNI_TAG_SIZE];       /**< @brief 8-byte text tag. Not necessarily null-terminated if it takes all 8 bytes. */
    zanni_virt_dev_e device_type;   /**< @brief 4-byte virtual device type. */
    zanni_msg_payload_u payload;    /**< @brief 4-byte status / payload. */
} zanni_message_t;

#if defined(__cplusplus)
static_assert(sizeof(zanni_message_t) == ZANNI_MSG_SIZE, "zanni_message_t size is wrong (ZANNI_MSG_SIZE)");
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(zanni_message_t) == ZANNI_MSG_SIZE, "zanni_message_t size is wrong (ZANNI_MSG_SIZE)");
#endif

/**
 * @brief Returns the version string of the protocol library (e.g. "0.1.0").
 * 
 * @return Pointer to a static null-terminated version string.
 */
const char* zanni_protocol_version(void);

/**
 * @brief Encodes a message structure into a binary buffer
 * @param[in] msg Pointer to the message to encode
 * @param[out] buffer Pointer to the destination buffer
 * @param[in] buffer_size Size of the destination buffer to prevent overflow
 * @return Status of encoding
 */
zanni_encode_status_e zanni_message_encode(const zanni_message_t* msg, uint8_t* buffer, size_t buffer_size);

/**
 * @brief Decodes a binary buffer into a message structure
 * @param[in] buffer Pointer to the source buffer containing the encoded data
 * @param[in] buffer_size Size of the source buffer 
 * @param[out] msg Pointer to the destination message structure
 * @return Status of decoding
 */
zanni_decode_status_e zanni_message_decode(const uint8_t* buffer, size_t buffer_size, zanni_message_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* ZANNI_PROTOCOL_H */
