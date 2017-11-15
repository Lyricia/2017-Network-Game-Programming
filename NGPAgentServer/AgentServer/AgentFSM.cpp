#pragma once
#include "stdafx.h"
#include "AgentFSM.h"

/////////////////////////////////////////////////////////////////////////////

AgentGlobalState * AgentGlobalState::Instance()
{
	static AgentGlobalState instance;

	return &instance;
}

void AgentGlobalState::Enter(Agent * agent)
{
}

void AgentGlobalState::Execute(Agent * agent, float fTimeElapsed)
{
	agent->update(fTimeElapsed);

	// ���� ������ �ٲ� �� �ִٸ� ������ �ٲٴ� ���·� ��ȯ�Ѵ�.
	if (agent->IsDirectionChangable())
		agent->GetFSM()->ChangeState(ChangeDirection::Instance());
}

void AgentGlobalState::Exit(Agent * agent)
{
}

/////////////////////////////////////////////////////////////////////////////

Wandering * Wandering::Instance()
{
	static Wandering instance;

	return &instance;
}

void Wandering::Enter(Agent * agent)
{
}

void Wandering::Execute(Agent * agent, float fTimeElapsed)
{
}

void Wandering::Exit(Agent * agent)
{
}

/////////////////////////////////////////////////////////////////////////////

ChangeDirection * ChangeDirection::Instance()
{
	static ChangeDirection instance;

	return &instance;
}


void ChangeDirection::Enter(Agent * agent)
{
}

void ChangeDirection::Execute(Agent * agent, float fTimeElapsed)
{
	// send message -> ������ ������ �ٲ� ���̶� �޼����� ���� ���̴�. ���⼭
	agent->GetFSM()->ChangeState(Wandering::Instance());
}

void ChangeDirection::Exit(Agent * agent)
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

void Shooting::Enter(Agent * agent)
{
}

void Shooting::Execute(Agent * agent, float fTimeElapsed)
{
}

void Shooting::Exit(Agent * agent)
{
}

