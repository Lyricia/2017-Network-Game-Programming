#pragma once

#include "Scene/Scene.h"
#include "Camera\Camera.h"

#define DIR_LEFT			0x01
#define DIR_RIGHT			0x02
#define DIR_UP				0x04
#define DIR_DOWN			0x08

class CObject;
class CPlayer;
class CEffect;

struct RoomInfo;

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

	bool OnCreate(wstring&& tag, CGameWorld* pGameWorld) override;
	void PreprocessingUpdate(float fTimeElapsed);
	void Update(float fTimeElapsed) override;
	void PhysicsUpdate(float fTimeElapsed);
	void Draw(ID2D1HwndRenderTarget * pd2dRenderTarget) override;

	void ProcessInput(float fTimeElapsed);
	void ProcessMsgs();
	void SendMsgs();

	void RegisterRoomInfo(RoomInfo* room) { m_pRoomInfo = room; }

private:
	RoomInfo*						m_pRoomInfo;

	shared_ptr<CResourceManager>	m_pResMng			{ nullptr };

	ComPtr<ID2D1Bitmap1>			m_bmpBackGround;
	ComPtr<ID2D1Bitmap1>			m_bmpCrossHair;
	D2D1_POINT_2F					m_ptMouseCursor;

	CCamera							m_Camera;
	std::vector<CObject*>			m_vecObjects;
	std::list<CEffect*>				m_lstEffects;

	D2D_POINT_2F					m_ptCamera;
};

