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

	// 서버가 서비스하기 시작하는 함수. High level design에 정의 되어있는 순서도를 따라 로직이 수행된다.
	virtual void Run();

	// Run()함수의 초반부에서 호출되며 메인 서버의 연결 요청을 기다린다. 메인 서버가 연결되면 recv 스레드가 생성된다.
	void AcceptMainServer();
	// 룸의 생성을 알리고 에이전트의 생성을 요청하는 메시지를 받으면 생성된 룸의 정보를 
	// 바탕으로 룸 리스트에 룸을 추가하고 에이전트들을 생성하여 정보를 메인 서버에 전달한다.
	void CreateAgentsToRoom(UINT room_id);
	// 게임이 종료된 룸이 있으면 호출되는 함수. 
	// 게임 종료를 알리는 메시지를 받아 게임이 종료된 룸에 사용되던 시스템 자원을 회수하고 룸을 삭제한다.
	void DeleteAgentsFromRoom(UINT room_id);
	//std::list<NGPMSG*>&	GetMsgQueue() { return m_MsgQueue; }

	void EnterCriticalSection() { ::EnterCriticalSection(&ServerCs); }
	void LeaveCriticalSection() { ::LeaveCriticalSection(&ServerCs); }
};


static int recvn(SOCKET s, char *buf, int len, int flags);

// 단일 룸의 게임 월드 최신화
static DWORD WINAPI UpdateWorld(LPVOID arg);

static DWORD WINAPI RunGameWorld(LPVOID arg);

static DWORD WINAPI MessageDispatcher(LPVOID arg);


struct ThreadPack {
	AgentServer* server;
	int			m_roomid;
};