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

bool CScene::OnCreate(std::wstring&& tag, CGameWorld* pGameWorld)
{
	m_strTag = std::move(tag);

	m_pGameWorld = pGameWorld;
	if (!pGameWorld)
	{
		_DEBUG_ERROR("framework is must be vaild!");
		return false;
	}

	return true;
}
