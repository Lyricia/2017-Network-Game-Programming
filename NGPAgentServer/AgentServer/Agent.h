#pragma once
#include "stdafx.h"
#include "State.h"
#include "StateMachine.h"
#include "AgentFSM.h"



class Agent {

protected:
	// ������Ʈ�� ���̵� ��
	UINT      m_ID;
	D2D_POINT_2F m_direction = {0,0};
	//���±��
	StateMachine<Agent>*  m_pStateMachine;

	// ������ �ٲٴ� ������ �����ϱ� ���� Ÿ�̸Ӵ�.
	// ������Ʈ�� update �޼��� ȣ�� �� m_changedir_timer���� m_next_change_dir_timer ���� ũ��
	// ���¸� ������ȯ ���·� �����Ѵ�. �̴� �������¿��� �����Ѵ�.
	float	m_changedir_timer = 0;
	// ������ ������ �󸶳� �ð��� ���� �� �ٲٴ��� �����̸� �����Ѵ�.
	float	m_next_change_dir_timer = 0;

	// ����� ���� Ÿ�̸Ӵ�.
	float	m_shoot_timer = 0;
public:
	Agent();
	~Agent();
	void update(float fTimeElapsed);
	StateMachine<Agent>*  GetFSM()const { return m_pStateMachine; }

	bool IsDirectionChangable() const { return (m_changedir_timer > m_next_change_dir_timer); };
	void ResetChangeDirectionTimer() { m_next_change_dir_timer = rand() % 7 + 5; } // 5 ~ 12 �� �̳��� ������ ���� ���ŵȴ�.

	int           GetID() const { return m_ID; }

};