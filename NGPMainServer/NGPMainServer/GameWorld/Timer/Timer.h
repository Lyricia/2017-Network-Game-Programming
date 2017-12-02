#pragma once

#include <chrono>

class CTimer
{
private:
	using clock = std::chrono::system_clock;

public:
	static constexpr BYTE TIME_STATE_WAIT_MAX_FPS		= 0x00;
	static constexpr BYTE TIME_STATE_UPDATE_CAPTION		= 0x01;
	static constexpr BYTE TIME_STATE_UPDATE_GAMELOOP	= 0x02;
	static constexpr BYTE TIME_STATE_PROCESS_MESSAGE	= 0x04;

public:
	CTimer();
	virtual ~CTimer();

	BYTE Update();
	float GetFPS() const { return { 0.5f + m_olddCumulativefps / static_cast<float>(m_oldnCumulativefpsCount) }; }
	float GetTimeElapsed() const { return m_TimeElapsed; }

	void SetUpdateCaptionHwnd(HWND hWnd);
	void UpdateCaption();

protected:

	clock::time_point m_current_time		{ clock::now() };

	float		m_TimeElapsed				{ 0.f };

	float		m_dCumulativefps			{ 0.f };
	int			m_nCumulativefpsCount		{ 0 };

	float		m_olddCumulativefps			{ 0.f };
	int			m_oldnCumulativefpsCount	{ 0 };

	clock::time_point m_LastUpdate_time		{ clock::now() };
	clock::time_point m_LastProcessMsg_time	{ clock::now() };

	HWND		m_hWnd{ nullptr };
	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH];
	int			m_TitleLength{ 0 };

#if defined(SHOW_CAPTIONFPS)
	int			m_nLastFps{ 0 };
#endif

};
