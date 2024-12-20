#ifndef _SSOCKET_H_
#define _SSOCKET_H_

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // For inet_pton
#include <fstream>
#include "request_handle.h"

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8888

void sendAll(SOCKET socket, const char* data, size_t size);

void sendData(SOCKET clientSocket, const string& subject, const string& mailBody, const vector<BYTE>& data);

void responder(SOCKET clientSocket, string received_data);

SOCKET initializeServerSocket();

SOCKET listenForClient(SOCKET serverSocket);

void runServer();

#endif // !_SSOCKET_H_