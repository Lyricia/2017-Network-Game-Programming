#include "stdafx.h"
#include "AgentServer.h"

// TCP 환경에서 데이터를 완전히 받아내기 위한 함수
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


DWORD WINAPI RunGameWorld(LPVOID arg)
{
	int msgtype = 0;
	std::list<NGPMSG*> *MsgQueue = (std::list<NGPMSG*>*)arg;

	ActionInfo* Actionlist = new ActionInfo();
	ObjInfo* Objlist = new ObjInfo();

	while (1)
	{
		NGPMSG msg;
		if (!MsgQueue->empty())
		{
			msgtype = DispatchMSG(MsgQueue->front(), *Actionlist, *Objlist);
			MsgQueue->pop_front();
		}
		else continue;

		switch (msgtype)
		{
		case MSGTYPE::MSGACTION::MOVE:
			break;

		case MSGTYPE::MSGACTION::SHOOT:

			break;

		case MSGTYPE::MSGACTION::BUILDTURRET:

			break;

		case MSGTYPE::MSGACTION::RELOAD:

			break;


		case MSGTYPE::MSGSTATE::AIAGENTINFO:
			break;

		case MSGTYPE::MSGSTATE::AICREATTIONREQUEST:
			break;

		case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
			break;

		case MSGTYPE::MSGSTATE::CLIENTREADY:
			break;

		case MSGTYPE::MSGSTATE::ROOMCREATION:
			break;


		case MSGTYPE::MSGUPDATE::ADJUSTPOS:
			break;

		case MSGTYPE::MSGUPDATE::CREATEOBJECT:
			break;

		case MSGTYPE::MSGUPDATE::DELETEOBJECT:
			break;

		case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
			break;

		}
	}

	return 0;
}

DWORD WINAPI RecvMessage(LPVOID arg)
{
	ConnectionInfo* Main_Server = (ConnectionInfo*)arg;
	int retval;
	char buf[BUFSIZE + 1];
	int len;

	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(Main_Server->addr.sin_addr), ntohs(Main_Server->addr.sin_port));

	while (1) {
		// 데이터 받기(고정 길이)
		retval = recvn(Main_Server->sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// 데이터 받기(가변 길이)
		retval = recvn(Main_Server->sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		std::cout << "ID : " << Main_Server->ID << " " << buf << std::endl;

		retval = send(Main_Server->sock, "test", sizeof("test"), NULL);
		
		if (retval == SOCKET_ERROR)
		{
			closesocket(Main_Server->sock);
			return 0;
		}
	}

	closesocket(Main_Server->sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(Main_Server->addr.sin_addr), ntohs(Main_Server->addr.sin_port));
	return 0;
}


DWORD WINAPI RoomProcess(LPVOID arg)
{
	RoomInfo* room = (RoomInfo*)arg;

	int i = 0;
	int retval;
	
	//////////////////////




	//////////////////////



	return 0;

}

void AgentServer::Run()
{
	AcceptMainServer();
	
	

}

void AgentServer::AcceptMainServer()
{

	


	// 데이터 통신용 버퍼
	char buf[BUFSIZE];
	// 주소의 길이를 받아낸다.
	int addrlen;
	// 받아낸 데이터 길이를 저장한다.
	int datalen;
	// 오류 시 결과 값을 저장한다.
	int retval;
	
	//~~~~~~~~~~~~~~~~~<서버 연결을 위한 정보를 입력한다>~~~~~~~~~~~~~~~~~~~~~~~~~~

	//std::string SERVERIP;
	int SERVERPORT;
	

	//std::cout << "Insert MainServer IP :: ";
	//std::cin >> SERVERIP;

	std::cout << "Insert MainServer PORT :: ";
	std::cin >> SERVERPORT;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// socket()
	m_MainServer.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_MainServer.sock == INVALID_SOCKET) err_quit("socket()");

	std::cout << "Create Socket" << std::endl;

	// connect()
	ZeroMemory(&m_MainServer.addr , sizeof(m_MainServer.addr));
	m_MainServer.addr.sin_family = AF_INET;
	m_MainServer.addr.sin_addr.s_addr = inet_addr(SERVERIP);
	m_MainServer.addr.sin_port = htons(SERVERPORT);

	retval = connect(m_MainServer.sock, (SOCKADDR *)&m_MainServer.addr, sizeof(m_MainServer.addr));
	std::cout << "Connect" << std::endl;

	if (retval == SOCKET_ERROR)
	{
		err_quit("connect()");
	}
	
	std::cout << "[TCP 서버] 메인서버 접속: IP 주소 :: " << inet_ntoa(m_MainServer.addr.sin_addr) << "포트 번호 :: " << ntohs(m_MainServer.addr.sin_port) << std::endl;

	HANDLE hThread;
	hThread = CreateThread(NULL, 0, RecvMessage, (LPVOID)m_MainServer.sock, 0, NULL);

}

void AgentServer::CreateAgentsToRoom()
{



}

void AgentServer::DeleteAgentsFromRoom()
{




}

void RoomInfo::UpdateWorld()
{
}
