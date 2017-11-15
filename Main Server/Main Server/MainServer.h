#pragma once

struct ConnectionInfo {
	int				ID;
	SOCKET			sock;
	SOCKADDR_IN		addr;
};

struct RoomInfo {
	int						RoomID;
	list<ConnectionInfo*>	clientlist;
	ConnectionInfo*			serverinfo;
	list<NGPMSG*>			MsgQueue;
};

class MainServer : public Server
{
	//list<RoomInfo*>			m_RoomList;
	//list<ClientInfo*>			m_WaitingClientList;
	//ConnectedServerInfo		m_AgentServer;
	list<RoomInfo*>				m_RoomList;
	list<ConnectionInfo*>		m_WaitingClientList;
	list<NGPMSG*>				m_MsgQueue;

public:
	MainServer() {};
	~MainServer() {};


	virtual void Run();

	void ConnectAgentServer();
	void RequestAddAgentServer();
	void CreateRoom();
	void DeleteRoom();
};


