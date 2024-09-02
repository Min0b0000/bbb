#pragma once

#include <math.h>


#if 1
#if __cplusplus >= 201703L
#define MM_ALIGNED_DATA(x)		alignas(x)
#define MM_ALIGNED_STRUCT(x)	struct alignas(x)
#elif defined(__GNUC__)
#define MM_ALIGNED_DATA(x)		__attribute__ ((aligned(x)))
#define MM_ALIGNED_STRUCT(x)	struct __attribute__ ((aligned(x)))
#else
#define MM_ALIGNED_DATA(x)		__declspec(align(x))
#define MM_ALIGNED_STRUCT(x)	__declspec(align(x)) struct
#endif
#else
#define MM_ALIGNED_DATA(x)		
#define MM_ALIGNED_STRUCT(x)	struct
#endif


const float F_PI		= 3.14159265358979f;

const float F_PID6 = F_PI / 6.f; /**	< 30	*/
const float F_PID4 = F_PI / 4.f; /**	< 45	*/
const float F_PID3 = F_PI / 3.f; /**	< 60	*/
const float F_PID2 = F_PI / 2.f; /**	< 90	*/
const float F_PIM2 = F_PI * 2.f; /**	< 360	*/

const float F_DEG2RAD	= (F_PI / 180.f);
const float F_RAD2DEG	= (180.f / F_PI);

#define Deg2Rad(x) ((x) * F_DEG2RAD)
#define Rad2Deg(x) ((x) * F_RAD2DEG)

typedef MM_ALIGNED_STRUCT(16) float2 { union { float v[2]; float m[1][2]; struct { float x, y; }; }; } float2;
typedef MM_ALIGNED_STRUCT(16) float3 { union { float v[3]; float m[1][3]; struct { float x, y, z; }; }; } float3;
typedef MM_ALIGNED_STRUCT(16) float4 { union { float v[4]; float m[1][4]; struct { float x, y, z, w; }; }; } float4;

typedef MM_ALIGNED_STRUCT(16) long2 { union { long v[2]; long m[1][2]; struct { long x, y; }; }; } long2;
typedef MM_ALIGNED_STRUCT(16) long3 { union { long v[3]; long m[1][3]; struct { long x, y, z; }; }; } long3;
typedef MM_ALIGNED_STRUCT(16) long4 { union { long v[4]; long m[1][4]; struct { long x, y, z, w; }; }; } long4;

typedef MM_ALIGNED_STRUCT(16) float2x2
{
	union
	{
		float v[4];
		float m[2][2];
		struct
		{
			float m00, m01;
			float m10, m11;
		};
	};
} float2x2;

typedef MM_ALIGNED_STRUCT(16) float3x3
{
	union
	{
		float v[9];
		float m[3][3];
		struct
		{
			float m00, m01, m02;
			float m10, m11, m12;
			float m20, m21, m22;
		};
	};
} float3x3;

typedef MM_ALIGNED_STRUCT(16) float4x4
{
	union
	{
		float v[16];
		float m[4][4];
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
	};
} float4x4;



void				fset(float* _pX, float _fVal, int _iCount);
void				fcpy(float* _pDest, const float* _pSrc, int _iCount);
void				fswp(float* _pX, float* _pY, int _iCount);

void				fadd(float* _pDest, const float* _pSrc, int _iCount);
void				fneg(float* _pX, int _iCount);
void				fsub(float* _pDest, const float* _pSrc, int _iCount);
void				fscl(float* _pX, float _pVal, int _iCount);
void				fcrs(float3* _pDest, const float3* _pX, const float3* _pY);
float 				fdot(const float* _pX, const float* _pY, int _iCount);
float 				fnrm(const float* _pX, int _iCount);
int 				fnmz(float* _pX, int _iCount);

void				fxTnp(float* _pX, int _iDms);
void				fxmul(float* _pDest, const float* _pX, const float* _pY, int _iRowX, int _iColRow, int _iColY);

void				f3x3mul(float3x3* _pDest, const float3x3* _pX, const float3x3* _pY);
void				f4x4mul(float4x4* _pDest, const float4x4* _pX, const float4x4* _pY);

#if 0

void				f3x3f3mul(float3* _pDest, const float3x3* _pTrans, const float3* _pSrc);
void				f4x4f4mul(float4* _pDest, const float4x4* _pTrans, const float4* _pSrc);

#else

#define				f3x3f3mul(_pDest, _pTrans, _pSrc) ( \
(_pDest)->v[0] = (_pSrc)->v[0] * ((_pTrans)->v[0] + (_pTrans)->v[3] + (_pTrans)->v[6]), \
(_pDest)->v[1] = (_pSrc)->v[1] * ((_pTrans)->v[1] + (_pTrans)->v[4] + (_pTrans)->v[7]), \
(_pDest)->v[2] = (_pSrc)->v[2] * ((_pTrans)->v[2] + (_pTrans)->v[5] + (_pTrans)->v[8]))

#define				f4x4f4mul(_pDest, _pTrans, _pSrc) ( \
(_pDest)->v[0] = (_pSrc)->v[0] * ((_pTrans)->v[0] + (_pTrans)->v[4] + (_pTrans)->v[8]	+ (_pTrans)->v[12]), \
(_pDest)->v[1] = (_pSrc)->v[1] * ((_pTrans)->v[1] + (_pTrans)->v[5] + (_pTrans)->v[9]	+ (_pTrans)->v[13]), \
(_pDest)->v[2] = (_pSrc)->v[2] * ((_pTrans)->v[2] + (_pTrans)->v[6] + (_pTrans)->v[10]	+ (_pTrans)->v[14]), \
(_pDest)->v[3] = (_pSrc)->v[3] * ((_pTrans)->v[3] + (_pTrans)->v[7] + (_pTrans)->v[11]	+ (_pTrans)->v[15]))

#endif

void				fxIdentity(float* _pX, int _iRow, int _iCol);
#define				f2x2Identity(_pX) ((_pX)[0] = 1.f, (_pX)[3] = 1.f)
#define				f3x3Identity(_pX) ((_pX)[0] = 1.f, (_pX)[4] = 1.f, (_pX)[8]  = 1.f)
#define				f4x4Identity(_pX) ((_pX)[0] = 1.f, (_pX)[5] = 1.f, (_pX)[10] = 1.f, (_pX)[15] = 1.f)

void				f3x3Translation(float3x3* _pDest, const float2* _pPos);
void				f3x3Rotation(float3x3* _pDest, float _fRad);
void				f3x3Scale(float3x3* _pDest, const float2* _pScl);

void				f4x4Translation(float4x4* _pDest, const float3* _pPos);
void				f4x4RotationX(float4x4* _pDest, float _fRad);
void				f4x4RotationY(float4x4* _pDest, float _fRad);
void				f4x4RotationZ(float4x4* _pDest, float _fRad);
// void				f4x4QRotation(float4x4* _pDest); /** Quaternion */
void				f4x4Scale(float4x4* _pDest, const float3* _pScl);

void				f4x4Perspective(float4x4* _pDest, float _fFovy, float _fAspect, float _fZnear, float _fZfar);
void				f4x4Ortho(float4x4* _pDest, float _fWidth, float _fHeight, float _fZnear, float _fZfar);
void				f4x4LookAt(float4x4* _pDest, const float3* _pEye, const float3* _pAt, const float3* _pUp);
void				f4x4Viewport(float4x4* _pDest, const float _pXYWH[4], float _zmin, float _zmax);