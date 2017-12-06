#pragma once
#include "stdafx.h"
#include "Server\AgentServer.h"


// ���±���� ���ʰ� �Ǵ� Ŭ������.
template <class entity_type>
class State
{
public:

	virtual ~State() {}

	//���¿� �������� �� ȣ��Ǵ� �޼����.
	virtual void Enter(entity_type*) = 0;

	//���� ������Ʈ�� ������ �޼���� 
	virtual void Execute(entity_type*, float fTimeElapsed, RoomInfo* pRoomInfo) = 0;

	//���¸� �������� �� ȣ��Ǵ� �޼����.
	virtual void Exit(entity_type*) = 0;
};