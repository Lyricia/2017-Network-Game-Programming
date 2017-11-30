#pragma once
#include "stdafx.h"
#include "Server.h"

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << "Error :: "<< msg << std::endl;
	//MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void Server::Initialize()
{
	// ���� �߻��� Ȯ���ϱ� ���� ��
	int retval;

	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0)
	{
		std::cout << "���� ����!" << std::endl;

		// Assert
		return;
	}

	std::cout << "��Ĺ �ʱ�ȭ �Ϸ�" << std::endl;

	m_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSock == INVALID_SOCKET)
	{
		err_display("socket()");
		// Assert
		return;
	}
	std::cout << "���� ���� ���� �Ϸ�" << std::endl;

	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_ServerAddr.sin_port = htons(m_ServerPort);

	retval = bind(m_ListenSock, (SOCKADDR *)&m_ServerAddr, sizeof(m_ServerAddr));
	if (retval == SOCKET_ERROR)
	{
		err_display("bind()");
		// Assert
		return;
	}

	std::cout << "���� �ּ� ���� �Ϸ�" << std::endl;

	retval = listen(m_ListenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		err_display("listen()");
		// Assert
		return;
	}
	std::cout << "���� ������ ����" << std::endl;


}

void Server::Release()
{

	// ������ �ݴ´�.

	//closesocket(m_ListenSock);
	//std::cout << "close ListenSock" << std::endl;

	WSACleanup();
	std::cout << "WSACleanup" << std::endl;

}
