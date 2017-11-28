#pragma once

class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

private:
	ComPtr<ID2D1Bitmap1>		m_imgAim;
	ComPtr<ID2D1Bitmap1>		m_imgAim_big;
	ComPtr<ID2D1Bitmap1>		m_imgAim_trans;
	ComPtr<ID2D1Bitmap1>		m_imgBrick_Sheet;
	ComPtr<ID2D1Bitmap1>		m_imgCharacter_sheet;
	ComPtr<ID2D1Bitmap1>		m_imgExplosion256;
	ComPtr<ID2D1Bitmap1>		m_imgGrenade;
	ComPtr<ID2D1Bitmap1>		m_imgGun;
	ComPtr<ID2D1Bitmap1>		m_imgMagicBlast;
	ComPtr<ID2D1Bitmap1>		m_imgTurret_barrel;
	ComPtr<ID2D1Bitmap1>		m_imgTurret_base_sheet;
};