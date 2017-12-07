#pragma once
#include "stdafx.h"
#include "State.h"
#include "StateMachine.h"
#include "AgentFSM.h"
#include "Object\Unit\Unit.h"
#include "Server\AgentServer.h"
//#define WITH_RENDER_AGENT

#define AGENT_BLOCK_STUN_TIME		0.2
#define AGENT_REFLACTION_FACTOR		0.8f
#define AGENT_MAX_VELOCITY			100.f
#define AGENT_SHOOT_TIME			2.f
#define AGENT_SHOOT_RANGE			1000.f
#define AGENT_MUZZLE_OFFSET			64.f
#define AGENT_SHOOT_SIGHT			1000.f

class CEffect;
class CResourceManager;
class CAgent : public CUnit {

protected:
	//���±��
	StateMachine<CAgent>*	m_pStateMachine;

#ifdef WITH_RENDER_AGENT
	ComPtr<ID2D1Bitmap1>			m_bmpImage;
	D2D_POINT_2F					m_ptCurrImg;
	UINT							m_nSpriteImgWidth;
	UINT							m_nSpriteImgHeight;

	ComPtr<ID2D1Bitmap1>			m_bmpWeaponImage;
	D2D_RECT_F						m_rcWeaponSize;

	ComPtr<ID2D1SolidColorBrush>	m_pd2dLineBrush;
	
#endif
	D2D_POINT_2F					m_ptMuzzleDirection;
	D2D_POINT_2F					m_ptMuzzleStartPos;
	D2D_POINT_2F					m_ptMuzzleEndPos;

	int								m_iTeamTag;

	// Ÿ��
	CObject*						m_pTarget;
	bool							m_bisShootable{ false };
	// �������� ��ġ ������ ���� �䷮
	D2D_POINT_2F					m_ptTargetPos;
	float							m_fClosestTargetDistance;

	float							m_fBlockStunTimer;
	bool							m_bCollision;

	D2D_POINT_2F					m_ptDirection;
	D2D_POINT_2F					m_ptMoveDirection;

	D2D_POINT_2F					m_ptVelocity;

	bool							m_bMove;
	bool							m_bShoot;

	RoomInfo*						m_pRoomInfo;

	// ������ �ٲٴ� ������ �����ϱ� ���� Ÿ�̸Ӵ�.
	// ������Ʈ�� update �޼��� ȣ�� �� m_changedir_timer���� m_next_change_dir_timer ���� ũ��
	// ���¸� ������ȯ ���·� �����Ѵ�. �̴� �������¿��� �����Ѵ�.
	float	m_changedir_timer = 0;
	// ������ ������ �󸶳� �ð��� ���� �� �ٲٴ��� �����̸� �����Ѵ�.
	float	m_next_change_dir_timer = 0;

	// ����� ���� Ÿ�̸Ӵ�.
	float	m_shoot_timer = 0;
public:
	CAgent(D2D_POINT_2F pt, D2D_RECT_F rc = RectF());

	virtual ~CAgent();

	virtual void Update(float fTimeElapsed) override;
#ifdef WITH_RENDER_AGENT
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;
	virtual void RegisterResourceManager(std::shared_ptr<CResourceManager> resMng);
#endif
	StateMachine<CAgent>*  GetFSM()const { return m_pStateMachine; }

	void RegisterRoomInfo(RoomInfo*	pRoomInfo) { m_pRoomInfo = pRoomInfo; }
	
	bool IsDirectionChangable() const { return (m_changedir_timer > m_next_change_dir_timer); }

	// ��� ����ġ�� ���� �� �ִ°�.
	bool IsCanbeShootable() const { return (m_shoot_timer > AGENT_SHOOT_TIME); }
	// ��� ����ġ�� �����°�
	bool IsShootReady() const { return m_bisShootable; }

	void ResetChangeDirectionTimer() {
		m_changedir_timer = 0;
		m_next_change_dir_timer = rand() % 5 + 1;
	} // 1 ~ 5 �� �̳��� ������ ���� ���ŵȴ�.
	void ResetShootTimer() {
		m_shoot_timer = 0;
	} 

	virtual void Collide(float atk) override;

	void SetShootSwitch(bool shoot_switch) { m_bisShootable = shoot_switch; }

	void Move(const D2D_POINT_2F& ptVelocity);
	void Move(RoomInfo* pRoom, const D2D_POINT_2F& ptMoveDirection);
	void Move(const D2D_POINT_2F& ptVelocity, float fTimeElapsed);
	void Reflection(const D2D_POINT_2F& ptReflect = Point2F());
	void Stop();

	void Shoot();
	void Shoot(const D2D_POINT_2F & ptHitPos);

	void RayCastingToShoot(std::vector<CObject*>& pvecObjects);
	void RunStateMachine(float fTimeElapsed) {
		m_pStateMachine->Update(fTimeElapsed, m_pRoomInfo); 
	}

	void InterActionCheck(std::vector<CObject*>& pObjects);
	virtual void SetObjectInfo(LPVOID info) override;
	void SetHP(float hp) { m_fHP = hp; }

	void SetMoveDirection(D2D_POINT_2F pt) { m_ptMoveDirection = pt; }
	void SetDirection(D2D_POINT_2F pt) { m_ptDirection = pt; }

	CObject*	GetTarget() { return m_pTarget; }
	D2D_POINT_2F GetTargetPos() const { return m_ptTargetPos; }

	D2D_POINT_2F GetVelocity() const { return m_ptVelocity; }
	D2D_POINT_2F GetMoveDirection() const { return m_ptMoveDirection; }
	D2D_POINT_2F GetDirection() const { return m_ptDirection; }



	D2D_POINT_2F GetMuzzleDirection() const { return m_ptMuzzleDirection; }
	D2D_POINT_2F GetMuzzleStartPos()const { return m_ptMuzzleStartPos; }
	D2D_POINT_2F GetMuzzleEndPos()	const { return m_ptMuzzleEndPos; }
};