#include "stdafx.h"
#include "Server.h"
#include "MainServer.h"

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

DWORD WINAPI testfunc(LPVOID arg) 
{
	clientinfo* client = (clientinfo*)arg;
	int retval;
	char buf[64 + 1];
	int len;
	
	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->clientaddr.sin_addr), ntohs(client->clientaddr.sin_port));

	while (1) {
		// 데이터 받기(고정 길이)
		retval = recvn(client->clientsock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 받기(가변 길이)
		retval = recvn(client->clientsock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;	

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(client->clientaddr.sin_addr),
			ntohs(client->clientaddr.sin_port), buf);
	}

	closesocket(client->clientsock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->clientaddr.sin_addr), ntohs(client->clientaddr.sin_port));
	return 0;
}

void MainServer::Run()
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	HANDLE hThread;

	while (1) 
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(m_ListenSock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) 
		{
			cout << "client socket error" << endl;
			break;
		}

		clientinfo* newclient = new clientinfo();
		newclient->clientaddr = clientaddr;
		newclient->clientsock = client_sock;
		m_WaitingClientList.push_back(newclient);
		hThread = CreateThread(NULL, 0, testfunc, (LPVOID)newclient, 0, NULL);
		
		if (hThread == NULL) 
		{
			cout << "Thread Creation failed" << endl;
			//Assert
			return;
		}
	} 
}

void MainServer::ConnectAgentServer()
{
}

void MainServer::RequestAddAgentServer()
{
}

void MainServer::CreateRoom()
{
}

void MainServer::DeleteRoom()
{
}
