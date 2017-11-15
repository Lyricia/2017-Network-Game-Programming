#include "stdafx.h"
#include "Agent.h"

Agent::Agent()
{
}

Agent::~Agent()
{
}

void Agent::update(float fTimeElapsed)
{
	m_pStateMachine->Update(fTimeElapsed);
}
