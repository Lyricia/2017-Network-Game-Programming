#pragma once
#include "stdafx.h"
#include "Server.h"


struct ConnectionInfo {
	int				ID;
	SOCKET			sock;
	SOCKADDR_IN		addr;
	HANDLE			RecvThreadHandle;
};

struct RoomInfo {
		int							RoomID;
		HANDLE						hGameWorld;
		std::list<NGPMSG*>			*MsgQueue;
		ConnectionInfo				*serverinfo;
		std::list<ConnectionInfo*>	clientlist;
		std::list<ConnectionInfo*>	agentlist;
		//GameWorld				GameWorld;

		void UpdateWorld();
		void SendMsgs(SOCKET sock, NGPMSG &msg, size_t msgsize) {
			send(sock, (char*)&msg, msgsize, 0);
		}
};


class AgentServer : public Server
{
private:
	ConnectedServerInfo		m_MainServer;
	std::list<RoomInfo*>	m_RoomList;

public:
	AgentServer() {};
	~AgentServer() {};

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
};