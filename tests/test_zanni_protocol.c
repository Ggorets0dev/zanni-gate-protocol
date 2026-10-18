#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "zanni_protocol.h"
#include "zanni_version.h"

/** @brief Counter of total executed tests. */
static int tests_run = 0;

/** @brief Counter of failed tests. */
static int tests_failed = 0;

#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        printf("\n  [FAIL] %s:%d: Assertion '%s' failed\n", __FILE__, __LINE__, #cond); \
        return 1; \
    } \
} while (0)

#define RUN_TEST(fn) do { \
    printf("  [RUN]  %-38s", #fn); \
    tests_run++; \
    if (fn() == 0) { \
        printf(" [OK]\n"); \
    } else { \
        tests_failed++; \
    } \
} while (0)

/**
 * @brief Tests retrieval of the protocol library version string and version constants.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_version(void)
{
    const char* ver = zanni_protocol_version();
    TEST_ASSERT(ver != NULL);
    TEST_ASSERT(strcmp(ver, ZANNI_PROTOCOL_VERSION_STRING) == 0);
    TEST_ASSERT(strlen(ver) > 0);
    TEST_ASSERT(ZANNI_PROTOCOL_VERSION_MAJOR >= 0);
    return 0;
}

/**
 * @brief Verifies protocol structure sizes and constant values.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_sizes_and_layout(void)
{
    TEST_ASSERT(ZANNI_MSG_SIZE == 20);
    TEST_ASSERT(ZANNI_TAG_SIZE == 8);
    TEST_ASSERT(sizeof(zanni_message_t) == ZANNI_MSG_SIZE);
    TEST_ASSERT(sizeof(zanni_virt_dev_e) == sizeof(uint32_t));
    TEST_ASSERT(sizeof(zanni_msg_payload_u) == sizeof(uint32_t));
    return 0;
}

/**
 * @brief Tests NULL argument and buffer boundary handling in zanni_message_encode.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_encode_null_and_bounds(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t msg;
    memset(&msg, 0, sizeof(msg));

    /* NULL pointers */
    TEST_ASSERT(zanni_message_encode(NULL, buf, sizeof(buf)) == -1);
    TEST_ASSERT(zanni_message_encode(&msg, NULL, sizeof(buf)) == -1);

    /* Buffer size too small */
    TEST_ASSERT(zanni_message_encode(&msg, buf, 0) == -1);
    TEST_ASSERT(zanni_message_encode(&msg, buf, ZANNI_MSG_SIZE - 1) == -1);

    /* Valid buffer size */
    TEST_ASSERT(zanni_message_encode(&msg, buf, ZANNI_MSG_SIZE) == 0);
    TEST_ASSERT(zanni_message_encode(&msg, buf, ZANNI_MSG_SIZE + 10) == 0);

    return 0;
}

/**
 * @brief Tests NULL argument and buffer boundary handling in zanni_message_decode.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_decode_null_and_bounds(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t msg;
    memset(buf, 0, sizeof(buf));

    /* NULL pointers */
    TEST_ASSERT(zanni_message_decode(NULL, sizeof(buf), &msg) == ZANNI_DCD_STATUS_ERROR);
    TEST_ASSERT(zanni_message_decode(buf, sizeof(buf), NULL) == ZANNI_DCD_STATUS_ERROR);

    /* Buffer size too small */
    TEST_ASSERT(zanni_message_decode(buf, 0, &msg) == ZANNI_DCD_STATUS_ERROR);
    TEST_ASSERT(zanni_message_decode(buf, ZANNI_MSG_SIZE - 1, &msg) == ZANNI_DCD_STATUS_ERROR);

    return 0;
}

/**
 * @brief Tests complete encode and decode roundtrip with device status payload.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_encode_decode_roundtrip(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t tx;
    zanni_message_t rx;

    memset(&tx, 0, sizeof(tx));
    memset(&rx, 0, sizeof(rx));

    tx.major_proto_ver = ZANNI_PROTOCOL_VERSION_MAJOR;
    memcpy(tx.tag, "DEVICE1", 8);
    tx.device_type = ZANNI_VIRT_DEV_TOGGLE;
    tx.payload.dev_status = 0x12345678;

    TEST_ASSERT(zanni_message_encode(&tx, buf, sizeof(buf)) == 0);

    zanni_decode_status_e status = zanni_message_decode(buf, sizeof(buf), &rx);
    TEST_ASSERT(status == ZANNI_DCD_STATUS_OK);
    TEST_ASSERT(rx.major_proto_ver == tx.major_proto_ver);
    TEST_ASSERT(strncmp(rx.tag, "DEVICE1", ZANNI_TAG_SIZE) == 0);
    TEST_ASSERT(rx.device_type == tx.device_type);
    TEST_ASSERT(rx.payload.dev_status == 0x12345678);

    return 0;
}

/**
 * @brief Tests complete encode and decode roundtrip with execution status payload.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_encode_decode_payload_exec_status(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t tx;
    zanni_message_t rx;

    memset(&tx, 0, sizeof(tx));
    memset(&rx, 0, sizeof(rx));

    tx.major_proto_ver = ZANNI_PROTOCOL_VERSION_MAJOR;
    memcpy(tx.tag, "EXEC_CMD", 8);
    tx.device_type = ZANNI_VIRT_DEV_TOGGLE;
    tx.payload.exec_status = ZANNI_EXEC_STATUS_ERROR;

    TEST_ASSERT(zanni_message_encode(&tx, buf, sizeof(buf)) == 0);

    zanni_decode_status_e status = zanni_message_decode(buf, sizeof(buf), &rx);
    TEST_ASSERT(status == ZANNI_DCD_STATUS_OK);
    TEST_ASSERT(rx.payload.exec_status == ZANNI_EXEC_STATUS_ERROR);

    tx.payload.exec_status = ZANNI_EXEC_STATUS_OK;
    TEST_ASSERT(zanni_message_encode(&tx, buf, sizeof(buf)) == 0);
    status = zanni_message_decode(buf, sizeof(buf), &rx);
    TEST_ASSERT(status == ZANNI_DCD_STATUS_OK);
    TEST_ASSERT(rx.payload.exec_status == ZANNI_EXEC_STATUS_OK);

    return 0;
}

/**
 * @brief Verifies exact Big-Endian byte layout in serialized binary buffer.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_binary_layout_big_endian(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t msg;
    memset(&msg, 0, sizeof(msg));
    memset(buf, 0xFF, sizeof(buf));

    msg.major_proto_ver = 0x01020304;
    memcpy(msg.tag, "ABCDEFG", 7);
    msg.device_type = (zanni_virt_dev_e)0x05060708;
    msg.payload.dev_status = 0x090A0B0C;

    TEST_ASSERT(zanni_message_encode(&msg, buf, sizeof(buf)) == 0);

    /* Tag check (bytes 0..6: "ABCDEFG", byte 7: '\0') */
    TEST_ASSERT(memcmp(buf, "ABCDEFG\0", 8) == 0);

    /* major_proto_ver (bytes 8..11, Big Endian) */
    TEST_ASSERT(buf[8]  == 0x01);
    TEST_ASSERT(buf[9]  == 0x02);
    TEST_ASSERT(buf[10] == 0x03);
    TEST_ASSERT(buf[11] == 0x04);

    /* device_type (bytes 12..15, Big Endian) */
    TEST_ASSERT(buf[12] == 0x05);
    TEST_ASSERT(buf[13] == 0x06);
    TEST_ASSERT(buf[14] == 0x07);
    TEST_ASSERT(buf[15] == 0x08);

    /* payload (bytes 16..19, Big Endian) */
    TEST_ASSERT(buf[16] == 0x09);
    TEST_ASSERT(buf[17] == 0x0A);
    TEST_ASSERT(buf[18] == 0x0B);
    TEST_ASSERT(buf[19] == 0x0C);

    return 0;
}

/**
 * @brief Tests handling of major protocol version mismatch during decode.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_decode_version_mismatch(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t tx;
    zanni_message_t rx;

    memset(&tx, 0, sizeof(tx));
    memset(&rx, 0, sizeof(rx));

    /* Encode with a version different from ZANNI_PROTOCOL_VERSION_MAJOR */
    tx.major_proto_ver = ZANNI_PROTOCOL_VERSION_MAJOR + 42;
    memcpy(tx.tag, "VER_FAIL", 8);
    tx.device_type = ZANNI_VIRT_DEV_TOGGLE;
    tx.payload.dev_status = 1;

    TEST_ASSERT(zanni_message_encode(&tx, buf, sizeof(buf)) == 0);

    zanni_decode_status_e status = zanni_message_decode(buf, sizeof(buf), &rx);
    TEST_ASSERT(status == ZANNI_DCD_STATUS_WRONG_VER);
    TEST_ASSERT(rx.major_proto_ver == ZANNI_PROTOCOL_VERSION_MAJOR + 42);

    return 0;
}

/**
 * @brief Tests tag handling and null-termination in buffer and decoded message.
 * 
 * @return 0 on success, 1 on assertion failure.
 */
static int test_tag_null_termination(void)
{
    uint8_t buf[ZANNI_MSG_SIZE];
    zanni_message_t tx;
    zanni_message_t rx;

    memset(&tx, 0, sizeof(tx));
    memset(&rx, 0, sizeof(rx));

    /* Fill all 8 bytes of tag with non-null chars */
    memcpy(tx.tag, "12345678", 8);
    tx.major_proto_ver = ZANNI_PROTOCOL_VERSION_MAJOR;

    TEST_ASSERT(zanni_message_encode(&tx, buf, sizeof(buf)) == 0);
    /* 8th byte in buffer must be null-terminated */
    TEST_ASSERT(buf[ZANNI_TAG_SIZE - 1] == '\0');
    TEST_ASSERT(strncmp((const char*)buf, "1234567", 7) == 0);

    TEST_ASSERT(zanni_message_decode(buf, sizeof(buf), &rx) == ZANNI_DCD_STATUS_OK);
    TEST_ASSERT(rx.tag[ZANNI_TAG_SIZE - 1] == '\0');

    return 0;
}

int main(void)
{
    printf("=== Zanni Protocol Test Suite ===\n");

    RUN_TEST(test_version);
    RUN_TEST(test_sizes_and_layout);
    RUN_TEST(test_encode_null_and_bounds);
    RUN_TEST(test_decode_null_and_bounds);
    RUN_TEST(test_encode_decode_roundtrip);
    RUN_TEST(test_encode_decode_payload_exec_status);
    RUN_TEST(test_binary_layout_big_endian);
    RUN_TEST(test_decode_version_mismatch);
    RUN_TEST(test_tag_null_termination);

    printf("\n=== Summary: %d/%d tests passed", tests_run - tests_failed, tests_run);
    if (tests_failed > 0) {
        printf(" (%d FAILED) ===\n", tests_failed);
        return 1;
    }
    printf(" ===\n");
    return 0;
}
