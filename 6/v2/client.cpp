#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define BUFF_SIZE 1024

DWORD WINAPI ServerThread(LPVOID lpParam) {
	SOCKET serverSocket = (SOCKET)lpParam;
	char buffer[1024];
	while (true) {
		int bytesReceived = recv(serverSocket, buffer, 1024, 0);

		if (bytesReceived <= 0)
			break;

		std::cout << "\nMessage received: " << buffer;
	}

	closesocket(serverSocket);
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
	SOCKET clientSocket = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (iResult != NO_ERROR) {
		processError("Couldn't initialize WinSock", WSAGetLastError());
		return 1;
	}
	else
		std::cout << "WinSock initialization completed" << std::endl;


	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		processError("Couldn't initialize socket", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Client socket initialization complete" << std::endl;


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

	iResult = connect(clientSocket, (sockaddr*)&serverInfo, sizeof(sockaddr));
	if (iResult != NO_ERROR) {
		processError("Couldn't connect to server", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Server connection established successfully" << std::endl;

		char servIP[22];
		inet_ntop(AF_INET, &serverInfo.sin_addr, servIP, INET_ADDRSTRLEN);

		std::cout << "Server IP: " << servIP << std::endl;
	}

	CreateThread(NULL, 0, ServerThread, (LPVOID)clientSocket, NULL, NULL);

	std::vector<char> clientBuff(BUFF_SIZE);
	short packetSize = 0;

	while (true) {
		std::cout << "Enter your message: ";
		std::fgets(clientBuff.data(), BUFF_SIZE, stdin);

		packetSize = send(clientSocket, clientBuff.data(), clientBuff.size(), 0);

		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x')
			break;

		if (packetSize == SOCKET_ERROR) {
			processError("Couldn't send message to server", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}