#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>
#include <map>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

uint16_t tooBig(uint16_t hostshort) {
	return (hostshort >> 8) | (hostshort << 8);
}

void handleclient(SOCKET client) {
	char buffer[512];
	int bytesofmessage = recv(client, buffer, sizeof(buffer), 0);

	std::cout << "Message from client" << std::string(buffer, bytesofmessage) << std::endl << std::endl; 

	//closesocket(client);
}

void ParseRequest(string &request) {
	size_t pos = 0;
	size_t endpos;

	endpos = request.find("\r\n", 0);
	//string typeRequest = request.substr()

	map<string, string> message;

}

int main() {
	//you need to start up WSA for to get access to the api for sockets
	WSADATA wsaData;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock version 2.2

	if (wsResult != 0) {
		std::cerr << "WSAStartup failed with error: " << wsResult << std::endl;
		std::cerr << "Error description: " << WSAGetLastError() << std::endl;
		return 1;
	}

	//what kind of ip protocol IPV4 or IPV6, what packet protocol UDP or TCP etc
	SOCKET serversoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(serversoc == INVALID_SOCKET) {
		std::cerr << "Socket creation failed" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = tooBig(8080);

	if (bind(serversoc, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		std::cerr << "Error with binding" << WSAGetLastError() << std::endl;
		WSACleanup();
		closesocket(serversoc);
		return 0;
	}

	if (listen(serversoc, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen error: " << WSAGetLastError() << std::endl;
		closesocket(serversoc);
		WSACleanup();
		return 1;
	}

	std::cout << "Server is listening on port 8080" << std::endl;


	while (true) {
		sockaddr_in clientAddress;
		int clientSize = sizeof(clientAddress);
		SOCKET client = accept(serversoc, (struct sockaddr*)&clientAddress, &clientSize);

		if (client == SOCKET_ERROR) {
			std::cerr << "client socket error" << WSAGetLastError() << std::endl;
			continue;
		}

		char clientIP[INET_ADDRSTRLEN];  // INET_ADDRSTRLEN is 16 for IPv4 addresses
		inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, sizeof(clientIP));

		
		std::cout << "Client connected from " << clientIP << std::endl;

		std::thread clientThread(handleclient, client);
		clientThread.detach();
	}

	closesocket(serversoc);
	WSACleanup();
	return 1;
}
