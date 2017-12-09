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

void CGameWorld::Stop()
{
	m_bRun = false;
}

void CGameWorld::Run()
{
	MSG msg;

	// 기본 메시지 루프입니다.
	while (m_bRun)
	{
		CheckClients();
		ProcessMsgs();

		if (!m_pTimer->Update()) continue;

		// 1초에 30번
		Update(m_pTimer->GetTimeElapsed());

		// 내가 작업한 결과를 서버에게 Send한다.
		SendMsgs();
	}
}

void CGameWorld::ProcessMsgs()
{
	//std::cout << "ProcessMsgs" << std::endl;
	m_pMainScene->ProcessMsgs();
}
void CGameWorld::SendMsgs()
{
	m_pMainScene->SendMsgs();
}

void CGameWorld::CheckClients()
{
	//std::cout << "CheckClient" << std::endl;

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