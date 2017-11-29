#pragma once
#include "stdafx.h"
#include "State.h"
#include "StateMachine.h"
#include "AgentFSM.h"
#include "Object\Unit\Unit.h"

//#define WITH_RENDER_AGENT

#define AGENT_BLOCK_STUN_TIME		0.2
#define AGENT_REFLACTION_FACTOR		0.8f
#define AGENT_MAX_VELOCITY			100.f
#define AGENT_SHOOT_TIME			0.3
#define AGENT_SHOOT_RANGE			1000.f

#define AGENT_SHOOT_SIGHT			100.f
class CResourceManager;
class CAgent : public CUnit {

protected:
	//상태기계
	StateMachine<CAgent>*	m_pStateMachine;

#ifdef WITH_RENDER_AGENT
	ComPtr<ID2D1Bitmap1>			m_bmpImage;
	D2D_POINT_2F					m_ptCurrImg;
	UINT							m_nSpriteImgWidth;
	UINT							m_nSpriteImgHeight;

	ComPtr<ID2D1Bitmap1>			m_bmpWeaponImage;
	D2D_RECT_F						m_rcWeaponSize;

	ComPtr<ID2D1SolidColorBrush>	m_pd2dLineBrush;
	D2D_POINT_2F					m_ptMuzzleDirection;
	D2D_POINT_2F					m_ptMuzzleStartPos;
	D2D_POINT_2F					m_ptMuzzleEndPos;
#endif

	// 사격 대상
	CObject*						m_pTarget;

	float							m_fBlockStunTimer;
	bool							m_bCollision;
	
	D2D_POINT_2F					m_ptDirection;
	D2D_POINT_2F					m_ptVelocity;

	// 방향을 바꾸는 순간을 판정하기 위한 타이머다.
	// 에이전트의 update 메서드 호출 시 m_changedir_timer값이 m_next_change_dir_timer 보다 크면
	// 상태를 방향전환 상태로 변경한다. 이는 전역상태에서 판정한다.
	float	m_changedir_timer = 0;
	// 다음번 방향을 얼마나 시간이 지난 후 바꾸는지 딜레이를 저장한다.
	float	m_next_change_dir_timer = 0;

	// 사격을 위한 타이머다.
	float	m_shoot_timer = 0;
public:
	CAgent(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());

	virtual ~CAgent();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	StateMachine<CAgent>*  GetFSM()const { return m_pStateMachine; }

	bool IsDirectionChangable() const { return (m_changedir_timer > m_next_change_dir_timer); };

	void ResetChangeDirectionTimer() { 
		m_changedir_timer = 0;
		m_next_change_dir_timer = rand() % 5 + 1; } // 1 ~ 5 초 이내에 방향이 새로 갱신된다.

	virtual void RegisterResourceManager(std::shared_ptr<CResourceManager> resMng);

	virtual void Collide(float atk) override;

	void Move(const D2D_POINT_2F& ptVelocity, float fTimeElapsed);
	void Reflection(const D2D_POINT_2F& ptReflect = Point2F());
	void Stop();

	void Shoot();
	void RayCastingToShoot(std::vector<CObject*>& pvecObjects);

	void SetHP(float hp) { m_fHP = hp; }
	void SetTarget(CObject* target) { m_pTarget = target; }

	void SetDirection(D2D_POINT_2F pt) { m_ptDirection = pt; }

	CObject* GetTarget() const { return m_pTarget; }
	D2D_POINT_2F GetVelocity() const { return m_ptVelocity; }
	D2D_POINT_2F GetDirection() const { return m_ptDirection; }
};