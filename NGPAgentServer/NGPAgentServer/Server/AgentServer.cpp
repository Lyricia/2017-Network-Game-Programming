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


// Update World
DWORD WINAPI UpdateWorld(LPVOID arg)
{
	AgentServer* Server = (AgentServer*)arg;
	int msgtype = 0;
	

	ActionInfo* Actionlist = new ActionInfo();
	ObjInfo* Objlist = new ObjInfo();

	while (1)
	{

	}

	

	return 0;
}

DWORD WINAPI MessageDispatcher(LPVOID arg)
{
	AgentServer* Server = (AgentServer*)arg;
	int retval;
	int len;

	NGPMSG* msg;

	while (1) {
		

		if (!Server->m_MsgQueue.empty())
		{

			Server->EnterCriticalSection();
			msg = Server->m_MsgQueue.front();
			Server->m_MsgQueue.pop_front();
			Server->LeaveCriticalSection();

			switch (msg->header.MSGTYPE)
			{
			case MSGTYPE::CLIENTGAMEOVER:
			{
				printf("Delete Room %d \n", msg->header.ROOMNO);
				Server->DeleteAgentsFromRoom(msg->header.ROOMNO);
				break;
			}
			case MSGTYPE::AICREATTIONREQUEST:
			{
				Server->CreateAgentsToRoom(msg->header.ROOMNO);
				break;
			}

			default:
				break;

			}

			delete msg;

		}
	}


	return 0;
}



AgentServer::AgentServer()
{
	
}

AgentServer::~AgentServer()
{
	for (auto& p : m_RoomList) delete p;
	for (auto& p : m_MsgQueue) delete p;

	m_RoomList.clear();
	m_MsgQueue.clear();
}

// 룸 정보를 받아와서 지속적인 갱신을 한다.
DWORD WINAPI RunGameWorld(LPVOID arg)
{
	ThreadPack* threadpack = (ThreadPack*)arg;

	AgentServer* Server = threadpack->server;
	int			room_id = threadpack->m_roomid;

	RoomInfo*	room = nullptr;

	for (auto& d : Server->m_RoomList)
	{
		if (d->RoomID == room_id)
		{
			room = d;
			break;
		}
	}

	std::cout << "Running Room" << room->RoomID << std::endl;
	room->GameWorld.Run();
	Server->m_RoomList.remove_if([room](RoomInfo* pRoom)->bool {
		if (pRoom->RoomID == room->RoomID)
		{
			delete pRoom;
			return true;
		}
		return false;
	});
	std::cout << "Delete Room " << std::endl;
	return 0;
}

void AgentServer::Run()
{
	::InitializeCriticalSection(&ServerCs);

	AcceptMainServer();

	int retval;
	char buf[BUFSIZE + 1];
	NGPMSG* msg = nullptr;
	int len;
	////////////////////////////////////////////////////////////////////////////////////////
	
	HANDLE Dispatcher = CreateThread(NULL, 0, MessageDispatcher, this , 0, NULL);

	while (m_MainServer->sock) {

		msg = new NGPMSG();

		// 데이터 받기(고정 길이)
		// 메세지의 헤더만큼 읽는다.
		//std::cout << "ReceiveMsg" << std::endl;

		retval = recvn(m_MainServer->sock,
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
			//m_MainServer.
			EnterCriticalSection();
			m_MsgQueue.push_back(msg);
			LeaveCriticalSection();
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
			for (auto&d : m_RoomList)
			{
				if (d->RoomID == msg->header.ROOMNO)
				{
					d->EnterCriticalSection();
					d->MsgQueue.push_back(msg);
					d->LeaveCriticalSection();
					break;
				}
			}
			break;
		}

		}
	}
	closesocket(m_MainServer->sock);
	///////////////////////////////////////////////////////////////////////////////////////

}

void AgentServer::AcceptMainServer()
{
	// 주소의 길이를 받아낸다.
	int addrlen;
	// 오류 시 결과 값을 저장한다.
	int retval;
	m_MainServer = new ConnectionInfo();

	addrlen = sizeof(m_MainServer->addr);
	m_MainServer->sock = accept(m_ListenSock, (SOCKADDR *)&m_MainServer->addr, &addrlen);
	if (m_MainServer->sock == INVALID_SOCKET)
	{
		std::cout << "client socket error" << std::endl;
	}
	std::cout << "MainServer Connection Complete" << std::endl;
}

void AgentServer::CreateAgentsToRoom(UINT room_id)
{

	RoomInfo* newroom = new RoomInfo();
	newroom->RoomID = room_id;
	newroom->serverinfo = m_MainServer;
	newroom->GameWorld.RegisterRoomInfo(newroom);
	newroom->GameWorld.Initailize();

	ThreadPack* threadpack = new ThreadPack();
	threadpack->server = this;
	threadpack->m_roomid = room_id;

	newroom->hGameWorld = CreateThread(NULL, 0, RunGameWorld, (LPVOID)threadpack, 0, NULL);

	if (newroom->hGameWorld == NULL)
	{
		std::cout << "Thread Creation failed" << std::endl;
		//Assert
		return;
	}
	m_RoomList.push_back(newroom);
	std::cout << "Create New Room :: " << newroom->RoomID << std::endl;

}

void AgentServer::DeleteAgentsFromRoom(UINT room_id)
{
	for(auto& pRoom : m_RoomList)
		if (pRoom->RoomID == room_id)
			pRoom->GameWorld.Stop();
}
