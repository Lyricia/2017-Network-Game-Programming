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
int recvn(SOCKET s, char * buf, int len, int flags)
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

CClient::CClient()
	: m_Local_id(-1)
{
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
	m_MainServer.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_MainServer.sock == INVALID_SOCKET)
		err_quit("socket()");
}

void CClient::Release()
{
	// closesocket()
	closesocket(m_MainServer.sock);
	// 윈속 종료
	WSACleanup();

	for (auto& p : m_MsgQueue)
		delete p;
	m_MsgQueue.clear();
}

void CClient::ConnectServer()
{
	// connect()
	m_MainServer.addr.sin_family = AF_INET;
	m_MainServer.addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_MainServer.addr.sin_port = htons(SERVERPORT);
	int retval = connect(m_MainServer.sock, (SOCKADDR *)&m_MainServer.addr, sizeof(m_MainServer.addr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	NGPMSG msg;
	retval = recvn(m_MainServer.sock, (char*)&msg, sizeof(NGPMSG), 0);
	if (retval == SOCKET_ERROR) {
		return;
	}

	m_MainServer.pMsgQueue = &m_MsgQueue;
	m_MainServer.hReceiver = CreateThread(NULL, 0, RecvMessage, (LPVOID)&m_MainServer, 0, NULL);
}

void CClient::SendMsgs(char* buf, UINT buf_size)
{
	int retval = send(m_MainServer.sock, buf, buf_size, 0);
}

DWORD RecvMessage(LPVOID arg)
{
	ConnectedServerInfo* main_server = (ConnectedServerInfo*)arg;
	int retval;
	char buf[64 + 1];
	int len;

	printf("\n[TCP 클라이언트] 서버접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(main_server->addr.sin_addr), ntohs(main_server->addr.sin_port));

	while (1) {
		// 데이터 받기(고정 길이)
		retval = recvn(main_server->sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 받기(가변 길이)
		retval = recvn(main_server->sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		cout << "ID : " << " " << buf << endl;

		retval = send(main_server->sock, "test", sizeof("test"), NULL);
		if (retval == SOCKET_ERROR)
		{
			closesocket(main_server->sock);
			return 0;
		}
	}

	closesocket(main_server->sock);
	printf("[TCP 클라이언트] 서버연결 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(main_server->addr.sin_addr), ntohs(main_server->addr.sin_port));
	return 0;
}
