#ifndef _CSOCKET_H_
#define _CSOCKET_H_

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // For inet_pton

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define SERVER_IP "127.0.0.1" // Server IP address
#define PORT 8888

using namespace std;

void runClient(string request, string ip_addr) {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char recvBuf[1024];
    string dataToSend = request + "|" + ip_addr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return;
    }

    // Create a socket for the client
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        WSACleanup();
        return;
    }

    // Configure server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Use inet_pton instead of inet_addr
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        cout << "Invalid address or address not supported\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Connection to server failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }
    cout << "Connected to server.\n";

    // Send a message to the server
    send(clientSocket, dataToSend.c_str(), dataToSend.size(), 0);

    // Receive response from the server
    int recvSize = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);
    if (recvSize == SOCKET_ERROR) {
        cout << "Receive failed\n";
    }
    else {
        recvBuf[recvSize] = '\0';
        cout << "Received from server: " << recvBuf << "\n";
    }

    // Close the connection
    closesocket(clientSocket);
    WSACleanup();
}

#endif // !_CSOCKET_H_