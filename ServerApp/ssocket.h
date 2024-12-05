#ifndef _SSOCKET_H_
#define _SSOCKET_H_

#include <winsock2.h>
#include <iostream>
#include <ws2tcpip.h> // For inet_pton

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8888

using namespace std;

void runServer() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return;
    }

    // Create a socket for the server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    // Configure server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    cout << "Server is listening on port " << PORT << "...\n";

    // Accept a connection from a client
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    cout << "Client connected.\n";

    char recvBuf[1024];
    int recvSize;

    // Receive data from the client
    recvSize = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);
    if (recvSize == SOCKET_ERROR) {
        cout << "Receive failed\n";
    }
    else {
        recvBuf[recvSize] = '\0'; // Null-terminate the received data
        cout << "Received from client: " << recvBuf << "\n";

        // Echo the message back to the client
        send(clientSocket, recvBuf, recvSize, 0);
    }

    // Close the connections
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

#endif // !_SSOCKET_H_