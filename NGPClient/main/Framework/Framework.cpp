#include "stdafx.h"

#include "IndRes/IndRes.h"
#include "ResourceManager/ResourceManager.h"
#include "Timer/Timer.h"
#include "Scene/Scene.h"
#include "Scene/Main/MainScene.h"

#include "Client\Client.h"
#include "Framework.h"

CFramework::CFramework() 
{
}
CFramework::~CFramework()
{
}

bool CFramework::OnCreate(HWND hWnd, HINSTANCE hInst, shared_ptr<CIndRes> indres, shared_ptr<CTimer> timer)
{
	RegisterIndRes(indres);
	RegisterTimer(timer);

	m_hWnd = hWnd;
	m_hInst = hInst;
	::GetClientRect(m_hWnd, &m_rcClient);
	::ShowCursor(FALSE);

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	m_pClient = make_shared<CClient>();
	m_pClient->Initialize();
	if (!m_pClient->ConnectServer()) return false;

	m_pIndRes->CreateHwndRenderTarget(hWnd, &m_pd2dRenderTarget);
	m_pResourceManager = 
		make_shared<CResourceManager>(m_pIndRes.get(), m_pd2dRenderTarget.Get());
	BuildScene<CMainScene>(L"Main"s);
	return true;
}

void CFramework::BuildScene(wstring Tag, const unique_ptr<CScene>& scene)
{
	scene->OnCreate(move(Tag), this);
}

void CFramework::FrameAdvance()
{
	Update(m_pTimer->GetTimeElapsed());
	Draw();
}

void CFramework::Draw()
{
	m_pd2dRenderTarget->BeginDraw();
	m_pd2dRenderTarget->Clear(ColorF{ ColorF::LightGray });

	if (m_pCurrentScene) m_pCurrentScene->Draw(m_pd2dRenderTarget.Get());

	m_pd2dRenderTarget->EndDraw();
}

void CFramework::Update(float fTimeElapsed)
{
	if (m_pCurrentScene) m_pCurrentScene->Update(fTimeElapsed);
}

void CFramework::ProcessMsgs()
{
	if (m_pCurrentScene) m_pCurrentScene->ProcessMsgs();
}

bool CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:	break;
	case WM_MBUTTONDOWN:	break;
	case WM_RBUTTONDOWN:	break;
	case WM_MOUSEMOVE:		break;
	case WM_LBUTTONUP:		break;
	case WM_MBUTTONUP:		break;
	case WM_RBUTTONUP:		break;
	case WM_MOUSEWHEEL:		break;
	default:
		return false;
	}

	return true;
}

bool CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		default:
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}

LRESULT CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	// 선처리 구문 : 개별 Scene에서 처리할 것인지 확인
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			auto nWndClientWidth = static_cast<LONG>(LOWORD(lParam));
			auto nWndClientHeight = static_cast<LONG>(HIWORD(lParam));

			if (nWndClientWidth < CLIENT_MINIMUM_WIDTH - GetMarginWidth())
				nWndClientWidth = CLIENT_MINIMUM_WIDTH - GetMarginWidth();
				
			if (nWndClientHeight < CLIENT_MINIMUM_HEIGHT - GetMarginHeight())
				nWndClientHeight = CLIENT_MINIMUM_HEIGHT - GetMarginHeight();

			m_rcClient = RectL(0, 0, nWndClientWidth, nWndClientHeight);

			m_pd2dRenderTarget->Resize(SizeU(nWndClientWidth, nWndClientHeight));
			
			Draw();

			return DefWindowProc(hWnd, nMessageID, wParam, lParam);
		}

	default:
		if (m_pCurrentScene)	if (!m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam)) break;
								else return 0;
	}

	// 후처리 구문 : Scene에서 처리되지 않고 남는 부분을 처리
	switch (nMessageID)
	{
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = CLIENT_MINIMUM_WIDTH;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = CLIENT_MINIMUM_HEIGHT;

		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

void CFramework::RestartCurrScene()
{
	auto DestroyTag = m_pCurrentScene->Tag();
	if (DestroyTag == L"Main"s)
	{
		m_pClient->DisconnectServer();
		m_pClient->ConnectServer();
	}
	m_pCurrentScene = nullptr;
	m_lstScenes.remove_if([&](const unique_ptr<CScene>& s){ 
		return s->FindByTag(DestroyTag); });
	BuildScene<CMainScene>(DestroyTag);
}

void CFramework::ChangeScene(wstring Tag, bool bDestroyPostScene)
{
	auto pChangeScene = FindScene(Tag);
	if (!pChangeScene) return _DEBUG_ERROR("생성되지 않은 Scene을 참조하려 했습니다!");

	if (m_pCurrentScene && bDestroyPostScene)
	{
		auto& DestroyTag = m_pCurrentScene->Tag();

		m_pCurrentScene = nullptr;

		m_lstScenes.remove_if(
			[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(DestroyTag); }
		);
	}
	m_pCurrentScene = pChangeScene;
}

CScene * CFramework::FindScene(std::wstring Tag)
{
	auto ChangeScene = find_if(begin(m_lstScenes), end(m_lstScenes),
		[&] (const unique_ptr<CScene>& s)
		{ return s->FindByTag(Tag); }
	);
	return ChangeScene == end(m_lstScenes) ? nullptr : ChangeScene->get();
}

LRESULT CALLBACK CFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	auto self = ::GetUserDataPtr<CFramework*>(hWnd);
	if (!self) return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	static auto DestroyWindow = [&] ()
	{
		::SetUserDataPtr(hWnd, nullptr);
		::PostQuitMessage(0);
	};
	
	switch (nMessageID)
	{
	case WM_DESTROY:
		DestroyWindow();
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}
