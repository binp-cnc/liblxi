#include "lxi.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int LXI_DeviceCreate(LXI_Device *device, const char *hostname, int port) {
	struct hostent *host;
	int ret = 0;

	host = gethostbyname(hostname);
	if (host == NULL) {
#ifdef LXI_PRINT_ERRORS
		perror("LXI_ERROR_RESOLVE_HOST");
#endif
		ret = LXI_ERROR_RESOLVE_HOST;
		goto quit;
	}

	bzero(&device->addr, sizeof(device->addr));
	device->addr.sin_family = AF_INET;

	bcopy(host->h_addr, &device->addr.sin_addr.s_addr, host->h_length);
	device->addr.sin_port = htons(port);

	device->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (device->sockfd < 0) {
#ifdef LXI_PRINT_ERRORS
		perror("LXI_ERROR_SOCKET_OPEN");
#endif
		ret = LXI_ERROR_SOCKET_OPEN;
		goto quit;
	}
	
	if (connect(device->sockfd, (struct sockaddr *) &device->addr, sizeof(device->addr)) < 0) {
#ifdef LXI_PRINT_ERRORS
		perror("LXI_ERROR_SOCKET_CONNECT");
#endif
		ret = LXI_ERROR_SOCKET_CONNECT;
		goto close_socket;
	}

	return ret;

close_socket:
	close(device->sockfd);
	device->sockfd = -1;
quit:
	return ret;
}

int LXI_DeviceDestroy(LXI_Device *device) {
	if (device->sockfd >= 0) {
		close(device->sockfd);
	}
	return 0;
}

int LXI_Send(LXI_Device *device, const char *request, int req_size) {
	int len;
	int nbytes = 0;

	while (nbytes < req_size) {
		len = write(device->sockfd, request + nbytes, req_size - nbytes);
		if (len < 0) {
#ifdef LXI_PRINT_ERRORS
			perror("LXI_ERROR_SOCKET_WRITE");
#endif
			return LXI_ERROR_SOCKET_WRITE;
		}
		nbytes += len;
	}
	if (req_size > 0 && request[req_size - 1] != '\n') {
		do {
			len = write(device->sockfd, "\n", 1);
			if (len < 0) {
#ifdef LXI_PRINT_ERRORS
				perror("LXI_ERROR_SOCKET_WRITE");
#endif
				return LXI_ERROR_SOCKET_WRITE;
			}
		} while (len <= 0);
	}
	return 0;
}

int LXI_Receive(LXI_Device *device, char *response_buffer, int resbuf_size) {
	int len;
	int nbytes = 0;

	while (nbytes < resbuf_size) {
		len = read(device->sockfd, response_buffer + nbytes, resbuf_size - nbytes);
		if (len < 0) {
#ifdef LXI_PRINT_ERRORS
			perror("LXI_ERROR_SOCKET_READ");
#endif
			return LXI_ERROR_SOCKET_READ;
		}
		nbytes += len;
		if (response_buffer[nbytes - 1] == '\n') {
			response_buffer[nbytes - 1] = '\0';
			return nbytes - 1;
		}
	}
	return resbuf_size;
}

int LXI_Evaluate(LXI_Device *device, const char *request, int req_size, char *response_buffer, int resbuf_size) {
	int s = 0;
	int has_response = 0;

	s = LXI_Send(device, request, req_size);
	if (s < 0) {
		return s;
	}

	if ((req_size > 0 && request[req_size - 1] == '?') || (req_size > 1 && request[req_size - 2] == '?')) {
		has_response = 1;
	}
	if (!has_response) {
		return 0;
	}

	s = LXI_Receive(device, response_buffer, resbuf_size);

	return s;
}
