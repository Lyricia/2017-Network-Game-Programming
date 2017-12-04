#pragma once
#include "stdafx.h"

// 상태기계의 기초가 되는 클래스다.
template <class entity_type>
class State
{
public:

	virtual ~State() {}

	//상태에 진입했을 때 호출되는 메서드다.
	virtual void Enter(entity_type*) = 0;

	//상태 업데이트시 시행할 메서드다 
	virtual void Execute(entity_type*, float fTimeElapsed) = 0;

	//상태를 빠져나올 때 호출되는 메서드다.
	virtual void Exit(entity_type*) = 0;
};