#include "stdafx.h"

#include "IndRes\IndRes.h"
#include "Timer\Timer.h"
#include "ResourceManager\ResourceManager.h"
#include "Scene\Main\MainScene.h"
#include "Server\Main\MainServer.h"
#include "GameWorld.h"


void CGameWorld::Update(float fTimeElapsed)
{
	m_pMainScene->Update(fTimeElapsed);
}

void CGameWorld::Draw()
{
	m_pd2dRenderTarget->BeginDraw();
	m_pd2dRenderTarget->Clear(ColorF{ ColorF::LightGray });

	m_pMainScene->Draw(m_pd2dRenderTarget.Get());

	m_pd2dRenderTarget->EndDraw();
}

CGameWorld::CGameWorld()
	: m_hInstance(nullptr)
	, m_hWnd(nullptr)
	, m_pIndRes(nullptr)
	, m_pTimer(nullptr)
	, m_pd2dRenderTarget(nullptr)
	, m_iMarginWidth(0)
	, m_iMarginHeight(0)
	, m_pMainScene(nullptr)
{
}

CGameWorld::~CGameWorld()
{
}

bool CGameWorld::Initailize(shared_ptr<CIndRes> indres)
{
	m_hInstance = GetModuleHandle(NULL);
	m_pTimer = make_unique<CTimer>();
	m_pIndRes = indres;

	RegisterWndClass();

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance())
	{
		return false;
	}

	::GetClientRect(m_hWnd, &m_rcClient);
	::ShowCursor(FALSE);

	// Ŭ������ ������ ���ν��� ����
	::SetUserDataPtr(m_hWnd, this);

	m_pIndRes->CreateHwndRenderTarget(m_hWnd, &m_pd2dRenderTarget);
	m_pResMng = make_shared<CResourceManager>(m_pIndRes.get(), m_pd2dRenderTarget.Get());

	m_pMainScene = make_unique<CMainScene>();
	m_pMainScene->RegisterRoomInfo(m_pRoomInfo);
	m_pMainScene->OnCreate(L"Main"s, this);
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
		static int i = 0;
		printf("\rmessage loop test %d", i++);
		if (!m_pTimer->Update()) continue;
		Update(m_pTimer->GetTimeElapsed());
		Draw();
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
	if (m_pRoomInfo) {
		m_pRoomInfo->clientlist.remove_if([&](ConnectionInfo* client)->bool {
			DWORD exitcode = 0;
			GetExitCodeThread(client->RecvThreadHandle, &exitcode);
			if (exitcode != STILL_ACTIVE) {
				cout << "Client ID : " << client->ID << " Terminated" << endl;
				return true;
			}
			return false;
		});

		if (m_pRoomInfo->clientlist.empty())
		{
			cout << "Terminate Room " << m_pRoomInfo->RoomID << endl;
			PostQuitMessage(NULL);
			//ExitThread(NULL);
		}
	}
}

//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM CGameWorld::RegisterWndClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TITLESTRING;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance()
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ��� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL CGameWorld::InitInstance()
{
	//	������ ��Ÿ��
	DWORD dwStyle = 0
		| WS_OVERLAPPED 	// ����Ʈ ������. Ÿ��Ʋ �ٿ� ũ�� ������ �ȵǴ� ��輱�� ������. �ƹ��� ��Ÿ�ϵ� ���� ������ �� ��Ÿ���� ����ȴ�.
		| WS_CAPTION 		// Ÿ��Ʋ �ٸ� ���� �����츦 ����� WS_BORDER ��Ÿ���� �����Ѵ�.
		| WS_SYSMENU		// �ý��� �޴��� ���� �����츦 �����.
		| WS_MINIMIZEBOX	// �ּ�ȭ ��ư�� �����.
		| WS_BORDER			// �ܼ����� �� ��輱�� ����� ũ�� ������ �� �� ����.
		| WS_THICKFRAME		// ũ�� ������ ������ �β��� ��輱�� ������. WS_BORDER�� ���� ����� �� ����.
		;					// �߰��� �ʿ��� ������ ��Ÿ���� http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm ����.

	// ����ũ�� ������ ������
	RECT getWinSize;
	::GetWindowRect(::GetDesktopWindow(), &getWinSize);

	//	Ŭ���̾�Ʈ ������
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	//	������ ����� ������ �߰��Ǵ�(ĸ��, �ܰ��� ��) ũ�⸦ ����.
	::AdjustWindowRect(&rc, dwStyle, FALSE);

	m_iMarginWidth = rc.right - rc.left - CLIENT_WIDTH;
	m_iMarginHeight = rc.bottom - rc.top - CLIENT_HEIGHT;

	//	Ŭ���̾�Ʈ ������ǥ(left, top)
	//	����ũ���� �߾ӿ� Ŭ���̾�Ʈ�� ��ġ�ϵ��� ����
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	������ ����
	m_hWnd = CreateWindow(
		TITLESTRING			//	������ Ŭ���� ��
		, TITLESTRING			//	ĸ�� ǥ�� ���ڿ�
		, dwStyle				//	������ ��Ÿ��
		, ptClientWorld.x		//	�θ� ������ ��� ������ ������ǥ : x
		, ptClientWorld.y		//	�θ� ������ ��� ������ ������ǥ : y
		, rc.right - rc.left	//	������ ������ : width
		, rc.bottom - rc.top	//	������ ������ : height
		, NULL					//	�θ� ������.
		, NULL					//	�޴� �ڵ�
		, m_hInstance				//	�ν��Ͻ� �ڵ�
		, NULL					//	�߰� �Ķ���� : NULL
	);

	//	���� ���н� ���α׷� ����
	//	Ȯ�� : WndProc�� default msg handler�� DefWindowProc �Լ��� ��ȯ�ϴ°�?
	if (!m_hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
			, NULL
			, GetLastError()
			, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
			, reinterpret_cast<LPTSTR>(&lpMsgBuf)
			, 0
			, NULL);
		MessageBox(NULL, reinterpret_cast<LPCTSTR>(lpMsgBuf), L"Create Window Fail", MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

	m_pTimer->SetUpdateCaptionHwnd(m_hWnd);

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

LRESULT CGameWorld::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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
		//OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		//OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	default:
		break;
	}

	return DefWindowProc(hWnd, nMessageID, wParam, lParam);
}

LRESULT CGameWorld::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	auto self = ::GetUserDataPtr<CGameWorld*>(hWnd);
	if (!self) return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	static auto DestroyWindow = [&]()
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
