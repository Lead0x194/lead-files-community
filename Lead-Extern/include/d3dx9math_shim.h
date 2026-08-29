//
// d3dx9math_shim.h
//
// Drop-in replacement for the deprecated <dx9/d3dx9math.h>, backed by DirectXMath
// (Windows SDK, header-only). Defining __D3DX9MATH_H__ suppresses the vendored
// D3DX math header so the rest of d3dx9 (textures/fonts/mesh) keeps working while
// the math no longer depends on d3dx9.lib. Types derive from the D3D9 POD bases
// (D3DVECTOR/D3DMATRIX/D3DCOLORVALUE) so they stay binary-compatible with the
// remaining d3dx9 surface. Function semantics mirror D3DX exactly.
//
#pragma once

#include <dx9/d3d9.h>
#include <DirectXMath.h>
#include <cmath>

#ifndef __D3DX9MATH_H__
#define __D3DX9MATH_H__

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
#ifndef D3DX_PI
#define D3DX_PI    ((FLOAT)  3.141592654f)
#endif
#ifndef D3DX_1BYPI
#define D3DX_1BYPI ((FLOAT)  0.318309886f)
#endif

#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

//----------------------------------------------------------------------------
// 2D Vector
//----------------------------------------------------------------------------
typedef struct D3DXVECTOR2
{
	FLOAT x, y;

	D3DXVECTOR2() {}
	D3DXVECTOR2(CONST FLOAT* pf) : x(pf[0]), y(pf[1]) {}
	D3DXVECTOR2(FLOAT fx, FLOAT fy) : x(fx), y(fy) {}

	operator FLOAT* () { return &x; }
	operator CONST FLOAT* () const { return &x; }

	D3DXVECTOR2& operator += (CONST D3DXVECTOR2& v) { x += v.x; y += v.y; return *this; }
	D3DXVECTOR2& operator -= (CONST D3DXVECTOR2& v) { x -= v.x; y -= v.y; return *this; }
	D3DXVECTOR2& operator *= (FLOAT f) { x *= f; y *= f; return *this; }
	D3DXVECTOR2& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; x *= inv; y *= inv; return *this; }

	D3DXVECTOR2 operator + () const { return *this; }
	D3DXVECTOR2 operator - () const { return D3DXVECTOR2(-x, -y); }

	D3DXVECTOR2 operator + (CONST D3DXVECTOR2& v) const { return D3DXVECTOR2(x + v.x, y + v.y); }
	D3DXVECTOR2 operator - (CONST D3DXVECTOR2& v) const { return D3DXVECTOR2(x - v.x, y - v.y); }
	D3DXVECTOR2 operator * (FLOAT f) const { return D3DXVECTOR2(x * f, y * f); }
	D3DXVECTOR2 operator / (FLOAT f) const { FLOAT inv = 1.0f / f; return D3DXVECTOR2(x * inv, y * inv); }

	friend D3DXVECTOR2 operator * (FLOAT f, CONST D3DXVECTOR2& v) { return D3DXVECTOR2(f * v.x, f * v.y); }

	BOOL operator == (CONST D3DXVECTOR2& v) const { return x == v.x && y == v.y; }
	BOOL operator != (CONST D3DXVECTOR2& v) const { return x != v.x || y != v.y; }
} D3DXVECTOR2, *LPD3DXVECTOR2;

//----------------------------------------------------------------------------
// 3D Vector (derives from D3DVECTOR for layout compatibility)
//----------------------------------------------------------------------------
typedef struct D3DXVECTOR3 : public D3DVECTOR
{
	D3DXVECTOR3() {}
	D3DXVECTOR3(CONST FLOAT* pf) { x = pf[0]; y = pf[1]; z = pf[2]; }
	D3DXVECTOR3(CONST D3DVECTOR& v) { x = v.x; y = v.y; z = v.z; }
	D3DXVECTOR3(FLOAT fx, FLOAT fy, FLOAT fz) { x = fx; y = fy; z = fz; }

	operator FLOAT* () { return &x; }
	operator CONST FLOAT* () const { return &x; }

	D3DXVECTOR3& operator += (CONST D3DXVECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	D3DXVECTOR3& operator -= (CONST D3DXVECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	D3DXVECTOR3& operator *= (FLOAT f) { x *= f; y *= f; z *= f; return *this; }
	D3DXVECTOR3& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; x *= inv; y *= inv; z *= inv; return *this; }

	D3DXVECTOR3 operator + () const { return *this; }
	D3DXVECTOR3 operator - () const { return D3DXVECTOR3(-x, -y, -z); }

	D3DXVECTOR3 operator + (CONST D3DXVECTOR3& v) const { return D3DXVECTOR3(x + v.x, y + v.y, z + v.z); }
	D3DXVECTOR3 operator - (CONST D3DXVECTOR3& v) const { return D3DXVECTOR3(x - v.x, y - v.y, z - v.z); }
	D3DXVECTOR3 operator * (FLOAT f) const { return D3DXVECTOR3(x * f, y * f, z * f); }
	D3DXVECTOR3 operator / (FLOAT f) const { FLOAT inv = 1.0f / f; return D3DXVECTOR3(x * inv, y * inv, z * inv); }

	friend D3DXVECTOR3 operator * (FLOAT f, CONST struct D3DXVECTOR3& v) { return D3DXVECTOR3(f * v.x, f * v.y, f * v.z); }

	BOOL operator == (CONST D3DXVECTOR3& v) const { return x == v.x && y == v.y && z == v.z; }
	BOOL operator != (CONST D3DXVECTOR3& v) const { return x != v.x || y != v.y || z != v.z; }
} D3DXVECTOR3, *LPD3DXVECTOR3;

//----------------------------------------------------------------------------
// 4D Vector
//----------------------------------------------------------------------------
typedef struct D3DXVECTOR4
{
	FLOAT x, y, z, w;

	D3DXVECTOR4() {}
	D3DXVECTOR4(CONST FLOAT* pf) : x(pf[0]), y(pf[1]), z(pf[2]), w(pf[3]) {}
	D3DXVECTOR4(CONST D3DVECTOR& v, FLOAT fw) : x(v.x), y(v.y), z(v.z), w(fw) {}
	D3DXVECTOR4(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw) : x(fx), y(fy), z(fz), w(fw) {}

	operator FLOAT* () { return &x; }
	operator CONST FLOAT* () const { return &x; }

	D3DXVECTOR4& operator += (CONST D3DXVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	D3DXVECTOR4& operator -= (CONST D3DXVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	D3DXVECTOR4& operator *= (FLOAT f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	D3DXVECTOR4& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }

	D3DXVECTOR4 operator + () const { return *this; }
	D3DXVECTOR4 operator - () const { return D3DXVECTOR4(-x, -y, -z, -w); }

	D3DXVECTOR4 operator + (CONST D3DXVECTOR4& v) const { return D3DXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
	D3DXVECTOR4 operator - (CONST D3DXVECTOR4& v) const { return D3DXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
	D3DXVECTOR4 operator * (FLOAT f) const { return D3DXVECTOR4(x * f, y * f, z * f, w * f); }
	D3DXVECTOR4 operator / (FLOAT f) const { FLOAT inv = 1.0f / f; return D3DXVECTOR4(x * inv, y * inv, z * inv, w * inv); }

	friend D3DXVECTOR4 operator * (FLOAT f, CONST D3DXVECTOR4& v) { return D3DXVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w); }

	BOOL operator == (CONST D3DXVECTOR4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
	BOOL operator != (CONST D3DXVECTOR4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }
} D3DXVECTOR4, *LPD3DXVECTOR4;

//----------------------------------------------------------------------------
// Matrix (derives from D3DMATRIX, row-major, same as D3DX)
//----------------------------------------------------------------------------
typedef struct D3DXMATRIX : public D3DMATRIX
{
	D3DXMATRIX() {}
	D3DXMATRIX(CONST FLOAT* pf) { memcpy(&_11, pf, sizeof(D3DMATRIX)); }
	D3DXMATRIX(CONST D3DMATRIX& m) { memcpy(&_11, &m, sizeof(D3DMATRIX)); }
	D3DXMATRIX(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
	           FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
	           FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
	           FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44)
	{
		_11 = f11; _12 = f12; _13 = f13; _14 = f14;
		_21 = f21; _22 = f22; _23 = f23; _24 = f24;
		_31 = f31; _32 = f32; _33 = f33; _34 = f34;
		_41 = f41; _42 = f42; _43 = f43; _44 = f44;
	}

	FLOAT& operator () (UINT r, UINT c) { return m[r][c]; }
	FLOAT  operator () (UINT r, UINT c) const { return m[r][c]; }

	operator FLOAT* () { return &_11; }
	operator CONST FLOAT* () const { return &_11; }

	D3DXMATRIX& operator *= (CONST D3DXMATRIX& mat);
	D3DXMATRIX& operator += (CONST D3DXMATRIX& mat) { for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] += mat.m[i][j]; return *this; }
	D3DXMATRIX& operator -= (CONST D3DXMATRIX& mat) { for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] -= mat.m[i][j]; return *this; }
	D3DXMATRIX& operator *= (FLOAT f) { for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] *= f; return *this; }
	D3DXMATRIX& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) m[i][j] *= inv; return *this; }

	D3DXMATRIX operator + () const { return *this; }
	D3DXMATRIX operator - () const { D3DXMATRIX r; for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) r.m[i][j] = -m[i][j]; return r; }

	D3DXMATRIX operator * (CONST D3DXMATRIX& mat) const;
	D3DXMATRIX operator + (CONST D3DXMATRIX& mat) const { D3DXMATRIX r(*this); r += mat; return r; }
	D3DXMATRIX operator - (CONST D3DXMATRIX& mat) const { D3DXMATRIX r(*this); r -= mat; return r; }
	D3DXMATRIX operator * (FLOAT f) const { D3DXMATRIX r(*this); r *= f; return r; }
	D3DXMATRIX operator / (FLOAT f) const { D3DXMATRIX r(*this); r /= f; return r; }

	friend D3DXMATRIX operator * (FLOAT f, CONST D3DXMATRIX& mat) { return mat * f; }

	BOOL operator == (CONST D3DXMATRIX& mat) const { return 0 == memcmp(this, &mat, sizeof(D3DMATRIX)); }
	BOOL operator != (CONST D3DXMATRIX& mat) const { return 0 != memcmp(this, &mat, sizeof(D3DMATRIX)); }
} D3DXMATRIX, *LPD3DXMATRIX;

#if (_MSC_VER >= 1300)
__declspec(align(16))
#endif
typedef struct D3DXMATRIXA16 : public D3DXMATRIX
{
	D3DXMATRIXA16() {}
	D3DXMATRIXA16(CONST FLOAT* pf) : D3DXMATRIX(pf) {}
	D3DXMATRIXA16(CONST D3DMATRIX& m) : D3DXMATRIX(m) {}
	D3DXMATRIXA16(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
	              FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
	              FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
	              FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44)
		: D3DXMATRIX(f11, f12, f13, f14, f21, f22, f23, f24,
		             f31, f32, f33, f34, f41, f42, f43, f44) {}
} D3DXMATRIXA16, *LPD3DXMATRIXA16;

//----------------------------------------------------------------------------
// Quaternion
//----------------------------------------------------------------------------
typedef struct D3DXQUATERNION
{
	FLOAT x, y, z, w;

	D3DXQUATERNION() {}
	D3DXQUATERNION(CONST FLOAT* pf) : x(pf[0]), y(pf[1]), z(pf[2]), w(pf[3]) {}
	D3DXQUATERNION(FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw) : x(fx), y(fy), z(fz), w(fw) {}

	operator FLOAT* () { return &x; }
	operator CONST FLOAT* () const { return &x; }

	D3DXQUATERNION& operator += (CONST D3DXQUATERNION& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
	D3DXQUATERNION& operator -= (CONST D3DXQUATERNION& q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
	D3DXQUATERNION& operator *= (CONST D3DXQUATERNION& q);
	D3DXQUATERNION& operator *= (FLOAT f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	D3DXQUATERNION& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }

	D3DXQUATERNION operator + () const { return *this; }
	D3DXQUATERNION operator - () const { return D3DXQUATERNION(-x, -y, -z, -w); }

	D3DXQUATERNION operator + (CONST D3DXQUATERNION& q) const { return D3DXQUATERNION(x + q.x, y + q.y, z + q.z, w + q.w); }
	D3DXQUATERNION operator - (CONST D3DXQUATERNION& q) const { return D3DXQUATERNION(x - q.x, y - q.y, z - q.z, w - q.w); }
	D3DXQUATERNION operator * (CONST D3DXQUATERNION& q) const;
	D3DXQUATERNION operator * (FLOAT f) const { return D3DXQUATERNION(x * f, y * f, z * f, w * f); }
	D3DXQUATERNION operator / (FLOAT f) const { FLOAT inv = 1.0f / f; return D3DXQUATERNION(x * inv, y * inv, z * inv, w * inv); }

	friend D3DXQUATERNION operator * (FLOAT f, CONST D3DXQUATERNION& q) { return D3DXQUATERNION(f * q.x, f * q.y, f * q.z, f * q.w); }

	BOOL operator == (CONST D3DXQUATERNION& q) const { return x == q.x && y == q.y && z == q.z && w == q.w; }
	BOOL operator != (CONST D3DXQUATERNION& q) const { return x != q.x || y != q.y || z != q.z || w != q.w; }
} D3DXQUATERNION, *LPD3DXQUATERNION;

//----------------------------------------------------------------------------
// Plane
//----------------------------------------------------------------------------
typedef struct D3DXPLANE
{
	FLOAT a, b, c, d;

	D3DXPLANE() {}
	D3DXPLANE(CONST FLOAT* pf) : a(pf[0]), b(pf[1]), c(pf[2]), d(pf[3]) {}
	D3DXPLANE(FLOAT fa, FLOAT fb, FLOAT fc, FLOAT fd) : a(fa), b(fb), c(fc), d(fd) {}

	operator FLOAT* () { return &a; }
	operator CONST FLOAT* () const { return &a; }

	D3DXPLANE& operator *= (FLOAT f) { a *= f; b *= f; c *= f; d *= f; return *this; }
	D3DXPLANE& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; a *= inv; b *= inv; c *= inv; d *= inv; return *this; }

	D3DXPLANE operator + () const { return *this; }
	D3DXPLANE operator - () const { return D3DXPLANE(-a, -b, -c, -d); }

	D3DXPLANE operator * (FLOAT f) const { return D3DXPLANE(a * f, b * f, c * f, d * f); }
	D3DXPLANE operator / (FLOAT f) const { FLOAT inv = 1.0f / f; return D3DXPLANE(a * inv, b * inv, c * inv, d * inv); }

	friend D3DXPLANE operator * (FLOAT f, CONST D3DXPLANE& p) { return D3DXPLANE(f * p.a, f * p.b, f * p.c, f * p.d); }

	BOOL operator == (CONST D3DXPLANE& p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }
	BOOL operator != (CONST D3DXPLANE& p) const { return a != p.a || b != p.b || c != p.c || d != p.d; }
} D3DXPLANE, *LPD3DXPLANE;

//----------------------------------------------------------------------------
// Color
//----------------------------------------------------------------------------
typedef struct D3DXCOLOR
{
	FLOAT r, g, b, a;

	D3DXCOLOR() {}
	D3DXCOLOR(CONST FLOAT* pf) : r(pf[0]), g(pf[1]), b(pf[2]), a(pf[3]) {}
	D3DXCOLOR(CONST D3DCOLORVALUE& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
	D3DXCOLOR(FLOAT fr, FLOAT fg, FLOAT fb, FLOAT fa) : r(fr), g(fg), b(fb), a(fa) {}
	D3DXCOLOR(DWORD argb)
	{
		CONST FLOAT f = 1.0f / 255.0f;
		a = f * (FLOAT)(unsigned char)(argb >> 24);
		r = f * (FLOAT)(unsigned char)(argb >> 16);
		g = f * (FLOAT)(unsigned char)(argb >> 8);
		b = f * (FLOAT)(unsigned char)(argb >> 0);
	}

	operator DWORD () const
	{
		auto clamp = [](FLOAT v) -> DWORD { return v <= 0.0f ? 0u : (v >= 1.0f ? 255u : (DWORD)(v * 255.0f + 0.5f)); };
		return (clamp(a) << 24) | (clamp(r) << 16) | (clamp(g) << 8) | clamp(b);
	}

	operator FLOAT* () { return &r; }
	operator CONST FLOAT* () const { return &r; }
	operator D3DCOLORVALUE* () { return (D3DCOLORVALUE*)&r; }
	operator CONST D3DCOLORVALUE* () const { return (CONST D3DCOLORVALUE*)&r; }
	operator D3DCOLORVALUE& () { return *((D3DCOLORVALUE*)&r); }
	operator CONST D3DCOLORVALUE& () const { return *((CONST D3DCOLORVALUE*)&r); }

	D3DXCOLOR& operator += (CONST D3DXCOLOR& c) { r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
	D3DXCOLOR& operator -= (CONST D3DXCOLOR& c) { r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
	D3DXCOLOR& operator *= (FLOAT f) { r *= f; g *= f; b *= f; a *= f; return *this; }
	D3DXCOLOR& operator /= (FLOAT f) { FLOAT inv = 1.0f / f; r *= inv; g *= inv; b *= inv; a *= inv; return *this; }

	D3DXCOLOR operator + () const { return *this; }
	D3DXCOLOR operator - () const { return D3DXCOLOR(-r, -g, -b, -a); }

	D3DXCOLOR operator + (CONST D3DXCOLOR& c) const { return D3DXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a); }
	D3DXCOLOR operator - (CONST D3DXCOLOR& c) const { return D3DXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a); }
	D3DXCOLOR operator * (FLOAT f) const { return D3DXCOLOR(r * f, g * f, b * f, a * f); }
	D3DXCOLOR operator / (FLOAT f) const { FLOAT inv = 1.0f / f; return D3DXCOLOR(r * inv, g * inv, b * inv, a * inv); }

	friend D3DXCOLOR operator * (FLOAT f, CONST D3DXCOLOR& c) { return D3DXCOLOR(f * c.r, f * c.g, f * c.b, f * c.a); }

	BOOL operator == (CONST D3DXCOLOR& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
	BOOL operator != (CONST D3DXCOLOR& c) const { return r != c.r || g != c.g || b != c.b || a != c.a; }
} D3DXCOLOR, *LPD3DXCOLOR;

//----------------------------------------------------------------------------
// Internal helpers (DirectXMath bridge)
//----------------------------------------------------------------------------
namespace d3dx9math_shim_detail
{
	inline DirectX::XMVECTOR LoadV2(CONST D3DXVECTOR2* p) { return DirectX::XMLoadFloat2(reinterpret_cast<CONST DirectX::XMFLOAT2*>(p)); }
	inline DirectX::XMVECTOR LoadV3(CONST D3DXVECTOR3* p) { return DirectX::XMLoadFloat3(reinterpret_cast<CONST DirectX::XMFLOAT3*>(p)); }
	inline DirectX::XMVECTOR LoadV4(CONST D3DXVECTOR4* p) { return DirectX::XMLoadFloat4(reinterpret_cast<CONST DirectX::XMFLOAT4*>(p)); }
	inline DirectX::XMVECTOR LoadQ(CONST D3DXQUATERNION* p) { return DirectX::XMLoadFloat4(reinterpret_cast<CONST DirectX::XMFLOAT4*>(p)); }
	inline DirectX::XMVECTOR LoadP(CONST D3DXPLANE* p) { return DirectX::XMLoadFloat4(reinterpret_cast<CONST DirectX::XMFLOAT4*>(p)); }
	inline DirectX::XMMATRIX LoadM(CONST D3DXMATRIX* p) { return DirectX::XMLoadFloat4x4(reinterpret_cast<CONST DirectX::XMFLOAT4X4*>(p)); }
	inline void StoreV2(D3DXVECTOR2* p, DirectX::FXMVECTOR v) { DirectX::XMStoreFloat2(reinterpret_cast<DirectX::XMFLOAT2*>(p), v); }
	inline void StoreV3(D3DXVECTOR3* p, DirectX::FXMVECTOR v) { DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(p), v); }
	inline void StoreV4(D3DXVECTOR4* p, DirectX::FXMVECTOR v) { DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(p), v); }
	inline void StoreM(D3DXMATRIX* p, DirectX::FXMMATRIX m) { DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(p), m); }
}

//----------------------------------------------------------------------------
// 2D Vector functions
//----------------------------------------------------------------------------
inline FLOAT D3DXVec2Length(CONST D3DXVECTOR2* pV) { return sqrtf(pV->x * pV->x + pV->y * pV->y); }
inline FLOAT D3DXVec2Dot(CONST D3DXVECTOR2* pV1, CONST D3DXVECTOR2* pV2) { return pV1->x * pV2->x + pV1->y * pV2->y; }
inline FLOAT D3DXVec2CCW(CONST D3DXVECTOR2* pV1, CONST D3DXVECTOR2* pV2) { return pV1->x * pV2->y - pV1->y * pV2->x; }

inline D3DXVECTOR2* D3DXVec2Normalize(D3DXVECTOR2* pOut, CONST D3DXVECTOR2* pV)
{
	using namespace d3dx9math_shim_detail;
	StoreV2(pOut, DirectX::XMVector2Normalize(LoadV2(pV)));
	return pOut;
}

//----------------------------------------------------------------------------
// 3D Vector functions
//----------------------------------------------------------------------------
inline FLOAT D3DXVec3Length(CONST D3DXVECTOR3* pV) { return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z); }
inline FLOAT D3DXVec3LengthSq(CONST D3DXVECTOR3* pV) { return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z; }
inline FLOAT D3DXVec3Dot(CONST D3DXVECTOR3* pV1, CONST D3DXVECTOR3* pV2) { return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z; }

inline D3DXVECTOR3* D3DXVec3Cross(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV1, CONST D3DXVECTOR3* pV2)
{
	D3DXVECTOR3 v(pV1->y * pV2->z - pV1->z * pV2->y,
	              pV1->z * pV2->x - pV1->x * pV2->z,
	              pV1->x * pV2->y - pV1->y * pV2->x);
	*pOut = v;
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3Add(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV1, CONST D3DXVECTOR3* pV2)
{
	pOut->x = pV1->x + pV2->x; pOut->y = pV1->y + pV2->y; pOut->z = pV1->z + pV2->z; return pOut;
}

inline D3DXVECTOR3* D3DXVec3Scale(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV, FLOAT s)
{
	pOut->x = pV->x * s; pOut->y = pV->y * s; pOut->z = pV->z * s; return pOut;
}

inline D3DXVECTOR3* D3DXVec3Lerp(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV1, CONST D3DXVECTOR3* pV2, FLOAT s)
{
	pOut->x = pV1->x + s * (pV2->x - pV1->x);
	pOut->y = pV1->y + s * (pV2->y - pV1->y);
	pOut->z = pV1->z + s * (pV2->z - pV1->z);
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV)
{
	using namespace d3dx9math_shim_detail;
	StoreV3(pOut, DirectX::XMVector3Normalize(LoadV3(pV)));
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV, CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	StoreV3(pOut, DirectX::XMVector3TransformCoord(LoadV3(pV), LoadM(pM)));
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV, CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	StoreV3(pOut, DirectX::XMVector3TransformNormal(LoadV3(pV), LoadM(pM)));
	return pOut;
}

inline D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* pOut, CONST D3DXVECTOR3* pV, CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	StoreV4(pOut, DirectX::XMVector3Transform(LoadV3(pV), LoadM(pM)));
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3Project(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV,
	CONST D3DVIEWPORT9* pViewport, CONST D3DXMATRIX* pProjection,
	CONST D3DXMATRIX* pView, CONST D3DXMATRIX* pWorld)
{
	using namespace d3dx9math_shim_detail;
	DirectX::XMVECTOR v = DirectX::XMVector3Project(LoadV3(pV),
		(FLOAT)pViewport->X, (FLOAT)pViewport->Y, (FLOAT)pViewport->Width, (FLOAT)pViewport->Height,
		pViewport->MinZ, pViewport->MaxZ, LoadM(pProjection), LoadM(pView), LoadM(pWorld));
	StoreV3(pOut, v);
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3Unproject(D3DXVECTOR3* pOut, CONST D3DXVECTOR3* pV,
	CONST D3DVIEWPORT9* pViewport, CONST D3DXMATRIX* pProjection,
	CONST D3DXMATRIX* pView, CONST D3DXMATRIX* pWorld)
{
	using namespace d3dx9math_shim_detail;
	DirectX::XMVECTOR v = DirectX::XMVector3Unproject(LoadV3(pV),
		(FLOAT)pViewport->X, (FLOAT)pViewport->Y, (FLOAT)pViewport->Width, (FLOAT)pViewport->Height,
		pViewport->MinZ, pViewport->MaxZ, LoadM(pProjection), LoadM(pView), LoadM(pWorld));
	StoreV3(pOut, v);
	return pOut;
}

//----------------------------------------------------------------------------
// 4D Vector functions
//----------------------------------------------------------------------------
inline D3DXVECTOR4* D3DXVec4Transform(D3DXVECTOR4* pOut, CONST D3DXVECTOR4* pV, CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	StoreV4(pOut, DirectX::XMVector4Transform(LoadV4(pV), LoadM(pM)));
	return pOut;
}

//----------------------------------------------------------------------------
// Matrix functions
//----------------------------------------------------------------------------
inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* pOut)
{
	pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.0f;
	pOut->_12 = pOut->_13 = pOut->_14 = 0.0f;
	pOut->_21 = pOut->_23 = pOut->_24 = 0.0f;
	pOut->_31 = pOut->_32 = pOut->_34 = 0.0f;
	pOut->_41 = pOut->_42 = pOut->_43 = 0.0f;
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, CONST D3DXMATRIX* pM1, CONST D3DXMATRIX* pM2)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixMultiply(LoadM(pM1), LoadM(pM2)));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* pOut, CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixTranspose(LoadM(pM)));
	return pOut;
}

inline FLOAT D3DXMatrixDeterminant(CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(LoadM(pM)));
}

inline D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* pOut, FLOAT* pDeterminant, CONST D3DXMATRIX* pM)
{
	using namespace d3dx9math_shim_detail;
	DirectX::XMVECTOR det;
	DirectX::XMMATRIX inv = DirectX::XMMatrixInverse(&det, LoadM(pM));
	if (pDeterminant)
		*pDeterminant = DirectX::XMVectorGetX(det);
	StoreM(pOut, inv);
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranslation(D3DXMATRIX* pOut, FLOAT x, FLOAT y, FLOAT z)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixTranslation(x, y, z));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixScaling(D3DXMATRIX* pOut, FLOAT sx, FLOAT sy, FLOAT sz)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixScaling(sx, sy, sz));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* pOut, FLOAT Angle)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixRotationX(Angle));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* pOut, FLOAT Angle)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixRotationY(Angle));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* pOut, FLOAT Angle)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixRotationZ(Angle));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationAxis(D3DXMATRIX* pOut, CONST D3DXVECTOR3* pV, FLOAT Angle)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixRotationAxis(LoadV3(pV), Angle));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationQuaternion(D3DXMATRIX* pOut, CONST D3DXQUATERNION* pQ)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixRotationQuaternion(LoadQ(pQ)));
	return pOut;
}

// D3DX takes (yaw, pitch, roll); DirectXMath takes (pitch, yaw, roll).
inline D3DXMATRIX* D3DXMatrixRotationYawPitchRoll(D3DXMATRIX* pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixLookAtRH(D3DXMATRIX* pOut, CONST D3DXVECTOR3* pEye, CONST D3DXVECTOR3* pAt, CONST D3DXVECTOR3* pUp)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixLookAtRH(LoadV3(pEye), LoadV3(pAt), LoadV3(pUp)));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixPerspectiveFovRH(D3DXMATRIX* pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixPerspectiveFovRH(fovy, Aspect, zn, zf));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixOrthoRH(D3DXMATRIX* pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixOrthographicRH(w, h, zn, zf));
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixOrthoOffCenterRH(D3DXMATRIX* pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf)
{
	using namespace d3dx9math_shim_detail;
	StoreM(pOut, DirectX::XMMatrixOrthographicOffCenterRH(l, r, b, t, zn, zf));
	return pOut;
}

//----------------------------------------------------------------------------
// Quaternion functions
//----------------------------------------------------------------------------
inline D3DXQUATERNION* D3DXQuaternionConjugate(D3DXQUATERNION* pOut, CONST D3DXQUATERNION* pQ)
{
	pOut->x = -pQ->x; pOut->y = -pQ->y; pOut->z = -pQ->z; pOut->w = pQ->w;
	return pOut;
}

// D3DX convention: Out = Q1 * Q2 (rotation Q2 followed by Q1). Explicit formula
// to avoid DirectXMath's reversed XMQuaternionMultiply operand order.
inline D3DXQUATERNION* D3DXQuaternionMultiply(D3DXQUATERNION* pOut, CONST D3DXQUATERNION* pQ1, CONST D3DXQUATERNION* pQ2)
{
	D3DXQUATERNION q;
	q.x = pQ2->w * pQ1->x + pQ2->x * pQ1->w + pQ2->y * pQ1->z - pQ2->z * pQ1->y;
	q.y = pQ2->w * pQ1->y - pQ2->x * pQ1->z + pQ2->y * pQ1->w + pQ2->z * pQ1->x;
	q.z = pQ2->w * pQ1->z + pQ2->x * pQ1->y - pQ2->y * pQ1->x + pQ2->z * pQ1->w;
	q.w = pQ2->w * pQ1->w - pQ2->x * pQ1->x - pQ2->y * pQ1->y - pQ2->z * pQ1->z;
	*pOut = q;
	return pOut;
}

inline D3DXQUATERNION* D3DXQuaternionRotationAxis(D3DXQUATERNION* pOut, CONST D3DXVECTOR3* pV, FLOAT Angle)
{
	using namespace d3dx9math_shim_detail;
	DirectX::XMVECTOR axis = DirectX::XMVector3Normalize(LoadV3(pV));
	DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axis, Angle);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(pOut), q);
	return pOut;
}

// D3DX takes (yaw, pitch, roll); DirectXMath takes (pitch, yaw, roll).
inline D3DXQUATERNION* D3DXQuaternionRotationYawPitchRoll(D3DXQUATERNION* pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll)
{
	DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll);
	DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(pOut), q);
	return pOut;
}

//----------------------------------------------------------------------------
// Plane functions
//----------------------------------------------------------------------------
inline FLOAT D3DXPlaneDotCoord(CONST D3DXPLANE* pP, CONST D3DXVECTOR3* pV)
{
	return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
}

inline D3DXPLANE* D3DXPlaneNormalize(D3DXPLANE* pOut, CONST D3DXPLANE* pP)
{
	FLOAT norm = sqrtf(pP->a * pP->a + pP->b * pP->b + pP->c * pP->c);
	if (norm)
	{
		FLOAT inv = 1.0f / norm;
		pOut->a = pP->a * inv; pOut->b = pP->b * inv; pOut->c = pP->c * inv; pOut->d = pP->d * inv;
	}
	else
	{
		pOut->a = pOut->b = pOut->c = pOut->d = 0.0f;
	}
	return pOut;
}

//----------------------------------------------------------------------------
// Color functions
//----------------------------------------------------------------------------
inline D3DXCOLOR* D3DXColorModulate(D3DXCOLOR* pOut, CONST D3DXCOLOR* pC1, CONST D3DXCOLOR* pC2)
{
	pOut->r = pC1->r * pC2->r;
	pOut->g = pC1->g * pC2->g;
	pOut->b = pC1->b * pC2->b;
	pOut->a = pC1->a * pC2->a;
	return pOut;
}

//----------------------------------------------------------------------------
// Out-of-class operator definitions that depend on the free functions
//----------------------------------------------------------------------------
inline D3DXMATRIX D3DXMATRIX::operator * (CONST D3DXMATRIX& mat) const
{
	D3DXMATRIX out;
	D3DXMatrixMultiply(&out, this, &mat);
	return out;
}

inline D3DXMATRIX& D3DXMATRIX::operator *= (CONST D3DXMATRIX& mat)
{
	D3DXMatrixMultiply(this, this, &mat);
	return *this;
}

inline D3DXQUATERNION D3DXQUATERNION::operator * (CONST D3DXQUATERNION& q) const
{
	D3DXQUATERNION out;
	D3DXQuaternionMultiply(&out, this, &q);
	return out;
}

inline D3DXQUATERNION& D3DXQUATERNION::operator *= (CONST D3DXQUATERNION& q)
{
	D3DXQuaternionMultiply(this, this, &q);
	return *this;
}

//----------------------------------------------------------------------------
// Matrix stack (replacement for ID3DXMatrixStack / D3DXCreateMatrixStack)
//----------------------------------------------------------------------------
#include <vector>

struct ID3DXMatrixStack
{
	std::vector<D3DXMATRIX> m_stack;

	ID3DXMatrixStack() { D3DXMATRIX id; D3DXMatrixIdentity(&id); m_stack.push_back(id); }

	ULONG AddRef() { return 1; }
	ULONG Release() { delete this; return 0; }

	D3DXMATRIX* GetTop() { return &m_stack.back(); }

	void LoadIdentity() { D3DXMatrixIdentity(&m_stack.back()); }
	void LoadMatrix(CONST D3DXMATRIX* pM) { m_stack.back() = *pM; }

	void Push() { m_stack.push_back(m_stack.back()); }
	void Pop() { if (m_stack.size() > 1) m_stack.pop_back(); }

	void MultMatrix(CONST D3DXMATRIX* pM) { D3DXMatrixMultiply(&m_stack.back(), &m_stack.back(), pM); }
	void MultMatrixLocal(CONST D3DXMATRIX* pM) { D3DXMatrixMultiply(&m_stack.back(), pM, &m_stack.back()); }

	void Scale(FLOAT x, FLOAT y, FLOAT z) { D3DXMATRIX m; D3DXMatrixScaling(&m, x, y, z); MultMatrix(&m); }
	void ScaleLocal(FLOAT x, FLOAT y, FLOAT z) { D3DXMATRIX m; D3DXMatrixScaling(&m, x, y, z); MultMatrixLocal(&m); }
	void Translate(FLOAT x, FLOAT y, FLOAT z) { D3DXMATRIX m; D3DXMatrixTranslation(&m, x, y, z); MultMatrix(&m); }
	void TranslateLocal(FLOAT x, FLOAT y, FLOAT z) { D3DXMATRIX m; D3DXMatrixTranslation(&m, x, y, z); MultMatrixLocal(&m); }
	void RotateAxis(CONST D3DXVECTOR3* pV, FLOAT angle) { D3DXMATRIX m; D3DXMatrixRotationAxis(&m, pV, angle); MultMatrix(&m); }
	void RotateAxisLocal(CONST D3DXVECTOR3* pV, FLOAT angle) { D3DXMATRIX m; D3DXMatrixRotationAxis(&m, pV, angle); MultMatrixLocal(&m); }
	void RotateYawPitchRoll(FLOAT yaw, FLOAT pitch, FLOAT roll) { D3DXMATRIX m; D3DXMatrixRotationYawPitchRoll(&m, yaw, pitch, roll); MultMatrix(&m); }
	void RotateYawPitchRollLocal(FLOAT yaw, FLOAT pitch, FLOAT roll) { D3DXMATRIX m; D3DXMatrixRotationYawPitchRoll(&m, yaw, pitch, roll); MultMatrixLocal(&m); }
};
typedef ID3DXMatrixStack* LPD3DXMATRIXSTACK;

inline HRESULT D3DXCreateMatrixStack(DWORD /*Flags*/, LPD3DXMATRIXSTACK* ppStack)
{
	if (!ppStack)
		return E_FAIL;
	*ppStack = new ID3DXMatrixStack();
	return S_OK;
}

#endif // __D3DX9MATH_H__

// Pull in the remaining D3DX surface (core/tex/mesh/effect/shader); the
// deprecated math header is suppressed by the guard defined above.
#include <dx9/d3dx9.h>
