#include "stdafx.h"

#include "Framework/Framework.h"
#include "Framework/ResourceManager/ResourceManager.h"
#include "Framework/IndRes/IndRes.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"

#include "MainScene.h"


CMainScene::CMainScene()
{
}
CMainScene::~CMainScene()
{
	if (!m_vecObjects.empty())
		for (auto& p : m_vecObjects)
			delete p;
	m_vecObjects.clear();
	if(m_pPlayer) delete m_pPlayer;
}

bool CMainScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_pPlayer->Shoot();
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
	m_pResMng = pFramework->GetResourceManager();

	auto rcClient = pFramework->GetClientSize();
	m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
	auto rendertarget = pFramework->GetRenderTarget();

	m_bmpCrossHair = m_pResMng->GetImageRef(ResImgName::aim);

	m_pPlayer = new CPlayer(Point2F());
	m_pPlayer->RegisterResourceManager(m_pResMng);

	m_Camera.SetPosition(m_pPlayer->GetPos());
	m_Camera.SetAnchor(Point2F(0.0f, 0.0f));

	CPlayer* other_player = new CPlayer(Point2F(-100, 10));
	other_player->RegisterResourceManager(m_pResMng);
	m_vecObjects.push_back(other_player);

	CBrick* brick = new CBrick(Point2F(100, 200));
	brick->RegisterResourceManager(m_pResMng);
	m_vecObjects.push_back(brick);

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

	m_Camera.SetPosition(m_pPlayer->GetPos());

	m_pPlayer->Update(fTimeElapsed);
	m_pPlayer->LookAt(
		(m_ptMouseCursor * m_Camera.GetScaleFactor()) + 
		m_pPlayer->GetPos());
	m_pPlayer->RayCastingToShoot(m_vecObjects);

	for (auto& p : m_vecObjects)
		p->Update(fTimeElapsed);

	PhysicsUpdate(fTimeElapsed);
}

void CMainScene::PhysicsUpdate(float fTimeElapsed)
{
	D2D_POINT_2F ptPlayerPos =  m_pPlayer->GetPos();
	float fPlayerRadius = m_pPlayer->GetSize().right;
	D2D_POINT_2F dir = Point2F();

	for (auto& p : m_vecObjects)
	{
		switch (p->GetTag())
		{
		case CObject::Type::Player:
		{

			break;
		}
		case CObject::Type::Brick:
		{
			dir = Normalize(p->GetPos() - ptPlayerPos);
			if (PtInRect(
					&(p->GetSize() + p->GetPos())
					, ptPlayerPos + (dir * fPlayerRadius))
				)
			{
				m_pPlayer->Reflection(-1.f * dir);
				m_pPlayer->Move(- PLAYER_VELOCITY * dir * fTimeElapsed);
			}
			break;
		}
		}
	}
}

void CMainScene::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	auto cameramtx = m_Camera.RegenerateViewMatrix();
	pd2dRenderTarget->SetTransform(cameramtx);

	for (int i = -50; i < 50; ++i)
		for (int j = -50; j < 50; ++j)
		{
			if ((i + j) % 2)
				pd2dRenderTarget->FillRectangle(
					RectF(-32, -32, 32, 32) + 
					Point2F(64.f*j, 64.f*i)
					, m_pResMng->brGreen.Get());
			else
				pd2dRenderTarget->FillRectangle(
					RectF(-32, -32, 32, 32) +
					Point2F(64.f *j, 64.f *i)
					, m_pResMng->brGreenYellow.Get());
		}

	for (auto& p : m_vecObjects)
		p->Draw(pd2dRenderTarget);

	m_pPlayer->Draw(pd2dRenderTarget);
	m_pPlayer->DrawUI(pd2dRenderTarget, m_Camera.GetScaleFactor());

	pd2dRenderTarget->DrawBitmap(
		m_bmpCrossHair.Get()
		, SizeToRect(m_bmpCrossHair->GetSize()) + 
		m_ptMouseCursor * m_Camera.GetScaleFactor() +
		m_pPlayer->GetPos());

	auto pt = m_ptMouseCursor * m_Camera.GetScaleFactor() +
		m_pPlayer->GetPos();
	printf("\r %f, %f", pt.x, pt.y);
}

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

		D2D_POINT_2F ptDir = Point2F();
		if (dwDirection & DIR_UP) ptDir.y += -1;
		if (dwDirection & DIR_DOWN) ptDir.y += 1;
		if (dwDirection & DIR_LEFT) ptDir.x += -1;
		if (dwDirection & DIR_RIGHT) ptDir.x += 1;
		if (dwDirection && Length(ptDir))
		{
			m_pPlayer->Move(Normalize(ptDir) * PLAYER_VELOCITY * fTimeElapsed);
		}
	}
	static RECT rcWindow;
	static POINT ptCursorPos;
	::GetWindowRect(m_hWnd, &rcWindow);
	::GetCursorPos(&ptCursorPos);
	auto rcClient = m_pFramework->GetClientSize();
	m_ptMouseCursor = Point2F(
		ptCursorPos.x - rcWindow.left - 5 - rcClient.right / 2
		, ptCursorPos.y - rcWindow.top - 30 - rcClient.bottom / 2);
}