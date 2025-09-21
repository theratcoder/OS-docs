# RatOS Networking Specification
## Overview

RatOS provides a networking API inspired by BSD sockets and WinSock, unified under the RatAPI naming conventions.
The goal is to offer developers a familiar yet cohesive API for socket programming, while preparing the ground for higher-level abstractions (e.g., event integration, async models).

---

## Design Principles

* Familiarity: Developers experienced with either BSD sockets or WinSock should feel at home.

* Consistency: All functions follow RatAPI naming conventions (RatXxxYyy).

* Extensibility: Designed to integrate with RatOS’s event loop, async I/O, and IPC subsystems.

* Simplicity: Provide a thin but consistent layer; avoid bloating RatAPI with high-level protocols.

---

## Core Concepts

* Sockets: Primary handle for network communication.

* Addressing: IPv4 and IPv6 supported, with extensibility for future protocols.

* Blocking vs Non-blocking: Both modes supported, with event-loop integration for async usage.

* Events: Sockets can post messages (e.g., WM_SOCKET) into message queues for integration with GUI/message loop models.

---

## API Overview
### Types
```c
typedef int RatSocket;          // Socket handle (opaque)
typedef enum {
    RAT_SOCKET_STREAM,          // TCP
    RAT_SOCKET_DGRAM,           // UDP
    RAT_SOCKET_RAW              // Raw socket
} RatSocketType;

typedef enum {
    RAT_SOCKET_FAMILY_IPV4,
    RAT_SOCKET_FAMILY_IPV6
} RatSocketFamily;

typedef struct {
    RatSocketFamily family;
    unsigned short port;
    char address[64];           // Textual representation (IPv4/IPv6)
} RatSocketAddress;

typedef enum {
    RAT_STATUS_OK,
    RAT_STATUS_ERROR,
    RAT_STATUS_WOULD_BLOCK,
    RAT_STATUS_CLOSED
} RatStatus;
```

### Functions
```c
RatSocket   RatSocketCreate(RatSocketFamily family, RatSocketType type);
RatStatus   RatSocketBind(RatSocket sock, const RatSocketAddress* addr);
RatStatus   RatSocketListen(RatSocket sock, int backlog);
RatSocket   RatSocketAccept(RatSocket sock, RatSocketAddress* addr_out);
RatStatus   RatSocketConnect(RatSocket sock, const RatSocketAddress* addr);
RatStatus   RatSocketSend(RatSocket sock, const void* buf, size_t len, size_t* sent);
RatStatus   RatSocketRecv(RatSocket sock, void* buf, size_t len, size_t* recvd);
RatStatus   RatSocketClose(RatSocket sock);

// Optional integration with message/event loops
RatStatus   RatSocketAttachToEventLoop(RatSocket sock, RatHandle eventLoop);
RatStatus   RatSocketSetNonBlocking(RatSocket sock, bool enable);
```

### Events

When attached to a message loop or async system, sockets can generate messages:
```c
WM_SOCKET_CONNECT

WM_SOCKET_ACCEPT

WM_SOCKET_READ

WM_SOCKET_WRITE

WM_SOCKET_CLOSE
```
Handlers can process these in the same manner as window messages.

---

## Future Extensions

* Async I/O wrappers (`RatSocketSendAsync`, `RatSocketRecvAsync`).

* TLS/SSL layer via external libraries.

* Multicast, broadcast, and advanced socket options.

* High-level networking libraries (HTTP, FTP) layered on top of RatAPI.

---

*RatOS Networking — version 0.1-draft*