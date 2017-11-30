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
	


	// ���� ������ �ٲ� �� �ִٸ� ������ �ٲٴ� ���·� ��ȯ�Ѵ�.
	// ������ ��ü������ ����
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

	// send message -> ������ ������ �ٲ� ���̶� �޼����� ���� ���̴�. 
	// ������ ��ü������ ���� ����
	D2D_POINT_2F dir{ -10 + rand() % 20,-10 + rand() % 20 };
	agent->SetDirection(Normalize(dir));
	agent->GetFSM()->ChangeState(Wandering::Instance());

}

void ChangeDirection::Exit(CAgent * agent)
{
	// ������ȯ Ÿ�̸Ӹ� �ٽ� �����Ѵ�.
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

