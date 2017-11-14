#pragma once

class clientinfo {
public:
	SOCKET clientsock;
	SOCKADDR_IN clientaddr;
};


class MainServer : public Server
{
	//list<RoomInfo*>			m_RoomList;
	//list<ClientInfo*>			m_WaitingClientList;
	//ConnectedServerInfo		m_AgentServer;

	list<clientinfo*>			m_WaitingClientList;
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


