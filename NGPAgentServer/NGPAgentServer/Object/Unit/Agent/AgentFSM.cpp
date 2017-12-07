#pragma once
#include "stdafx.h"
#include "AgentFSM.h"

/////////////////////////////////////////////////////////////////////////////

AgentGlobalState * AgentGlobalState::Instance()
{
	static AgentGlobalState instance;

	return &instance;
}

void AgentGlobalState::Enter(CAgent * agent)
{
}

void AgentGlobalState::Execute(CAgent * agent, float fTimeElapsed, RoomInfo* pRoomInfo)
{
	// 현재 방향을 바꿀 수 있다면 방향을 바꾸는 상태로 전환한다.
	// 지금은 자체적으로 변경

}

void AgentGlobalState::Exit(CAgent * agent)
{
}

/////////////////////////////////////////////////////////////////////////////

Wandering * Wandering::Instance()
{
	static Wandering instance;

	return &instance;
}

void Wandering::Enter(CAgent * agent)
{
}

void Wandering::Execute(CAgent * agent, float fTimeElapsed, RoomInfo* pRoomInfo)
{

	UCHAR type = MSGTYPE::MSGACTION::MOVE;
	UCHAR roomNo = pRoomInfo->RoomID;
	UINT objNo = agent->GetID();
	ActionInfo action_info;
	action_info.LookDirection = agent->GetMoveDirection();
	action_info.MoveDirection = agent->GetMoveDirection();

	NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	
	pRoomInfo->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;

	agent->LookAt(agent->GetDirection() + agent->GetPos());

	if (agent->IsDirectionChangable())
		agent->GetFSM()->ChangeState(ChangeDirection::Instance());
}

void Wandering::Exit(CAgent * agent)
{
}

/////////////////////////////////////////////////////////////////////////////

ChangeDirection * ChangeDirection::Instance()
{
	static ChangeDirection instance;

	return &instance;
}


void ChangeDirection::Enter(CAgent * agent)
{
}

void ChangeDirection::Execute(CAgent * agent, float fTimeElapsed, RoomInfo* pRoomInfo)
{

	// send message -> 서버에 방향을 바꿀 것이란 메세지를 보낼 것이다. 
	// 지금은 자체적으로 상태 변경
	D2D_POINT_2F dir{ -10 + rand() % 20,-10 + rand() % 20 };
	agent->SetMoveDirection(Normalize(dir));
	
	UCHAR type = MSGTYPE::MSGACTION::MOVE;
	UCHAR roomNo = pRoomInfo->RoomID;
	UINT objNo = agent->GetID();
	ActionInfo action_info;
	action_info.LookDirection = agent->GetDirection();
	action_info.MoveDirection = agent->GetMoveDirection();

	NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	
	std::cout <<"ROOM - " << pRoomInfo->RoomID 
		<<"Agent No"<< objNo <<
		"SendMessage Change Direction" << std::endl;

	pRoomInfo->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;


	agent->GetFSM()->RevertToPreviousState();

}

void ChangeDirection::Exit(CAgent * agent)
{
	// 방향전환 타이머를 다시 갱신한다.
	agent->ResetChangeDirectionTimer();
}

/////////////////////////////////////////////////////////////////////////////

Shooting * Shooting::Instance()
{
	static Shooting instance;

	return &instance;
}

void Shooting::Enter(CAgent * agent)
{
}

void Shooting::Execute(CAgent * agent, float fTimeElapsed, RoomInfo* pRoomInfo)
{
	//agent->Stop();
	agent->LookAt(agent->GetTargetPos());

	D2D_POINT_2F dir{ agent->GetTargetPos() - agent->GetPos() };
	agent->SetDirection(Normalize(dir));

	{
		UCHAR type = MSGTYPE::MSGACTION::MOVE;
		UCHAR roomNo = pRoomInfo->RoomID;
		UINT objNo = agent->GetID();
		ActionInfo action_info;
		action_info.LookDirection = agent->GetDirection();
		action_info.MoveDirection = agent->GetMoveDirection();

		NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);

		pRoomInfo->SendMsgs((char*)msg, sizeof(NGPMSG));
		delete msg;
	}



	if (agent->IsCanbeShootable())
	{
		agent->ResetShootTimer();

		D2D_POINT_2F ptMuzzleDirection = agent->GetDirection();
		D2D_POINT_2F ptMuzzleStartPos = agent->GetPos() + ptMuzzleDirection * AGENT_MUZZLE_OFFSET;

		UCHAR type = MSGTYPE::MSGACTION::SHOOT;
		UCHAR roomNo = pRoomInfo->RoomID;
		UINT objNo = agent->GetID();
		ActionInfo action_info;
		action_info.TargetHitPos = agent->GetMuzzleEndPos();

		if (agent->GetTarget())	action_info.TargetID = agent->GetTarget()->GetID();
		else					action_info.TargetID = INVALID_OBJECT_ID;

		NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
		pRoomInfo->SendMsgs((char*)msg, sizeof(NGPMSG));

		delete msg;

		std::cout << "ROOM - " << pRoomInfo->RoomID
			<< "Agent No" << objNo <<
			"SendMessage Shoot " << std::endl;
		if (agent->GetTarget()) std::cout << "Target No :: " << agent->GetTarget()->GetID() << std::endl;
	}

	
	agent->LookAt(agent->GetDirection() + agent->GetPos());
	D2D_POINT_2F dist = Point2F();
	float distance = 999999;

	if (agent->GetTarget())
	{
		dist = agent->GetTarget()->GetPos() - agent->GetPos();
		distance = Length(dist);
	}
	if (distance > AGENT_SHOOT_RANGE) {
		agent->GetFSM()->ChangeState(Wandering::Instance());
	}
}

void Shooting::Exit(CAgent * agent)
{
}

