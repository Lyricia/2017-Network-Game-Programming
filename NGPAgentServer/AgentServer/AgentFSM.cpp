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

	// 현재 방향을 바꿀 수 있다면 방향을 바꾸는 상태로 전환한다.
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
	// send message -> 서버에 방향을 바꿀 것이란 메세지를 보낼 것이다. 여기서
	agent->GetFSM()->ChangeState(Wandering::Instance());
}

void ChangeDirection::Exit(Agent * agent)
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

void Shooting::Enter(Agent * agent)
{
}

void Shooting::Execute(Agent * agent, float fTimeElapsed)
{
}

void Shooting::Exit(Agent * agent)
{
}

