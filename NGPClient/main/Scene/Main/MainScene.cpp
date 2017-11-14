#include "stdafx.h"

#include "Framework/Framework.h"
#include "Framework/IndRes/IndRes.h"

#include "MainScene.h"


CMainScene::CMainScene()
{
}
CMainScene::~CMainScene()
{
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
		auto pt = Point2F(LOWORD(lParam)- rcClient.right/2
			, HIWORD(lParam)- rcClient.bottom/2);
		m_upItem->LookAt(pt);
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
		case 'A': m_ptPlayer.x -= 10.f;
			break;				 
		case 'W': m_ptPlayer.y -= 10.f;
			break;				 
		case 'D': m_ptPlayer.x += 10.f;
			break;				 
		case 'S': m_ptPlayer.y += 10.f;
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

	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Crimson }, &m_pd2dsbrDefault);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::Green }, &m_pd2dsbrGrid1);
	rendertarget->CreateSolidColorBrush(ColorF{ ColorF::GreenYellow }, &m_pd2dsbrGrid2);

	m_Camera.SetPosition(m_ptPlayer);
	m_Camera.SetAnchor(Point2F(0.0f, 0.0f));

	m_upItem = make_unique<CUnit>(
		Point2F(100, 100)
		, RectF(-10, -10, 10, 10));
	m_upItem->RegisterImage(
		m_pIndRes.get()
		, rendertarget.Get()
		, "Assets/Buckler.png");

	return true;
}

void CMainScene::Update(float fTimeElapsed)
{
	//fPositionX += (100 * fTimeElapsed);
	//if (fPositionX > 800) fPositionX -= 800.f;
	//
	//if ((currImg += fTimeElapsed * 10) >= 4.f)
	//	currImg -= 4.f;
	m_Camera.SetPosition(m_ptPlayer);
	m_upItem->Update(fTimeElapsed);
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
					RectF(-10, -10, 10, 10) + 
					Point2F(20.f*j, 20.f*i)
					, m_pd2dsbrGrid1.Get());
			else
				pd2dRenderTarget->FillRectangle(
					RectF(-10, -10, 10, 10) + 
					Point2F(20.f*j, 20.f*i)
					, m_pd2dsbrGrid2.Get());
		}

	pd2dRenderTarget->FillRectangle(
		RectF(-10, -10, 10, 10) + m_ptPlayer
		, m_pd2dsbrDefault.Get());
	pd2dRenderTarget->DrawRectangle(
		RectF(60, 60, 80, 80)
		, m_pd2dsbrDefault.Get());

	m_upItem->Draw(pd2dRenderTarget);
}
