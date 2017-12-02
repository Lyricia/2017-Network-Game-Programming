#pragma once
#include "Object\Object.h"

class CResourceManager;
class CUnit : public CObject
{
public:
	CUnit(D2D_POINT_2F pt, D2D_RECT_F rc);
	virtual ~CUnit();

	virtual void Update(float fTimeElapsed) override;
	virtual void Draw(ID2D1HwndRenderTarget* pd2dRenderTarget) override;

	virtual void RegisterResourceManager(shared_ptr<CResourceManager> resMng);

	virtual void Collide(float atk) {}
	void LookAt(const D2D1_POINT_2F& target_pos);
	void SetHP(float hp) { m_fHP = hp; }
	void SetDirection(const D2D1_POINT_2F& direction) { m_ptDirection = direction; }

	const D2D1_POINT_2F& GetDirection() const { return m_ptDirection; }
	bool IsDie() const { return m_fHP <= 0; }

protected:
	D2D1_POINT_2F					m_ptDirection;
	D2D1_POINT_2F					m_ptTargetPos;

	D2D1_MATRIX_3X2_F				m_mtxRotate;

	shared_ptr<CResourceManager>	m_pResMng;
	D2D1_SIZE_U						m_szImg;
	D2D1_POINT_2F					m_ptCurrImg;

	float							m_fHP;
	float							m_fBlockStunTimer;
};

