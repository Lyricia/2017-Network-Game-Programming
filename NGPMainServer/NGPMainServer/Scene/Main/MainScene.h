#pragma once

#include "Scene/Scene.h"

#define DIR_LEFT			0x01
#define DIR_RIGHT			0x02
#define DIR_UP				0x04
#define DIR_DOWN			0x08

#define GAMEOVER_TIMER		2.0f

class CObject;
class CPlayer;

struct RoomInfo;

class CMainScene :
	public CScene
{
public:
	using Base = CScene;
public:
	CMainScene();
	~CMainScene();

	bool OnCreate(wstring&& tag, CGameWorld* pGameWorld) override;
	void PreprocessingUpdate(float fTimeElapsed);
	void Update(float fTimeElapsed) override;
	void PhysicsUpdate(float fTimeElapsed);

	void ProcessMsgs();
	void SendMsgs();

	void RegisterRoomInfo(RoomInfo* room) { m_pRoomInfo = room; }

private:
	RoomInfo*						m_pRoomInfo;
	UINT							m_ObjectIDCounter;
	UINT							m_nBrick;

	std::vector<CObject*>			m_vecObjects;

	D2D_POINT_2F					m_ptCamera;

	float							m_fCheckGameOverTime;
	bool							m_bPlayerDie;
};

