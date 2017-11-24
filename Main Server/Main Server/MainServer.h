#pragma once

struct ConnectionInfo {
	int				ID;
	SOCKET			sock;
	SOCKADDR_IN		addr;
	HANDLE			RecvThreadHandle;
};

struct RoomInfo {
	int						RoomID;
	ConnectionInfo*			serverinfo;
	list<ConnectionInfo*>	clientlist;
	//list<ConnectionInfo*>	agentlist;
	list<NGPMSG*>			MsgQueue;
	//GameWorld				GameWorld;
	//HANDLE				hGameWorld

	//void UpdateWorld();
	void SendMsgs(SOCKET sock, NGPMSG &msg, size_t msgsize){
		send(sock, (char*)&msg, msgsize, 0);
	}
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


