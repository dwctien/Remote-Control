#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void runServer(short port);

#endif // !_REQUEST_HANDLER_H_
