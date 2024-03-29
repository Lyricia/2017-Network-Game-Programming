#pragma once

#include "Scene/Scene.h"
#include "Camera\Camera.h"

#define DIR_LEFT			0x01
#define DIR_RIGHT			0x02
#define DIR_UP				0x04
#define DIR_DOWN			0x08

class CClient;

class CObject;
class CPlayer;
class CEffect;
class CMainScene :
	public CScene
{
public:
	using Base = CScene;
public:
	CMainScene();
	~CMainScene();

	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)	override;
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)		override;
	bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)	override;

	bool OnCreate(wstring&& tag, CFramework* pFramework)	override;
	void ProcessMsgs()										override;
	void Update(float fTimeElapsed)							override;
	void Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)		override;

	void ProcessInput(float fTimeElapsed);

private:
	shared_ptr<CClient>				m_pClient			{ nullptr };

	shared_ptr<CResourceManager>	m_pResMng			{ nullptr };

	ComPtr<ID2D1Bitmap1>			m_bmpBackGround;
	ComPtr<ID2D1Bitmap1>			m_bmpCrossHair;
	D2D1_POINT_2F					m_ptMouseCursor;

	CCamera							m_Camera;
	std::vector<CObject*>			m_vecObjects;
	std::list<CEffect*>				m_lstEffects;

	CPlayer*						m_pPlayer			{ nullptr };
};

