#pragma once
#include "stdafx.h"
#include "State.h"
#include "Agent.h"

class CAgent;

class AgentGlobalState : public State<CAgent>
{
private:

	AgentGlobalState() {}
	AgentGlobalState(const AgentGlobalState&);
	AgentGlobalState& operator=(const AgentGlobalState&);

public:

	static AgentGlobalState* Instance();

	virtual void Enter(CAgent* agent);

	virtual void Execute(CAgent* agent, float fTimeElapsed);

	virtual void Exit(CAgent* agent);
};

// ���ƴٴϴ� ����, Default �� ���´�.
class Wandering : public State<CAgent>
{
private:

	Wandering() {}
	Wandering(const Wandering&);
	Wandering& operator=(const Wandering&);

public:

	static Wandering* Instance();

public:

	virtual void Enter(CAgent* agent);

	virtual void Execute(CAgent* agent, float fTimeElapsed);

	virtual void Exit(CAgent* agent);

};


// �̵� ������ �����ϴ� ����
class ChangeDirection : public State<CAgent>
{
private:

	ChangeDirection() {}
	ChangeDirection(const ChangeDirection&);
	ChangeDirection& operator=(const ChangeDirection&);

public:

	static ChangeDirection* Instance();

public:

	virtual void Enter(CAgent* agent);

	virtual void Execute(CAgent* agent, float fTimeElapsed);

	virtual void Exit(CAgent* agent);



};

class Shooting : public State<CAgent>
{
private:

	Shooting() {}
	Shooting(const Shooting&);
	Shooting& operator=(const Shooting&);

public:

	static Shooting* Instance();

public:

	virtual void Enter(CAgent* agent);

	virtual void Execute(CAgent* agent, float fTimeElapsed);

	virtual void Exit(CAgent* agent);



};