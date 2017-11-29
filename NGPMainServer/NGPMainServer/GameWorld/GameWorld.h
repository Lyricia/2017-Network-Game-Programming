#pragma once

class CIndRes;
class CResourceManager;
class CTimer;

class CMainScene;

struct RoomInfo;

class CGameWorld
{
private:

private:
	void Update(float fTimeElapsed);

public:
	CGameWorld();
	~CGameWorld();

	bool Initailize();
	void Run();
	void ProcessMsgs();
	void SendMsgs();
	void CheckClients();

	void RegisterRoomInfo(RoomInfo* room) { m_pRoomInfo = room; }

	int								GetMarginWidth()		const { return m_iMarginWidth; }
	int								GetMarginHeight()		const { return m_iMarginHeight; }
	HWND							GethWnd()				const { return m_hWnd; }
	shared_ptr<CIndRes>				GetIndRes()				const { return m_pIndRes; }
	shared_ptr<CResourceManager>	GetResourceManager()	const { return m_pResMng; }
	ComPtr<ID2D1HwndRenderTarget>	GetRenderTarget()		const { return m_pd2dRenderTarget; }
	RoomInfo*						GetRoomInfo()			const { return m_pRoomInfo; }
	D2D_RECT_L						GetClientSize()			const { return m_rcClient; }

private:
	RoomInfo*						m_pRoomInfo;

	HINSTANCE						m_hInstance;
	HWND							m_hWnd;

	shared_ptr<CIndRes>				m_pIndRes;
	shared_ptr<CResourceManager>	m_pResMng;
	unique_ptr<CTimer>				m_pTimer;

	ComPtr<ID2D1HwndRenderTarget>	m_pd2dRenderTarget;

	D2D_RECT_L						m_rcClient;

	int								m_iMarginWidth;
	int								m_iMarginHeight;

	unique_ptr<CMainScene>			m_pMainScene;

private:
	ATOM RegisterWndClass();
	BOOL InitInstance();

	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};