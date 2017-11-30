#include "stdafx.h"

#include "Timer\Timer.h"
#include "Scene\Main\MainScene.h"
#include "Server\AgentServer.h"
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
	m_pTimer = std::make_unique<CTimer>();

	m_pMainScene = std::make_unique<CMainScene>();
	m_pMainScene->RegisterRoomInfo(m_pRoomInfo);
	m_pMainScene->OnCreate(L"Main", this);
	return true;
}

void CGameWorld::Run()
{
	MSG msg;

	// �⺻ �޽��� �����Դϴ�.
	while (true)
	{
		CheckClients();
		ProcessMsgs();
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!m_pTimer->Update()) continue;

		// 1�ʿ� 30��
		Update(m_pTimer->GetTimeElapsed());

		// ���� �۾��� ����� �������� Send�Ѵ�.
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

void CGameWorld::CheckClients()
{
	//if (m_pRoomInfo) {
	//	m_pRoomInfo->clientlist.remove_if([&](ConnectionInfo* client)->bool {
	//		DWORD exitcode = 0;
	//		GetExitCodeThread(client->RecvThreadHandle, &exitcode);
	//		if (exitcode != STILL_ACTIVE) {
	//			std::cout << "Client ID : " << client->ID << " Terminated" << std::endl;
	//			return true;
	//		}
	//		return false;
	//	});
	//
	//	if (m_pRoomInfo->clientlist.empty())
	//	{
	//		std::cout << "Terminate Room " << m_pRoomInfo->RoomID << std::endl;
	//		ExitThread(NULL);
	//	}
	//}
}