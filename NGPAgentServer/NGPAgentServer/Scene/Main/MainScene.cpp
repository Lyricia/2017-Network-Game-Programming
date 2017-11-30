#include "stdafx.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Projectile\Grenade\Grenade.h"

#include "Server\AgentServer.h"

#include "MainScene.h"


CMainScene::CMainScene()
{
}
CMainScene::~CMainScene()
{
	for (auto& p : m_vecObjects)
		delete p;
	m_vecObjects.clear();
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
				CBrick* brick = new CBrick(Point2F((j - map_size_half)*g_iMapSize, (i - map_size_half)*g_iMapSize));
				m_vecObjects.push_back(brick);
			}
		}

	for (auto& p : m_pRoomInfo->clientlist)
	{
		CPlayer* player = new CPlayer(Point2F(-100, 10));
		p->ID = player->GetID();
		m_vecObjects.push_back(player);
		UCHAR type = MSGTYPE::MSGSTATE::ROOMCREATION;
		UCHAR roomNo = m_pRoomInfo->RoomID;
		UINT objNo = p->ID;

		NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 0, NULL, NULL);
		send(p->sock, (char*)msg, sizeof(NGPMSG), 0);
	}

	return true;
}

void CMainScene::PreprocessingUpdate(float fTimeElapsed)
{
	for (auto& iter = m_vecObjects.begin(); iter != m_vecObjects.end();)
	{
		switch ((*iter)->GetTag())
		{
		case CObject::Type::Player:
		{
			CPlayer* player = static_cast<CPlayer*>((*iter));
			if (player->IsDie())
			{
				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>((*iter));
			if (brick->IsBroken())
			{
				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		case CObject::Type::Grenade:
		{
			CGrenade* grenade = static_cast<CGrenade*>((*iter));
			if (grenade->IsExplosion())
			{
				grenade->Explosion(m_vecObjects);

				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		default:
			++iter;
			break;
		}
	}
}

void CMainScene::Update(float fTimeElapsed)
{
	PreprocessingUpdate(fTimeElapsed);

	for (auto& p : m_vecObjects)
		p->Update(fTimeElapsed);

	PhysicsUpdate(fTimeElapsed);
}

void CMainScene::PhysicsUpdate(float fTimeElapsed)
{
	D2D_POINT_2F dir = Point2F();

	for (auto& p : m_vecObjects)
	{
		switch (p->GetTag())
		{
		case CObject::Type::Player:
		{
			for (auto& q : m_vecObjects)
			{
				if (p == q) continue;
				switch (q->GetTag())
				{
				case CObject::Type::Brick:
				{
					dir = Normalize(q->GetPos() - p->GetPos());
					if (PtInRect(
						&(q->GetSize() + q->GetPos())
						, p->GetPos() + (dir * p->GetSize().right))
						)
					{
						CPlayer* player = static_cast<CPlayer*>(p);
						player->Reflection(-1.f * dir);
						player->Move(-PLAYER_VELOCITY * dir * fTimeElapsed);
					}
					break;
				}
				}
			}
			break;
		}
		case CObject::Type::Brick:
		{
			break;
		}
		case CObject::Type::Grenade:
		{
			for (auto& q : m_vecObjects)
			{
				if (p == q) continue;
				switch (q->GetTag())
				{
				case CObject::Type::Brick:
				{
					dir = Normalize(q->GetPos() - p->GetPos());
					if (PtInRect(
						&(q->GetSize() + q->GetPos())
						, p->GetPos() + (dir * p->GetSize().right))
						)
					{
						CGrenade* grenade = static_cast<CGrenade*>(p);
						grenade->Reflection(-1.f * dir);
					}
					break;
				}
				}
			}
			break;
		}
		}

	}
}

void CMainScene::ProcessMsgs()
{
	int msgtype = 0;

	//ActionInfo* Actionlist = new ActionInfo();
	//ObjInfo* Objlist = new ObjInfo();

	//NGPMSG msg;
	//::ZeroMemory(&msg, sizeof(msg));
	//if (!m_pRoomInfo->MsgQueue.empty())
	//{
	//	msgtype = DispatchMSG(m_pRoomInfo->MsgQueue.front(), *Actionlist, *Objlist);
	//	m_pRoomInfo->MsgQueue.pop_front();
	//}
	//else return;
	//
	//switch (msgtype)
	//{
	//case MSGTYPE::MSGACTION::MOVE:
	//	cout << "c ID: " << msg.header.OBJECTNO << " move!" << endl;
	//	break;
	//case MSGTYPE::MSGACTION::SHOOT:
	//	break;
	//case MSGTYPE::MSGACTION::BUILDTURRET:
	//	break;
	//case MSGTYPE::MSGACTION::RELOAD:
	//	break;
	//case MSGTYPE::MSGSTATE::AIAGENTINFO:
	//	break;
	//case MSGTYPE::MSGSTATE::AICREATTIONREQUEST:
	//	break;
	//case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
	//	break;
	//case MSGTYPE::MSGSTATE::CLIENTREADY:
	//	break;
	//case MSGTYPE::MSGSTATE::ROOMCREATION:
	//	break;
	//case MSGTYPE::MSGUPDATE::ADJUSTPOS:
	//	break;
	//case MSGTYPE::MSGUPDATE::CREATEOBJECT:
	//	break;
	//case MSGTYPE::MSGUPDATE::DELETEOBJECT:
	//	break;
	//case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
	//	break;
	//}

}

void CMainScene::SendMsgs()
{
	//CreateMSG(MSGTYPE::MSGUPDATE::ADJUSTPOS, )
	for (auto client : m_pRoomInfo->clientlist)
	{
		// send world update to clients
	}
}
