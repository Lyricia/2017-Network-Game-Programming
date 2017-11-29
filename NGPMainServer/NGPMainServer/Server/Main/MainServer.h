#pragma once
#include "Server\Server.h"

struct ConnectionInfo {
	int				ID;
	SOCKET			sock;
	SOCKADDR_IN		addr;
	HANDLE			RecvThreadHandle;
	list<NGPMSG*>*	*pMsgQueue;
};

struct RoomInfo {
	int						RoomID;
	ConnectionInfo			*serverinfo;
	list<ConnectionInfo*>	clientlist;
	//list<ConnectionInfo*>	agentlist;
	CGameWorld				GameWorld;
	list<NGPMSG*>			MsgQueue;
	HANDLE					hGameWorld;
		
	void UpdateWorld();
	void SendMsgs();
	//void SendMsgs(SOCKET sock, NGPMSG &msg, size_t msgsize){
	//	send(sock, (char*)&msg, msgsize, 0);
	//}
};

class MainServer : public Server
{
	//list<RoomInfo*>			m_RoomList;
	//list<ClientInfo*>			m_WaitingClientList;
	//ConnectedServerInfo		m_AgentServer;
	list<RoomInfo*>				m_RoomList;
	list<ConnectionInfo*>		m_WaitingClientList;
	list<NGPMSG*>				m_MsgQueue;

	shared_ptr<CIndRes>			m_IndRes;

public:
	MainServer();
	~MainServer();

	virtual void Run();

	void ConnectAgentServer();
	void RequestAddAgentServer();
	void CreateRoom();
	void DeleteRoom();
};


