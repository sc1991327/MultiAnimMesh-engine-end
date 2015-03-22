/**********************************************************************

绘制天空盒类。
The Method of use：
	1：实例化一个对象及其变换矩阵。
		cSkybox* XXX;	D3DXMATRIX aaa;
	2：在OnCreateDevice()方法中初始化。
		XXX = new cSkybox();
		V_RETURN(XXX->LoadSkybox(pd3dDevice,L""));
		XXX->InitSkybox(10,10,300);
	3：在OnFrameMove()方法中通过对上面定义的矩阵变换来对地形对象做整体变换。
	4：绘制地形在OnFrameRander()方法中
		V( XXX->DrawSkybox(&aaa) );
	5: OnDestoryDevice()中销毁对象。
		XXX->Destory();

**********************************************************************/

#pragma once

#include "dxstdafx.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }
#endif 
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

//--------------------------------------------------------------------------------------
// Name: class cSkybox
// Desc: 球形地形
//--------------------------------------------------------------------------------------
class cSkybox
{
private:
    LPDIRECT3DDEVICE9       m_pd3dDevice;
    LPDIRECT3DTEXTURE9      m_pTexture;
    LPDIRECT3DINDEXBUFFER9  m_pIndexBuf;
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuf;

    INT     m_nNumLatitudes;
    INT     m_nNumLongitudes;
    INT     m_nVertsPerLati;
    INT     m_nVertsPerLongi;
    INT     m_nNumVertices;     // 顶点数
    FLOAT   m_fSkyboxRadius;    // 半径

    struct SKYBOXVERTEX
    {
        FLOAT _x, _y, _z;
        FLOAT _u, _v;
        SKYBOXVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v) 
            : _x(x), _y(y), _z(z), _u(u), _v(v) {}
        static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
    };

public:
    cSkybox(IDirect3DDevice9 *pd3dDevice);
    virtual ~cSkybox(void);

public:
    HRESULT LoadSkybox(wchar_t *pTextureFile);
    HRESULT InitSkybox(INT nAlpha, INT nBeta, FLOAT nRadius);
    HRESULT DrawSkybox(D3DXMATRIX *pMatWorld, BOOL bDrawFrame=FALSE);
	HRESULT distory();
};
