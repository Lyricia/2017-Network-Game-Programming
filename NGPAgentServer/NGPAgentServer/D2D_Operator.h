#pragma once


inline D2D_POINT_2F operator+(const D2D_POINT_2F& a, const D2D_POINT_2F& b) { return { a.x + b.x, a.y + b.y }; }
inline D2D_POINT_2F operator-(const D2D_POINT_2F& a, const D2D_POINT_2F& b) { return { a.x - b.x, a.y - b.y }; }
inline D2D_POINT_2F operator*(float b, const D2D_POINT_2F& a) { return { static_cast<FLOAT>(a.x *b), static_cast<FLOAT>(a.y *b) }; }
inline D2D_POINT_2F operator*(const D2D_POINT_2F& a, float b) { return { static_cast<FLOAT>(a.x *b), static_cast<FLOAT>(a.y *b) }; }
inline void operator+=(D2D_POINT_2F& a, const D2D_POINT_2F& b) { a.x += b.x; a.y += b.y; }
inline void operator-=(D2D_POINT_2F& a, const D2D_POINT_2F& b) { a.x -= b.x; a.y -= b.y; }
inline bool operator==(const D2D_POINT_2F& p1, const D2D_POINT_2F& p2) { return (p1.x == p2.x && p1.y == p2.y) ? true : false; }
inline bool operator!=(const D2D_POINT_2F& p1, const D2D_POINT_2F& p2) { return (p1.x == p2.x && p1.y == p2.y) ? false : true; }
inline float operator*(const D2D_POINT_2F& a, const D2D_POINT_2F& b) { return { a.x * b.x + a.y * b.y }; }

inline D2D_RECT_F operator+(D2D_RECT_F rc, D2D_POINT_2F pt) { return { pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom }; }
inline D2D_RECT_F operator+(D2D_POINT_2F pt, D2D_RECT_F rc) { return { pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom }; }
inline D2D_RECT_F operator+(D2D_RECT_F rc_1, D2D_RECT_F rc_2) { return { rc_1.left + rc_2.left, rc_1.top + rc_2.top, rc_1.right + rc_2.right, rc_1.bottom + rc_2.bottom }; }

inline D2D_POINT_2F operator+(const D2D_POINT_2F& a, const POINT& b) { return Point2F(a.x + b.x, a.y + b.y); }
inline D2D_POINT_2F operator-(const D2D_POINT_2F& a, const POINT& b) { return Point2F(a.x - b.x, a.y - b.y); }

inline POINT operator+(POINT a, const POINT b) { return Point2L(a.x + b.x, a.y + b.y); }
inline POINT operator-(POINT a, const POINT b) { return Point2L(a.x - b.x, a.y - b.y); }
inline POINT operator*(float b, POINT a) { return Point2L(a.x *b, a.y *b); }
inline bool operator==(POINT p1, POINT p2) { return (p1.x == p2.x && p1.y == p2.y) ? true : false; }
inline bool operator!=(POINT p1, POINT p2) { return (p1.x == p2.x && p1.y == p2.y) ? false : true; }

inline RECT operator+(RECT rc, POINT pt) { return RectL(pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom); }
inline RECT operator+(POINT pt, RECT rc) { return RectL(pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom); }
inline RECT operator+(RECT rc_1, RECT rc_2) { return RectL(rc_1.left + rc_2.left, rc_1.top + rc_2.top, rc_1.right + rc_2.right, rc_1.bottom + rc_2.bottom); }
inline RECT operator-(RECT rc_1, RECT rc_2) { return RectL(rc_1.left - rc_2.left, rc_1.top - rc_2.top, rc_1.right - rc_2.right, rc_1.bottom - rc_2.bottom); }

inline D2D1_RECT_F MergeRect(D2D_RECT_F rc1, D2D_RECT_F rc2)
{
	D2D1_RECT_F rc;
	rc.left = min(min(rc1.left, rc1.right), min(rc2.left, rc2.right));
	rc.top = min(min(rc1.top, rc1.bottom), min(rc2.top, rc2.bottom));
	rc.right = max(max(rc1.left, rc1.right), max(rc2.left, rc2.right));
	rc.bottom = max(max(rc1.top, rc1.bottom), max(rc2.top, rc2.bottom));
	return rc;
}

template<typename Rect>
inline RECT D2DRectToWin32Rect(Rect&& rc)
{
	return
	{
		static_cast<LONG>(rc.left)
		, static_cast<LONG>(rc.top)
		, static_cast<LONG>(rc.right)
		, static_cast<LONG>(rc.bottom)
	};
}

template<typename Rect>
inline RECT D2DRectToWin32Rect(const Rect& rc)
{
	return
	{
		static_cast<LONG>(rc.left)
		, static_cast<LONG>(rc.top)
		, static_cast<LONG>(rc.right)
		, static_cast<LONG>(rc.bottom)
	};
}

inline bool PtInRect(const D2D_RECT_F* rc, const D2D_POINT_2F pt)
{
	RECT win32rc = D2DRectToWin32Rect(*rc);
	POINT win32pt = { static_cast<LONG>(pt.x), static_cast<LONG>(pt.y) };
	return static_cast<bool>(::PtInRect(&win32rc, win32pt) == TRUE);
}

inline bool PtInRect(const D2D_RECT_F* rc, const POINT pt)
{
	RECT win32rc = D2DRectToWin32Rect(*rc);
	return static_cast<bool>(::PtInRect(&win32rc, pt) == TRUE);
}

inline constexpr bool IsRectInvalid(const D2D_RECT_F& rc)
{
	return (rc.left == 0.f
		&&	rc.right == 0.f
		&&	rc.top == 0.f
		&&	rc.bottom == 0.f);
}

inline D2D_RECT_F SizeToRect(const D2D1_SIZE_F& sz)
{
	return RectF(
		-sz.width * 0.5f
		, -sz.height * 0.5f
		, sz.width * 0.5f
		, sz.height * 0.5f);
}

inline float Length(const D2D_POINT_2F& pt) { return sqrtf(pt.x*pt.x + pt.y*pt.y); }
inline D2D_POINT_2F Normalize(const D2D_POINT_2F& pt)
{
	float revlen = 1.f / Length(pt);
	return Point2F(pt.x * revlen, pt.y * revlen);
}

#define DEGREE_TO_RADIAN(x) ((x) * 0.0174532925199433)
#define RADIAN_TO_DEGREE(x) ((x) * 57.29577951308233)