#pragma once

class CIndRes;
class CTimer;

class CGameWorld
{
private:

private:
	void Update(float fTimeElapsed);
	void Draw();

public:
	CGameWorld();
	~CGameWorld();

	bool Initailize(shared_ptr<CIndRes> indres);
	void Run();

	int GetMarginWidth() { return m_iMarginWidth; }
	int GetMarginHeight() { return m_iMarginHeight; }

private:
	HINSTANCE						m_hInstance;
	HWND							m_hWnd;

	shared_ptr<CIndRes>				m_pIndRes;
	unique_ptr<CTimer>				m_pTimer;

	ComPtr<ID2D1HwndRenderTarget>	m_pd2dRenderTarget;

	D2D_RECT_L						m_rcClient;

	int								m_iMarginWidth;
	int								m_iMarginHeight;

private:
	ATOM RegisterWndClass();
	BOOL InitInstance();

	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};