#pragma once
#include "stdafx.h"
#include "State.h"
#include "StateMachine.h"
#include "AgentFSM.h"



class Agent {

protected:
	// 에이전트의 아이디 값
	UINT      m_ID;
	D2D_POINT_2F m_direction = {0,0};
	//상태기계
	StateMachine<Agent>*  m_pStateMachine;

	// 방향을 바꾸는 순간을 판정하기 위한 타이머다.
	// 에이전트의 update 메서드 호출 시 m_changedir_timer값이 m_next_change_dir_timer 보다 크면
	// 상태를 방향전환 상태로 변경한다. 이는 전역상태에서 판정한다.
	float	m_changedir_timer = 0;
	// 다음번 방향을 얼마나 시간이 지난 후 바꾸는지 딜레이를 저장한다.
	float	m_next_change_dir_timer = 0;

	// 사격을 위한 타이머다.
	float	m_shoot_timer = 0;
public:
	Agent();
	~Agent();
	void update(float fTimeElapsed);
	StateMachine<Agent>*  GetFSM()const { return m_pStateMachine; }

	bool IsDirectionChangable() const { return (m_changedir_timer > m_next_change_dir_timer); };
	void ResetChangeDirectionTimer() { m_next_change_dir_timer = rand() % 7 + 5; } // 5 ~ 12 초 이내에 방향이 새로 갱신된다.

	int           GetID() const { return m_ID; }

};