#pragma once
#include "Server\Server.h"
#include "GameWorld\GameWorld.h"
#define MESSAGE_PROCESSING_TIME 0.1

#define AGENT_SERVER_IP		"127.0.0.1"
#define AGENT_SERVER_PORT		9001

struct ConnectionInfo {
	UINT				ID;
	SOCKET				sock;
	SOCKADDR_IN			addr;
	HANDLE				RecvThreadHandle;
	list<NGPMSG*>*		pMsgQueue;
	LPVOID				pUserdata;
	LPCRITICAL_SECTION	pCs;

	ConnectionInfo() 
		: RecvThreadHandle(NULL)
		, pMsgQueue(nullptr) {}
	~ConnectionInfo() {
		if (RecvThreadHandle)
		{
			closesocket(sock);
			//TerminateThread(RecvThreadHandle, 0);
		}
	}

	void EnterCriticalSection() { ::EnterCriticalSection(pCs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(pCs); }

};

struct RoomInfo {
	UCHAR					RoomID;
	ConnectionInfo			*serverinfo;
	list<ConnectionInfo*>	clientlist;
	list<LPVOID>			agentlist;

	ConnectionInfo*			AgentServer;
	CGameWorld				GameWorld;
	list<NGPMSG*>			MsgQueue;
	HANDLE					hGameWorld;
	CRITICAL_SECTION		Cs;
	
	RoomInfo() 
		: hGameWorld(NULL)
	{
		::InitializeCriticalSection(&Cs);
	}
	~RoomInfo() { 
		Release(); 
		::DeleteCriticalSection(&Cs);
		if (hGameWorld) {
			//TerminateThread(hGameWorld, 0);
		}
	}

	void EnterCriticalSection() { ::EnterCriticalSection(&Cs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&Cs); }

	void Release() {
		EnterCriticalSection();
		for (auto& p : MsgQueue) delete p;
		MsgQueue.clear();
		LeaveCriticalSection();

		for (auto& p : clientlist) delete p;
		clientlist.clear();
	}
};

class MainServer : public Server
{
	ConnectionInfo*				m_AgentServer;
	list<RoomInfo*>				m_RoomList;
	list<ConnectionInfo*>		m_WaitingClientList;
	list<NGPMSG*>				m_MsgQueue;

	UCHAR						m_iRoomCounter;

public:
	MainServer();
	~MainServer();

	virtual void Run();

	void ConnectAgentServer();
	void RequestAddAgentServer(UCHAR room_id);
	void CreateRoom();
	void DeleteRoom();

	ConnectionInfo* GetConnectionAgentServerInfo() { return m_AgentServer; }
	void TrySendMsgToRoom(NGPMSG* msg)
	{
		bool success_Send_msg_to_room = false;
		for (auto&d : m_RoomList)
		{
			if (d->RoomID == msg->header.ROOMNO)
			{
				//if(msg->header.MSGTYPE == MSGTYPE::MSGACTION::MOVE)
				//std::cout << "RECV AGENT MESSAGE" << msg->header.OBJECTNO << std::endl;
				d->EnterCriticalSection();
				d->MsgQueue.push_back(msg);
				d->LeaveCriticalSection();
				success_Send_msg_to_room = true;
				break;
			}
		}

		if (!success_Send_msg_to_room)
			delete msg;
	
	}
};

static int recvn(SOCKET s, char *buf, int len, int flags);
static DWORD WINAPI RunGameWorld(LPVOID arg);
static DWORD WINAPI RecvMessage(LPVOID arg);

// Recv Agent Server
static DWORD WINAPI AgentServerMessageReceiver(LPVOID arg);