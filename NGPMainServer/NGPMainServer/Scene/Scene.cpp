#include "stdafx.h"

#include "GameWorld/GameWorld.h"
#include "GameWorld/IndRes/IndRes.h"

#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:	break;
	case WM_MBUTTONDOWN:	break;
	case WM_RBUTTONDOWN:	break;
	case WM_MOUSEMOVE:		break;
	case WM_LBUTTONUP:		break;
	case WM_MBUTTONUP:		break;
	case WM_RBUTTONUP:		break;
	case WM_MOUSEWHEEL:		break;
	default:
		return false;
	}

	return(true);
}

bool CScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	default:
		return false;
	}

	return true;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		return true;
	}
	return(true);
}

bool CScene::OnCreate(wstring&& tag, CGameWorld* pGameWorld)
{
	m_strTag = move(tag);

	m_pGameWorld = pGameWorld;
	if (!pGameWorld)
	{
		_DEBUG_ERROR("framework is must be vaild!");
		return false;
	}
	m_hWnd = pGameWorld->GethWnd();
	m_pIndRes = pGameWorld->GetIndRes();

	return true;
}
