#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

#define NO_SYS                            1  /* Required by CYW43_LWIP */
// #define LWIP_ALTCP                        0  /* No TLS or SSL layer/security */
// #define LWIP_ALTCP_TLS                    0  /* No TLS or SSL layer/security */

#define LWIP_TIMEVAL_PRIVATE              0   /* Use the system-wide timeval definitions */
// #define LWIP_SOCKET                       1
// #define LWIP_COMPAT_SOCKETS               1
// #define LWIP_POSIX_SOCKETS_IO_NAMES       0     /* Causes conflicts with normal file operations */
// #define LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 2000  /* Speed up socket close (alternative to enabling SO_LINGER) - default is 20 secs */

#endif