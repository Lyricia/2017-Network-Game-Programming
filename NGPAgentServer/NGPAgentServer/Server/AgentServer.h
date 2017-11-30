#pragma once
#include "stdafx.h"
#include "Server.h"
#include "GameWorld\GameWorld.h"

struct ConnectionInfo {
	int				ID;
	SOCKET			sock;
	SOCKADDR_IN		addr;
	HANDLE			RecvThreadHandle;
	std::list<NGPMSG*>*	pMsgQueue;

	ConnectionInfo()
		: RecvThreadHandle(NULL)
		, pMsgQueue(nullptr) {}
	~ConnectionInfo() {
		if (RecvThreadHandle)
			TerminateThread(RecvThreadHandle, 0);
	}

};

struct RoomInfo {
		int							RoomID;
		HANDLE						hGameWorld;
		std::list<NGPMSG*>			MsgQueue;
		ConnectionInfo				*serverinfo;
		//std::list<ConnectionInfo*>	clientlist;
		//std::list<ConnectionInfo*>	agentlist;
		CGameWorld					GameWorld;

		RoomInfo() : hGameWorld(NULL) {}
		~RoomInfo() {
			for (auto& p : MsgQueue) delete p;
			//for (auto& p : clientlist) delete p;
			MsgQueue.clear();
			//clientlist.clear();
			if (hGameWorld) {
				TerminateThread(hGameWorld, 0);
			}
		}
};


class AgentServer : public Server
{
public:
	ConnectedServerInfo			m_MainServer;
	std::list<RoomInfo*>		m_RoomList;
	std::list<NGPMSG*>			m_MsgQueue;
	int							m_iRoomCounter;

	CRITICAL_SECTION			m_AgentServer_CS;



public:
	AgentServer();
	~AgentServer();

	// ������ �����ϱ� �����ϴ� �Լ�. High level design�� ���� �Ǿ��ִ� �������� ���� ������ ����ȴ�.
	virtual void Run();

	// Run()�Լ��� �ʹݺο��� ȣ��Ǹ� ���� ������ ���� ��û�� ��ٸ���. ���� ������ ����Ǹ� recv �����尡 �����ȴ�.
	void AcceptMainServer();
	// ���� ������ �˸��� ������Ʈ�� ������ ��û�ϴ� �޽����� ������ ������ ���� ������ 
	// �������� �� ����Ʈ�� ���� �߰��ϰ� ������Ʈ���� �����Ͽ� ������ ���� ������ �����Ѵ�.
	void CreateAgentsToRoom();
	// ������ ����� ���� ������ ȣ��Ǵ� �Լ�. 
	// ���� ���Ḧ �˸��� �޽����� �޾� ������ ����� �뿡 ���Ǵ� �ý��� �ڿ��� ȸ���ϰ� ���� �����Ѵ�.
	void DeleteAgentsFromRoom();

	//std::list<NGPMSG*>&	GetMsgQueue() { return m_MsgQueue; }
};


static int recvn(SOCKET s, char *buf, int len, int flags);

// ���� ���� ���� ���� �ֽ�ȭ
static DWORD WINAPI UpdateWorld(LPVOID arg);

static DWORD WINAPI RunGameWorld(LPVOID arg);

static DWORD WINAPI MessageDispatcher(LPVOID arg);
