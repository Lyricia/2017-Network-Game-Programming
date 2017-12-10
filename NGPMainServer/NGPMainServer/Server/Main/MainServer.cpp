#include "stdafx.h"
#include "GameWorld\Timer\Timer.h"
#include "GameWorld\GameWorld.h"

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

DWORD WINAPI RunGameWorld(LPVOID arg)
{
	RoomInfo* room = (RoomInfo*)arg;

	int i = 0;
	int retval;

	for (auto& client : room->clientlist)
	{
		client->pMsgQueue = &room->MsgQueue;
		client->pCs = &room->Cs;
		client->RecvThreadHandle = CreateThread(NULL, 0, RecvMessage, (LPVOID)client, 0, NULL);
	}
	room->GameWorld.Run();
	return 0;
}

DWORD WINAPI RecvMessage(LPVOID arg) 
{
	ConnectionInfo* client = (ConnectionInfo*)arg;
	int retval;
	
	NGPMSG* msg;

	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
	
	while (1) {
		msg = new NGPMSG();
		retval = recvn(client->sock, (char *)msg, sizeof(NGPMSG), 0);
		//retval = WSAGetLastError();
		if (retval == SOCKET_ERROR)
		{
			break;
		}

		client->EnterCriticalSection();
		client->pMsgQueue->push_back(msg);
		client->LeaveCriticalSection();

	}
	
	closesocket(client->sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));

	return 0;
}

DWORD AgentServerMessageReceiver(LPVOID arg)
{
	MainServer* Server = (MainServer*)arg;
	NGPMSG* msg = nullptr;

	while (Server->GetConnectionAgentServerInfo()->sock) {

		msg = new NGPMSG();

		// 데이터 받기(고정 길이)
		// 메세지의 헤더만큼 읽는다.
		//std::cout << "ReceiveMsg" << std::endl;

		int retval = recvn(Server->GetConnectionAgentServerInfo()->sock,
			(char *)msg, sizeof(NGPMSG), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}

		switch (msg->header.MSGTYPE)
		{
		case MSGTYPE::ROOMCREATION:
		case MSGTYPE::CLIENTREADY:
		case MSGTYPE::CLIENTGAMEOVER:
		case MSGTYPE::AICREATTIONREQUEST:
		case MSGTYPE::AIAGENTINFO:
		{
			delete msg;
			break;
		}
		case MSGTYPE::MOVE:
		case MSGTYPE::STOP:
		case MSGTYPE::SHOOT:
		case MSGTYPE::RELOAD:
		case MSGTYPE::GRENADE:
		case MSGTYPE::BUILDTURRET:
		case MSGTYPE::ADJUSTPOS:
		case MSGTYPE::CREATEOBJECT:
		case MSGTYPE::DELETEOBJECT:
		case MSGTYPE::UPDATEOBJECTSTATE:
		case MSGTYPE::UPDATEMAPSTATE:
		{
			Server->TrySendMsgToRoom(msg);
			break;
		}
		default:
			delete msg;
			break;

		}
	}




	
	return 0;
}

MainServer::MainServer()
	: m_iRoomCounter(0)
	, m_AgentServerIP(std::string())
{

}
MainServer::~MainServer()
{
	for (auto& p : m_RoomList) delete p;
	for (auto& p : m_WaitingClientList) delete p;
	for (auto& p : m_MsgQueue) delete p;
	
	m_RoomList.clear();
	m_WaitingClientList.clear();
	m_MsgQueue.clear();
}

void MainServer::Run()
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	UINT clientcounter = 0;

	ConnectAgentServer();
	HANDLE AgentReceiver = 
		CreateThread(NULL, 0, AgentServerMessageReceiver, this, 0, NULL);

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(m_ListenSock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			cout << "client socket error" << endl;
			break;
		}

		ConnectionInfo* newclient = new ConnectionInfo();
		newclient->ID = clientcounter++;
		newclient->addr = clientaddr;
		newclient->sock = client_sock;
		m_WaitingClientList.push_back(newclient);

		if (m_WaitingClientList.size() >= 3)
		{
			CreateRoom();
		}
		DeleteRoom();
	}

	delete m_AgentServer;
}

void MainServer::ConnectAgentServer()
{
	int retval;
	m_AgentServer = new ConnectionInfo();

	m_AgentServer->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_AgentServer->sock == INVALID_SOCKET)
	{
		cout << "Create Agent Socket failed" << endl;
		//Assert
		return;

	}

#ifdef DISAGLE_NAGLE_ALGORITHM
	// Nagle 알고리즘 해제 코드
	int flag = 1;
	retval = setsockopt(m_AgentServer->sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
	if (retval == -1) {
		std::cout << "네이글 알고리즘 해제 실패!" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
	char ip[512];
	while (true)
	{
		if (!m_AgentServerIP.size())
		{
			printf("\nInput agent server ip: ");
			fgets(ip, sizeof(ip), stdin);
			ip[strlen(ip) - 1] = '\0';
			m_AgentServerIP = std::string(ip);
		}
		// connect()
		m_AgentServer->addr.sin_family = AF_INET;
		m_AgentServer->addr.sin_addr.s_addr = inet_addr(m_AgentServerIP.c_str());
		m_AgentServer->addr.sin_port = htons(AGENT_SERVER_PORT);
		printf("Server try connect to the agent server..\n");
		retval = connect(m_AgentServer->sock, (SOCKADDR *)&m_AgentServer->addr, sizeof(m_AgentServer->addr));
		if (retval == SOCKET_ERROR)
		{
			m_AgentServerIP = std::string();
			printf("Invalid IP\n");
			continue;
		}
		else break;
	}
	printf("Connected the agent server.\n");
}

void MainServer::RequestAddAgentServer(UCHAR room_id)
{
	NGPMSG* msg = CreateMSG(
		MSGTYPE::AICREATTIONREQUEST
		, room_id
		, 0
		, 0
		, 0
		, nullptr
		, nullptr
	);
	
	int retval = send(m_AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);
	if (retval == SOCKET_ERROR) {
		//assert
	}

	delete msg;
}

void MainServer::CreateRoom()
{
	RoomInfo* newroom = new RoomInfo();
	newroom->RoomID = m_iRoomCounter++;
	newroom->AgentServer = m_AgentServer;

	for (int i = 0; i < 3; ++i)
	{
		newroom->clientlist.push_back(m_WaitingClientList.front());
		m_WaitingClientList.pop_front();
	}
	newroom->GameWorld.RegisterRoomInfo(newroom);
	newroom->GameWorld.Initailize();
	newroom->hGameWorld = CreateThread(NULL, 0, RunGameWorld, (LPVOID)newroom, 0, NULL);
	if (newroom->hGameWorld == NULL)
	{
		cout << "Thread Creation failed" << endl;
		//Assert
		return;
	}

	RequestAddAgentServer(newroom->RoomID);

	m_RoomList.push_back(newroom);
	cout << "Create New Room " << newroom->RoomID << endl;
}

void MainServer::DeleteRoom()
{

	m_RoomList.remove_if([ &, this ](RoomInfo* pRoom )->bool {
		if (pRoom->clientlist.empty())
		{
			delete pRoom;
			return true;
		}
		return false;
	});
}