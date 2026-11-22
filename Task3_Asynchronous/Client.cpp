#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

const int PORT = 54001;
const int BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup error\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation error\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cout << "Connection error\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to asynchronous server.\n";
    std::cout << "Enter message (exit - close):\n";

    std::string message;
    char buffer[BUFFER_SIZE];

    while (std::getline(std::cin, message)) {
        if (message == "exit") break;

        if (send(clientSocket, message.c_str(),
                 static_cast<int>(message.size()), 0) == SOCKET_ERROR) {
            std::cout << "Send error.\n";
            break;
        }

        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Server disconnected.\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << "Server: " << buffer << "\n";
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}