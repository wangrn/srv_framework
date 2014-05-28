/**
 * wangrn wang_rn@163.com
 * test
 */

#ifndef UTILS_H_
#define UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <iconv.h>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

const int SENDBUF_LEN = 1000000;
const int RECVBUF_LEN=100000;
const int MAX_PATH_LEN = 256;

int CreateUdpSocket(const char *ip, uint16_t port);

#endif
