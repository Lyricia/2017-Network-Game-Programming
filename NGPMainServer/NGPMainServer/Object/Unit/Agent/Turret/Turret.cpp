#include "stdafx.h"
#include "Turret.h"

#include "Server\Main\MainServer.h"
#include "Object\Brick\Brick.h"
#include "Object\Unit\Player\Player.h"


CTurret::CTurret(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CAgent(pt, rc)
{

	m_pTarget = nullptr;
	m_ptTargetPos = { 0,0 };
	m_fBlockStunTimer = 0;
	m_bCollision = false;
	m_bShoot = true;
	m_ptVelocity = Point2F();

	m_Tag = CObject::Type::Agent;
	m_next_change_dir_timer = rand() % 5 + 1;
	m_fHP = TURRET_MAX_HP;

	m_pParent = nullptr;
}

CTurret::~CTurret()
{
}

void CTurret::Update(float fTimeElapsed)
{
	Move(AGENT_VELOCITY * fTimeElapsed);

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * FRICTIONAL_DRAG;
	if (Length(m_ptVelocity) < FRICTIONAL_DRAG)
	{
		m_ptVelocity = Point2F();
		m_bMove = false;
	}


	if (m_bShoot)
	{
		m_fShootTimer += fTimeElapsed;
		if (m_fShootTimer > SHOOT_TIME)
		{
			m_bShoot = false;
			m_fShootTimer = 0.f;
		}
	}

	if (m_bCollision)
	{
		m_fBlockStunTimer += fTimeElapsed;
		if (m_fBlockStunTimer > BLOCK_STUN_TIME)
		{
			m_bCollision = false;
			m_fBlockStunTimer = 0.f;
		}
	}

}

void CTurret::Shoot()
{
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;

	if (m_pTarget)
	{
		switch (m_pTarget->GetTag())
		{
		case CObject::Type::Player:
		{
			CPlayer* player = static_cast<CPlayer*>(m_pTarget);
			player->Collide(TURRET_SHOOT_DAMAGE);
			player->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Agent:
		{
			CAgent* agent = static_cast<CAgent*>(m_pTarget);
			agent->Collide(TURRET_SHOOT_DAMAGE);
			agent->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>(m_pTarget);
			brick->Collide(TURRET_SHOOT_DAMAGE);
			break;
		}
		}
	}
	return;
}

void CTurret::Shoot(RoomInfo * pRoomInfo, CObject * pTarget, const D2D_POINT_2F & ptHitPos)
{
	m_pTarget = pTarget;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;
	m_ptMuzzleEndPos = ptHitPos;

	NGPMSG* msg = nullptr;

	UCHAR type = MSGTYPE::MSGACTION::SHOOT;
	UCHAR roomNo = pRoomInfo->RoomID;
	UINT objNo = GetID();
	ActionInfo action_info;
	action_info.TargetHitPos = m_ptMuzzleEndPos;
	action_info.TargetID = INVALID_OBJECT_ID;

	msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	for (auto& client : pRoomInfo->clientlist)
	{
		int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
		if (retval == SOCKET_ERROR) {
			//assert
		}
	}

	send(pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);

	delete msg;

	if (m_pTarget)
	{
		switch (m_pTarget->GetTag())
		{
		case CObject::Type::Player:
		{
			CPlayer* player = static_cast<CPlayer*>(m_pTarget);
			player->Collide(TURRET_SHOOT_DAMAGE);
			player->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Agent:
		{
			CAgent* agent = static_cast<CAgent*>(m_pTarget);
			agent->Collide(TURRET_SHOOT_DAMAGE);
			agent->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>(m_pTarget);
			brick->Collide(TURRET_SHOOT_DAMAGE);
			break;
		}
		}

		ObjInfo* objdata = new ObjInfo();
		objdata->ObjectID = INVALID_OBJECT_ID;
		objdata->ObjectType = OBJECTTYPE::Effect_ShootingHit;
		objdata->Position = ptHitPos;

		msg = CreateMSG(
			MSGTYPE::MSGUPDATE::CREATEOBJECT
			, pRoomInfo->RoomID
			, 0
			, 1
			, 0
			, objdata
			, nullptr
		);
		for (auto& client : pRoomInfo->clientlist)
		{
			int retval = send(client->sock, (char*)msg, sizeof(NGPMSG), NULL);
			if (retval == SOCKET_ERROR) {
				//assert
			}
		}

		send(pRoomInfo->AgentServer->sock, (char*)msg, sizeof(NGPMSG), NULL);


		delete msg;
		delete objdata;
	}
}
