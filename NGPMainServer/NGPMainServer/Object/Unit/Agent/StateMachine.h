#pragma once
#include "stdafx.h"
#include "State.h"

template <class entity_type>
class StateMachine
{
private:

	// 상태를 소유하고 있는 에이전트의 포인터를 담는다.
	entity_type*          m_pOwner;

	// 현재 에이전트의 상태를 나타낸다.
	State<entity_type>*   m_pCurrentState;

	// 에이전트가 직전에 가지고 있던 상태를 저장한다.
	State<entity_type>*   m_pPreviousState;

	//상태가 업데이트되는 매 순간마다 불러들인다.
	State<entity_type>*   m_pGlobalState;


public:

	StateMachine(entity_type* owner) :m_pOwner(owner),
		m_pCurrentState(NULL),
		m_pPreviousState(NULL),
		m_pGlobalState(NULL)
	{}

	virtual ~StateMachine() {}

	//유한 상태기계를 초기화 하기 위한 메서드들이다.
	void SetCurrentState(State<entity_type>* s) { m_pCurrentState = s; }
	void SetGlobalState(State<entity_type>* s) { m_pGlobalState = s; }
	void SetPreviousState(State<entity_type>* s) { m_pPreviousState = s; }

	//상태기계를 업데이트하기 위해 호출한다.
	void  Update(float fTimeElapsed )const
	{
		// 만약 전역 상태를 정의했으면 전역 상태를 실행한다. 없으면 무시한다.
		if (m_pGlobalState)   m_pGlobalState->Execute(m_pOwner, fTimeElapsed);

		//same for the current state
		if (m_pCurrentState) m_pCurrentState->Execute(m_pOwner, fTimeElapsed);
	}

	//상태를 변경하기 위해 호출하는 메서드이다.
	void  ChangeState(State<entity_type>* pNewState)
	{
		//assert(pNewState &&
		//	"<StateMachine::ChangeState>: trying to change to NULL state");

		//상태를 변경하기 전에 이전 상태를 저장한다.
		m_pPreviousState = m_pCurrentState;

		//상태를 변경하기전 현재 상태를 나갈때 동작을 하기위한 메서드를 호출한다.
		m_pCurrentState->Exit(m_pOwner);

		//현재 상태를 새로운 상태로 변경한다.
		m_pCurrentState = pNewState;

		// 새로운 상태로 진입할 떄 동작을 수행하기 위한 메서드를 호출한다.
		m_pCurrentState->Enter(m_pOwner);
	}

	//이전 상태로 다시 돌아간다.
	void  RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}

	//현재 상태와 인자로 넘긴 상태와 비교한다.
	bool  isInState(const State<entity_type>& st)const
	{
		return typeid(*m_pCurrentState) == typeid(st);
	}

	// 상태를 반환받느 메서드들이다.
	State<entity_type>*  CurrentState()  const { return m_pCurrentState; }
	State<entity_type>*  GlobalState()   const { return m_pGlobalState; }
	State<entity_type>*  PreviousState() const { return m_pPreviousState; }
};