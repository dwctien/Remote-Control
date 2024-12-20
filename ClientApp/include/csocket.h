#ifndef _CSOCKET_H_
#define _CSOCKET_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h> // For inet_pton
#include "html_generator.h"

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 8888
#define MAX_ALLOWED_SIZE (10 * 1024 * 1024) // 10 MB

using namespace std;

uint32_t receiveSize(SOCKET socket);

void receiveAll(SOCKET socket, char* buffer, size_t size);

void receiveData(SOCKET clientSocket, string& subject, string& mail_body, string& filename, vector<BYTE>& mail_data);

void runClient(string request, string server_ip, string& response_subject, string& response_body, string& filename, vector <BYTE>& response_data);

#endif // !_CSOCKET_H_