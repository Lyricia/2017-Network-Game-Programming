#include "stdafx.h"
#include "Timer.h"

using namespace std::chrono;

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}

BYTE CTimer::Update()
{
	BYTE TimeState = TIME_STATE_WAIT_MAX_FPS;
	auto now = clock::now();

	duration<float> timeElapsed = now - m_current_time;
	auto fElapsed = timeElapsed.count();

	/////////////////////////////////////////////////////////////////////////
	if (fElapsed <= MAX_FPS)								return TimeState;
	TimeState = TIME_STATE_UPDATE_GAMELOOP;

	m_current_time = now;
	m_TimeElapsed = fElapsed;
	float fps = 1.f / m_TimeElapsed;
	
	m_dCumulativefps += fps;
	m_nCumulativefpsCount++;

	duration<float> ProcessMsgElapsed = now - m_LastProcessMsg_time;
	if (ProcessMsgElapsed.count() > MAX_PROCESS_MSG_FPS)
	{
		TimeState |= TIME_STATE_PROCESS_MESSAGE;
		m_LastProcessMsg_time = now;
	}

	duration<float> UpdateElapsed = now - m_LastUpdate_time;

	/////////////////////////////////////////////////////////////////////////
	if (UpdateElapsed.count() <= MAX_UPDATE_FPS)			return TimeState;
	TimeState |= TIME_STATE_UPDATE_CAPTION;

	m_LastUpdate_time = now;

	m_olddCumulativefps = m_dCumulativefps;
	m_oldnCumulativefpsCount = m_nCumulativefpsCount;

	m_dCumulativefps = 0.f;
	m_nCumulativefpsCount = 0;

	if (m_hWnd) UpdateCaption();

	return TimeState;
}

void CTimer::SetUpdateCaptionHwnd(HWND hWnd)
{
	m_hWnd = hWnd;

	// 캡션 변경
#if defined(TITLESTRING)
	lstrcpy(m_CaptionTitle, TITLESTRING);
#else
	GetWindowText(m_hWnd, m_CaptionTitle, TITLE_MAX_LENGTH);
#endif

#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif

	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

}

void CTimer::UpdateCaption()
{
	// 프레임레이트를 출력합니다.
#if defined(SHOW_CAPTIONFPS)

	int nCurrentFps = static_cast<int>(GetFPS());

	if (m_nLastFps == nCurrentFps) return;

	m_nLastFps = nCurrentFps;

	_itow_s(m_nLastFps, m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif
}
