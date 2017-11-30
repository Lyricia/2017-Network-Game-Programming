#pragma once
#include "stdafx.h"
#include "Server.h"

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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
	// 오류 발생시 확인하기 위한 값
	int retval;

	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0)
	{
		std::cout << "윈속 에러!" << std::endl;

		// Assert
		return;
	}

	std::cout << "소캣 초기화 완료" << std::endl;

	m_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSock == INVALID_SOCKET)
	{
		err_display("socket()");
		// Assert
		return;
	}
	std::cout << "리슨 소켓 설정 완료" << std::endl;

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

	std::cout << "로컬 주소 설정 완료" << std::endl;

	retval = listen(m_ListenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		err_display("listen()");
		// Assert
		return;
	}
	std::cout << "서버 리스닝 시작" << std::endl;


}

void Server::Release()
{

	// 소켓을 닫는다.

	//closesocket(m_ListenSock);
	//std::cout << "close ListenSock" << std::endl;

	WSACleanup();
	std::cout << "WSACleanup" << std::endl;

}
