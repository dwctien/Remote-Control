#include "include/csocket.h"

uint32_t receiveSize(SOCKET socket) {
    uint32_t size = 0;
    int recvSize = recv(socket, reinterpret_cast<char*>(&size), sizeof(size), 0);
    if (recvSize == SOCKET_ERROR || recvSize != sizeof(size)) {
        cerr << "Failed to receive size. Code: " << WSAGetLastError() << "\n";
        return 0;
    }
    return ntohl(size);
}

void receiveAll(SOCKET socket, char* buffer, size_t size) {
    size_t totalReceived = 0;
    while (totalReceived < size) {
        int received = recv(socket, buffer + totalReceived, size - totalReceived, 0);
        if (received == SOCKET_ERROR || received == 0) {
            cerr << "Error: Failed to receive data. Code: " << WSAGetLastError() << "\n";
            return;
        }
        totalReceived += received;
    }
}

void receiveData(SOCKET clientSocket, string& subject, string& mail_body, string& filename, vector<BYTE>& mail_data) {
    // subject
    uint32_t subjectSize = receiveSize(clientSocket);
    if (subjectSize > 0) {
        char* buffer = new char[subjectSize + 1];
        memset(buffer, 0, subjectSize + 1);
        recv(clientSocket, buffer, subjectSize, 0);
        subject.assign(buffer, subjectSize);
        delete[] buffer;
    }

    // mail_body
    uint32_t bodySize = receiveSize(clientSocket);
    cout << "size: " << bodySize << "\n";
    if (bodySize > 0) {
        char* buffer = new char[bodySize + 1];
        memset(buffer, 0, bodySize + 1);
        receiveAll(clientSocket, buffer, bodySize);
        mail_body.assign(buffer, bodySize);
        delete[] buffer;
    }


    // mail_data (including filename)
    uint32_t dataSize = receiveSize(clientSocket);
    if (dataSize == 0) {
        return;
    }

    // Check reasonable size limit
    const uint32_t MAX_DATA_SIZE = 50 * 1024 * 1024; // 50 MB
    if (dataSize > MAX_DATA_SIZE) {
        cerr << "Error: Data size exceeds limit\n";
        return;
    }

    // Get filename length
    uint32_t filenameLength = 0;
    receiveAll(clientSocket, reinterpret_cast<char*>(&filenameLength), sizeof(filenameLength));
    filenameLength = ntohl(filenameLength); // Convert to correct byte order

    // Check filename size limit
    const uint32_t MAX_FILENAME_LENGTH = 256; // Limit filename to 256 characters
    if (filenameLength > MAX_FILENAME_LENGTH) {
        cerr << "Error: Filename length exceeds limit\n";
        return;
    }

    // Get filename
    if (filenameLength > 0) {
        char* buffer = new char[filenameLength + 1];
        memset(buffer, 0, filenameLength + 1);
        receiveAll(clientSocket, buffer, filenameLength);
        filename.assign(buffer, filenameLength);
        delete[] buffer;
    }

    // Get the actual data (file)
    size_t fileDataSize = dataSize - sizeof(filenameLength) - filenameLength;
    if (fileDataSize > 0) {
        mail_data.resize(fileDataSize);
        receiveAll(clientSocket, reinterpret_cast<char*>(mail_data.data()), fileDataSize);
    }
}

void runClient(string request, string server_ip, string& response_subject, string& response_body, string& filename, vector <BYTE>& response_data) {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        response_subject = "Reply for request: " + request;
        response_body = html_mail(request, html_msg("WSAStartup failed.", false, false));
        return;
    }

    // Create a socket for the client
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        response_subject = "Reply for request: " + request;
        response_body = html_mail(request, html_msg("Socket creation failed.", false, false));
        WSACleanup();
        return;
    }

    // Configure server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Use inet_pton instead of inet_addr
    if (inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr) <= 0) {
        response_subject = "Reply for request: " + request;
        response_body = html_mail(request, html_msg("Invalid IP address. Make sure the server is on and the server's IP address is correct.", false, false));
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        response_subject = "Reply for request: " + request;
        response_body = html_mail(request, html_msg("Connection to server failed. Make sure the server is on and the server's IP address is correct.", false, false));
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    cout << "Connected to server.\n";

    send(clientSocket, request.c_str(), request.size(), 0);

    receiveData(clientSocket, response_subject, response_body, filename, response_data);

    // Close the connection
    closesocket(clientSocket);
    WSACleanup();
}