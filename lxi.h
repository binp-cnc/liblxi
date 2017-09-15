#ifndef _LXI_H_
#define _LXI_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define LXI_ERROR_RESOLVE_HOST     -0x1010
#define LXI_ERROR_SOCKET_OPEN      -0x1110
#define LXI_ERROR_SOCKET_CONNECT   -0x1111
#define LXI_ERROR_SOCKET_READ      -0x1112
#define LXI_ERROR_SOCKET_WRITE     -0x1113


typedef struct LXI_Device {
	struct sockaddr_in addr;
	int sockfd;
} LXI_Device;

int LXI_DeviceCreate(LXI_Device *device, const char *hostname, int port);
int LXI_DeviceDestroy(LXI_Device *device);

int LXI_Send(LXI_Device *device, const char *request, int req_size);
int LXI_Receive(LXI_Device *device, char *response_buffer, int resbuf_size);
int LXI_Evaluate(LXI_Device *device, const char *request, int req_size, char *response_buffer, int resbuf_size);

#endif // _LXI_H_
