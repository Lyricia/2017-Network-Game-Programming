#pragma once

#include "Scene/Scene.h"
#include "Camera\Camera.h"

class CBrick;
class CPlayer;
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

	bool OnCreate(wstring&& tag, CFramework* pFramework) override;
	void Update(float fTimeElapsed) override;
	void Draw(ID2D1HwndRenderTarget * pd2dRenderTarget) override;

private:
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrGrid1		{ nullptr };
	ComPtr<ID2D1SolidColorBrush>	m_pd2dsbrGrid2		{ nullptr }	;

	ComPtr<ID2D1Bitmap1>			m_bmpCrossHair;
	D2D1_POINT_2F					m_ptMouseCursor;

	CCamera							m_Camera;
	CBrick*							m_pBrick			{ nullptr };

	CPlayer*						m_pPlayer			{ nullptr };
};

