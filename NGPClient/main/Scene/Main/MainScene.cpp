#include "stdafx.h"

#include "Framework/Framework.h"
#include "Framework/ResourceManager/ResourceManager.h"
#include "Framework/IndRes/IndRes.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Projectile\Grenade\Grenade.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Effect\Effect.h"
#include "Object\Unit\Agent\Turret\Turret.h"

#include "Framework\Client\Client.h"
#include "MainScene.h"


CMainScene::CMainScene()
{
}
CMainScene::~CMainScene()
{
	for (auto& p : m_vecObjects)
		delete p;
	m_vecObjects.clear();
	for (auto& p : m_lstEffects)
		delete p;
	m_lstEffects.clear();
}

bool CMainScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
	{
		
		break;
	}
	case WM_LBUTTONUP:		
	case WM_MBUTTONUP:		
	case WM_RBUTTONUP:		
	case WM_MOUSEWHEEL:		
		break;
	default:
		return false;
	}

	return(true);
}

bool CMainScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		auto rcClient = m_pFramework->GetClientSize();
		m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		return OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		return OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	default:
		return false;
	}

	return true;
}

bool CMainScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'Z': m_Camera.Scale(m_Camera.GetScale() * 2.f);
			break;
		case 'X': m_Camera.Scale(m_Camera.GetScale() * 0.5f);
			break;
		default:
			return false;
		}
		break;
	default:
		return false;
	}
	return(true);
}

bool CMainScene::OnCreate(wstring && tag, CFramework * pFramework)
{
	if (!Base::OnCreate(std::move(tag), pFramework)) return false;

	m_hWnd = pFramework->GethWnd();
	m_pClient = pFramework->GetClient();
	m_pResMng = pFramework->GetResourceManager();

	m_bmpBackGround = m_pResMng->GetImageRef(ResImgName::map_image);
	m_bmpCrossHair = m_pResMng->GetImageRef(ResImgName::aim);

	CObject::ResetIDCount();

	int map_size_half = g_iMapSize / 2;
	for(int i = 0; i < g_iMapSize; ++i)
		for (int j = 0; j < g_iMapSize; ++j)
		{
			if (g_iMap[j + i * g_iMapSize] == 1)
			{
				CBrick* brick = new CBrick(Point2F((j - map_size_half)*g_iMapSize, (i - map_size_half)*g_iMapSize));
				brick->RegisterResourceManager(m_pResMng);
				m_vecObjects.push_back(brick);
			}
			
		}

	for (int i = 0; i< 3; ++i)
	{
		CAgent* agent = new CAgent(Point2F(-100, 100 * i));
		agent->RegisterResourceManager(m_pResMng);
		m_vecObjects.push_back(agent);
	}

	for (int i=0; i< 3; ++i)
	{
		CPlayer* player = new CPlayer(Point2F(-100, 10));
		player->RegisterResourceManager(m_pResMng);
		m_vecObjects.push_back(player);
	}

	for (auto& p : m_vecObjects)
		if (p->GetID() == m_pClient->GetClientID())
			m_pPlayer = static_cast<CPlayer*>(p);

	auto rcClient = pFramework->GetClientSize();
	m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
	m_Camera.SetPosition(m_pPlayer->GetPos());
	m_Camera.SetAnchor(Point2F(0.0f, 0.0f));

	return true;
}

void CMainScene::ProcessMsgs()
{
	auto start_time		= std::chrono::system_clock::now();
	auto now			= std::chrono::system_clock::now();
	std::chrono::duration<float> timeElapsed = start_time - now;

	NGPMSG*		msg				= nullptr;
	ActionInfo* arrActionInfo	= nullptr;
	ObjInfo*	arrObjInfo		= nullptr;
	MapInfo*	arrMapInfo		= nullptr;

	while (timeElapsed.count() < MESSAGE_PROCESSING_TIME)
	{
		if (!m_pClient->MsgQueue().size()) return;
		m_pClient->EnterCriticalSection();
		NGPMSG* msg = m_pClient->MsgQueue().front();
		m_pClient->MsgQueue().pop_front();
		m_pClient->LeaveCriticalSection();

		int nObjInfo = msg->header.NUM_OBJINFO;
		switch (msg->header.MSGTYPE)
		{
		case MSGTYPE::MSGACTION::SHOOT:
		{
			D2D_POINT_2F hit_pos = Point2F();
			arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
			{
				for (auto iter = m_vecObjects.rbegin();
					iter != m_vecObjects.rend(); ++iter)
				{
					if (msg->header.OBJECTNO == (*iter)->GetID())
					{

						switch ((*iter)->GetTag())
						{
						case CObject::Type::Player:
						{
							CPlayer* player = static_cast<CPlayer*>(*iter);
							player->Shoot(arrActionInfo[i].TargetHitPos);
							break;
						}
						case CObject::Type::Agent:
						{
							CAgent* agent = static_cast<CAgent*>(*iter);
							agent->Shoot(arrActionInfo[i].TargetHitPos);
							break;
						}
						}
					}
				}
			}
			delete[] arrActionInfo;
			arrActionInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
		{
			MessageBox(NULL, L"½Â¸®!", L"GAME OVER!", MB_ICONEXCLAMATION);
			char input_c[10];
			while (true)
			{
				HWND consoleWindowHandle = GetConsoleWindow();
				SetWindowPos(
					consoleWindowHandle,
					HWND_TOPMOST,
					0, 0,
					0, 0,
					SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
				ShowWindow(consoleWindowHandle, SW_NORMAL);
				printf("Restart? ( y or n ) :");
				fgets(input_c, sizeof(input_c), stdin);
				input_c[strlen(input_c) - 1] = '\0';
				if (strcmp(input_c, "y") == 0)
				{
					m_pFramework->RestartCurrScene();
					return;
				}
				else if (strcmp(input_c, "n") == 0)
				{
					m_pFramework->Stop();
					return;
				}
			}
			return;
			break;
		}
		case MSGTYPE::MSGSTATE::CLIENTREADY:
			break;
		case MSGTYPE::MSGSTATE::ROOMCREATION:
			break;
		case MSGTYPE::MSGUPDATE::ADJUSTPOS:
			break;
		case MSGTYPE::MSGUPDATE::CREATEOBJECT:
		{
			arrObjInfo = new ObjInfo[msg->header.NUM_OBJINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < nObjInfo; ++i)
			{
				switch (arrObjInfo[i].ObjectType)
				{
				case OBJECTTYPE::Effect_ShootingHit:
				{
					auto& rc = SizeToRect(SizeF(32, 32));
					CEffect* effect = new CEffect(arrObjInfo[i].Position, rc);
					auto& img = m_pResMng->GetImageRef(ResImgName::MagicBlast);
					auto& sz = m_pResMng->GetImgLength(ResImgName::MagicBlast);
					effect->RegisterEffectSprite(img, sz);
					m_lstEffects.push_back(effect);
					break;
				}
				case OBJECTTYPE::Effect_Explosion:
				{
					auto rc = SizeToRect(SizeF(GRENADE_EXPLOSION_RANGE, GRENADE_EXPLOSION_RANGE));
					CEffect* effect = new CEffect(arrObjInfo[i].Position, rc);
					auto& img = m_pResMng->GetImageRef(ResImgName::Explosion256);
					auto& sz = m_pResMng->GetImgLength(ResImgName::Explosion256);
					effect->RegisterEffectSprite(img, sz);
					m_lstEffects.push_back(effect);
					break;
				}
				case OBJECTTYPE::Grenade:
				{
					CGrenade* grenade = new CGrenade(arrObjInfo[i].Position);
					grenade->RegisterResourceManager(m_pResMng);
					for (auto iter = m_vecObjects.rbegin();
						iter != m_vecObjects.rend(); ++iter)
						if (arrObjInfo[i].ParentID == (*iter)->GetID())
						{
							grenade->SetParent(*iter);
							break;
						}
					grenade->SetVelocity(arrObjInfo[i].Velocity);
					m_vecObjects.push_back(grenade);
					break;
				}
				case OBJECTTYPE::Turret:
				{
					CTurret* turret = new CTurret(arrObjInfo[i].Position);
					turret->RegisterResourceManager(m_pResMng);
					for (auto iter = m_vecObjects.rbegin();
						iter != m_vecObjects.rend(); ++iter)
						if (arrObjInfo[i].ParentID == (*iter)->GetID())
						{
							turret->SetParent(*iter);
							break;
						}
					m_vecObjects.push_back(turret);
					break;
				}
				}

			}
			delete[] arrObjInfo;
			arrObjInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGUPDATE::DELETEOBJECT:
		{
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (auto& iter = m_vecObjects.begin(); iter != m_vecObjects.end();)
			{
				if (msg->header.OBJECTNO == (*iter)->GetID())
				{
					if ((*iter) == m_pPlayer)
					{
						m_pPlayer = nullptr;
						MessageBox(NULL, L"ÆÐ¹è!", L"GAME OVER!", MB_ICONEXCLAMATION);
						char input_c[10];
						while (true)
						{
							HWND consoleWindowHandle = GetConsoleWindow();
							SetWindowPos(
								consoleWindowHandle,
								HWND_TOPMOST,
								0, 0,
								0, 0,
								SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
							ShowWindow(consoleWindowHandle, SW_NORMAL);
							printf("Restart? ( y or n ) :");
							fgets(input_c, sizeof(input_c), stdin);
							input_c[strlen(input_c) - 1] = '\0';
							if (strcmp(input_c, "y") == 0) 
							{
								m_pFramework->RestartCurrScene();
								return;
							}
							else if (strcmp(input_c, "n") == 0)
							{
								m_pFramework->Stop();
								return;
							}
						}
						return;
					}
					delete (*iter);
					iter = m_vecObjects.erase(iter);
				}
				else ++iter;
			}
			break;
		}
		case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
		{
			arrObjInfo = new ObjInfo[msg->header.NUM_OBJINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < nObjInfo; ++i)
				if (arrObjInfo[i].ObjectType != OBJECTTYPE::Brick)
				{
					for (auto iter = m_vecObjects.rbegin();
						iter != m_vecObjects.rend(); ++iter)
						if (arrObjInfo[i].ObjectID == (*iter)->GetID())
						{
							(*iter)->SetObjectInfo(&arrObjInfo[i]);
							break;
						}
				}
			delete[] arrObjInfo;
			arrObjInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGUPDATE::UPDATEMAPSTATE:
		{
			arrMapInfo = new MapInfo[msg->header.NUM_OBJINFO];
			DispatchMSG(msg, arrMapInfo);
			for (int i = 0; i < nObjInfo; ++i)
				for (auto& p : m_vecObjects)
					if (arrMapInfo[i].ObjectID == p->GetID())
					{
						p->SetObjectInfo(&arrMapInfo[i]);
						break;
					}
			delete[] arrMapInfo;
			arrMapInfo = nullptr;
			break;
		}
		}

		delete msg;
		msg = nullptr;

		now = std::chrono::system_clock::now();
		timeElapsed = start_time - now;
	}
}

void CMainScene::Update(float fTimeElapsed)
{
	if (m_pPlayer) ProcessInput(fTimeElapsed);

	for (auto& p : m_vecObjects)
		p->Update(fTimeElapsed);

	if (m_pPlayer) m_Camera.SetPosition(m_pPlayer->GetPos());
	if (m_pPlayer) m_pPlayer->RayCastingToShoot(m_vecObjects);

	for (auto& p : m_lstEffects)
		p->Update(fTimeElapsed);
}

void CMainScene::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	auto cameramtx = m_Camera.RegenerateViewMatrix();
	pd2dRenderTarget->SetTransform(cameramtx);

	auto& szImg = m_bmpBackGround->GetSize();
	auto& rcBK = SizeToRect(SizeF(szImg.width * 4, szImg.height * 4));

	pd2dRenderTarget->DrawBitmap(m_bmpBackGround.Get(), rcBK);

	for (auto& p : m_vecObjects)
		p->Draw(pd2dRenderTarget);

	if(m_pPlayer) m_pPlayer->Draw(pd2dRenderTarget);

	for (auto& p : m_lstEffects)
		p->Draw(pd2dRenderTarget);

	if (m_pPlayer)
	{
		m_pPlayer->DrawUI(pd2dRenderTarget, m_Camera.GetScaleFactor());

		pd2dRenderTarget->DrawBitmap(
			m_bmpCrossHair.Get()
			, SizeToRect(m_bmpCrossHair->GetSize()) +
			m_ptMouseCursor * m_Camera.GetScaleFactor() +
			m_pPlayer->GetPos());
	}
}

void CMainScene::ProcessInput(float fTimeElapsed)
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	D2D_POINT_2F ptDir = Point2F();

	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer['W'] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer['S'] & 0xF0) dwDirection |= DIR_DOWN;
		if (pKeyBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer['G'] & 0xF0) m_pPlayer->GrenadeOut(m_pClient.get());
		if (pKeyBuffer['E'] & 0xF0) m_pPlayer->DeployTurret(m_pClient.get());

		if (pKeyBuffer[VK_LBUTTON] & 0xF0) m_pPlayer->Shoot(m_pClient.get());
		if (pKeyBuffer[VK_SPACE] & 0xF0) m_pPlayer->Stop(m_pClient.get());

		if (dwDirection & DIR_UP) ptDir.y += -1;
		if (dwDirection & DIR_DOWN) ptDir.y += 1;
		if (dwDirection & DIR_LEFT) ptDir.x += -1;
		if (dwDirection & DIR_RIGHT) ptDir.x += 1;
		if (dwDirection && Length(ptDir)) ptDir = Normalize(ptDir);

	}
	static RECT rcWindow;
	static POINT ptCursorPos;
	::GetWindowRect(m_hWnd, &rcWindow);
	::GetCursorPos(&ptCursorPos);
	auto rcClient = m_pFramework->GetClientSize();
	m_ptMouseCursor = Point2F(
		ptCursorPos.x - rcWindow.left - 5 - rcClient.right / 2
		, ptCursorPos.y - rcWindow.top - 30 - rcClient.bottom / 2);

	m_pPlayer->LookAt((m_ptMouseCursor * m_Camera.GetScaleFactor()) + m_pPlayer->GetPos());
	m_pPlayer->Move(m_pClient.get(), ptDir);
}