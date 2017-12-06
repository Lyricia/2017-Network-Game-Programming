#include "stdafx.h"
#include "Unit.h"


CUnit::CUnit(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CObject(pt, rc)
	, m_ptDirection(Point2F())
	, m_fHP(0)
	, m_fBlockStunTimer(0)
{
}
CUnit::~CUnit()
{
}

void CUnit::Update(float fTimeElapsed)
{
}

//void CUnit::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
//{
//}

//void CUnit::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
//{
//	m_pResMng = resMng;
//}

void CUnit::LookAt(const D2D1_POINT_2F& target_pos)
{
	if(m_ptPos == target_pos) return;
	m_ptDirection = Normalize(target_pos - m_ptPos);
	//float angle = -acosf(m_ptDirection*Point2F(1, 0)); 
	//if (m_ptDirection.y > 0)
	//	angle = -angle;
	//m_mtxRotate = Matrix3x2F::Rotation(
	//	RADIAN_TO_DEGREE(angle), m_ptPos);
	//m_ptTargetPos = target_pos;
}
