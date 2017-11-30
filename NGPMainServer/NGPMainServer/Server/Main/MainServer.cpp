#include "stdafx.h"
#include "GameWorld\Timer\Timer.h"
#include "GameWorld\GameWorld.h"

#include "MainServer.h"

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
	RoomInfo* room = (RoomInfo*)arg;

	int i = 0;
	int retval;

	for (auto& client : room->clientlist)
	{
		client->pMsgQueue = &room->MsgQueue;
		client->pCs = &room->Cs;
		client->RecvThreadHandle = CreateThread(NULL, 0, RecvMessage, (LPVOID)client, 0, NULL);
	}
	room->GameWorld.Run();

	return 0;
}

DWORD WINAPI RecvMessage(LPVOID arg) 
{
	ConnectionInfo* client = (ConnectionInfo*)arg;
	int retval;
	
	NGPMSG* msg = new NGPMSG();

	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
	
	while (1) {
		retval = recvn(client->sock, (char *)msg, sizeof(NGPMSG), 0);
		//retval = WSAGetLastError();
		if (retval == SOCKET_ERROR)
		{
			break;
		}
		client->EnterCriticalSection();
		client->pMsgQueue->push_back(msg);
		client->LeaveCriticalSection();

		cout << "ID : " << client->ID << "msg id:" << msg->header.OBJECTNO << endl;
	}
	
	closesocket(client->sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
	return 0;
}

MainServer::MainServer()
	: m_iRoomCounter(0)
{

}
MainServer::~MainServer()
{
	for (auto& p : m_RoomList) delete p;
	for (auto& p : m_WaitingClientList) delete p;
	for (auto& p : m_MsgQueue) delete p;

	m_RoomList.clear();
	m_WaitingClientList.clear();
	m_MsgQueue.clear();
}

void MainServer::Run()
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	int clientcounter = 0;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(m_ListenSock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			cout << "client socket error" << endl;
			break;
		}

		ConnectionInfo* newclient = new ConnectionInfo();
		newclient->ID = clientcounter++;
		newclient->addr = clientaddr;
		newclient->sock = client_sock;
		m_WaitingClientList.push_back(newclient);

		if (m_WaitingClientList.size() >= 3)
		{
			CreateRoom();
		}
	} 
}

void MainServer::ConnectAgentServer()
{
}

void MainServer::RequestAddAgentServer()
{
}

void MainServer::CreateRoom()
{
	RoomInfo* newroom = new RoomInfo();
	newroom->RoomID = m_iRoomCounter++;

	for (int i = 0; i < 3; ++i)
	{
		newroom->clientlist.push_back(m_WaitingClientList.front());
		m_WaitingClientList.pop_front();
	}
	newroom->GameWorld.RegisterRoomInfo(newroom);
	newroom->GameWorld.Initailize();
	newroom->hGameWorld = CreateThread(NULL, 0, RunGameWorld, (LPVOID)newroom, 0, NULL);
	if (newroom->hGameWorld == NULL)
	{
		cout << "Thread Creation failed" << endl;
		//Assert
		return;
	}

	m_RoomList.push_back(newroom);
	cout << "Create New Room " << newroom->RoomID << endl;
}

void MainServer::DeleteRoom(UINT room_id)
{
	m_RoomList.remove_if([room_id](RoomInfo* pRoom)->bool {
		if (pRoom->RoomID == room_id)
		{
			delete pRoom;
			return true;
		}
		return false;
	});
}