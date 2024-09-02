#include "mmath.h"

#include <string.h>



void 
fset(float* _pX, float _fVal, int _iCount)
{
	for (int i = 0; _iCount > i; ++i)
	{
		_pX[i] = _fVal;
	}
}

void 
fcpy(float* _pDest, const float* _pSrc, int _iCount)
{
	for (int i = 0; _iCount > i; ++i)
	{
		_pDest[i] = _pSrc[i];
	}
}

void 
fswp(float* _pX, float* _pY, int _iCount)
{
	float temp;
	for (int i = 0; _iCount > i; ++i)
	{
		temp	= _pY[i];
		_pX[i]	= _pY[i];
		_pY[i]	= temp;
	}
}

void 
fadd(float* _pDest, const float* _pSrc, int _iCount)
{
	for (int i = 0; _iCount > i; ++i)
	{
		_pDest[i] = _pDest[i] + _pSrc[i];
	}
}

void 
fneg(float* _pX, int _iCount)
{
	for (int i = 0; _iCount > i; ++i)
	{
		_pX[i] = -_pX[i];
	}
}

void 
fsub(float* _pDest, const float* _pSrc, int _iCount)
{
	for (int i = 0; _iCount > i; ++i)
	{
		_pDest[i] = _pDest[i] - _pSrc[i];
	}
}

void 
fscl(float* _pX, float _pVal, int _iCount)
{
	for (int i = 0; _iCount > i; ++i)
	{
		_pX[i] = _pVal * _pX[i];
	}
}

void 
fcrs(float3* _pDest, const float3* _pX, const float3* _pY)
{
#if 1
	_pDest->x = (_pX->v[1] * _pY->v[2]) - (_pX->v[2] * _pY->v[1]);
	_pDest->y = (_pX->v[2] * _pY->v[0]) - (_pX->v[0] * _pY->v[2]);
	_pDest->z = (_pX->v[0] * _pY->v[1]) - (_pX->v[1] * _pY->v[0]);
#else
	_pDest->x = (_pX->v[2] * _pY->v[1]) - (_pX->v[1] * _pY->v[2]);
	_pDest->y = (_pX->v[0] * _pY->v[2]) - (_pX->v[2] * _pY->v[0]);
	_pDest->z = (_pX->v[1] * _pY->v[0]) - (_pX->v[0] * _pY->v[1]);
#endif
}

float 
fdot(const float* _pX, const float* _pY, int _iCount)
{
	float fr = 0.0f;
	for (int i = 0; _iCount > i; ++i)
	{
		fr += _pX[i] * _pY[i];
	} return fr;
}

float 
fnrm(const float* _pX, int _iCount)
{
	float fr = 0.0f;
	for (int i = 0; _iCount > i; ++i)
	{
		fr += powf(_pX[i], 2.f);
	} return sqrtf(fr);
}

int 
fnmz(float* _pX, int _iCount)
{
	float norm = fnrm(_pX, _iCount);
	if (0.0f == norm) return -1; // hmm...
	else for (int i = 0; _iCount > i; ++i)
	{
		_pX[i] /= norm;
	} return 0;
}

void 
fxTnp(float* _pX, int _iDms)
{
	int		pos;
	float	temp;
	for (int i = 0; _iDms > i; ++i)
	{
		pos = (_iDms + 1) * i;
		for (int j = _iDms - i - 1; j; --j)
		{
			temp					= _pX[pos + j];
			_pX[pos + j]			= _pX[pos + (_iDms * j)];
			_pX[pos + (_iDms * j)]	= temp;
		}
	}
}

void 
fxmul(float*		_pDest
	, const float*	_pX
	, const float*	_pY
	, int			_iRowX
	, int			_iColRow
	, int			_iColY)
{
	float temp;
	memset(_pDest, 0, sizeof(float) * _iRowX * _iColY);
	for (int m = 0; _iRowX > m; ++m)
	{
		for (int n = 0; _iColY > n; ++n)
		{
			temp = 0.f;
			for (int nm = 0; _iColRow > nm; ++nm)
			{
				temp = temp
					+ _pX[_iColRow	* m		+ nm]
					* _pY[_iColY	* nm	+ n];
			} _pDest[_iColY * m + n] = temp;
		}
	}
}

void 
f3x3mul(float3x3* _pDest, const float3x3* _pX, const float3x3* _pY)
{
	_pDest->m[0][0] = _pX->m[0][0] * _pY->m[0][0] + _pX->m[0][1] * _pY->m[1][0] + _pX->m[0][2] * _pY->m[2][0];
	_pDest->m[0][1] = _pX->m[0][0] * _pY->m[0][1] + _pX->m[0][1] * _pY->m[1][1] + _pX->m[0][2] * _pY->m[2][1];
	_pDest->m[0][2] = _pX->m[0][0] * _pY->m[0][2] + _pX->m[0][1] * _pY->m[1][2] + _pX->m[0][2] * _pY->m[2][2];
	_pDest->m[1][0] = _pX->m[1][0] * _pY->m[0][0] + _pX->m[1][1] * _pY->m[1][0] + _pX->m[1][2] * _pY->m[2][0];
	_pDest->m[1][1] = _pX->m[1][0] * _pY->m[0][1] + _pX->m[1][1] * _pY->m[1][1] + _pX->m[1][2] * _pY->m[2][1];
	_pDest->m[1][2] = _pX->m[1][0] * _pY->m[0][2] + _pX->m[1][1] * _pY->m[1][2] + _pX->m[1][2] * _pY->m[2][2];
	_pDest->m[2][0] = _pX->m[2][0] * _pY->m[0][0] + _pX->m[2][1] * _pY->m[1][0] + _pX->m[2][2] * _pY->m[2][0];
	_pDest->m[2][1] = _pX->m[2][0] * _pY->m[0][1] + _pX->m[2][1] * _pY->m[1][1] + _pX->m[2][2] * _pY->m[2][1];
	_pDest->m[2][2] = _pX->m[2][0] * _pY->m[0][2] + _pX->m[2][1] * _pY->m[1][2] + _pX->m[2][2] * _pY->m[2][2];
}

void 
f4x4mul(float4x4* _pDest, const float4x4* _pX, const float4x4* _pY) // todo
{
	_pDest->m[0][0] = _pX->m[0][0] * _pY->m[0][0] + _pX->m[0][1] * _pY->m[1][0] + _pX->m[0][2] * _pY->m[2][0] + _pX->m[0][3] * _pY->m[3][0];
	_pDest->m[0][1] = _pX->m[0][0] * _pY->m[0][1] + _pX->m[0][1] * _pY->m[1][1] + _pX->m[0][2] * _pY->m[2][1] + _pX->m[0][3] * _pY->m[3][1];
	_pDest->m[0][2] = _pX->m[0][0] * _pY->m[0][2] + _pX->m[0][1] * _pY->m[1][2] + _pX->m[0][2] * _pY->m[2][2] + _pX->m[0][3] * _pY->m[3][2];
	_pDest->m[0][3] = _pX->m[0][0] * _pY->m[0][3] + _pX->m[0][1] * _pY->m[1][3] + _pX->m[0][2] * _pY->m[2][3] + _pX->m[0][3] * _pY->m[3][3];
	_pDest->m[1][0] = _pX->m[1][0] * _pY->m[0][0] + _pX->m[1][1] * _pY->m[1][0] + _pX->m[1][2] * _pY->m[2][0] + _pX->m[1][3] * _pY->m[3][0];
	_pDest->m[1][1] = _pX->m[1][0] * _pY->m[0][1] + _pX->m[1][1] * _pY->m[1][1] + _pX->m[1][2] * _pY->m[2][1] + _pX->m[1][3] * _pY->m[3][1];
	_pDest->m[1][2] = _pX->m[1][0] * _pY->m[0][2] + _pX->m[1][1] * _pY->m[1][2] + _pX->m[1][2] * _pY->m[2][2] + _pX->m[1][3] * _pY->m[3][2];
	_pDest->m[1][3] = _pX->m[1][0] * _pY->m[0][3] + _pX->m[1][1] * _pY->m[1][3] + _pX->m[1][2] * _pY->m[2][3] + _pX->m[1][3] * _pY->m[3][3];
	_pDest->m[2][0] = _pX->m[2][0] * _pY->m[0][0] + _pX->m[2][1] * _pY->m[1][0] + _pX->m[2][2] * _pY->m[2][0] + _pX->m[2][3] * _pY->m[3][0];
	_pDest->m[2][1] = _pX->m[2][0] * _pY->m[0][1] + _pX->m[2][1] * _pY->m[1][1] + _pX->m[2][2] * _pY->m[2][1] + _pX->m[2][3] * _pY->m[3][1];
	_pDest->m[2][2] = _pX->m[2][0] * _pY->m[0][2] + _pX->m[2][1] * _pY->m[1][2] + _pX->m[2][2] * _pY->m[2][2] + _pX->m[2][3] * _pY->m[3][2];
	_pDest->m[2][3] = _pX->m[2][0] * _pY->m[0][3] + _pX->m[2][1] * _pY->m[1][3] + _pX->m[2][2] * _pY->m[2][3] + _pX->m[2][3] * _pY->m[3][3];
	_pDest->m[3][0] = _pX->m[3][0] * _pY->m[0][0] + _pX->m[3][1] * _pY->m[1][0] + _pX->m[3][2] * _pY->m[2][0] + _pX->m[3][3] * _pY->m[3][0];
	_pDest->m[3][1] = _pX->m[3][0] * _pY->m[0][1] + _pX->m[3][1] * _pY->m[1][1] + _pX->m[3][2] * _pY->m[2][1] + _pX->m[3][3] * _pY->m[3][1];
	_pDest->m[3][2] = _pX->m[3][0] * _pY->m[0][2] + _pX->m[3][1] * _pY->m[1][2] + _pX->m[3][2] * _pY->m[2][2] + _pX->m[3][3] * _pY->m[3][2];
	_pDest->m[3][3] = _pX->m[3][0] * _pY->m[0][3] + _pX->m[3][1] * _pY->m[1][3] + _pX->m[3][2] * _pY->m[2][3] + _pX->m[3][3] * _pY->m[3][3];
}

#ifndef f3x3f3mul

void 
f3x3f3mul(float3* _pDest, const float3x3* _pTrans, const float3* _pSrc)
{
	_pDest->v[0] = _pSrc->v[0] * (_pTrans->v[0] + _pTrans->v[3] + _pTrans->v[6]);
	_pDest->v[1] = _pSrc->v[1] * (_pTrans->v[1] + _pTrans->v[4] + _pTrans->v[7]);
	_pDest->v[2] = _pSrc->v[2] * (_pTrans->v[2] + _pTrans->v[5] + _pTrans->v[8]);
}

void 
f4x4f4mul(float4* _pDest, const float4x4* _pTrans, const float4* _pSrc)
{
	_pDest->v[0] = _pSrc->v[0] * (_pTrans->v[0] + _pTrans->v[4] + _pTrans->v[8]	 + _pTrans->v[12]);
	_pDest->v[1] = _pSrc->v[1] * (_pTrans->v[1] + _pTrans->v[5] + _pTrans->v[9]	 + _pTrans->v[13]);
	_pDest->v[2] = _pSrc->v[2] * (_pTrans->v[2] + _pTrans->v[6] + _pTrans->v[10] + _pTrans->v[14]);
	_pDest->v[3] = _pSrc->v[3] * (_pTrans->v[3] + _pTrans->v[7] + _pTrans->v[11] + _pTrans->v[15]);
}

#endif

void 
fxIdentity(float* _pX, int _iRow, int _iCol)
{
	int j = 0;
	memset(_pX, 0, sizeof(float) * _iRow * _iCol);
	for (int i = 0; _iRow > i; ++i)
	{
		if (_iCol < j)
		{
			break;
		} else
		{
			_pX[_iRow * i + j++] = 1.f;
		}
	}
}

void 
f3x3Translation(float3x3* _pDest, const float2* _pPos)
{
	memset(_pDest, 0, sizeof(float3x3));
	f3x3Identity(_pDest->v);
	_pDest->m20 = _pPos->x;
	_pDest->m21 = _pPos->y;
}

void 
f3x3Rotation(float3x3* _pDest, float _fRad)
{
	float s, c;
	s = sinf(_fRad);
    c = cosf(_fRad);
	memset(_pDest, 0, sizeof(float3x3));
	f3x3Identity(_pDest->v);
    _pDest->m00 =  c; 
    _pDest->m10 = -s; 
    _pDest->m01 =  s;
    _pDest->m11 =  c;
}

void 
f3x3Scale(float3x3* _pDest, const float2* _pScl)
{
	memset(_pDest, 0, sizeof(float3x3));
	f3x3Identity(_pDest->v);
	_pDest->m00 = _pScl->x;
	_pDest->m11 = _pScl->y;
}

void 
f4x4Translation(float4x4* _pDest, const float3* _pPos)
{
	memset(_pDest, 0, sizeof(float4x4));
	f4x4Identity(_pDest->v);
	_pDest->m30 = _pPos->x;
	_pDest->m31 = _pPos->y;
	_pDest->m32 = _pPos->z;
}

void 
f4x4RotationX(float4x4 * _pDest, float _fRad)
{
	float s, c;
	memset(_pDest, 0, sizeof(float4x4));
	f4x4Identity(_pDest->v);
	s = sinf(_fRad);
    c = cosf(_fRad);
    _pDest->m11 =  c;
    _pDest->m12 =  s;
    _pDest->m21 = -s;
    _pDest->m22 =  c;
}

void 
f4x4RotationY(float4x4* _pDest, float _fRad)
{
	float s, c;
	memset(_pDest, 0, sizeof(float4x4));
	f4x4Identity(_pDest->v);
	s = sinf(_fRad);
    c = cosf(_fRad);
	_pDest->m00 =  c;
	_pDest->m02 = -s;
	_pDest->m20 =  s;
	_pDest->m22 =  c;
}

void 
f4x4RotationZ(float4x4* _pDest, float _fRad)
{
	float s, c;
	memset(_pDest, 0, sizeof(float4x4));
	f4x4Identity(_pDest->v);
	s = sinf(_fRad);
	c = cosf(_fRad);
	_pDest->m00 =  c;
	_pDest->m01 =  s;
	_pDest->m10 = -s;
	_pDest->m11 =  c;
}

void 
f4x4Scale(float4x4* _pDest, const float3* _pScl)
{
	memset(_pDest, 0, sizeof(float4x4));
	f4x4Identity(_pDest->v);
	_pDest->m00 = _pScl->x;
    _pDest->m11 = _pScl->y;
    _pDest->m22 = _pScl->z;
}

void 
f4x4Perspective(
	float4x4*	_pDest
	, float		_fFovy
	, float		_fAspect
	, float		_fZnear
	, float		_fZfar)
{
	float range		= _fZfar / (_fZfar - _fZnear);
	float yScale	= 1.f / tanf(_fFovy / 2.f); // yScale = cot(fovY/2)
	float xScale	= yScale / _fAspect; // xScale = yScale / aspect ratio

	// xScale     0          0               0
	// 0        yScale       0               0
	// 0          0       zf/(zf-zn)         1
	// 0          0       -zn*zf/(zf-zn)     0
	memset(_pDest, 0, sizeof(float4x4));
	_pDest->m00 = xScale;
	_pDest->m11 = yScale;
	_pDest->m22 = range;
	_pDest->m23 = 1.f;
	_pDest->m32 = range * (-_fZnear);
}

void 
f4x4Ortho(
	float4x4*	_pDest
	, float		_fWidth
	, float		_fHeight
	, float		_fZnear
	, float		_fZfar)
{
	// 2/w  0    0           0
	// 0    2/h  0           0
	// 0    0    1/(zf-zn)   0
	// 0    0   -zn/(zf-zn)  1
	memset(_pDest, 0, sizeof(float4x4));
	_pDest->m00 = 2.f / _fWidth;
	_pDest->m11 = 2.f / _fHeight;
	_pDest->m22 = 1.f / (_fZfar - _fZnear);
	_pDest->m32 = -_fZnear / (_fZfar - _fZnear);
	_pDest->m33 = 1.f;
}

void 
f4x4LookAt(float4x4*	_pDest
	, const float3*		_pEye
	, const float3*		_pAt
	, const float3*		_pUp)
{
	float3 temp		= *_pEye;
	float3 zaxis	= *_pAt; // normal(At - Eye)
	float3 xaxis; // normal(cross(Up, zaxis))
	float3 yaxis; // cross(zaxis, xaxis)
	
	fsub(zaxis.v, temp.v, 3);
	fnmz(zaxis.v, 3);
	fcrs(&xaxis, _pUp, &zaxis);
	fnmz(xaxis.v, 3);
	fcrs(&yaxis, &zaxis, &xaxis);

	//  xaxis.x           yaxis.x           zaxis.x          0
	//  xaxis.y           yaxis.y           zaxis.y          0
	//  xaxis.z           yaxis.z           zaxis.z          0
	// -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
	memset(_pDest, 0, sizeof(float4x4));
	_pDest->m00 = xaxis.x; _pDest->m01 = yaxis.x; _pDest->m02 = zaxis.x;
	_pDest->m10 = xaxis.y; _pDest->m11 = yaxis.y; _pDest->m12 = zaxis.y;
	_pDest->m20 = xaxis.z; _pDest->m21 = yaxis.z; _pDest->m22 = zaxis.z;
	_pDest->m30 = -fdot(xaxis.v, _pEye->v, 3);
	_pDest->m31 = -fdot(yaxis.v, _pEye->v, 3);
	_pDest->m32 = -fdot(zaxis.v, _pEye->v, 3);
	_pDest->m33 = 1.f;
}

void 
f4x4Viewport(float4x4* _pDest, const float _pLTRB[4], float _zmin, float _zmax)
{
	float w = (_pLTRB[2] - _pLTRB[0]) / 2.f;
	float h = (_pLTRB[3] - _pLTRB[1]) / 2.f;

	//  width/2				0					0					0
	//  0					height/2			0					0
	//  0					0					max-min				0
	//  width / 2 + p.x		p.y + height / 2	min					1 
	// * p is coordinate of the upper-left corner *
	memset(_pDest, 0, sizeof(float4x4));
	_pDest->m00 = w;
	_pDest->m11 = -h;
	_pDest->m22 = _zmax - _zmin;
	_pDest->m30 = w + _pLTRB[0];
	_pDest->m31 = _pLTRB[1] + h;
	_pDest->m32 = _zmin;
	_pDest->m33 = 1.f;
}