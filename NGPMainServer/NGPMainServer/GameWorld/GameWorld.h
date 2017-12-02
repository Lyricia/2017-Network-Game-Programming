#pragma once

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
	bool CheckClients();

	void RegisterRoomInfo(RoomInfo* room) { m_pRoomInfo = room; }

	RoomInfo*						GetRoomInfo()			const { return m_pRoomInfo; }

private:
	RoomInfo*						m_pRoomInfo;

	unique_ptr<CTimer>				m_pTimer;

	unique_ptr<CMainScene>			m_pMainScene;
};