# zanni-protocol

Static C library for encoding and decoding messages according to the Zanni protocol.

## Message Format

Each message has a fixed size of 16 bytes and consists of:
- `tag` (8 bytes) — Text identifier (tag). If the string takes all 8 bytes, it is not null-terminated.
- `device_type` (4 bytes) — Device type (unsigned integer, encoded in Big-Endian).
- `status` (4 bytes) — Device status (unsigned integer, encoded in Big-Endian).

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

Include the header file `zanni_protocol.h` in your project.

### Encoding

```c
#include "zanni_protocol.h"
#include <stdio.h>
#include <string.h>

int main(void) 
{
    zanni_message_t msg;
    memset(&msg, 0, sizeof(zanni_message_t));
    strncpy(msg.tag, "MYDEVICE", ZANNI_TAG_SIZE);
    msg.device_type = 123;
    msg.status = 1;

    uint8_t buffer[ZANNI_MSG_SIZE];
    if (zanni_message_encode(&msg, buffer, sizeof(buffer)) == 0) 
    {
        printf("Successfully encoded!\n");
    }

    return 0;
}
```

### Decoding

```c
#include "zanni_protocol.h"
#include <stdio.h>

void process_data(const uint8_t* buffer, size_t size) 
{
    zanni_message_t msg;

    if (zanni_message_decode(buffer, size, &msg) == 0) 
    {
        /* Successfully decoded, inspect msg fields */
    }
}
```
