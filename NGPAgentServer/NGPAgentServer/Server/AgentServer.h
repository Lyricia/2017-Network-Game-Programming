#pragma once
#include "stdafx.h"
#include "Server.h"
#include "GameWorld\GameWorld.h"

#define MESSAGE_PROCESSING_TIME 0.1



struct ConnectionInfo {
	int				ID;
	SOCKET			sock;
	SOCKADDR_IN		addr;
	HANDLE			RecvThreadHandle;
	std::list<NGPMSG*>*	pMsgQueue;
	LPVOID				pUserdata;
	LPCRITICAL_SECTION	pCs;

	ConnectionInfo()
		: RecvThreadHandle(NULL)
		, pMsgQueue(nullptr) {

		pMsgQueue = new std::list<NGPMSG*>();
	
	}
	~ConnectionInfo() {
		if (RecvThreadHandle)
			TerminateThread(RecvThreadHandle, 0);
	}

	void EnterCriticalSection() { ::EnterCriticalSection(pCs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(pCs); }

};

struct RoomInfo {
		int							RoomID;
		HANDLE						hGameWorld;
		std::list<NGPMSG*>			MsgQueue;
		ConnectionInfo				*serverinfo;
		//std::list<ConnectionInfo*>	clientlist;
		//std::list<ConnectionInfo*>	agentlist;
		CGameWorld					GameWorld;
		CRITICAL_SECTION			roomCs;



		RoomInfo() : hGameWorld(NULL) {
			::InitializeCriticalSection(&roomCs);
		}
		~RoomInfo() {
			for (auto& p : MsgQueue) delete p;
			//for (auto& p : clientlist) delete p;
			MsgQueue.clear();
			//clientlist.clear();
			if (hGameWorld) {
				TerminateThread(hGameWorld, 0);
			}
		}

		void EnterCriticalSection() { ::EnterCriticalSection(&roomCs); }
		void LeaveCriticalSection() { ::LeaveCriticalSection(&roomCs); }

		void SendMsgs(char* buf, UINT buf_size)
		{
			int retval = send(serverinfo->sock, buf, buf_size, 0);
		}
};



class AgentServer : public Server
{
public:
	ConnectionInfo*				m_MainServer;
	std::list<RoomInfo*>		m_RoomList;
	std::list<NGPMSG*>			m_MsgQueue;
	int							m_iRoomCounter;
	CRITICAL_SECTION			ServerCs;
public:
	AgentServer();
	~AgentServer();

	// ������ �����ϱ� �����ϴ� �Լ�. High level design�� ���� �Ǿ��ִ� �������� ���� ������ ����ȴ�.
	virtual void Run();

	// Run()�Լ��� �ʹݺο��� ȣ��Ǹ� ���� ������ ���� ��û�� ��ٸ���. ���� ������ ����Ǹ� recv �����尡 �����ȴ�.
	void AcceptMainServer();
	// ���� ������ �˸��� ������Ʈ�� ������ ��û�ϴ� �޽����� ������ ������ ���� ������ 
	// �������� �� ����Ʈ�� ���� �߰��ϰ� ������Ʈ���� �����Ͽ� ������ ���� ������ �����Ѵ�.
	void CreateAgentsToRoom(UINT room_id);
	// ������ ����� ���� ������ ȣ��Ǵ� �Լ�. 
	// ���� ���Ḧ �˸��� �޽����� �޾� ������ ����� �뿡 ���Ǵ� �ý��� �ڿ��� ȸ���ϰ� ���� �����Ѵ�.
	void DeleteAgentsFromRoom(UINT room_id);
	//std::list<NGPMSG*>&	GetMsgQueue() { return m_MsgQueue; }

	void EnterCriticalSection() { ::EnterCriticalSection(&ServerCs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&ServerCs); }
};


static int recvn(SOCKET s, char *buf, int len, int flags);

// ���� ���� ���� ���� �ֽ�ȭ
static DWORD WINAPI UpdateWorld(LPVOID arg);

static DWORD WINAPI RunGameWorld(LPVOID arg);

static DWORD WINAPI MessageDispatcher(LPVOID arg);


struct ThreadPack {
	AgentServer* server;
	int			m_roomid;
};