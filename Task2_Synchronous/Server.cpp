#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

#pragma comment(lib, "Ws2_32.lib")

const int PORT = 54000;
const int BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup error\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation error\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "Bind error\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "Listen error\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Synchronous Echo Server started on port " << PORT << "\n";

    while (true) {
        std::cout << "Waiting for client...\n";
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept error\n";
            continue;
        }

        std::cout << "Client connected.\n";
        char buffer[BUFFER_SIZE];

        while (true) {
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

            if (bytesReceived <= 0) {
                std::cout << "Client disconnected.\n";
                break;
            }

            buffer[bytesReceived] = '\0';
            std::string message(buffer);

            std::transform(message.begin(), message.end(), message.begin(),
                [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

            send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);

            std::cout << "Received: " << buffer << "\n";
            std::cout << "Sent: " << message << "\n";
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}