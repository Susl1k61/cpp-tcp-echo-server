#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#pragma comment(lib, "Ws2_32.lib")

const int PORT = 54001;
const int BUFFER_SIZE = 1024;

bool sendAll(SOCKET socket, const std::string& message) {
    int totalSent = 0;
    int messageSize = static_cast<int>(message.size());

    while (totalSent < messageSize) {
        int sent = send(socket, message.c_str() + totalSent,
                        messageSize - totalSent, 0);
        if (sent == SOCKET_ERROR) return false;
        totalSent += sent;
    }
    return true;
}

std::string toUpperCase(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return text;
}

void removeClient(std::vector<SOCKET>& clients, size_t index) {
    closesocket(clients[index]);
    clients.erase(clients.begin() + static_cast<long long>(index));
}

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

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen error\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    u_long mode = 1;
    ioctlsocket(serverSocket, FIONBIO, &mode);

    std::vector<SOCKET> clients;

    std::cout << "Asynchronous Echo Server started on port " << PORT << "\n";
    std::cout << "Server can work with several clients at the same time.\n";

    while (true) {
        while (true) {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) break;

            ioctlsocket(clientSocket, FIONBIO, &mode);
            clients.push_back(clientSocket);

            std::cout << "Client connected. Total clients: "
                      << clients.size() << "\n";
        }

        for (size_t i = 0; i < clients.size();) {
            char buffer[BUFFER_SIZE];
            int bytesReceived = recv(clients[i], buffer, BUFFER_SIZE - 1, 0);

            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';

                std::string message(buffer);
                std::string response = toUpperCase(message);

                if (!sendAll(clients[i], response)) {
                    std::cout << "Send error. Client disconnected.\n";
                    removeClient(clients, i);
                    continue;
                }

                std::cout << "Client " << i + 1 << " sent: " << message;
                std::cout << "Client " << i + 1 << " received: " << response;
            }
            else if (bytesReceived == 0) {
                std::cout << "Client " << i + 1 << " disconnected.\n";
                removeClient(clients, i);
                continue;
            }
            else {
                int error = WSAGetLastError();
                if (error != WSAEWOULDBLOCK) {
                    std::cout << "Client " << i + 1
                              << " connection error. Client removed.\n";
                    removeClient(clients, i);
                    continue;
                }
            }

            ++i;
        }

        Sleep(10);
    }

    for (SOCKET client : clients) closesocket(client);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}