#pragma once
#include "stdafx.h"
#include "State.h"

template <class entity_type>
class StateMachine
{
private:

	// ���¸� �����ϰ� �ִ� ������Ʈ�� �����͸� ��´�.
	entity_type*          m_pOwner;

	// ���� ������Ʈ�� ���¸� ��Ÿ����.
	State<entity_type>*   m_pCurrentState;

	// ������Ʈ�� ������ ������ �ִ� ���¸� �����Ѵ�.
	State<entity_type>*   m_pPreviousState;

	//���°� ������Ʈ�Ǵ� �� �������� �ҷ����δ�.
	State<entity_type>*   m_pGlobalState;


public:

	StateMachine(entity_type* owner) :m_pOwner(owner),
		m_pCurrentState(NULL),
		m_pPreviousState(NULL),
		m_pGlobalState(NULL)
	{}

	virtual ~StateMachine() {}

	//���� ���±�踦 �ʱ�ȭ �ϱ� ���� �޼�����̴�.
	void SetCurrentState(State<entity_type>* s) { m_pCurrentState = s; }
	void SetGlobalState(State<entity_type>* s) { m_pGlobalState = s; }
	void SetPreviousState(State<entity_type>* s) { m_pPreviousState = s; }

	//���±�踦 ������Ʈ�ϱ� ���� ȣ���Ѵ�.
	void  Update(float fTimeElapsed )const
	{
		// ���� ���� ���¸� ���������� ���� ���¸� �����Ѵ�. ������ �����Ѵ�.
		if (m_pGlobalState)   m_pGlobalState->Execute(m_pOwner, fTimeElapsed);

		//same for the current state
		if (m_pCurrentState) m_pCurrentState->Execute(m_pOwner, fTimeElapsed);
	}

	//���¸� �����ϱ� ���� ȣ���ϴ� �޼����̴�.
	void  ChangeState(State<entity_type>* pNewState)
	{
		//assert(pNewState &&
		//	"<StateMachine::ChangeState>: trying to change to NULL state");

		//���¸� �����ϱ� ���� ���� ���¸� �����Ѵ�.
		m_pPreviousState = m_pCurrentState;

		//���¸� �����ϱ��� ���� ���¸� ������ ������ �ϱ����� �޼��带 ȣ���Ѵ�.
		m_pCurrentState->Exit(m_pOwner);

		//���� ���¸� ���ο� ���·� �����Ѵ�.
		m_pCurrentState = pNewState;

		// ���ο� ���·� ������ �� ������ �����ϱ� ���� �޼��带 ȣ���Ѵ�.
		m_pCurrentState->Enter(m_pOwner);
	}

	//���� ���·� �ٽ� ���ư���.
	void  RevertToPreviousState()
	{
		ChangeState(m_pPreviousState);
	}

	//���� ���¿� ���ڷ� �ѱ� ���¿� ���Ѵ�.
	bool  isInState(const State<entity_type>& st)const
	{
		return typeid(*m_pCurrentState) == typeid(st);
	}

	// ���¸� ��ȯ�޴� �޼�����̴�.
	State<entity_type>*  CurrentState()  const { return m_pCurrentState; }
	State<entity_type>*  GlobalState()   const { return m_pGlobalState; }
	State<entity_type>*  PreviousState() const { return m_pPreviousState; }
};