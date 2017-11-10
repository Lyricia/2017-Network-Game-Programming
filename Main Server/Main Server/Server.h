#pragma once
class Server
{
protected:
	WSADATA			m_WSA;
	SOCKET			m_ListenSock;
	SOCKADDR_IN		m_ServerAddr;
	u_short			m_ServerPort = 9000;
	//MsgQueue		m_MsgQueue;

public:
	Server() {};
	~Server() {};

	void Initialize();
	void Release();
	virtual void Run() = 0;
};

