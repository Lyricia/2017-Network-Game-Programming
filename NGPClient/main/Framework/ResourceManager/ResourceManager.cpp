#include "stdafx.h"
#include "Framework\IndRes\IndRes.h"
#include "ResourceManager.h"

CResourceManager::CResourceManager(
	  CIndRes*					pIndRes
	, ID2D1HwndRenderTarget*	pd2dRenderTarget)
{
	std::string file_path = "../../Resource/Sprite/";
	std::string file_format = ".png";
	int count = GetIdxFromEnum(ImgName::Count);
	for (int i = 0; i < count; ++i)
	{
		LoadImageFromFile(
			pIndRes->wicFactory()
			, pd2dRenderTarget
			, path(file_path + GetImgNameString(GetImgNameFromIdx(i)) + file_format).c_str()
			, &m_bmpImage[i] );
	}

	pIndRes->dwFactory()->CreateTextFormat(
		L"Arial"
		, nullptr
		, DWRITE_FONT_WEIGHT_BOLD
		, DWRITE_FONT_STYLE_NORMAL
		, DWRITE_FONT_STRETCH_NORMAL
		, 40.f
		, L"ko-kr"
		, &dwUITextFormat);

	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::White }, &brWhite);
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::LightYellow }, &brLightYellow);
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::DarkGray }, &brDarkGray);
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::Red }, &brRed);
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::Blue }, &brBlue);
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::Green }, &brGreen);
	pd2dRenderTarget->CreateSolidColorBrush(ColorF{ ColorF::GreenYellow }, &brGreenYellow);
}
CResourceManager::~CResourceManager()
{
}

CResourceManager::ImgName CResourceManager::GetImgNameFromIdx(UINT idx)
{
	return static_cast<ImgName>(idx);
}

ID2D1Bitmap1* CResourceManager::GetImage(ImgName name)
{
	return m_bmpImage[GetIdxFromEnum(name)].Get();
}

const ComPtr<ID2D1Bitmap1>& CResourceManager::GetImageRef(ImgName name)
{
	return m_bmpImage[GetIdxFromEnum(name)];
}

std::string CResourceManager::GetImgNameString(ImgName name)
{
	switch (name)
	{
	case ImgName::aim:					return std::string("aim");
	case ImgName::aim_big:				return std::string("aim_big");
	case ImgName::aim_trans:			return std::string("aim_trans");
	case ImgName::brick_Sheet:			return std::string("brick_Sheet");
	case ImgName::character_sheet:		return std::string("character_sheet");
	case ImgName::Explosion256:			return std::string("Explosion256");
	case ImgName::grenade:				return std::string("grenade");
	case ImgName::gun:					return std::string("gun");
	case ImgName::MagicBlast:			return std::string("MagicBlast");
	case ImgName::turret_barrel:		return std::string("turret_barrel");
	case ImgName::turret_base_sheet:	return std::string("turret_base_sheet");
	case ImgName::map_image:			return std::string("map_image");
	default:
		return std::string();
	}
	return std::string();
}

D2D_SIZE_U CResourceManager::GetImgLength(ImgName name)
{
	switch (name)
	{
	case ImgName::aim:					return SizeU(1, 1);
	case ImgName::aim_big:				return SizeU(1, 1);
	case ImgName::aim_trans:			return SizeU(1, 1);
	case ImgName::brick_Sheet:			return SizeU(5, 1);
	case ImgName::character_sheet:		return SizeU(20, 1);
	case ImgName::Explosion256:			return SizeU(8, 3);
	case ImgName::grenade:				return SizeU(26, 1);
	case ImgName::gun:					return SizeU(1, 1);
	case ImgName::MagicBlast:			return SizeU(16, 1);
	case ImgName::turret_barrel:		return SizeU(1, 1);
	case ImgName::turret_base_sheet:	return SizeU(20, 1);
	case ImgName::map_image:			return SizeU(1, 1);
	default:
		return D2D_SIZE_U();
	}
	return D2D_SIZE_U();
}
