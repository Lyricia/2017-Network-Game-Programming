#include "stdafx.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Projectile\Grenade\Grenade.h"

#include "Server\Main\MainServer.h"

#include "MainScene.h"

UINT g_nBrick = 0;

CMainScene::CMainScene()
	: m_ObjectIDCounter(0)
{
}
CMainScene::~CMainScene()
{
	for (auto& p : m_vecObjects)
		delete p;
	m_vecObjects.clear();
}

bool CMainScene::OnCreate(wstring && tag, CGameWorld* pGameWorld)
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
				brick->SetID(m_ObjectIDCounter++);
				brick->SetSize(RectF(-32, -32, 32, 32));
			}
		}

	g_nBrick = m_vecObjects.size();

	for(auto& p: m_pRoomInfo->clientlist)
	{
		CPlayer* player = new CPlayer(Point2F(-100, 10));
		player->SetID(m_ObjectIDCounter++);
		player->SetSize(RectF(-32, -32, 32, 32));

		p->ID = player->GetID();
		m_vecObjects.push_back(player);
		UCHAR type = MSGTYPE::MSGSTATE::ROOMCREATION;
		UCHAR roomNo = m_pRoomInfo->RoomID;
		UINT objNo = p->ID;

		p->pUserdata =(LPVOID)player;
		NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 0, NULL, NULL);
		send(p->sock, (char*)msg, sizeof(NGPMSG), 0);
		delete msg;
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
				g_nBrick--;
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
	auto start_time		= std::chrono::system_clock::now();
	auto now			= std::chrono::system_clock::now();
	std::chrono::duration<float> timeElapsed = start_time - now;
	
	NGPMSG*		msg				= nullptr;
	ActionInfo* arrActionInfo	= nullptr;
	ObjInfo*	arrObjInfo		= nullptr;
	MapInfo*	arrMapInfo		= nullptr;

	while (timeElapsed.count() < MESSAGE_PROCESSING_TIME)
	{
		if (!m_pRoomInfo->MsgQueue.size()) return;
		m_pRoomInfo->EnterCriticalSection();
		msg = m_pRoomInfo->MsgQueue.front();
		m_pRoomInfo->MsgQueue.pop_front();
		m_pRoomInfo->LeaveCriticalSection();

		switch (msg->header.MSGTYPE)
		{
		case MSGTYPE::MSGACTION::MOVE:
		{
			arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (auto& p : m_pRoomInfo->clientlist)
			{
				CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
				if (player->GetID() == msg->header.OBJECTNO)
				{
					for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
					{
						player->SetMoveDirection(arrActionInfo[i].MoveDirection);
						player->SetDirection(arrActionInfo[i].LookDirection);
					}
				}
			}
			delete[] arrActionInfo;
			arrActionInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGACTION::STOP:
		{
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (auto& p : m_pRoomInfo->clientlist)
			{
				CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
				if (player->GetID() == msg->header.OBJECTNO)
					player->Stop();
			}
			break;
		}
		case MSGTYPE::MSGACTION::SHOOT:
		{
			//arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			//DispatchMSG(msg, arrActionInfo, arrObjInfo);
			//for (auto& p : m_pRoomInfo->clientlist)
			//{
			//	CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
			//	if (player->GetID() == msg->header.OBJECTNO)
			//	{
			//		for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
			//		{
			//			player->SetMoveDirection(arrActionInfo[i].MoveDirection);
			//			player->SetDirection(arrActionInfo[i].LookDirection);
			//		}
			//	}
			//}
			//delete[] arrActionInfo;
			//arrActionInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGACTION::BUILDTURRET:
			break;
		case MSGTYPE::MSGACTION::RELOAD:
			break;
		case MSGTYPE::MSGSTATE::AIAGENTINFO:
			break;
		case MSGTYPE::MSGSTATE::AICREATTIONREQUEST:
			break;
		case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
			break;
		case MSGTYPE::MSGSTATE::CLIENTREADY:
			break;
		case MSGTYPE::MSGSTATE::ROOMCREATION:
			break;
		case MSGTYPE::MSGUPDATE::ADJUSTPOS:
			break;
		case MSGTYPE::MSGUPDATE::CREATEOBJECT:
			break;
		case MSGTYPE::MSGUPDATE::DELETEOBJECT:
			break;
		case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
			break;
		}

		delete msg;
		//delete[] arrObjInfo;
		//delete[] arrMapInfo;
		//msg					= nullptr;
		//arrObjInfo			= nullptr;
		//arrMapInfo			= nullptr;

		now = std::chrono::system_clock::now();
		timeElapsed = start_time - now;
	}
}

void CMainScene::SendMsgs()
{
	int nPlayer = m_pRoomInfo->clientlist.size();
	ObjInfo* objdata = new ObjInfo[nPlayer];
	int idx = 0, retval;

	for (auto &client : m_pRoomInfo->clientlist)
	{
		CPlayer* p = (CPlayer*)client->pUserdata;
		ObjInfo* tmp = (ObjInfo*)p->GetObjectInfo();
		objdata[idx++] = *tmp;
		delete tmp;
	}

	MapInfo* mapdata = new MapInfo[g_nBrick];
	idx = 0;
	for (auto &obj : m_vecObjects)
	{
		if (obj->GetTag() == CObject::Type::Brick)
		{
			MapInfo* tmp = (MapInfo*)obj->GetObjectInfo();
			mapdata[idx++] = *(tmp);
			delete tmp;
		}
	}

	NGPMSG* objmsg = CreateMSG(
		MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE
		, m_pRoomInfo->RoomID
		, 0
		, nPlayer
		, 0
		, objdata
		, nullptr
	);
	
	int nMapmsg = g_nBrick / MAPINFOBUFSIZE + 1;
	NGPMSG** mapmsg = new NGPMSG*[nMapmsg];
	int offset = MAPINFOBUFSIZE;

	for (int i = 0; i < nMapmsg; ++i)
	{
		mapmsg[i] = CreateMSG(
			MSGTYPE::MSGUPDATE::UPDATEMAPSTATE
			, m_pRoomInfo->RoomID
			, 0
			, (i == nMapmsg - 1) ? g_nBrick % MAPINFOBUFSIZE : MAPINFOBUFSIZE
			, mapdata + offset * i
		);
	}

	for (auto & client : m_pRoomInfo->clientlist)
	{
		objmsg->header.OBJECTNO = client->ID;

		retval = send(client->sock, (char*)objmsg, sizeof(NGPMSG), NULL);
		if(retval == SOCKET_ERROR){
			//assert
		}

		for (int i = 0; i < nMapmsg; ++i) {
			mapmsg[i]->header.OBJECTNO = client->ID;
			retval = send(client->sock, (char*)mapmsg[i], sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}
		}
	}

	delete objmsg;
	for (int i = 0; i < nMapmsg; ++i)
		delete mapmsg[i];

	delete[] mapmsg;
	delete[] mapdata;
	delete[] objdata;
}
