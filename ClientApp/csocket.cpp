#include "csocket.h"

uint32_t receiveSize(SOCKET socket) {
    uint32_t size = 0;
    int recvSize = recv(socket, reinterpret_cast<char*>(&size), sizeof(size), 0);
    if (recvSize == SOCKET_ERROR || recvSize != sizeof(size)) {
        cerr << "Failed to receive size. Code: " << WSAGetLastError() << "\n";
        return 0;
    }
    return ntohl(size);
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
    if (bodySize > 0) {
        char* buffer = new char[bodySize + 1];
        memset(buffer, 0, bodySize + 1);
        recv(clientSocket, buffer, bodySize, 0);
        mail_body.assign(buffer, bodySize);
        delete[] buffer;
    }

    // mail_data (including filename)
    uint32_t dataSize = receiveSize(clientSocket);
    if (dataSize > 0) {
        // Receive the filename length (4 bytes)
        uint32_t filenameLength = 0;
        int bytesReceived = recv(clientSocket, reinterpret_cast<char*>(&filenameLength), sizeof(filenameLength), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived < sizeof(filenameLength)) {
            cerr << "Error: Failed to receive filename length. Code: " << WSAGetLastError() << "\n";
            return;
        }
        // Ensure filenameLength is in correct byte order (big-endian)
        filenameLength = ntohl(filenameLength);

        // Receive the filename based on the received length
        if (filenameLength > 0) {
            char* buffer = new char[filenameLength + 1];
            memset(buffer, 0, filenameLength + 1);
            recv(clientSocket, buffer, filenameLength, 0);
            filename.assign(buffer, filenameLength);
            delete[] buffer;
        }
        // Receive the actual file data
        size_t totalReceived = 0;
        mail_data.resize(dataSize - sizeof(filenameLength) - filenameLength);
        while (totalReceived < mail_data.size()) {
            int recvSize = recv(clientSocket, reinterpret_cast<char*>(mail_data.data() + totalReceived), mail_data.size() - totalReceived, 0);
            if (recvSize == SOCKET_ERROR) {
                cerr << "Error: Failed to receive full binary data. Code: " << WSAGetLastError() << "\n";
                return;
            }
            totalReceived += recvSize;
        }
    }
}

bool saveVectorToFile(const string& filename, const vector<BYTE>& fileData) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << " for writing.\n";
        return false;
    }

    // Write the vector data to the file
    file.write(reinterpret_cast<const char*>(fileData.data()), fileData.size());
    file.close();
    return true;
}

void runClient(string request, string server_ip, string admin, string& response_subject, string& response_body, string& filename, vector <BYTE>& response_data) {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

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
    if (inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr) <= 0) {
        cout << "Invalid address or address not supported\n";
        response_subject = "Invalid address";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Connection to server failed\n";
        response_subject = "Fail to connect";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }
    cout << "Connected to server.\n";

    // Send request and admin's mail to server
    string message = "admin=" + admin + "&request=" + request;
    cout << "Data send to server: " << message << "\n";
    send(clientSocket, message.c_str(), message.size(), 0);

    receiveData(clientSocket, response_subject, response_body, filename, response_data);

    /*if (saveVectorToFile("screenshot.png", response_data)) {
        cout << "File saved successfully to " << "screen.png" << ".\n";
    }
    else {
        cerr << "Failed to save file.\n";
    }*/

    // Close the connection
    closesocket(clientSocket);
    WSACleanup();
}