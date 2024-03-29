#include "stdafx.h"
#include "Framework\ResourceManager\ResourceManager.h"
#include "Framework\Client\Client.h"
#include "Player.h"
#include "Object\Brick\Brick.h"
#include "Object\Unit\Agent\Agent.h"
#include "Object\Projectile\Grenade\Grenade.h"
#include "Object\Effect\Effect.h"


CPlayer::CPlayer(D2D_POINT_2F pt, D2D_RECT_F rc)
	: CUnit(pt, rc)
	, m_pTarget(nullptr)
	, m_rcWeaponSize(RectF())
	, m_fBlockStunTimer(0)
	, m_fShootTimer(0)
	, m_fGrenadeTimer(0)
	, m_bCollision(false)
	, m_bShoot(true)
	, m_bReload(false)
	, m_bGrenade(false)
	, m_ptVelocity(Point2F())
	, m_iAmmo(MAX_AMMO)
	, m_iGrenade(MAX_GRENADE)
	, m_iTurretKit(MAX_TURRET)
{
	m_Tag = CObject::Type::Player;
	m_fHP = PLAYER_MAX_HP;
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

	if (m_bGrenade)
	{
		m_fGrenadeTimer += fTimeElapsed;
		if (m_fGrenadeTimer > GRENADE_DELAY)
		{
			m_bGrenade = false;
			m_fGrenadeTimer = 0.f;
		}
	}

	if (m_bTurret)
	{
		m_fTurretTimer += fTimeElapsed;
		if (m_fTurretTimer > GRENADE_DELAY)
		{
			m_bTurret = false;
			m_fTurretTimer = 0.f;
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
	float aniWidthFactor = bmpSize.width / (float)m_szImg.width;
	float aniHeightFactor = bmpSize.height / (float)m_szImg.height;
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
		pd2dRenderTarget->FillRectangle(
			RectF(m_rcSize.left
				, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.2f
				, m_rcSize.left + (m_rcSize.right - m_rcSize.left) * (m_fShootTimer / RELOAD_TIME)
				, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f)
			+ m_ptPos
			, m_pResMng->brBlue.Get());
	}
	else if (m_bShoot)
	{
		m_pResMng->brLightYellow->SetOpacity(1 - (m_fShootTimer /SHOOT_TIME));
		pd2dRenderTarget->DrawLine(
			m_ptMuzzleStartPos
			, m_ptMuzzleEndPos
			, m_pResMng->brLightYellow.Get()
			, SHOOT_STROKE);
		m_pResMng->brLightYellow->SetOpacity(1.f);
	}
	// 체력바
	m_pResMng->brRed->SetOpacity(0.8f);
	pd2dRenderTarget->FillRectangle(
		RectF(m_rcSize.left
			, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f
			, m_rcSize.left + (m_rcSize.right - m_rcSize.left) * (m_fHP / PLAYER_MAX_HP)
			, m_rcSize.top)
		+ m_ptPos
		, m_pResMng->brRed.Get());
	m_pResMng->brRed->SetOpacity(1.0f);

	// 체력바 테두리
	pd2dRenderTarget->DrawRectangle(
		RectF(m_rcSize.left
			, m_rcSize.top - (m_rcSize.bottom - m_rcSize.top) * 0.1f
			, m_rcSize.left + (m_rcSize.right - m_rcSize.left) 
			, m_rcSize.top)
		+ m_ptPos
		, m_pResMng->brWhite.Get());
}

void CPlayer::DrawUI(ID2D1HwndRenderTarget * pd2dRenderTarget, float fScaleFactor)
{
	D2D_POINT_2F pt = Point2F(
		  -fScaleFactor*(CLIENT_WIDTH / 2) + 30
		, -fScaleFactor*(CLIENT_HEIGHT / 2) + 30);
	std::wstring str = L"HP";
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_pResMng->dwUITextFormat.Get()
		, RectF(pt.x + 1, pt.y, pt.x + 70, pt.y + 50)
		+ m_ptPos
		, m_pResMng->brRed.Get());
	
	// 체력바
	pd2dRenderTarget->FillRectangle(
		RectF(pt.x + 70 , pt.y + 10
			, pt.x + 70 + 300 * (m_fHP / PLAYER_MAX_HP) , pt.y + 35)
		+ m_ptPos
		, m_pResMng->brRed.Get());

	// 체력바 테두리
	pd2dRenderTarget->DrawRectangle(
		RectF(pt.x + 70, pt.y + 10
			, pt.x + 70 + 300 * (m_fHP / PLAYER_MAX_HP), pt.y + 35)
		+ m_ptPos
		, m_pResMng->brWhite.Get());

	pt.y += 50;
	str = L"Ammo: ";
	str += std::to_wstring(m_iAmmo);
	str += L" / Inf";
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_pResMng->dwUITextFormat.Get()
		, RectF(pt.x, pt.y, pt.x + 1000, pt.y + 50)
		+ m_ptPos
		, m_pResMng->brDarkGray.Get());

	pt.y += 45;
	str = L"Grenade: ";
	str += std::to_wstring(m_iGrenade);
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_pResMng->dwUITextFormat.Get()
		, RectF(pt.x, pt.y, pt.x + 300, pt.y + 50)
		+ m_ptPos
		, m_pResMng->brDarkGray.Get());
	
	pt.y += 45;
	str = L"Kit: ";
	str += std::to_wstring(m_iTurretKit);
	pd2dRenderTarget->DrawText(
		str.c_str()
		, static_cast<UINT>(str.length())
		, m_pResMng->dwUITextFormat.Get()
		, RectF(pt.x, pt.y, pt.x + 300, pt.y + 50)
		+ m_ptPos
		, m_pResMng->brDarkGray.Get());
}

void CPlayer::RegisterResourceManager(shared_ptr<CResourceManager> resMng)
{
	m_pResMng = resMng;
	m_bmpImage = m_pResMng->GetImageRef(ResImgName::character_sheet);
	m_szImg = m_pResMng->GetImgLength(ResImgName::character_sheet);

	m_ptCurrImg.x = m_Id % m_szImg.width;

	if (IsRectInvalid(m_rcSize))
	{
		auto sz = m_bmpImage->GetSize();
		sz.width /= m_szImg.width;
		sz.height /= m_szImg.height;
		m_rcSize = SizeToRect(sz);
	}
	
	m_bmpWeaponImage = m_pResMng->GetImage(ResImgName::gun);
	if (IsRectInvalid(m_rcWeaponSize))
		m_rcWeaponSize = SizeToRect(m_bmpWeaponImage->GetSize());
}

void CPlayer::Collide(float atk)
{
	if (m_bCollision) return;
	m_bCollision = true;
	m_fHP -= atk;
	if (m_fHP < 0)
		m_fHP = 0;
}

void CPlayer::SetObjectInfo(LPVOID info)
{
	ObjInfo* objinfo = static_cast<ObjInfo*>(info);
	float damage = m_fHP - objinfo->HP;

	m_iGrenade = objinfo->Ammo.Granade;
	m_iAmmo = objinfo->Ammo.GunAmmo;
	m_iTurretKit = objinfo->Ammo.TurretKit;
	m_ptDirection = objinfo->Direction;
	m_ptPos = objinfo->Position;

	if (damage > 0) Collide(damage);
	m_fHP = objinfo->HP;

	float angle = -acosf(m_ptDirection*Point2F(1, 0));
	if (m_ptDirection.y > 0)
		angle = -angle;
	m_mtxRotate = Matrix3x2F::Rotation(
		RADIAN_TO_DEGREE(angle), m_ptPos);
}

void CPlayer::Move(const D2D_POINT_2F& ptVelocity)
{
	m_ptVelocity += ptVelocity;
	float len = Length(m_ptVelocity);
	if (len > PLAYER_MAX_VELOCITY)
		m_ptVelocity -= Normalize(m_ptVelocity) * (len - PLAYER_MAX_VELOCITY);
}

void CPlayer::Move(CClient* pClient, const D2D_POINT_2F& ptMoveDirection)
{
	UCHAR type = MSGTYPE::MSGACTION::MOVE;
	UCHAR roomNo = pClient->GetRoomID();
	UINT objNo = GetID();
	ActionInfo action_info;
	action_info.LookDirection = m_ptDirection;
	action_info.MoveDirection = ptMoveDirection;

	NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	pClient->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;
}

void CPlayer::Reflection(const D2D_POINT_2F& ptDirReflect)
{
	if(ptDirReflect == Point2F())
		m_ptVelocity = m_ptVelocity * -REFLACTION_FACTOR;
	else
		m_ptVelocity = ptDirReflect * Length(m_ptVelocity * REFLACTION_FACTOR);
}

void CPlayer::Stop()
{
	m_ptVelocity = Point2F();
}

void CPlayer::Stop(CClient * pClient)
{
	UCHAR type = MSGTYPE::MSGACTION::STOP;
	UCHAR roomNo = pClient->GetRoomID();
	UINT objNo = GetID();

	NGPMSG* msg = CreateMSG(type, roomNo, objNo, 0, 0, NULL, NULL);
	pClient->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;
}

CEffect* CPlayer::Shoot()
{
	if (m_bShoot) return nullptr;
	if (m_bReload) return nullptr;

	if (m_iAmmo == 0)
	{
		m_bReload = true;
		return nullptr;
	}
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
			player->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Agent:
		{
			CAgent* agent = static_cast<CAgent*>(m_pTarget);
			agent->Collide(SHOOT_DAMAGE);
			agent->Move(m_ptMuzzleDirection * PLAYER_VELOCITY);
			break;
		}
		case CObject::Type::Brick:
		{
			CBrick* brick = static_cast<CBrick*>(m_pTarget);
			brick->Collide(SHOOT_DAMAGE);
			break;
		}
		}
		auto& rc = SizeToRect(SizeF(m_rcSize.right, m_rcSize.bottom));
		CEffect* effect = new CEffect(m_ptMuzzleEndPos, rc);
		auto& img = m_pResMng->GetImageRef(ResImgName::MagicBlast);
		auto& sz = m_pResMng->GetImgLength(ResImgName::MagicBlast);
		effect->RegisterEffectSprite(img, sz);
		return effect;
	}
	return nullptr;
}

void CPlayer::Shoot(const D2D_POINT_2F & ptHitPos)
{
	if (m_bShoot) return;
	if (m_bReload) return;

	if (m_iAmmo == 0)
	{
		m_bReload = true;
		return;
	}
	else --m_iAmmo;

	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;
	m_ptMuzzleEndPos = ptHitPos;
}

void CPlayer::Shoot(CClient * pClient)
{
	if (m_bShoot) return;
	if (m_bReload) return;

	NGPMSG* msg = nullptr;
	if (m_iAmmo == 0)
	{
		m_bReload = true;
		UCHAR type = MSGTYPE::MSGACTION::RELOAD;
		UCHAR roomNo = pClient->GetRoomID();
		UINT objNo = GetID();

		msg = CreateMSG(type, roomNo, objNo, 0, 0, NULL, NULL);
		pClient->SendMsgs((char*)msg, sizeof(NGPMSG));
		delete msg;
		return;
	}
	else --m_iAmmo;

	m_bShoot = true;
	m_ptMuzzleDirection = m_ptDirection;
	m_ptMuzzleStartPos = m_ptPos + m_ptMuzzleDirection * MUZZLE_OFFSET;

	UCHAR type = MSGTYPE::MSGACTION::SHOOT;
	UCHAR roomNo = pClient->GetRoomID();
	UINT objNo = GetID();
	ActionInfo action_info;
	action_info.TargetHitPos = m_ptMuzzleEndPos;

	if (m_pTarget)	action_info.TargetID = m_pTarget->GetID();
	else			action_info.TargetID = INVALID_OBJECT_ID;

	msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	pClient->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;
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
				if (p == this) break;
				if (Length(p->GetPos() - ptDevidedRay) < p->GetSize().right)
				{
					m_ptMuzzleEndPos = ptDevidedRay;
					m_pTarget = p;
					return;
				}
				break;
			}
			case CObject::Type::Agent:
			{
				if (p == this) break;
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

CObject * CPlayer::GrenadeOut()
{
	if (m_bGrenade) return nullptr;
	if (m_iGrenade == 0) return nullptr;
	--m_iGrenade;
	m_bGrenade = true;

	CGrenade* grenade = new CGrenade(m_ptPos);
	grenade->RegisterResourceManager(m_pResMng);
	grenade->SetParent(this);
	grenade->SetVelocity(m_ptDirection 
		* Length(m_ptTargetPos - m_ptPos) 
		* PROJECTILE_FRICTIONAL_DRAG);
	return grenade;
}

void CPlayer::GrenadeOut(CClient * pClient)
{
	if (m_bGrenade) return;
	if (m_iGrenade == 0) return;
	--m_iGrenade;
	m_bGrenade = true;

	NGPMSG* msg = nullptr;
	UCHAR type = MSGTYPE::MSGACTION::GRENADE;
	UCHAR roomNo = pClient->GetRoomID();
	UINT objNo = GetID();
	ActionInfo action_info;
	action_info.TargetPos = m_ptPos;
	action_info.SetVelocity = m_ptDirection
		* Length(m_ptTargetPos - m_ptPos)
		* PROJECTILE_FRICTIONAL_DRAG;

	msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	pClient->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;
}

void CPlayer::DeployTurret(CClient * pClient)
{
	if (m_bTurret) return;
	if (m_iTurretKit == 0) return;
	--m_iTurretKit;
	m_bTurret = true;

	NGPMSG* msg = nullptr;
	UCHAR type = MSGTYPE::MSGACTION::BUILDTURRET;
	UCHAR roomNo = pClient->GetRoomID();
	UINT objNo = GetID();
	ActionInfo action_info;
	action_info.TargetPos = m_ptPos;
	action_info.SetVelocity = m_ptDirection
		* Length(m_ptTargetPos - m_ptPos)
		* PROJECTILE_FRICTIONAL_DRAG;

	msg = CreateMSG(type, roomNo, objNo, 0, 1, NULL, &action_info);
	pClient->SendMsgs((char*)msg, sizeof(NGPMSG));
	delete msg;
}
