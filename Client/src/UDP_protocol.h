#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define ADDRESS "127.0.0.1"
#define PROTO_PORT 56700
#define BUFFER_SIZE 255

typedef struct {
	char operator;
	int first_number;
	int second_number;
} operation;

#endif /* PROTOCOL_H_ */
