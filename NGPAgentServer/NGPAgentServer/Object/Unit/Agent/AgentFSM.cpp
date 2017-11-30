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

void AgentGlobalState::Execute(CAgent * agent, float fTimeElapsed)
{
	


	// 현재 방향을 바꿀 수 있다면 방향을 바꾸는 상태로 전환한다.
	// 지금은 자체적으로 변경
	if (agent->IsDirectionChangable())

		agent->GetFSM()->ChangeState(ChangeDirection::Instance());
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

void Wandering::Execute(CAgent * agent, float fTimeElapsed)
{
	agent->Move(agent->GetDirection(), fTimeElapsed);
	agent->LookAt(agent->GetDirection() + agent->GetPos() );

	//if (agent->GetTarget())
	//	agent->GetFSM()->ChangeState(Shooting::Instance());

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

void ChangeDirection::Execute(CAgent * agent, float fTimeElapsed)
{

	// send message -> 서버에 방향을 바꿀 것이란 메세지를 보낼 것이다. 
	// 지금은 자체적으로 상태 변경
	D2D_POINT_2F dir{ -10 + rand() % 20,-10 + rand() % 20 };
	agent->SetDirection(Normalize(dir));
	agent->GetFSM()->ChangeState(Wandering::Instance());

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

void Shooting::Execute(CAgent * agent, float fTimeElapsed)
{
	agent->Stop();
	if(agent->GetTarget())
	agent->LookAt(agent->GetTarget()->GetPos() );
	agent->Shoot();

	D2D_POINT_2F dist = agent->GetTarget()->GetPos() - agent->GetPos();
	float distance = sqrt(dist.x*dist.x + dist.y*dist.y);
	if (distance > AGENT_SHOOT_RANGE) {
		agent->SetTarget(nullptr);
		agent->GetFSM()->ChangeState(Wandering::Instance());
	}
		

}

void Shooting::Exit(CAgent * agent)
{
}

