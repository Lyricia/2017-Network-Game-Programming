#pragma once

class MainServer : public Server
{
	//list<RoomInfo*>			m_RoomList;
	//list<ClientInfo*>			m_WaitingClientList;
	//ConnectedServerInfo		m_AgentServer;

public:
	MainServer() {};
	~MainServer() {};

	virtual void Run();

	void ConnectAgentServer();
	void RequestAddAgentServer();
	void CreateRoom();
	void DeleteRoom();
};

