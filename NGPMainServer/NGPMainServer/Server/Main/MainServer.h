#pragma once
#include "Server\Server.h"
#include "GameWorld\GameWorld.h"

struct ConnectionInfo {
	int					ID;
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
			TerminateThread(RecvThreadHandle, 0);
	}

	void EnterCriticalSection() { ::EnterCriticalSection(pCs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(pCs); }

};

struct RoomInfo {
	UINT					RoomID;
	ConnectionInfo			*serverinfo;
	list<ConnectionInfo*>	clientlist;
	//list<ConnectionInfo*>	agentlist;
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
		::DeleteCriticalSection(&Cs);
		for (auto& p : MsgQueue) delete p;
		for (auto& p : clientlist) delete p;
		MsgQueue.clear();
		clientlist.clear();
		if (hGameWorld) {
			TerminateThread(hGameWorld, 0);
		}
	}

	void EnterCriticalSection() { ::EnterCriticalSection(&Cs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&Cs); }
};

class MainServer : public Server
{
	//ConnectedServerInfo		m_AgentServer;
	list<RoomInfo*>				m_RoomList;
	list<ConnectionInfo*>		m_WaitingClientList;
	list<NGPMSG*>				m_MsgQueue;

	int							m_iRoomCounter;

public:
	MainServer();
	~MainServer();

	virtual void Run();

	void ConnectAgentServer();
	void RequestAddAgentServer();
	void CreateRoom();
	void DeleteRoom(UINT room_id);
};

static int recvn(SOCKET s, char *buf, int len, int flags);
static DWORD WINAPI RunGameWorld(LPVOID arg);
static DWORD WINAPI RecvMessage(LPVOID arg);

