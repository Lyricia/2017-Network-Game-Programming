#include "stdafx.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Projectile\Grenade\Grenade.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Unit\Agent\Turret\Turret.h"
#include "Server\AgentServer.h"

#include "MainScene.h"

#define OBJECT_RECT RectF(-32, -32, 32, 32)
UINT g_nBrick = 0;

CMainScene::CMainScene()
	: m_ObjectIDCounter(0)
{
}
CMainScene::~CMainScene()
{
	Release();
}

bool CMainScene::OnCreate(std::wstring && tag, CGameWorld* pGameWorld)
{
	if (!Base::OnCreate(std::move(tag), pGameWorld)) return false;

	int map_size_half = g_iMapSize / 2;
	for (int i = 0; i < g_iMapSize; ++i)
		for (int j = 0; j < g_iMapSize; ++j)
		{
			if (g_iMap[j + i * g_iMapSize] == 1)
			{
				CBrick* brick = new CBrick(Point2F((j - map_size_half)*g_iMapSize, 
					(i - map_size_half)*g_iMapSize));
				brick->SetID(ObjectID++);
				brick->SetSize(OBJECT_RECT);
				m_vecObjects.push_back(brick);
			}
		}


	for (int i = 0; i< 3; ++i)
	{
		CAgent* agent = new CAgent(Point2F(-100, 100 * i));
		agent->RegisterRoomInfo(m_pRoomInfo);
		agent->SetID(ObjectID++);
		agent->SetSize(OBJECT_RECT);
		agent->SetAgentType(CAgent::AgentType::Bot);
		m_vecObjects.push_back(agent);
	}

	for (int i = 0; i< 3; ++i)
	{
		CPlayer* player = new CPlayer(Point2F(-100, 10));
		m_vecObjects.push_back(player);
		player->SetSize(OBJECT_RECT);
		player->SetID(ObjectID++);
	}



	return true;
}


void CMainScene::Update(float fTimeElapsed)
{

	for (auto& p : m_vecObjects)
	{
		p->Update(fTimeElapsed);

		if (p->GetTag() == CObject::Type::Agent)
		{
			CAgent* agent = static_cast<CAgent*>(p);
			agent->InterActionCheck(m_vecObjects);
			agent->RayCastingToShoot(m_vecObjects);
			agent->RunStateMachine(fTimeElapsed);
		}
		
	}

}

void CMainScene::Release()
{
	for (auto& p : m_vecObjects)
		delete p;
	m_vecObjects.clear();
}

void CMainScene::ProcessMsgs()
{
	auto start_time = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<float> timeElapsed = start_time - now;

	NGPMSG*		msg = nullptr;
	ActionInfo* arrActionInfo = nullptr;
	ObjInfo*	arrObjInfo = nullptr;
	MapInfo*	arrMapInfo = nullptr;

	while (timeElapsed.count() < MESSAGE_PROCESSING_TIME)
	{
		if (!m_pRoomInfo->MsgQueue.size()) return;
		m_pRoomInfo->EnterCriticalSection();
		NGPMSG* msg = m_pRoomInfo->MsgQueue.front();
		m_pRoomInfo->MsgQueue.pop_front();
		m_pRoomInfo->LeaveCriticalSection();

		int nObjInfo = msg->header.NUM_OBJINFO;
		switch (msg->header.MSGTYPE)
		{
		case MSGTYPE::MSGACTION::SHOOT:
		{
			//std::cout << "Room" << m_pRoomInfo->RoomID << "Shoot!" << std::endl;
			D2D_POINT_2F hit_pos = Point2F();
			arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
			{
				for (auto iter = m_vecObjects.rbegin();
					iter != m_vecObjects.rend(); ++iter)
				{
					if (msg->header.OBJECTNO == (*iter)->GetID())
					{
						switch ((*iter)->GetTag())
						{
						case CObject::Type::Player:
						{
							CPlayer* player = static_cast<CPlayer*>(*iter);
							player->Shoot(arrActionInfo[i].TargetHitPos);
							break;
						}
						case CObject::Type::Agent:
						{
							CAgent* agent = static_cast<CAgent*>(*iter);
							agent->Shoot(arrActionInfo[i].TargetHitPos);
							
							break;
						}
						}
					}
				}
			}
			delete[] arrActionInfo;
			arrActionInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
			break;
		case MSGTYPE::MSGSTATE::CLIENTREADY:
			break;
		case MSGTYPE::MSGSTATE::ROOMCREATION:
			break;
		case MSGTYPE::MSGUPDATE::ADJUSTPOS:
			break;
		case MSGTYPE::MSGUPDATE::CREATEOBJECT:
		{
			arrObjInfo = new ObjInfo[msg->header.NUM_OBJINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < nObjInfo; ++i)
			{
				switch (arrObjInfo[i].ObjectType)
				{
				case OBJECTTYPE::Effect_ShootingHit:
				{
					break;
				}
				case OBJECTTYPE::Effect_Explosion:
				{
					break;
				}
				case OBJECTTYPE::Grenade:
				{
					std::cout << "Room" << m_pRoomInfo->RoomID << "Granada!" << std::endl;
					CGrenade* grenade = new CGrenade(arrObjInfo[i].Position);
					for (auto iter = m_vecObjects.rbegin();
						iter != m_vecObjects.rend(); ++iter)
						if (arrObjInfo[i].ParentID == (*iter)->GetID())
						{
							grenade->SetParent(*iter);
							break;
						}
					grenade->SetVelocity(arrObjInfo[i].Velocity);
					grenade->SetID(ObjectID++);
					m_vecObjects.push_back(grenade);
					break;
				}
				case OBJECTTYPE::Turret:
				{
					std::cout << "Room" << m_pRoomInfo->RoomID << "Deploy Turret!" << std::endl;
					CTurret* turret = new CTurret(arrObjInfo[i].Position);
					for (auto iter = m_vecObjects.rbegin();
						iter != m_vecObjects.rend(); ++iter)
						if (arrObjInfo[i].ParentID == (*iter)->GetID())
						{
							turret->SetParent(*iter);
							break;
						}
					turret->SetID(ObjectID++);
					turret->SetSize(OBJECT_RECT);
					turret->SetAgentType(CAgent::AgentType::Turret);
					turret->RegisterRoomInfo(m_pRoomInfo);
					m_vecObjects.push_back(turret);
					break;
				}
				}

			}
			delete[] arrObjInfo;
			arrObjInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGUPDATE::DELETEOBJECT:
		{
			std::cout << "Room" << m_pRoomInfo->RoomID << "DeleteObject!" << std::endl;
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (auto& iter = m_vecObjects.begin(); iter != m_vecObjects.end();)
			{
				if (msg->header.OBJECTNO == (*iter)->GetID())
				{
					delete (*iter);
					iter = m_vecObjects.erase(iter);
				}
				else ++iter;
			}
			break;
		}
		case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
		{
			//std::cout << "Room" << m_pRoomInfo->RoomID << "UpdateObjectz!" << std::endl;
			arrObjInfo = new ObjInfo[msg->header.NUM_OBJINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < nObjInfo; ++i)
				if (arrObjInfo[i].ObjectType != OBJECTTYPE::Brick)
				{
					for (auto iter = m_vecObjects.rbegin();
						iter != m_vecObjects.rend(); ++iter)
						if (arrObjInfo[i].ObjectID == (*iter)->GetID())
						{
							if (arrObjInfo[i].ObjectID == 710)
								cout << endl;
							(*iter)->SetObjectInfo(&arrObjInfo[i]);
							break;
						}
				}
			delete[] arrObjInfo;
			arrObjInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGUPDATE::UPDATEMAPSTATE:
		{
			arrMapInfo = new MapInfo[msg->header.NUM_OBJINFO];
			DispatchMSG(msg, arrMapInfo);
			for (int i = 0; i < nObjInfo; ++i)
				for (auto& p : m_vecObjects)
					if (arrMapInfo[i].ObjectID == p->GetID())
					{
						p->SetObjectInfo(&arrMapInfo[i]);
						break;
					}
			delete[] arrMapInfo;
			arrMapInfo = nullptr;
			break;
		}
		}

		delete msg;
		msg = nullptr;

		now = std::chrono::system_clock::now();
		timeElapsed = start_time - now;
	}
}

void CMainScene::SendMsgs()
{

}
