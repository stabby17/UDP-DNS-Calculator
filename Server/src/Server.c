#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "UDP_protocol.h"

void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

void errorhandler(char* errorMessage) {
    printf("%s\n", errorMessage);
}

int add(int first_number, int second_number) { return first_number + second_number; }
int sub(int first_number, int second_number) { return first_number - second_number; }
int mult(int first_number, int second_number) { return first_number * second_number; }
double division(int first_number, int second_number) { return (double)first_number / second_number; }

int main(void) {
    // Initialize Winsock
#if defined WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Error at WSAStartup\n");
        return EXIT_FAILURE;
    }
#endif

    // Create socket
    int serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket < 0) {
        errorhandler("Error at socket()\n");
        clearwinsock();
        return EXIT_FAILURE;
    }

    // Initialize the server address structure
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PROTO_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);


    if ((bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) < 0) {
        errorhandler("Error at bind()\n");
        closesocket(serverSocket);
        clearwinsock();
        return EXIT_FAILURE;
    }

    printf("Server started on address %s, port %d\n", ADDRESS, PROTO_PORT);

    // Receive the data from the client
    struct sockaddr_in clientAddress;
    int clientAddressLength = sizeof(clientAddress);
    operation receivedOperation;
    for (;;) {
        printf("Waiting for data from client...\n");

        int bytesReceived = recvfrom(serverSocket, (char*)&receivedOperation, sizeof(receivedOperation), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (bytesReceived < 0) {
            printf("Error at recvfrom()\n");
            closesocket(serverSocket);
            clearwinsock();
            return EXIT_FAILURE;
        }

        // // Print the data received from the client and the client information
        struct hostent* remotehost = gethostbyaddr((char*)&clientAddress.sin_addr, 4, AF_INET);
        printf("\nOperation request from client %s, ip %s\n", remotehost->h_name, inet_ntoa(clientAddress.sin_addr));
        printf("Operation: %d %c %d\n", receivedOperation.first_number, receivedOperation.operator, receivedOperation.second_number);

        // Send the result back to the client
        int result = 0;
        switch (receivedOperation.operator) {
            case '+':
                result = add(receivedOperation.first_number, receivedOperation.second_number);
                break;
            case '-':
                result = sub(receivedOperation.first_number, receivedOperation.second_number);
                break;
            case '*':
            case 'x':
            case 'X':
                result = mult(receivedOperation.first_number, receivedOperation.second_number);
                break;
            case '/':
                result = division(receivedOperation.first_number, receivedOperation.second_number);
                break;
        }

        // Store the result in the operation structure
        operation resultOperation;
        resultOperation.first_number = result;

        // Send the result back to the client
        int bytesSent = sendto(serverSocket, (char*)&resultOperation, sizeof(resultOperation), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
        if (bytesSent < 0) {
            errorhandler("Error at sendto()\n");
            closesocket(serverSocket);
            clearwinsock();
            return EXIT_FAILURE;
        }

        printf("Result %d sent to client\n\n", result);
    }

    closesocket(serverSocket);
    clearwinsock();
    return EXIT_SUCCESS;
}