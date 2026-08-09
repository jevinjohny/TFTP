# TFTP File Transfer

## Protocol-Based UDP File Transfer
A lightweight C project demonstrating a simple TFTP-style file transfer system using UDP. The implementation includes a command-driven client and a server that handles read and write requests.

---

## Key Features

- Client/server communication over UDP
- Supports basic TFTP opcodes: RRQ, WRQ, DATA, ACK
- Default server port: `6969`
- Client commands: `connect`, `get`, `put`, `disconnect`
- Shared helper logic in `common/` for packet handling and file transfer

---

## System Overview

### 1. Client
- Provides a command prompt for connect, download, upload, and disconnect operations
- Uses UDP sockets with timeouts

### 2. Server
- Listens for incoming TFTP requests
- Processes read and write requests, then calls send/receive routines

### 3. Common Code
- Implements file send and receive behavior
- Defines packet structures, opcodes, and transfer constants

---

## Requirements

- GCC or compatible C compiler
- POSIX-compatible terminal environment

## Build

From `client/`:

```bash
cd client
make
```

From `server/`:

```bash
cd server
make
```

---

## Main Files

- `client/tftp_client.c` — client command interface
- `client/tftp_client.h` — client declarations
- `server/tftp_server.c` — server main loop and request handler
- `common/tftp.c` — shared transfer functions
- `common/tftp.h` — packet definitions and constants
