#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("WSAStartup failed\n");
		return 1;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("socket failed: %d\n", WSAGetLastError());
		return 1;
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(9000);  // IOCPサーバーと同じポート
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("Connecting to server...\n");

	if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("connect failed: %d\n", WSAGetLastError());
		closesocket(sock);
		return 1;
	}

	printf("Connected!\n");

	char sendBuf[1024];
	char recvBuf[1024];

	while (true) {
		printf("Input: ");
		fgets(sendBuf, sizeof(sendBuf), stdin);

		// 改行を削除
		size_t len = strlen(sendBuf);
		if (len > 0 && sendBuf[len - 1] == '\n') {
			sendBuf[len - 1] = '\0';
		}

		if (strcmp(sendBuf, "exit") == 0) {
			break;
		}

		// 送信
		int sent = send(sock, sendBuf, (int)strlen(sendBuf), 0);
		if (sent == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			break;
		}

		// 受信
		int received = recv(sock, recvBuf, sizeof(recvBuf)-1, 0);
		if (received <= 0) {
			printf("Server disconnected.\n");
			break;
		}

		recvBuf[received] = '\0';
		printf("Echo: %s\n", recvBuf);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}