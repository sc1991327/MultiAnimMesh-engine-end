/**********************************************************************

������պ��ࡣ
The Method of use��
	1��ʵ����һ��������任����
		cSkybox* XXX;	D3DXMATRIX aaa;
	2����OnCreateDevice()�����г�ʼ����
		XXX = new cSkybox();
		V_RETURN(XXX->LoadSkybox(pd3dDevice,L""));
		XXX->InitSkybox(10,10,300);
	3����OnFrameMove()������ͨ�������涨��ľ���任���Ե��ζ���������任��
	4�����Ƶ�����OnFrameRander()������
		V( XXX->DrawSkybox(&aaa) );
	5: OnDestoryDevice()�����ٶ���
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
// Desc: ���ε���
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
    INT     m_nNumVertices;     // ������
    FLOAT   m_fSkyboxRadius;    // �뾶

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
