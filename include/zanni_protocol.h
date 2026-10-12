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
 * @brief Total size of the serialized protocol message in bytes (8 + 4 + 4 = 16 bytes).
 */
#define ZANNI_MSG_SIZE 16

/**
 * @brief Size of the message text tag field in bytes.
 */
#define ZANNI_TAG_SIZE 8

/**
 * @brief Virtual device types supported by the protocol.
 */
typedef enum zanni_virt_dev_e 
{
    ZANNI_VIRT_DEV_TOGGLE, /**< @brief Toggle virtual device. */
    ZANNI_VIRT_DEV_MAX     /**< @brief Upper bound / count of virtual device types. */
} zanni_virt_dev_e;

/**
 * @brief Protocol message data structure.
 */
typedef struct 
{
    char tag[ZANNI_TAG_SIZE];     /**< @brief 8-byte text tag. Not necessarily null-terminated if it takes all 8 bytes. */
    zanni_virt_dev_e device_type; /**< @brief 4-byte virtual device type. */
    uint32_t status;              /**< @brief 4-byte status / payload. */
} zanni_message_t;

#if defined(__cplusplus)
static_assert(sizeof(zanni_virt_dev_e) == sizeof(uint32_t), "zanni_virt_dev_e size is wrong");
static_assert(sizeof(zanni_message_t) == ZANNI_MSG_SIZE, "zanni_message_t size is wrong (ZANNI_MSG_SIZE)");
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(zanni_virt_dev_e) == sizeof(uint32_t), "zanni_virt_dev_e size is wrong");
_Static_assert(sizeof(zanni_message_t) == ZANNI_MSG_SIZE, "zanni_message_t size is wrong (ZANNI_MSG_SIZE)");
#endif

/**
 * @brief Returns the version string of the protocol library (e.g. "0.1.0").
 * 
 * @return Pointer to a static null-terminated version string.
 */
const char* zanni_protocol_version(void);

/**
 * @brief Encodes a message structure into a 16-byte binary buffer.
 * 
 * @param[in] msg Pointer to the message to encode.
 * @param[out] buffer Pointer to the destination buffer (must be at least 16 bytes).
 * @param[in] buffer_size Size of the destination buffer to prevent overflow.
 * @return 0 on success, -1 on invalid arguments or insufficient buffer size.
 */
int zanni_message_encode(const zanni_message_t* msg, uint8_t* buffer, size_t buffer_size);

/**
 * @brief Decodes a 16-byte binary buffer into a message structure.
 * 
 * @param[in] buffer Pointer to the source buffer containing the encoded data.
 * @param[in] buffer_size Size of the source buffer (must be at least 16 bytes).
 * @param[out] msg Pointer to the destination message structure.
 * @return 0 on success, -1 on invalid arguments or insufficient buffer size.
 */
int zanni_message_decode(const uint8_t* buffer, size_t buffer_size, zanni_message_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* ZANNI_PROTOCOL_H */
