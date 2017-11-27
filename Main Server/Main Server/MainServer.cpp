#include "stdafx.h"
#include "Server.h"
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
	int msgtype = 0;
	list<NGPMSG*> *MsgQueue = (list<NGPMSG*>*)arg;
	
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
	ConnectionInfo* client = (ConnectionInfo*)arg;
	int retval;
	char buf[64 + 1];
	int len;
		
	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
	
	while (1) {
		// 데이터 받기(고정 길이)
		retval = recvn(client->sock, (char *)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;
	
		// 데이터 받기(가변 길이)
		retval = recvn(client->sock, buf, len, 0);
		if (retval == SOCKET_ERROR) {
			break;
		}
		else if (retval == 0)
			break;	
	
		// 받은 데이터 출력
		buf[retval] = '\0';
		cout << "ID : " << client->ID << " " << buf << endl;

		retval = send(client->sock, "test", sizeof("test"), NULL);
		if (retval == SOCKET_ERROR)
		{
			closesocket(client->sock);
			return 0;
		}
	}
	
	closesocket(client->sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
	return 0;
}

DWORD WINAPI RoomProcess(LPVOID arg) 
{
	RoomInfo* room = (RoomInfo*)arg;
	
	int i = 0;
	int retval;

	for (auto c : room->clientlist)
	{
		c->RecvThreadHandle = CreateThread(NULL, 0, RecvMessage, (LPVOID)c, 0, NULL);
	}
	
	room->hGameWorld = CreateThread(NULL, 0, RunGameWorld, (LPVOID)&room->MsgQueue, 0, NULL);
	if (room->hGameWorld == NULL) {
		cout << "Thread Creation Failed" << endl;
		return 0;
	}
	

	while (1) {

		if (room) {
			room->clientlist.remove_if([&](ConnectionInfo* client)->bool {
				DWORD exitcode = 0;
				GetExitCodeThread(client->RecvThreadHandle, &exitcode);
				if (exitcode != STILL_ACTIVE) {
					cout << "Client ID : " << client->ID << " Terminated" << endl;
					return true;
				}
				return false;
			});

			if (room->clientlist.empty())
			{
				cout << "Terminate Room " << room->RoomID << endl;
				ExitThread(NULL);
			}
		}
		
		//CreateMSG(MSGTYPE::MSGUPDATE::ADJUSTPOS, )
		for (auto c : room->clientlist)
		{
			// send world update to clients
		}
		
		//cout << "Room " << room->RoomID << " is running client : " << room->clientlist.size() << endl;
	}
	return 0;
}

void MainServer::Run()
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	int roomcounter = 0;
	int clientcounter = 0;
	HANDLE hThread;

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
			RoomInfo* newroom = new RoomInfo();
			newroom->RoomID = roomcounter++;

			for (int i = 0; i < 3; ++i) 
			{
				newroom->clientlist.push_back(m_WaitingClientList.front());
				m_WaitingClientList.pop_front();
			}

			hThread = CreateThread(NULL, 0, RoomProcess, (LPVOID)newroom, 0, NULL);
			if (hThread == NULL) 
			{
				cout << "Thread Creation failed" << endl;
				//Assert
				return;
			}

			m_RoomList.push_back(newroom);
			cout << "Create New Room " << newroom->RoomID << endl;
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
}

void MainServer::DeleteRoom()
{
}
