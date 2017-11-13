#include "stdafx.h"

#include "IndRes/IndRes.h"
#include "Timer/Timer.h"

#include "GameWorld.h"
#define MAX_LOADSTRING 100

void CGameWorld::Update(float fTimeElapsed)
{

}

void CGameWorld::Draw()
{
	m_pd2dRenderTarget->BeginDraw();
	m_pd2dRenderTarget->Clear(ColorF{ ColorF::LightGray });


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

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance())
	{
		return false;
	}

	::GetClientRect(m_hWnd, &m_rcClient);

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	m_pIndRes->CreateHwndRenderTarget(m_hWnd, &m_pd2dRenderTarget);

	return true;
}

void CGameWorld::Run()
{
	MSG msg;

	// 기본 메시지 루프입니다.
	while (true)
	{
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
	}
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//   함수: InitInstance()
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//        이 함수를 통해 인스턴스 핸들을 멤버 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL CGameWorld::InitInstance()
{
	//	윈도우 스타일
	DWORD dwStyle = 0
		| WS_OVERLAPPED 	// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
		| WS_CAPTION 		// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
		| WS_SYSMENU		// 시스템 메뉴를 가진 윈도우를 만든다.
		| WS_MINIMIZEBOX	// 최소화 버튼을 만든다.
		| WS_BORDER			// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
		| WS_THICKFRAME		// 크기 조정이 가능한 두꺼운 경계선을 가진다. WS_BORDER와 같이 사용할 수 없다.
		;					// 추가로 필요한 윈도우 스타일은 http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm 참고.

	// 데스크톱 윈도우 사이즈
	RECT getWinSize;
	::GetWindowRect(::GetDesktopWindow(), &getWinSize);

	//	클라이언트 사이즈
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	//	윈도우 사이즈에 실제로 추가되는(캡션, 외곽선 등) 크기를 보정.
	::AdjustWindowRect(&rc, dwStyle, FALSE);

	m_iMarginWidth = rc.right - rc.left - CLIENT_WIDTH;
	m_iMarginHeight = rc.bottom - rc.top - CLIENT_HEIGHT;

	//	클라이언트 절대좌표(left, top)
	//	데스크톱의 중앙에 클라이언트가 위치하도록 설정
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	윈도우 생성
	m_hWnd = CreateWindow(
		TITLESTRING			//	윈도우 클래스 명
		, TITLESTRING			//	캡션 표시 문자열
		, dwStyle				//	윈도우 스타일
		, ptClientWorld.x		//	부모 윈도우 기반 윈도우 시작좌표 : x
		, ptClientWorld.y		//	부모 윈도우 기반 윈도우 시작좌표 : y
		, rc.right - rc.left	//	윈도우 사이즈 : width
		, rc.bottom - rc.top	//	윈도우 사이즈 : height
		, NULL					//	부모 윈도우.
		, NULL					//	메뉴 핸들
		, m_hInstance				//	인스턴스 핸들
		, NULL					//	추가 파라메터 : NULL
	);

	//	생성 실패시 프로그램 종료
	//	확인 : WndProc의 default msg handler가 DefWindowProc 함수를 반환하는가?
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
