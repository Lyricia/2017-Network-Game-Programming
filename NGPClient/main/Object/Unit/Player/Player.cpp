#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "Player.h"
#include "Object\Brick\Brick.h"


CPlayer::CPlayer(
	CIndRes* pIndRes
	, ID2D1HwndRenderTarget* pd2dRenderTarget
	, D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_bmpImage(nullptr)
	, m_bmpWeaponImage(nullptr)
	, m_pTarget(nullptr)
	, m_rcWeaponSize(RectF())
	, m_fBlockStunTimer(0)
	, m_bCollision(false)
	, m_fShootTimer(0)
	, m_bShoot(true)
	, m_bReload(false)
	, m_ptVelocity(Point2F())
	, m_iAmmo(MAX_AMMO)
	, m_iGrenade(MAX_GRENADE)
	, m_iTurretKit(MAX_TURRET)
{
	m_Tag = CObject::Type::Player;
	m_fHP = PLAYER_MAX_HP;

	pd2dRenderTarget->CreateSolidColorBrush(
		ColorF{ ColorF::White }
		, &m_pd2dOutlineBrush);

	pd2dRenderTarget->CreateSolidColorBrush(
		ColorF{ ColorF::LightYellow }
		, &m_pd2dLineBrush);

	pd2dRenderTarget->CreateSolidColorBrush(
		ColorF{ ColorF::DarkGray }
		, &m_pd2dTextBrush);

	pd2dRenderTarget->CreateSolidColorBrush(
		ColorF{ ColorF::Red }
		, &m_pd2dGaugeBrush);
	m_pd2dGaugeBrush->SetOpacity(0.8f);

	pIndRes->dwFactory()->CreateTextFormat(
		L"Arial"
		, nullptr
		, DWRITE_FONT_WEIGHT_BOLD
		, DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH_NORMAL
		, 40.f
		, L"ko-kr"
		, &m_dwTextFormat);
}
CPlayer::~CPlayer()
{
}

void CPlayer::Update(float fTimeElapsed)
{
	m_ptPos += m_ptVelocity * fTimeElapsed;

	m_ptVelocity -= m_ptVelocity * fTimeElapsed * FRICTIONAL_DRAG;
	if (Length(m_ptVelocity) < FRICTIONAL_DRAG)
		m_ptVelocity = Point2F();

	if (m_bReload)
	{
		m_fShootTimer += fTimeElapsed;
		if (m_fShootTimer > RELOAD_TIME)
		{
			m_bReload = false;
			m_bShoot = false;
			m_fShootTimer = 0.f;
			m_iAmmo = MAX_AMMO;
		}
	}
	else if (m_bShoot)
	{
		m_fShootTimer += fTimeElapsed;
		if (m_fShootTimer > SHOOT_TIME)
		{
			m_bShoot = false;
			m_fShootTimer = 0.f;
		}
	}
	if (m_bCollision)
	{
		m_fBlockStunTimer += fTimeElapsed;
		if (m_fBlockStunTimer > BLOCK_STUN_TIME)
		{
			m_bCollision = false;
			m_fBlockStunTimer = 0.f;
		}
	}
}

void CPlayer::Draw(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	D2D_MATRIX_3X2_F transform;
	pd2dRenderTarget->GetTransform(&transform);
	pd2dRenderTarget->SetTransform(m_mtxRotate*transform);

	auto bmpSize = m_bmpImage->GetSize();
	float aniWidthFactor = bmpSize.width / (float)m_nSpriteImgWidth;
	float aniHeightFactor = bmpSize.height / (float)m_nSpriteImgHeight;
	pd2dRenderTarget->DrawBitmap(
		m_bmpImage.Get()
		, m_rcSize + m_ptPos
		, 1.f
		, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
		, RectF(
			aniWidthFactor * (int)m_ptCurrImg.x
			, aniHeightFactor * (int)m_ptCurrImg.y
			, aniWidthFactor * ((int)m_ptCurrImg.x + 1)
			, aniHeightFactor * ((int)m_ptCurrImg.y + 1)));

	pd2dRenderTarget->DrawBitmap(
		m_bmpWeaponImage.Get()
		, m_rcWeaponSize + m_ptPos);

	pd2dRenderTarget->SetTransform(transform);

	if (m_bReload)
	{
		m_pd2dGaugeBrush->SetColor(ColorF{ ColorF::Blue });
		pd2dRenderTarget->FillRectangle(
			RectF(m_rcSize.left
				, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.2f
				, m_rcSize.left + (m_rcSize.right - m_rcSize.left) * (m_fShootTimer / RELOAD_TIME)
				, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f)
			+ m_ptPos
			, m_pd2dGaugeBrush.Get());
		m_pd2dGaugeBrush->SetColor(ColorF{ ColorF::Red });
	}
	else if (m_bShoot)
	{
		m_pd2dLineBrush->SetOpacity(1 - (m_fShootTimer /SHOOT_TIME));
		pd2dRenderTarget->DrawLine(
			m_ptMuzzleStartPos
			, m_ptMuzzleEndPos
			, m_pd2dLineBrush.Get()
			, SHOOT_STROKE);
	}

	pd2dRenderTarget->FillRectangle(
		RectF(m_rcSize.left
			, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f
			, m_rcSize.left + (m_rcSize.right - m_rcSize.left) * (m_fHP / PLAYER_MAX_HP)
			, m_rcSize.top)
		+ m_ptPos
		, m_pd2dGaugeBrush.Get());

	pd2dRenderTarget->DrawRectangle(
		RectF(m_rcSize.left
			, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f
			, m_rcSize.left + (m_rcSize.right - m_rcSize.left) 
			, m_rcSize.top)
		+ m_ptPos
		, m_pd2dOutlineBrush.Get());
}

void CPlayer::DrawUI(ID2D1HwndRenderTarget * pd2dRenderTarget)
{
	D2D_POINT_2F pt = Point2F(-CLIENT_WIDTH / 2 + 30, -CLIENT_HEIGHT / 2 + 30);
	std::wstring str = L"HP";
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_dwTextFormat.Get()
		, RectF(pt.x + 1, pt.y, pt.x + 70, pt.y + 50)
		+ m_ptPos
		, m_pd2dGaugeBrush.Get());
	
	pd2dRenderTarget->FillRectangle(
		RectF(pt.x + 70 , pt.y + 10
			, pt.x + 70 + 300 * (m_fHP / PLAYER_MAX_HP) , pt.y + 35)
		+ m_ptPos
		, m_pd2dGaugeBrush.Get());

	pd2dRenderTarget->DrawRectangle(
		RectF(pt.x + 70, pt.y + 10
			, pt.x + 70 + 300 * (m_fHP / PLAYER_MAX_HP), pt.y + 35)
		+ m_ptPos
		, m_pd2dOutlineBrush.Get());

	pt.y += 50;
	str = L"Ammo: ";
	str += std::to_wstring(m_iAmmo);
	str += L" / Inf";
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_dwTextFormat.Get()
		, RectF(pt.x, pt.y, pt.x + 1000, pt.y + 50)
		+ m_ptPos
		, m_pd2dTextBrush.Get());

	pt.y += 45;
	str = L"Grenade: ";
	str += std::to_wstring(m_iGrenade);
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_dwTextFormat.Get()
		, RectF(pt.x, pt.y, pt.x + 300, pt.y + 50)
		+ m_ptPos
		, m_pd2dTextBrush.Get());

	pt.y += 45;
	str = L"Kit: ";
	str += std::to_wstring(m_iTurretKit);
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_dwTextFormat.Get()
		, RectF(pt.x, pt.y, pt.x + 300, pt.y + 50)
		+ m_ptPos
		, m_pd2dTextBrush.Get());
}

void CPlayer::RegisterImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename)
{
	LoadImageFromFile(
		  pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpWeaponImage
	);

	if (IsRectInvalid(m_rcWeaponSize))
		m_rcWeaponSize = SizeToRect(m_bmpWeaponImage->GetSize());
}

void CPlayer::RegisterSpriteImage(CIndRes * pIndRes, ID2D1HwndRenderTarget * pd2dRenderTarget, path filename, D2D_POINT_2F ptLength)
{
	m_nSpriteImgWidth = ptLength.x;
	m_nSpriteImgHeight = ptLength.y;

	LoadImageFromFile(
		pIndRes->wicFactory()
		, pd2dRenderTarget
		, filename.c_str()
		, &m_bmpImage
	);
	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= ptLength.x;
		sz.height /= ptLength.y;
		m_rcSize = SizeToRect(sz);
	}
}

void CPlayer::Collide(float atk)
{
	if (m_bCollision) return;
	m_bCollision = true;
	m_fHP -= atk;
	if (m_fHP < 0)
		m_fHP = 0;
}

void CPlayer::Move(const D2D_POINT_2F& ptVelocity)
{
	m_ptVelocity += ptVelocity;
	float len = Length(m_ptVelocity);
	if (len > MAX_VELOCITY)
		m_ptVelocity -= Normalize(m_ptVelocity) * (len - MAX_VELOCITY);
}

void CPlayer::Reflection(const D2D_POINT_2F& ptReflect)
{
	if(ptReflect == Point2F())
		m_ptVelocity = m_ptVelocity * -REFLACTION_FACTOR;
	else
		m_ptVelocity = ptReflect * Length(m_ptVelocity * REFLACTION_FACTOR);
}

void CPlayer::Stop()
{
	m_ptVelocity = Point2F();
}

void CPlayer::Shoot()
{
	if (m_bShoot) return;
	if (m_bReload) return;

	if (m_iAmmo == 0) m_bReload = true;
	else --m_iAmmo;
	
	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;

	if (m_pTarget)
	{
		switch (m_pTarget->GetTag())
		{
		case CObject::Type::Player:
		{
			CPlayer* player = static_cast<CPlayer*>(m_pTarget);
			player->Collide(SHOOT_DAMAGE);
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>(m_pTarget);
			brick->Collide(SHOOT_DAMAGE);
			break;
		}
		}
	}
}

void CPlayer::RayCastingToShoot(std::vector<CObject*>& pvecObjects)
{
	if (m_bShoot) return;
	if (m_bReload) return;
	for (float ratio = 0; ratio < 1; ratio += 0.01f)
	{
		D2D1_POINT_2F ptDevidedRay = m_ptPos + (m_ptDirection * SHOOT_RANGE * ratio);
		for (auto& p : pvecObjects)
		{
			switch (p->GetTag())
			{
			case CObject::Type::Player:
			{
				if (Length(p->GetPos() - ptDevidedRay) < p->GetSize().right)
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				break;
			}
			case CObject::Type::Brick:
			{
				if (PtInRect(&(p->GetPos() + p->GetSize()), ptDevidedRay))
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				break;
			}
			}
		}
	}
	m_pTarget = nullptr;
	m_ptMuzzleEndPos = m_ptPos + (m_ptDirection * SHOOT_RANGE);
}
