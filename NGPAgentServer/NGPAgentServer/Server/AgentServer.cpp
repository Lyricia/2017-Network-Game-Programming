#include "stdafx.h"
#include "AgentServer.h"

// TCP 환경에서 데이터를 완전히 받아내기 위한 함수
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


// Update World
DWORD WINAPI UpdateWorld(LPVOID arg)
{
	int msgtype = 0;
	

	ActionInfo* Actionlist = new ActionInfo();
	ObjInfo* Objlist = new ObjInfo();

	while (1)
	{

	}

	return 0;
}

DWORD WINAPI MessageDispatcher(LPVOID arg)
{
	AgentServer* Server = (AgentServer*)arg;
	int retval;
	int len;


	while (1) {
		


		if (!Server->m_MsgQueue.empty())
		{

			NGPMSG msg =  Server->m_MsgQueue.front();
			Server->m_MsgQueue.front();






		}
		


		if (retval == SOCKET_ERROR)
		{
			return 0;
		}
	}





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

// 룸 정보를 받아와서 지속적인 갱신을 한다.
DWORD WINAPI RunGameWorld(LPVOID arg)
{
	RoomInfo* room = (RoomInfo*)arg;
	int retval;

	room->GameWorld.Run();

	return 0;
}

void AgentServer::Run()
{
	AcceptMainServer();
	
	int retval;
	char buf[BUFSIZE + 1];
	int len;
	////////////////////////////////////////////////////////////////////////////////////////
	
	HANDLE Dispatcher = CreateThread(NULL, 0, MessageDispatcher, this , 0, NULL);



	while (1) {
		NGPMSG* msg = new NGPMSG();

		// 데이터 받기(고정 길이)
		// 메세지의 헤더만큼 읽는다.
		retval = recvn(m_MainServer.sock, (char *)&msg, sizeof(NGPMSG), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;

		

		m_MsgQueue.push_back(msg);

		if (retval == SOCKET_ERROR)
		{
			closesocket(m_MainServer.sock);
			return;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////

}

void AgentServer::AcceptMainServer()
{
	// 주소의 길이를 받아낸다.
	int addrlen;
	// 오류 시 결과 값을 저장한다.
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
	
	//HANDLE hThread = CreateThread(NULL, 0, RoomProcess, (LPVOID)newroom, 0, NULL);
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


	std::cout << "Delete Room "  << std::endl;

}
