#pragma once

class CClient;

class CIndRes;
class CResourceManager;
class CTimer;

class CScene;

class CFramework
{
public:
	CFramework();
	~CFramework();

	void RegisterIndRes(shared_ptr<CIndRes> indres) { m_pIndRes = indres; }
	void RegisterTimer(shared_ptr<CTimer> timer) { m_pTimer = timer; }

	// Framework 만들 때
	void OnCreate(HWND hWnd, HINSTANCE hInst, shared_ptr<CIndRes> indres, shared_ptr<CTimer> timer);

	// Scene 만들 때
	template<typename Scene>
	Scene* BuildScene(wstring Tag, bool bChangeThis = true)
	{
		static_assert(is_base_of_v<CScene, Scene>, "Scene is must be based on CScene!");

		unique_ptr<CScene> scene{ make_unique<Scene>() };
		BuildScene(Tag, scene);
		m_lstScenes.push_back(move(scene));
		if (bChangeThis) ChangeScene(Tag);

		return static_cast<Scene*>(FindScene(Tag));
	}

private:
	void BuildScene(wstring Tag, const unique_ptr<CScene>& scene);

public:
	void FrameAdvance();
	void Draw();
	void Update(float fTimeElapsed);
	void ProcessMsgs();

	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void ChangeScene(std::wstring Tag, bool bDestroyPostScene = false);
	CScene* FindScene(std::wstring Tag);

	// Getter
public:
	HWND							GethWnd()				const { return m_hWnd; }
	shared_ptr<CClient>				GetClient()				const { return m_pClient; }
	shared_ptr<CIndRes>				GetIndRes()				const { return m_pIndRes; }
	shared_ptr<CResourceManager>	GetResourceManager()	const { return m_pResourceManager; }
	ComPtr<ID2D1HwndRenderTarget>	GetRenderTarget()		const { return m_pd2dRenderTarget; }
	D2D_RECT_L						GetClientSize()			const { return m_rcClient; }

	// Framework에 필요한 것들.
private:
	HWND							m_hWnd				{ nullptr };
	HINSTANCE						m_hInst				{ nullptr };
	shared_ptr<CClient>				m_pClient			{ nullptr };
																   
	shared_ptr<CIndRes>				m_pIndRes			{ nullptr };
	shared_ptr<CResourceManager>	m_pResourceManager	{ nullptr };
	shared_ptr<CTimer>				m_pTimer			{ nullptr };
																   
	ComPtr<ID2D1HwndRenderTarget>	m_pd2dRenderTarget	{ nullptr };

	D2D_RECT_L						m_rcClient;

	list<unique_ptr<CScene>>		m_lstScenes;
	CScene*							m_pCurrentScene		{ nullptr };

};
