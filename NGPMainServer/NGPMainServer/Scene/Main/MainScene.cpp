#include "stdafx.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Projectile\Grenade\Grenade.h"
#include "Object\Effect\Effect.h"

#include "Server\Main\MainServer.h"

#include "MainScene.h"


CMainScene::CMainScene()
{
}
CMainScene::~CMainScene()
{
	for (auto& p : m_vecObjects)
		delete p;
	m_vecObjects.clear();
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
		auto rcClient = m_pGameWorld->GetClientSize();
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

bool CMainScene::OnCreate(wstring && tag, CGameWorld* pGameWorld)
{
	if (!Base::OnCreate(std::move(tag), pGameWorld)) return false;

	//m_hWnd = pGameWorld->GethWnd();
	//m_pResMng = pGameWorld->GetResourceManager();

	//auto rcClient = pGameWorld->GetClientSize();
	//m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
	//auto rendertarget = pGameWorld->GetRenderTarget();
	//
	//m_bmpBackGround = m_pResMng->GetImageRef(ResImgName::map_image);
	//m_bmpCrossHair = m_pResMng->GetImageRef(ResImgName::aim);

	//m_ptCamera = Point2F();
	//m_Camera.SetPosition(m_ptCamera);
	//m_Camera.SetAnchor(Point2F(0.0f, 0.0f));

	int map_size_half = g_iMapSize / 2;
	for (int i = 0; i < g_iMapSize; ++i)
		for (int j = 0; j < g_iMapSize; ++j)
		{
			if (g_iMap[j + i * g_iMapSize] == 1)
			{
				CBrick* brick = new CBrick(Point2F((j - map_size_half)*g_iMapSize, (i - map_size_half)*g_iMapSize));
				m_vecObjects.push_back(brick);
			}
		}

	for(auto& p: m_pRoomInfo->clientlist)
	{
		CPlayer* player = new CPlayer(Point2F(-100, 10));
		p->ID = player->GetID();
		m_vecObjects.push_back(player);
		//send(p->sock, &p->ID, sizeof(int), 0);
	}

	return true;
}

void CMainScene::PreprocessingUpdate(float fTimeElapsed)
{
	for (auto& iter = m_vecObjects.begin(); iter != m_vecObjects.end();)
	{
		switch ((*iter)->GetTag())
		{
		case CObject::Type::Player:
		{
			CPlayer* player = static_cast<CPlayer*>((*iter));
			if (player->IsDie())
			{
				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>((*iter));
			if (brick->IsBroken())
			{
				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		case CObject::Type::Grenade:
		{
			CGrenade* grenade = static_cast<CGrenade*>((*iter));
			if (grenade->IsExplosion())
			{
				grenade->Explosion(m_vecObjects);

				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		default:
			++iter;
			break;
		}
	}
}

void CMainScene::Update(float fTimeElapsed)
{
	ProcessInput(fTimeElapsed);
	PreprocessingUpdate(fTimeElapsed);

	m_Camera.SetPosition(m_ptCamera);

	for (auto& p : m_vecObjects)
		p->Update(fTimeElapsed);

	PhysicsUpdate(fTimeElapsed);
}

void CMainScene::PhysicsUpdate(float fTimeElapsed)
{
	D2D_POINT_2F dir = Point2F();

	for (auto& p : m_vecObjects)
	{
		switch (p->GetTag())
		{
		case CObject::Type::Player:
		{
			for (auto& q : m_vecObjects)
			{
				if (p == q) continue;
				switch (q->GetTag())
				{
				case CObject::Type::Brick:
				{
					dir = Normalize(q->GetPos() - p->GetPos());
					if (PtInRect(
						&(q->GetSize() + q->GetPos())
						, p->GetPos() + (dir * p->GetSize().right))
						)
					{
						CPlayer* player = static_cast<CPlayer*>(p);
						player->Reflection(-1.f * dir);
						player->Move(-PLAYER_VELOCITY * dir * fTimeElapsed);
					}
					break;
				}
				}
			}
			break;
		}
		case CObject::Type::Brick:
		{
			break;
		}
		case CObject::Type::Grenade:
		{
			for (auto& q : m_vecObjects)
			{
				if (p == q) continue;
				switch (q->GetTag())
				{
				case CObject::Type::Brick:
				{
					dir = Normalize(q->GetPos() - p->GetPos());
					if (PtInRect(
						&(q->GetSize() + q->GetPos())
						, p->GetPos() + (dir * p->GetSize().right))
						)
					{
						CGrenade* grenade = static_cast<CGrenade*>(p);
						grenade->Reflection(-1.f * dir);
					}
					break;
				}
				}
			}
			break;
		}
		}
		
	}
}

//void CMainScene::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
//{
//	auto cameramtx = m_Camera.RegenerateViewMatrix();
//	pd2dRenderTarget->SetTransform(cameramtx);
//
//	auto& szImg = m_bmpBackGround->GetSize();
//	auto& rcBK = SizeToRect(SizeF(szImg.width * 4, szImg.height * 4));
//
//	pd2dRenderTarget->DrawBitmap(m_bmpBackGround.Get(), rcBK);
//
//	for (auto& p : m_vecObjects)
//		p->Draw(pd2dRenderTarget);
//
//	for (auto& p : m_lstEffects)
//		p->Draw(pd2dRenderTarget);
//
//	pd2dRenderTarget->DrawBitmap(
//		m_bmpCrossHair.Get()
//		, SizeToRect(m_bmpCrossHair->GetSize()) + 
//		m_ptMouseCursor * m_Camera.GetScaleFactor() +
//		m_ptCamera);
//
//	auto pt = m_ptMouseCursor * m_Camera.GetScaleFactor() +
//		m_ptCamera;
//	printf("\r %f, %f", pt.x, pt.y);
//}

void CMainScene::ProcessInput(float fTimeElapsed)
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;

	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer['W'] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer['S'] & 0xF0) dwDirection |= DIR_DOWN;
		if (pKeyBuffer['A'] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer['D'] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer['G'] & 0xF0);

		D2D_POINT_2F ptDir = Point2F();
		if (dwDirection & DIR_UP) ptDir.y += -1;
		if (dwDirection & DIR_DOWN) ptDir.y += 1;
		if (dwDirection & DIR_LEFT) ptDir.x += -1;
		if (dwDirection & DIR_RIGHT) ptDir.x += 1;
		if (dwDirection && Length(ptDir))
		{
			m_ptCamera += Normalize(ptDir) * 100 * fTimeElapsed;
		}

		if (pKeyBuffer[VK_LBUTTON] & 0xF0)
		{
			//CEffect* effect = m_pPlayer->Shoot();
			//if (effect)
			//	m_lstEffects.push_back(effect);
		}
	}
}

void CMainScene::ProcessMsgs()
{
	int msgtype = 0;

	//ActionInfo* Actionlist = new ActionInfo();
	//ObjInfo* Objlist = new ObjInfo();

	//NGPMSG msg;
	//::ZeroMemory(&msg, sizeof(msg));
	//if (!m_pRoomInfo->MsgQueue.empty())
	//{
	//	msgtype = DispatchMSG(m_pRoomInfo->MsgQueue.front(), *Actionlist, *Objlist);
	//	m_pRoomInfo->MsgQueue.pop_front();
	//}
	//else return;
	//
	//switch (msgtype)
	//{
	//case MSGTYPE::MSGACTION::MOVE:
	//	cout << "c ID: " << msg.header.OBJECTNO << " move!" << endl;
	//	break;
	//case MSGTYPE::MSGACTION::SHOOT:
	//	break;
	//case MSGTYPE::MSGACTION::BUILDTURRET:
	//	break;
	//case MSGTYPE::MSGACTION::RELOAD:
	//	break;
	//case MSGTYPE::MSGSTATE::AIAGENTINFO:
	//	break;
	//case MSGTYPE::MSGSTATE::AICREATTIONREQUEST:
	//	break;
	//case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
	//	break;
	//case MSGTYPE::MSGSTATE::CLIENTREADY:
	//	break;
	//case MSGTYPE::MSGSTATE::ROOMCREATION:
	//	break;
	//case MSGTYPE::MSGUPDATE::ADJUSTPOS:
	//	break;
	//case MSGTYPE::MSGUPDATE::CREATEOBJECT:
	//	break;
	//case MSGTYPE::MSGUPDATE::DELETEOBJECT:
	//	break;
	//case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
	//	break;
	//}

}

void CMainScene::SendMsgs()
{
	//CreateMSG(MSGTYPE::MSGUPDATE::ADJUSTPOS, )
	for (auto client : m_pRoomInfo->clientlist)
	{
		// send world update to clients
	}
}
