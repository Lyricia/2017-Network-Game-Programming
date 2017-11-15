#pragma once
#include "stdafx.h"
#include "State.h"
#include "Agent.h"

class Agent;

class AgentGlobalState : public State<Agent>
{
private:

	AgentGlobalState() {}
	AgentGlobalState(const AgentGlobalState&);
	AgentGlobalState& operator=(const AgentGlobalState&);

public:

	static AgentGlobalState* Instance();

	virtual void Enter(Agent* agent);

	virtual void Execute(Agent* agent, float fTimeElapsed);

	virtual void Exit(Agent* agent);
};

// ���ƴٴϴ� ����, Default �� ���´�.
class Wandering : public State<Agent>
{
private:

	Wandering() {}
	Wandering(const Wandering&);
	Wandering& operator=(const Wandering&);

public:

	static Wandering* Instance();

public:

	virtual void Enter(Agent* agent);

	virtual void Execute(Agent* agent, float fTimeElapsed);

	virtual void Exit(Agent* agent);

};


// �̵� ������ �����ϴ� ����
class ChangeDirection : public State<Agent>
{
private:

	ChangeDirection() {}
	ChangeDirection(const ChangeDirection&);
	ChangeDirection& operator=(const ChangeDirection&);

public:

	static ChangeDirection* Instance();

public:

	virtual void Enter(Agent* agent);

	virtual void Execute(Agent* agent, float fTimeElapsed);

	virtual void Exit(Agent* agent);



};

class Shooting : public State<Agent>
{
private:

	Shooting() {}
	Shooting(const Shooting&);
	Shooting& operator=(const Shooting&);

public:

	static Shooting* Instance();

public:

	virtual void Enter(Agent* agent);

	virtual void Execute(Agent* agent, float fTimeElapsed);

	virtual void Exit(Agent* agent);



};