#include "include/ssocket.h"

void sendAll(SOCKET socket, const char* data, size_t size) {
    size_t totalSent = 0;
    while (totalSent < size) {
        int sent = send(socket, data + totalSent, size - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            cerr << "Error: Failed to send data. Code: " << WSAGetLastError() << "\n";
            return;
        }
        totalSent += sent;
    }
}

void sendData(SOCKET clientSocket, const string& subject, const string& mail_body, const vector<BYTE>& mail_data) {
    // Send the subject
    uint32_t subjectSize = htonl(subject.size());
    send(clientSocket, reinterpret_cast<const char*>(&subjectSize), sizeof(subjectSize), 0);
    send(clientSocket, subject.c_str(), subject.size(), 0);
    
    // Send the mail_body
    uint32_t bodySize = htonl(mail_body.size());
    sendAll(clientSocket, reinterpret_cast<const char*>(&bodySize), sizeof(bodySize));
    sendAll(clientSocket, mail_body.c_str(), mail_body.size()); 
    

    // Send the data (image or video) after the body
    if (!mail_data.empty()) {
        uint32_t dataSize = htonl(mail_data.size());
        sendAll(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        sendAll(clientSocket, reinterpret_cast<const char*>(mail_data.data()), mail_data.size()); 
    }
    else {
        uint32_t dataSize = 0;
        dataSize = htonl(dataSize);
        sendAll(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    }
}

void responder(SOCKET clientSocket, string received_data) {
    string request = received_data;

    cout << "Received request: " << request << "\n";
    cout << "Processing request...\n";

    Function func;
    vector<string> params;
    auto parse_result = parseRequest(request, func, params);

    string subject = "Reply for request: " + parse_result["command"];
    string mail_body;
    vector<BYTE> mail_data;

    if (!func || parse_result["msg"] != "Parse request successfully.") {
        mail_body = html_mail(request, html_msg(parse_result["msg"], false, true));
    }
    else {
        try {
            Response content = func(params);
            mail_body = html_mail(request, content.first);
            mail_data = content.second;

            // Check the size of attachment file
            const size_t MAX_ATTACHMENT_SIZE = 20 * 1024 * 1024; // 20 MB
            if (mail_data.size() > MAX_ATTACHMENT_SIZE) {
                mail_body = html_mail(request, html_msg("The attached file is too large to send via email.", false, false));
                mail_data.clear();
            }
        }
        catch (const exception& e) {
            mail_body = html_mail(request, html_msg("The format of arguments might be incorrect.", false, true));
        }
    }

    sendData(clientSocket, subject, mail_body, mail_data);
    cout << "Response sent.\n";
}

SOCKET initializeServerSocket() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return INVALID_SOCKET;
    }

    // Create a socket for the server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Configure server address and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the address
    // std::bind will call the function from the <functional> library, not from Winsock.
    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    cout << "Server is listening on port " << PORT << "...\n";

    return serverSocket;
}

SOCKET listenForClient(SOCKET serverSocket) {
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // Accept a connection from a client
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept failed\n";
        return INVALID_SOCKET;
    }

    cout << "Client connected.\n";
    return clientSocket;
}

void runServer() {
    // Initialize
    SOCKET serverSocket = initializeServerSocket();
    if (serverSocket == INVALID_SOCKET) {
        return;
    }
    
    // Listen for client to connect
    while (true) {  
        SOCKET clientSocket = listenForClient(serverSocket);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Error accepting client connection. Retrying..." << endl;
            continue;
        }
        char recvBuf[1024];
        int recvSize;

        recvSize = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);

        string received_data(recvBuf, recvSize);
        responder(clientSocket, received_data);
        closesocket(clientSocket);
        cout << "Client disconnected.\n\n";
    }

    closesocket(serverSocket);
    WSACleanup();
}