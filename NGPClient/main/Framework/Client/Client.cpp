#include "stdafx.h"
#include "Client.h"

//#define DISAGLE_NAGLE_ALGORITHM

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
	, m_ServerIP()
{
	ZeroMemory(&m_MainServer, sizeof(m_MainServer));
}
CClient::~CClient()
{
	Release();
}

void CClient::Initialize()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

#ifdef DISAGLE_NAGLE_ALGORITHM
	// Nagle �˰��� ���� �ڵ�
	int flag = 1;
	int retval = setsockopt(m_MainServer.sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	if (retval == -1) {
		std::cout << "���̱� �˰��� ���� ����!" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif

	::InitializeCriticalSection(&m_CS);
}

void CClient::Release()
{
	DisconnectServer();
	
	::DeleteCriticalSection(&m_CS);

	// ���� ����
	WSACleanup();
}

bool CClient::ConnectServer()
{
	int retval;
	char ip[BUFFER_SIZE];

	// socket()
	m_MainServer.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_MainServer.sock == INVALID_SOCKET) err_quit("socket()");

	while (true)
	{
		if (!m_ServerIP.size())
		{
			printf("\nInput server ip (Exit - q): ");
			fgets(ip, sizeof(ip), stdin);
			ip[strlen(ip) - 1] = '\0';
			if (strcmp(ip, "q") == 0) return false;
			m_ServerIP = std::string(ip);
		}
		// connect()
		m_MainServer.addr.sin_family = AF_INET;
		m_MainServer.addr.sin_addr.s_addr = inet_addr(m_ServerIP.c_str());
		m_MainServer.addr.sin_port = htons(SERVERPORT);
		printf("Client try connect to the main server..\n");
		retval = connect(m_MainServer.sock, (SOCKADDR *)&m_MainServer.addr, sizeof(m_MainServer.addr));
		if (retval == SOCKET_ERROR)
		{
			m_ServerIP = std::string();
			printf("Invalid IP\n");
			continue;
		}
		else break;
	}
	printf("Connected the main server.\n");
	printf("Waiting for start game.\n");

	NGPMSG msg;
	while (true)
	{
		retval = recvn(m_MainServer.sock, (char*)&msg, sizeof(NGPMSG), 0);
		if (retval == SOCKET_ERROR) err_quit("Connect recvn()");
		if (msg.header.MSGTYPE == MSGTYPE::MSGSTATE::ROOMCREATION)
		{
			m_Local_id = msg.header.OBJECTNO;
			m_Room_id = msg.header.ROOMNO;
			break;
		}
	}

	m_MainServer.pMsgQueue = &m_MsgQueue;
	m_MainServer.pCS = &m_CS;
	m_MainServer.hReceiver = CreateThread(NULL, 0, RecvMessage, (LPVOID)&m_MainServer, 0, NULL);
	return true;
}

void CClient::DisconnectServer()
{	
	//m_ServerIP = std::string();
	for (auto& p : m_MsgQueue)
		delete p;
	m_MsgQueue.clear();
	if (m_MainServer.sock)
	{
		closesocket(m_MainServer.sock);
		m_MainServer.sock = NULL;
	}
}

void CClient::SendMsgs(char* buf, UINT buf_size)
{
	int retval = send(m_MainServer.sock, buf, buf_size, 0);
}

DWORD RecvMessage(LPVOID arg)
{
	ConnectedServerInfo* main_server = (ConnectedServerInfo*)arg;
	int retval;

	NGPMSG* msg;

	printf("\n[TCP Ŭ���̾�Ʈ] Receiver Ready: Server IP Address=%s, Server Port=%d\n",
		inet_ntoa(main_server->addr.sin_addr), ntohs(main_server->addr.sin_port));

	while (1) {
		msg = new NGPMSG();
		retval = recvn(main_server->sock, (char *)msg, sizeof(NGPMSG), 0);
		//retval = WSAGetLastError();
		if (retval == SOCKET_ERROR)
		{
			break;
		}
		main_server->EnterCriticalSection();
		main_server->pMsgQueue->push_back(msg);
		main_server->LeaveCriticalSection();
	}
	if (main_server->sock)
	{
		closesocket(main_server->sock);
		main_server->sock = NULL;
	}
	printf("[TCP Ŭ���̾�Ʈ] �������� ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(main_server->addr.sin_addr), ntohs(main_server->addr.sin_port));

	return 0;
}
