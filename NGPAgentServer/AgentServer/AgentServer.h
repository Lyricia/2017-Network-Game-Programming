#pragma once
#include "stdafx.h"
#include "Server.h"

class AgentServer : public Server
{

	//ConnectedServerInfo		m_AgentServer;
	//list<RoomInfo*>			m_RoomList;

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