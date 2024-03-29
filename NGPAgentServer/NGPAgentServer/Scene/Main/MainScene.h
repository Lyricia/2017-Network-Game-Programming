#pragma once

#include "Scene/Scene.h"

#define DIR_LEFT			0x01
#define DIR_RIGHT			0x02
#define DIR_UP				0x04
#define DIR_DOWN			0x08

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

	bool OnCreate(std::wstring&& tag, CGameWorld* pGameWorld) override;
	void Update(float fTimeElapsed) override;

	void Release();

	void ProcessMsgs();
	void SendMsgs();

	void RegisterRoomInfo(RoomInfo* room) { m_pRoomInfo = room; }

private:
	RoomInfo*						m_pRoomInfo;
	UINT							m_ObjectIDCounter;

	std::vector<CObject*>			m_vecObjects;

	D2D_POINT_2F					m_ptCamera;

	UINT							ObjectID = 0;
};

