#include "stdafx.h"

#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"
#include "Object\Projectile\Grenade\Grenade.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Unit\Agent\Turret\Turret.h"
#include "Server\Main\MainServer.h"

#include "MainScene.h"

#define OBJECT_RECT RectF(-32, -32, 32, 32)

CMainScene::CMainScene()
	: m_ObjectIDCounter(0)
	, m_nBrick(0)
	, m_fCheckGameOverTime(0)
	, m_bPlayerDie(false)
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
	std::vector<D2D_POINT_2F> vecPathes;
	vecPathes.reserve(g_iMapSize*g_iMapSize);

	int map_size_half = g_iMapSize / 2;
	for (int i = 0; i < g_iMapSize; ++i)
		for (int j = 0; j < g_iMapSize; ++j)
		{
			if (g_iMap[j + i * g_iMapSize] == 1)
			{
				CBrick* brick = new CBrick(Point2F(
					  map_size_half + (j - map_size_half)*g_iMapSize
					, map_size_half + (i - map_size_half)*g_iMapSize));
				m_vecObjects.push_back(brick);
				brick->SetID(m_ObjectIDCounter++);
				brick->SetSize(OBJECT_RECT);
			}
			else vecPathes.push_back(Point2F(
				  map_size_half + (j - map_size_half)*g_iMapSize
				, map_size_half + (i - map_size_half)*g_iMapSize));
		}

	m_nBrick = m_vecObjects.size();
	int nPathes = vecPathes.size();
	for (int i = 0; i< 3; ++i)
	{
		CAgent* agent = new CAgent(vecPathes[rand() % nPathes]);
		agent->SetID(m_ObjectIDCounter++);
		agent->SetSize(OBJECT_RECT);
		agent->SetAgentType(CAgent::AgentType::Bot);
		m_vecObjects.push_back(agent);
		m_pRoomInfo->agentlist.push_back((LPVOID)agent);
	}

	for(auto& p: m_pRoomInfo->clientlist)
	{
		CPlayer* player = new CPlayer(vecPathes[rand() % nPathes]);
		player->SetID(m_ObjectIDCounter++);
		player->SetSize(OBJECT_RECT);

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
				NGPMSG* msg = CreateMSG(
					MSGTYPE::MSGUPDATE::DELETEOBJECT
					, m_pRoomInfo->RoomID
					, (*iter)->GetID()
					, 0, 0, nullptr, nullptr);
				for (auto& client : m_pRoomInfo->clientlist)
					if (client->pUserdata)
					{
						CPlayer* p = static_cast<CPlayer*>(client->pUserdata);
						if (p->GetID() == player->GetID())
						{
							client->pUserdata = nullptr;
							m_bPlayerDie = true;
						}

						int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
					}

				// 서버에게 삭제될 오브젝트 전송
				int retval = send(m_pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}

				delete msg;
				delete (*iter);
				iter = m_vecObjects.erase(iter);
			}
			else
				++iter;
			break;
		}
		case CObject::Type::Agent:
		{
			CAgent* agent = static_cast<CAgent*>((*iter));
			if (agent->IsDie())
			{
				NGPMSG* msg = CreateMSG(
					MSGTYPE::MSGUPDATE::DELETEOBJECT
					, m_pRoomInfo->RoomID
					, (*iter)->GetID()
					, 0, 0, nullptr, nullptr);

				for (auto& client : m_pRoomInfo->clientlist)
					if (client->pUserdata)
					{
						int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
					}

				// 에이전트 서버에게 삭제될 오브젝트 전송
				int retval = send(m_pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}

				delete msg;

				for (auto& p : m_pRoomInfo->agentlist)
					if (p)
					{
						CAgent* a = static_cast<CAgent*>(p);
						if (a->GetID() == agent->GetID())
							p = nullptr;
					}
				m_pRoomInfo->agentlist.remove_if([](LPVOID lp)->bool {
					if (lp) return false;
					return true;
				});
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
				NGPMSG* msg = CreateMSG(
					MSGTYPE::MSGUPDATE::DELETEOBJECT
					, m_pRoomInfo->RoomID
					, (*iter)->GetID()
					, 0, 0, nullptr, nullptr);
				for (auto& client : m_pRoomInfo->clientlist)
					if (client->pUserdata)
					{
						int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
					}

				// 에이전트 서버에게 블럭 제거 정보 전송
				int retval = send(m_pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}

				delete msg;

				delete (*iter);
				iter = m_vecObjects.erase(iter);
				m_nBrick--;
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

				ObjInfo* objdata = new ObjInfo();
				objdata->ObjectID = INVALID_OBJECT_ID;
				objdata->ObjectType = OBJECTTYPE::Effect_Explosion;
				objdata->Position = grenade->GetPos();

				NGPMSG* msg = CreateMSG(
					MSGTYPE::MSGUPDATE::CREATEOBJECT
					, m_pRoomInfo->RoomID
					, 0
					, 1
					, 0
					, objdata
					, nullptr
				);
				for (auto& client : m_pRoomInfo->clientlist)
					if (client->pUserdata)
					{
						int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
					}
				delete msg;
				delete objdata;

				msg = CreateMSG(
					MSGTYPE::MSGUPDATE::DELETEOBJECT
					, m_pRoomInfo->RoomID
					, grenade->GetID()
					, 0, 0, nullptr, nullptr);
				for (auto& client : m_pRoomInfo->clientlist)
					if (client->pUserdata)
					{
						int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
					}

				// 에이전트 서버에게 수류탄 제거 정보 전송
				int retval = send(m_pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}

				delete msg;

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

	if (m_bPlayerDie)
	{
		m_fCheckGameOverTime += fTimeElapsed;
		if (m_fCheckGameOverTime > GAMEOVER_TIMER)
		{
			m_bPlayerDie = false;
			m_fCheckGameOverTime = 0.0f;

			m_pRoomInfo->clientlist.remove_if([](ConnectionInfo* pClientInfo)->bool {
				if (pClientInfo->pUserdata) return false;
				printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
					inet_ntoa(pClientInfo->addr.sin_addr), ntohs(pClientInfo->addr.sin_port));
				delete pClientInfo;
				return true;
			});
			if (m_pRoomInfo->clientlist.size() == 1)
			{
				for (auto& client : m_pRoomInfo->clientlist)
					if (client->pUserdata)
					{
						NGPMSG* msg = CreateMSG(
							MSGTYPE::MSGSTATE::CLIENTGAMEOVER
							, m_pRoomInfo->RoomID
							, client->ID
							, 0, 0, nullptr, nullptr);
						int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
						delete msg;
					}
				m_bPlayerDie = true;
			}
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
		case CObject::Type::Agent:
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
						CAgent* agent = static_cast<CAgent*>(p);
						agent->Reflection(-1.f * dir);
						agent->SetDirection(-1.f * dir);
						agent->Move(-PLAYER_VELOCITY * dir * fTimeElapsed);
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
		auto pos = p->GetPos();
		if (pos.x > 2048) pos.x = 2048;
		else if (pos.x < -2048) pos.x = -2048;
		if (pos.y > 2048) pos.y = 2048;
		else if (pos.y < -2048) pos.y = -2048;
		p->SetPos(pos);
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
				if (p->pUserdata)
				{
					CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
					if (player->GetID() == msg->header.OBJECTNO)
						for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
						{
							player->SetMoveDirection(arrActionInfo[i].MoveDirection);
							player->SetDirection(arrActionInfo[i].LookDirection);
						}
				}
			}

			for (auto& p : m_pRoomInfo->agentlist)
			{
				CAgent* agent = static_cast<CAgent*>(p);
				if (agent->GetID() == msg->header.OBJECTNO)
					for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
					{
						agent->SetMoveDirection(arrActionInfo[i].MoveDirection);
						agent->SetDirection(arrActionInfo[i].LookDirection);
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
				if (p->pUserdata)
				{
					CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
					if (player->GetID() == msg->header.OBJECTNO)
						player->Stop();
				}

			break;
		}
		case MSGTYPE::MSGACTION::SHOOT:
		{
			CObject* target = nullptr;
			D2D_POINT_2F hit_pos = Point2F();
			arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
			{
				hit_pos = arrActionInfo[i].TargetHitPos;
				for (auto& p : m_vecObjects)
					if (p->GetID() == arrActionInfo[i].TargetID)
					{
						target = p;
						break;
					}

				for (auto& p : m_pRoomInfo->clientlist)
					if (p->pUserdata)
					{
						CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
						if (player->GetID() == msg->header.OBJECTNO)
						{
							player->Shoot(m_pRoomInfo, target, hit_pos);
							break;
						}
					}

				for (auto& p : m_pRoomInfo->agentlist)
				{
					CAgent* agent = static_cast<CAgent*>(p);
					if (agent->GetID() == msg->header.OBJECTNO)
					{
						if (agent->GetAgentType() == CAgent::AgentType::Bot)
						{
							agent->Shoot(m_pRoomInfo, target, hit_pos);
						}
						else
						{
							CTurret* turret = static_cast<CTurret*>(agent);
							turret->Shoot(m_pRoomInfo, target, hit_pos);
						}
						break;
					}
				}

			}
			delete[] arrActionInfo;
			arrActionInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGACTION::RELOAD:
		{
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (auto& p : m_pRoomInfo->clientlist)
				if (p->pUserdata)
				{
					CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
					if (player->GetID() == msg->header.OBJECTNO)
						player->Reload();
				}
			break;
		}
		case MSGTYPE::MSGACTION::GRENADE:
		{
			arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
			{
				CGrenade* grenade = new CGrenade(arrActionInfo[i].TargetPos, OBJECT_RECT);
				grenade->SetID(m_ObjectIDCounter++);
				grenade->SetVelocity(arrActionInfo[i].SetVelocity);
				for (auto& p : m_pRoomInfo->clientlist)
					if (p->pUserdata)
					{
						CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
						if (player->GetID() == msg->header.OBJECTNO)
						{
							player->GrenadeOut();
							grenade->SetParent(player);
							break;
						}
					}
				m_vecObjects.push_back(grenade);

				ObjInfo* objdata = new ObjInfo();
				objdata->ObjectID = grenade->GetID();
				objdata->ObjectType = OBJECTTYPE::Grenade;
				objdata->Position = arrActionInfo[i].TargetPos;
				objdata->Velocity = arrActionInfo[i].SetVelocity;
				objdata->ParentID = msg->header.OBJECTNO;

				NGPMSG* grenade_msg = CreateMSG(
					MSGTYPE::MSGUPDATE::CREATEOBJECT
					, m_pRoomInfo->RoomID
					, 0
					, 1
					, 0
					, objdata
					, nullptr
				);
				for (auto& client : m_pRoomInfo->clientlist)
				{
					int retval = send(client->sock, (char*)grenade_msg, sizeof(NGPMSG), NULL);
					if (retval == SOCKET_ERROR) {
						//assert
					}
				}

				// 에이전트 서버에게 수류탄 생성 정보를 보낸다.
				int retval = send(m_pRoomInfo->AgentServer->sock, (char*)grenade_msg, sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}
				delete grenade_msg;
				delete objdata;
			}
			delete[] arrActionInfo;
			arrActionInfo = nullptr;
			break;
		}
		case MSGTYPE::MSGACTION::BUILDTURRET:
		{
			arrActionInfo = new ActionInfo[msg->header.NUM_ACTIONINFO];
			DispatchMSG(msg, arrActionInfo, arrObjInfo);
			for (int i = 0; i < msg->header.NUM_ACTIONINFO; ++i)
			{
				CTurret* turret = new CTurret(arrActionInfo[i].TargetPos, OBJECT_RECT);
				turret->SetID(m_ObjectIDCounter++);
				turret->SetSize(OBJECT_RECT);
				turret->SetAgentType(CAgent::AgentType::Turret);

				for (auto& p : m_pRoomInfo->clientlist)
					if (p->pUserdata)
					{
						CPlayer* player = static_cast<CPlayer*>(p->pUserdata);
						if (player->GetID() == msg->header.OBJECTNO)
						{
							player->DeployTurret();
							turret->SetParent(player);
							break;
						}
					}
				m_vecObjects.push_back(turret);
				m_pRoomInfo->agentlist.push_back((LPVOID)turret);

				ObjInfo* objdata = new ObjInfo();
				objdata->ObjectID = turret->GetID();
				objdata->ObjectType = OBJECTTYPE::Turret;
				objdata->Position = arrActionInfo[i].TargetPos;
				objdata->Velocity = arrActionInfo[i].SetVelocity;
				objdata->ParentID = msg->header.OBJECTNO;

				NGPMSG* turret_msg = CreateMSG(
					MSGTYPE::MSGUPDATE::CREATEOBJECT
					, m_pRoomInfo->RoomID
					, 0
					, 1
					, 0
					, objdata
					, nullptr
				);
				for (auto& client : m_pRoomInfo->clientlist)
					if(client->pUserdata)
					{
						int retval = send(client->sock, (char*)turret_msg, sizeof(NGPMSG), NULL);
						if (retval == SOCKET_ERROR) {
							//assert
						}
					}

				// 에이전트 서버에게 터렛 생성 정보를 보낸다.
				int retval = send(m_pRoomInfo->AgentServer->sock, (char*)turret_msg, sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}
				delete turret_msg;
				delete objdata;
			}
			delete[] arrActionInfo;
			arrActionInfo = nullptr;
			break;
		}
		}
		delete msg;

		now = std::chrono::system_clock::now();
		timeElapsed = start_time - now;
	}
}

void CMainScene::SendMsgs()
{
	int idx = 0;
	int retval = 0;
	int nGrenade = 0;

	MapInfo* mapdata = new MapInfo[m_nBrick];
	
	for (auto &obj : m_vecObjects)
	{
		switch (obj->GetTag())
		{
		case CObject::Type::Brick:
		{
			MapInfo* tmp = (MapInfo*)obj->GetObjectInfo();
			mapdata[idx++] = *(tmp);
			delete tmp;
			break;
		}
		case CObject::Type::Grenade:
		{
			++nGrenade;
			break;
		}
		}
	}
	int nMapmsg = (m_nBrick / MAPINFOBUFSIZE) + 1;
	NGPMSG** mapmsg = new NGPMSG*[nMapmsg];
	int offset = MAPINFOBUFSIZE;
	MapInfo* ptr = mapdata;
	for (int i = 0; i < nMapmsg; ++i)
	{
		mapmsg[i] = CreateMSG(
			MSGTYPE::MSGUPDATE::UPDATEMAPSTATE
			, m_pRoomInfo->RoomID
			, 0
			, (i == nMapmsg - 1) ? m_nBrick % MAPINFOBUFSIZE : MAPINFOBUFSIZE
			, ptr + offset * i
		);
	}
	delete[] mapdata;

	int nPlayer = m_pRoomInfo->clientlist.size();
	ObjInfo* playerdata = new ObjInfo[nPlayer];
	idx = 0;
	for (auto &client : m_pRoomInfo->clientlist)
		if (client->pUserdata)
		{
			CPlayer* p = static_cast<CPlayer*>(client->pUserdata);
			ObjInfo* tmp = (ObjInfo*)p->GetObjectInfo();
			playerdata[idx++] = *tmp;
			delete tmp;
		}
	NGPMSG* playermsg = CreateMSG(
		MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE
		, m_pRoomInfo->RoomID
		, 0
		, nPlayer
		, 0
		, playerdata
		, nullptr
	);
	delete[] playerdata;

	ObjInfo* Grenadedata = new ObjInfo[nGrenade];
	idx = 0;
	for (auto iter = m_vecObjects.rbegin();
		iter != m_vecObjects.rend(); ++iter)
	{
		if ((*iter)->GetTag() == CObject::Type::Grenade)
		{
			ObjInfo* tmp = (ObjInfo*)(*iter)->GetObjectInfo();
			Grenadedata[idx++] = *tmp;
			delete tmp;
		}
	}
	NGPMSG* grenademsg = CreateMSG(
		MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE
		, m_pRoomInfo->RoomID
		, 0
		, nGrenade
		, 0
		, Grenadedata
		, nullptr
	);
	delete[] Grenadedata;

	int nAgent = m_pRoomInfo->agentlist.size();
	ObjInfo* agentdata = new ObjInfo[nAgent];
	idx = 0;
	for (auto& agent : m_pRoomInfo->agentlist)
	{
		if (!agent) continue;
		CAgent* a = static_cast<CAgent*>(agent);
		ObjInfo* tmp = (ObjInfo*)(a)->GetObjectInfo();
		agentdata[idx++] = *tmp;
		delete tmp;
	}

	NGPMSG* agentmsg = CreateMSG(
		MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE
		, m_pRoomInfo->RoomID
		, 0
		, nAgent
		, 0
		, agentdata
		, nullptr
	);
	delete[] agentdata;

	for (auto & client : m_pRoomInfo->clientlist)
		if (client->pUserdata)
		{
			playermsg->header.OBJECTNO = client->ID;

			retval = send(client->sock, (char*)playermsg, sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}

			retval = send(client->sock, (char*)grenademsg, sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}

			for (int i = 0; i < nMapmsg; ++i) {
				mapmsg[i]->header.OBJECTNO = client->ID;
				retval = send(client->sock, (char*)mapmsg[i], sizeof(NGPMSG), NULL);
				if (retval == SOCKET_ERROR) {
					//assert
				}
			}

			retval = send(client->sock, (char*)agentmsg, sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}
		}

	// 에이전트 서버에 오브젝트 업데이트 정보 전송
	{
		playermsg->header.OBJECTNO = m_pRoomInfo->AgentServer->ID;
		retval = send(m_pRoomInfo->AgentServer->sock, (char*)playermsg, sizeof(NGPMSG), NULL);
		if (retval == SOCKET_ERROR) {
			//assert
		}

		retval = send(m_pRoomInfo->AgentServer->sock, (char*)grenademsg, sizeof(NGPMSG), NULL);
		if (retval == SOCKET_ERROR) {
			//assert
		}

		for (int i = 0; i < nMapmsg; ++i) {
			mapmsg[i]->header.OBJECTNO = m_pRoomInfo->AgentServer->ID;
			retval = send(m_pRoomInfo->AgentServer->sock, (char*)mapmsg[i], sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}
		}

		retval = send(m_pRoomInfo->AgentServer->sock, (char*)agentmsg, sizeof(NGPMSG), NULL);
		if (retval == SOCKET_ERROR) {
			//assert
		}
	}

	delete playermsg;
	delete grenademsg;
	for (int i = 0; i < nMapmsg; ++i)
		delete mapmsg[i];
	delete[] mapmsg;
	delete agentmsg;
}
