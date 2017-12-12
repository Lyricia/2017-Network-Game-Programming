#include "stdafx.h"

#include "Timer\Timer.h"
#include "Scene\Main\MainScene.h"
#include "Server\Main\MainServer.h"
#include "GameWorld.h"


void CGameWorld::Update(float fTimeElapsed)
{
	m_pMainScene->Update(fTimeElapsed);
}

CGameWorld::CGameWorld()
	: m_pTimer(nullptr)
	, m_pMainScene(nullptr)
{
}

CGameWorld::~CGameWorld()
{
}

bool CGameWorld::Initailize()
{
	m_pTimer = make_unique<CTimer>();

	m_pMainScene = make_unique<CMainScene>();
	m_pMainScene->RegisterRoomInfo(m_pRoomInfo);
	m_pMainScene->OnCreate(L"Main"s, this);
	return true;
}

void CGameWorld::Run()
{
	MSG msg;

	// 기본 메시지 루프입니다.
	while (true)
	{
		if (!CheckClients()) break;
		ProcessMsgs();
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		BYTE time_state = m_pTimer->Update();
		if (time_state & CTimer::TIME_STATE_UPDATE_GAMELOOP)
			Update(m_pTimer->GetTimeElapsed());
		if (time_state & CTimer::TIME_STATE_PROCESS_MESSAGE)
			SendMsgs();
	}
}

void CGameWorld::ProcessMsgs()
{
	m_pMainScene->ProcessMsgs();
}
void CGameWorld::SendMsgs()
{
	m_pMainScene->SendMsgs();
}

bool CGameWorld::CheckClients()
{
	if (m_pRoomInfo) {
		m_pRoomInfo->clientlist.remove_if([&](ConnectionInfo* client)->bool {
			DWORD exitcode = 0;
			GetExitCodeThread(client->RecvThreadHandle, &exitcode);
			if (exitcode != STILL_ACTIVE) {
				cout << "Client ID : " << client->ID << " Terminated" << endl;
				delete client;
				return true;
			}
			return false;
		});

		if (m_pRoomInfo->clientlist.empty())
		{
			cout << "Terminate Room " << m_pRoomInfo->RoomID << endl;

			NGPMSG* msg = CreateMSG(
				MSGTYPE::CLIENTGAMEOVER
				, m_pRoomInfo->RoomID
				, 0
				, 0
				, 0
				, nullptr
				, nullptr
			);

			int retval = send(m_pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}

			return false;
		}
	}
	return true;
}