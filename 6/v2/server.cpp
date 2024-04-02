#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define BUFF_SIZE 1024

std::vector<SOCKET> clients;

DWORD WINAPI ClientThread(LPVOID lpParam) {
	SOCKET clientSocket = (SOCKET)lpParam;
	char buffer[1024];
	while (true) {
		int bytesReceived = recv(clientSocket, buffer, 1024, 0);
		if (bytesReceived <= 0 || buffer[0] == 'x' && buffer[1] == 'x' && buffer[2] == 'x') {
			closesocket(clientSocket);
			break;
		}

		for (SOCKET otherClientSocket : clients) {
			if (otherClientSocket != clientSocket) {
				send(otherClientSocket, buffer, bytesReceived, 0);
			}
		}
	}

	clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
	return 0;
}

void processError(std::string message, int errorCode) {
	std::cout << message << ". Error " << errorCode << std::endl;
}

int main(int argc, char** argv) {
	const char SERV_IP[] = "127.0.0.1"; // localhost
	const int PORT_NUM = 8000;
	int iResult;

	WSADATA wsData;
	SOCKET serverSocket = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (iResult != NO_ERROR) {
		processError("Couldn't initialize WinSock", WSAGetLastError());
		return 1;
	}
	else
		std::cout << "WinSock initialization completed" << std::endl;


	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		processError("Couldn't initialize socket", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Server socket initialization complete" << std::endl;


	in_addr ip_addr;
	iResult = inet_pton(AF_INET, SERV_IP, &ip_addr);
	if (iResult <= 0) {
		processError("Couldn't translate IP to numeric format", 1);
		return 1;
	}

	sockaddr_in serverInfo;
	ZeroMemory(&serverInfo, sizeof(serverInfo));

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr = ip_addr;
	serverInfo.sin_port = htons(PORT_NUM);

	iResult = bind(serverSocket, (sockaddr*)&serverInfo, sizeof(serverInfo));
	if (iResult == SOCKET_ERROR) {
		processError("Couldn't bind socket", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Socket binding completed" << std::endl;

	iResult = listen(serverSocket, SOMAXCONN);
	if (iResult != NO_ERROR) {
		processError("Couldn't start listener", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Listening..." << std::endl;

	while (true) {
		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));
		int clientInfo_sz = sizeof(clientInfo);

		SOCKET clientConn = accept(serverSocket, (sockaddr*)&clientInfo, &clientInfo_sz);

		if (clientConn == INVALID_SOCKET) {
			processError("Couldn't connect to client", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		else {
			std::cout << "Client connection established successfully" << std::endl;

			char clientIP[22];
			inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);

			std::cout << "Client IP: " << clientIP << std::endl;

			clients.push_back(clientConn);
			CreateThread(NULL, 0, ClientThread, (LPVOID)clientConn, NULL, NULL);
		}
	}

	closesocket(serverSocket);
	WSACleanup();

	return 0;
}