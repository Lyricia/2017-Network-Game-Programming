#include "stdafx.h"
#include "AgentServer.h"

// TCP ȯ�濡�� �����͸� ������ �޾Ƴ��� ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


DWORD WINAPI RunGameWorld(LPVOID arg)
{
	int msgtype = 0;
	std::list<NGPMSG*> *MsgQueue = (std::list<NGPMSG*>*)arg;

	ActionInfo* Actionlist = new ActionInfo();
	ObjInfo* Objlist = new ObjInfo();

	while (1)
	{
		NGPMSG msg;
		if (!MsgQueue->empty())
		{
			msgtype = DispatchMSG(MsgQueue->front(), *Actionlist, *Objlist);
			MsgQueue->pop_front();
		}
		else continue;

		switch (msgtype)
		{
		case MSGTYPE::MSGACTION::MOVE:
			break;

		case MSGTYPE::MSGACTION::SHOOT:

			break;

		case MSGTYPE::MSGACTION::BUILDTURRET:

			break;

		case MSGTYPE::MSGACTION::RELOAD:

			break;


		case MSGTYPE::MSGSTATE::AIAGENTINFO:
			break;

		case MSGTYPE::MSGSTATE::AICREATTIONREQUEST:
			break;

		case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
			break;

		case MSGTYPE::MSGSTATE::CLIENTREADY:
			break;

		case MSGTYPE::MSGSTATE::ROOMCREATION:
			break;


		case MSGTYPE::MSGUPDATE::ADJUSTPOS:
			break;

		case MSGTYPE::MSGUPDATE::CREATEOBJECT:
			break;

		case MSGTYPE::MSGUPDATE::DELETEOBJECT:
			break;

		case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
			break;

		}
	}

	return 0;
}

DWORD WINAPI RecvMessage(LPVOID arg)
{
	ConnectionInfo* Main_Server = (ConnectionInfo*)arg;
	int retval;
	char buf[BUFSIZE + 1];
	int len;

	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(Main_Server->addr.sin_addr), ntohs(Main_Server->addr.sin_port));

	while (1) {
		// ������ �ޱ�(���� ����)
		retval = recvn(Main_Server->sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// ������ �ޱ�(���� ����)
		retval = recvn(Main_Server->sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		std::cout << "ID : " << Main_Server->ID << " " << buf << std::endl;

		retval = send(Main_Server->sock, "test", sizeof("test"), NULL);
		
		if (retval == SOCKET_ERROR)
		{
			closesocket(Main_Server->sock);
			return 0;
		}
	}

	closesocket(Main_Server->sock);

	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(Main_Server->addr.sin_addr), ntohs(Main_Server->addr.sin_port));
	return 0;
}



AgentServer::AgentServer()
{
}

AgentServer::~AgentServer()
{
	for (auto& p : m_RoomList) delete p;
	for (auto& p : m_MsgQueue) delete p;

	m_RoomList.clear();
	m_MsgQueue.clear();
}

void AgentServer::Run()
{
	AcceptMainServer();
	
	int retval;
	char buf[BUFSIZE + 1];
	int len;


	while (1) {
		// ������ �ޱ�(���� ����)
		retval = recvn(m_MainServer.sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		// ������ �ޱ�(���� ����)
		retval = recvn(m_MainServer.sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		//retval = send(Main_Server->sock, "test", sizeof("test"), NULL);

		if (retval == SOCKET_ERROR)
		{
			closesocket(m_MainServer.sock);
			return;
		}
	}

}

void AgentServer::AcceptMainServer()
{
	// �ּ��� ���̸� �޾Ƴ���.
	int addrlen;
	// ���� �� ��� ���� �����Ѵ�.
	int retval;
	

	addrlen = sizeof(m_MainServer.addr);
	m_MainServer.sock = accept(m_ListenSock, (SOCKADDR *)&m_MainServer.addr, &addrlen);
	if (m_MainServer.sock == INVALID_SOCKET)
	{
		std::cout << "client socket error" << std::endl;
	}


}

void AgentServer::CreateAgentsToRoom()
{

	RoomInfo* newroom = new RoomInfo();
	newroom->RoomID = m_iRoomCounter++;

	newroom->GameWorld.RegisterRoomInfo(newroom);
	newroom->GameWorld.Initailize();
	newroom->hGameWorld = CreateThread(NULL, 0, RunGameWorld, (LPVOID)newroom, 0, NULL);

	if (newroom->hGameWorld == NULL)
	{
		std::cout << "Thread Creation failed" << std::endl;
		//Assert
		return;
	}

	m_RoomList.push_back(newroom);
	std::cout << "Create New Room " << newroom->RoomID << std::endl;


}

void AgentServer::DeleteAgentsFromRoom()
{




}
