#include "user.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <cstring>

#define SERVER_PORT 32032
#define MAX_LEN 200

