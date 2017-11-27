#include "stdafx.h"
#include "Client.h"

void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

CClient::CClient()
	: m_Local_id(-1)
	, m_Local_sock(NULL)
{
	ZeroMemory(&m_Local_addr, sizeof(m_Local_addr));
	ZeroMemory(&m_MainServer, sizeof(m_MainServer));
}
CClient::~CClient()
{
	Release();
}

void CClient::Initialize()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// socket()
	m_Local_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Local_sock == INVALID_SOCKET) 
		err_quit("socket()");
}

void CClient::Release()
{
	// closesocket()
	closesocket(m_Local_sock);
	// 윈속 종료
	WSACleanup();
}

void CClient::ConnectServer()
{
	// connect()
	m_Local_addr.sin_family = AF_INET;
	m_Local_addr.sin_addr.s_addr = inet_addr("");
	m_Local_addr.sin_port = htons(SERVERPORT);
	int retval = connect(m_Local_sock, (SOCKADDR *)&m_Local_addr, sizeof(m_Local_addr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");
}

void CClient::SendMsgs()
{
	int retval = send(m_Local_sock, m_pBuffer, BUFFER_SIZE, 0);

}
