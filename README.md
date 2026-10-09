# zanni-protocol

Static C library for encoding and decoding messages according to the Zanni protocol. Used over TCP/IP in all ZanniGate projects.

## Message Format

Each message has a fixed size of 16 bytes and consists of:
- `tag` (8 bytes) — Text identifier (tag). If the string takes all 8 bytes, it is not null-terminated.
- `device_type` (4 bytes) — Device type (unsigned integer, encoded in Big-Endian).
- `status` (4 bytes) — Device status (unsigned integer, encoded in Big-Endian).

## Versioning

The library version is defined in `CMakeLists.txt` via `project(zanni-protocol VERSION x.y.z)`.
CMake automatically generates `zanni_version.h` providing:
- `ZANNI_PROTOCOL_VERSION_MAJOR`
- `ZANNI_PROTOCOL_VERSION_MINOR`
- `ZANNI_PROTOCOL_VERSION_PATCH`
- `ZANNI_PROTOCOL_VERSION_STRING`
- Runtime API: `const char* zanni_protocol_version(void);`

To append the version to the output static library file name (e.g. `zanni-protocol-0.1.0.lib`), pass `-DZANNI_VERSION_IN_OUTPUT_NAME=ON` to CMake.

## Build

### Using Makefile

A Makefile wrapper around CMake is provided:

```bash
# Build the project
make

# Or run specific targets
make config
make build
make clean
make rebuild
```

### Using CMake directly

```bash
cmake -B build
cmake --build build --config Release
```

## API Usage

```c
#include "zanni_protocol.h"

/* Encode */
uint8_t buf[ZANNI_MSG_SIZE];
zanni_message_t tx = { .tag = "DEVICE01", .device_type = 42, .status = 1 };
zanni_message_encode(&tx, buf, sizeof(buf));

/* Decode */
zanni_message_t rx;
zanni_message_decode(buf, sizeof(buf), &rx);

/* Get library version */
const char* ver = zanni_protocol_version();
```
