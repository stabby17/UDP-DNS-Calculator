#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "UDP_protocol.h"
#include "Validation.h"

void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

void errorhandler(char* errorMessage) {
    printf("%s\n", errorMessage);
}

int main(int argc, char* argv[]) {
    // Initialize Winsock
#if defined WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Error at WSAStartup\n");
        return EXIT_FAILURE;
    }
#endif
    
    // Hostname and port set by default
    char hostname[BUFFER_SIZE] = "localhost";
    int port = PROTO_PORT;
    
    // If the user has inserted the hostname and the port
    if (argc == 2) {
        char delimiter[] = ":";
        char* token = strtok(argv[1], delimiter);
        strcpy(hostname, token);
        token = strtok(NULL, delimiter);
        port = atoi(token);
    }
    else {
        printf("Hostname and port not specified, using default values\n");
    }

    // Hostname resolution: get the server address
    struct hostent* host = gethostbyname(hostname);
    if (host == NULL) {
        printf("Error at gethostbyname\n");
        clearwinsock();
        return EXIT_FAILURE;
    }

    // Get the server IP address: hostname resolved to IP address
    struct in_addr* serverAddressIP = (struct in_addr*)host->h_addr_list[0];

    // Create socket
    int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket < 0) {
        printf("Error at socket()\n");
        clearwinsock();
        return EXIT_FAILURE;
    }

    // Initialize the server address structure
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr = *serverAddressIP;

    for (;;) {
        // Get the operation from the user
        operation op;
        char inputString[BUFFER_SIZE] = ""; // Store the string inserted by the user
        // Check if the string is in the correct format
        do {
            printf("Insert the string containing data (max 255 char) ['=' to exit] \n (Format: \"operator[space]num1[space]num2\"):\n");
            fgets(inputString, BUFFER_SIZE, stdin);
            inputString[strcspn(inputString, "\n")] = 0; // Remove the newline character

            // Exit if the user inserts "="
            if (strcmp(inputString, "=") == 0)
                return EXIT_SUCCESS;
        } while (lenght_check(strlen(inputString)) || string_check(inputString));

        // Parse the string
        sscanf(inputString, "%c %d %d", &op.operator, &op.first_number, &op.second_number);

        // Send the operation to the server
        int bytesSent = sendto(clientSocket, (char*)&op, sizeof(op), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if (bytesSent < 0) {
            printf("Error at sendto()\n");
            closesocket(clientSocket);
            clearwinsock();
            return EXIT_FAILURE;
        }

        // Receive the result from the server
        operation result; // Store the result in operation struct
        int serverAddressLength = sizeof(serverAddress);
        int bytesReceived = recvfrom(clientSocket, (char*)&result, sizeof(result), 0, (struct sockaddr*)&serverAddress, &serverAddressLength);
        if (bytesReceived < 0) {
            printf("Error at recvfrom()\n");
            closesocket(clientSocket);
            clearwinsock();
            return EXIT_FAILURE;
        }

        // Print the result
        printf("Result received from server %s, ip %s: \n%d %c %d = %d\n\n",
            host->h_name, inet_ntoa(*serverAddressIP), op.first_number, op.operator, op.second_number, result.first_number);
    }

    closesocket(clientSocket);
    clearwinsock();
    return EXIT_SUCCESS;

}
