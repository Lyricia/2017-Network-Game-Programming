#include "stdafx.h"

#include "Framework/Framework.h"
#include "Framework/IndRes/IndRes.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"

#include "MainScene.h"


CMainScene::CMainScene()
{
}
CMainScene::~CMainScene()
{
	if(m_pBrick) delete m_pBrick;
	if(m_pPlayer) delete m_pPlayer;
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
		auto rcClient = m_pFramework->GetClientSize();
		m_ptMouseCursor = Point2F(
			  LOWORD(lParam)- rcClient.right / 2
			, HIWORD(lParam)- rcClient.bottom / 2);
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
		case 'A': m_pPlayer->Move(-10.f, 0);
			break;		   
		case 'W': m_pPlayer->Move(0, -10.f);
			break;		   
		case 'D': m_pPlayer->Move(10.f, 0);
			break;		   
		case 'S': m_pPlayer->Move(0, 10.f);
			break;
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

	auto rcClient = pFramework->GetClientSize();
	m_Camera.SetClientSize(Point2F(rcClient.right, rcClient.bottom));
	auto rendertarget = pFramework->GetRenderTarget();

	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Green }, &m_pd2dsbrGrid1);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::GreenYellow }, &m_pd2dsbrGrid2);

	auto hwndrendertarget = rendertarget.Get();
	LoadImageFromFile(
		m_pIndRes->wicFactory()
		, hwndrendertarget
		//, path("../../Resource/Sprite/aim.png").c_str()
		//, path("../../Resource/Sprite/aim_big.png").c_str()
		, path("../../Resource/Sprite/aim_trans.png").c_str()
		, &m_bmpCrossHair);

	m_pPlayer = new CPlayer(Point2F());
	m_pPlayer->RegisterSpriteImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "../../Resource/Sprite/character-sheet.png"
		, Point2F(20, 1));
	m_pPlayer->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "../../Resource/Sprite/gun.png");

	m_Camera.SetPosition(m_pPlayer->GetPos());
	m_Camera.SetAnchor(Point2F(0.0f, 0.0f));

	m_pBrick = new CBrick(Point2F(100, 200));
	m_pBrick->RegisterSpriteImage(
		  m_pIndRes.get()
		, rendertarget.Get()
		, "../../Resource/Sprite/brick-Sheet.png"
		, Point2F(5, 1));

	return true;
}

void CMainScene::Update(float fTimeElapsed)
{
	m_Camera.SetPosition(m_pPlayer->GetPos());
	m_pPlayer->Update(fTimeElapsed);
	m_pPlayer->LookAt(m_ptMouseCursor);
	m_pBrick->Update(fTimeElapsed);
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
					, m_pd2dsbrGrid1.Get());
			else
				pd2dRenderTarget->FillRectangle(
					RectF(-32, -32, 32, 32) +
					Point2F(64.f *j, 64.f *i)
					, m_pd2dsbrGrid2.Get());
		}

	m_pBrick->Draw(pd2dRenderTarget);
	m_pPlayer->Draw(pd2dRenderTarget);

	pd2dRenderTarget->DrawBitmap(
		m_bmpCrossHair.Get()
		, SizeToRect(m_bmpCrossHair->GetSize()) + 
		m_Camera.GetInterpolatedPosFromCamera(m_ptMouseCursor));
}
