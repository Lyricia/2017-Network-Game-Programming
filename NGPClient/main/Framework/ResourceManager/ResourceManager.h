#pragma once

template <class ENUM>
constexpr UINT GetIdxFromEnum(ENUM e) { return static_cast<UINT>(e); }

class CIndRes;
class CResourceManager
{
public:
	enum class ImgName {
		  aim = 0
		, aim_big
		, aim_trans
		, brick_Sheet
		, character_sheet
		, Explosion256
		, grenade
		, gun
		, MagicBlast
		, turret_barrel
		, turret_base_sheet
		, Count
	};

public:
	CResourceManager(
		  CIndRes*					pIndRes
		, ID2D1HwndRenderTarget*	pd2dRenderTarget);
	~CResourceManager();

	ImgName						GetImgNameFromIdx(UINT idx);
	ID2D1Bitmap1*				GetImage(ImgName name);
	const ComPtr<ID2D1Bitmap1>&	GetImageRef(ImgName name);
	std::string					GetImgNameString(ImgName name);
	D2D_SIZE_U					GetImgLength(ImgName name);

private:
	ComPtr<ID2D1Bitmap1>			m_bmpImage[GetIdxFromEnum(ImgName::Count)];

public:
	ComPtr<IDWriteTextFormat>		dwUITextFormat;
	ComPtr<ID2D1SolidColorBrush>	brWhite;
	ComPtr<ID2D1SolidColorBrush>	brRed;
	ComPtr<ID2D1SolidColorBrush>	brBlue;
	ComPtr<ID2D1SolidColorBrush>	brDarkGray;
	ComPtr<ID2D1SolidColorBrush>	brLightYellow;
	ComPtr<ID2D1SolidColorBrush>	brGreen;
	ComPtr<ID2D1SolidColorBrush>	brGreenYellow;
};

using ResImgName = CResourceManager::ImgName;