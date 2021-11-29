/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : client.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.4
 * - Date : Nov. 29, 2021.
 * - Description : client header file
 * **************************************************************************************************/

#include "user.h"
#include "dept.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <cstring>

#define MAX_LEN 1000

