#include "stdafx.h"
#include "Server.h"

void Server::Initialize()
{
	int retval;

	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0) 
	{
		cout << "WinSock Error" << endl;

		// Assert
		return;
	}
	cout << "WinSock Initialized" << endl;

	m_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSock == INVALID_SOCKET)
	{
		cout << "Listen Socket Error" << endl;

		// Assert
		return;
	}
	cout << "Listen Socket Initialized" << endl;
	


#ifdef DISAGLE_NAGLE_ALGORITHM
	// Nagle 알고리즘 해제 코드
	int flag = 1;
	retval = setsockopt(m_ListenSock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	if (retval == -1) {
		std::cout << "네이글 알고리즘 해제 실패!" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif

	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_ServerAddr.sin_port = htons(m_ServerPort);

	retval = bind(m_ListenSock, (SOCKADDR *)&m_ServerAddr, sizeof(m_ServerAddr));
	if (retval == SOCKET_ERROR)
	{
		cout << "Bind Error" << endl;
		// Assert
		return;
	}
	cout << "Local Address Initialized" << endl;

	retval = listen(m_ListenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		cout << "Listening Failed" << endl;
		// Assert
		return;
	}
	cout << "Start Server Listening" << endl;
}

void Server::Release()
{
	// close Client Sock
	
	closesocket(m_ListenSock);
	cout << "close ListenSock" << endl;
	WSACleanup();
	cout << "WSACleanup" << endl;
}
