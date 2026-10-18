# zanni-protocol

Static C library for encoding and decoding messages according to the Zanni protocol. Used over TCP/IP in all ZanniGate projects.

## Message Format

Each message has a fixed size of 20 bytes and consists of:
- `tag` (8 bytes) — Text identifier (tag).
- `major_proto_ver` (4 bytes) — Major protocol version (unsigned integer, Big-Endian).
- `device_type` (4 bytes) — Device type (unsigned integer, Big-Endian).
- `payload` (4 bytes) — Device status / execution status (Big-Endian).

## Versioning

The library version is defined in `CMakeLists.txt` via `project(zanni-protocol VERSION x.y.z)`.
CMake automatically generates `zanni_version.h` providing:
- `ZANNI_PROTOCOL_VERSION_MAJOR`
- `ZANNI_PROTOCOL_VERSION_MINOR`
- `ZANNI_PROTOCOL_VERSION_PATCH`
- `ZANNI_PROTOCOL_VERSION_STRING`
- Runtime API: `const char* zanni_protocol_version(void);`

To append the version to the output static library file name (e.g. `zanni-protocol-0.2.0.lib`), pass `-DZANNI_VERSION_IN_OUTPUT_NAME=ON` to CMake.

## Build and Test

### Using Makefile

A Makefile wrapper around CMake is provided:

```bash
# Build the project
make

# Run tests
make test

# Or run specific targets
make config
make build
make test
make clean
make rebuild
```

### Using CMake directly

```bash
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## API Usage

```c
#include "zanni_protocol.h"

/* Encode */
uint8_t buf[ZANNI_MSG_SIZE];
zanni_message_t tx = {
    .major_proto_ver = ZANNI_PROTOCOL_VERSION_MAJOR,
    .tag = "DEVICE01",
    .device_type = ZANNI_VIRT_DEV_TOGGLE,
    .payload = { .dev_status = 1 }
};
zanni_message_encode(&tx, buf, sizeof(buf));

/* Decode */
zanni_message_t rx;
zanni_decode_status_e status = zanni_message_decode(buf, sizeof(buf), &rx);
if (status == ZANNI_DCD_STATUS_OK) {
    /* Message successfully decoded */
}

/* Get library version */
const char* ver = zanni_protocol_version();
```

## License

MIT © 2026 Ggorets0dev

