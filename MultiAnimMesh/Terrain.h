/**********************************************************************

���ø߶�ͼ��RAM�Ҷ�ͼ�����ɵ��Ρ�
The Method of use��
	1��ʵ����һ��������任����
		cTerrain* XXX;	D3DXMATRIX aaa;
	2����OnCreateDevice()�����г�ʼ����
		XXX = new cTerrain();
		V_RETURN(XXX->LoadTerrain(pd3dDevice, L"aaa.raw", L"aaa.bmp"));
		XXX->InitTerrain(64, 64, 10.0f, 0.3f);
	3����OnFrameMove()������ͨ�������涨��ľ���任���Ե��ζ���������任��
	4�����Ƶ�����OnFrameRander()������
		V( XXX->DrawTerrain(&aaa) );
	5: OnDestoryDevice()�����ٶ���
		XXX->Destory();

	6: getHeight()��������ʹ�����崦�ڵ����ϡ�
**********************************************************************/

#pragma once

#include <vector>
#include <fstream>
#include "dxstdafx.h"


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }
#endif 
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

class cTerrain{
	struct TERRAINVERTEX
	{
		FLOAT _x, _y, _z;
		FLOAT _u, _v;
		TERRAINVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v) 
			:_x(x), _y(y), _z(z), _u(u), _v(v) {}

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};

	//Varible
	private:
		LPDIRECT3DDEVICE9       m_pd3dDevice;		//inner device object
		LPDIRECT3DTEXTURE9      m_pTexture;			//texture
		//the next two buffer use to DrawIndexPrimitive method to draw terrain.
		LPDIRECT3DINDEXBUFFER9  m_pIndexBuf;		//Index Buffer
		LPDIRECT3DVERTEXBUFFER9 m_pVertexBuf;		//Vertex Buffer

		INT                     m_nCellsPerRow;     // ÿ�е�Ԫ����
		INT                     m_nCellsPerCol;     // ÿ�е�Ԫ����
		INT                     m_nVertsPerRow;     // ÿ�ж�����
		INT                     m_nVertsPerCol;     // ÿ�ж�����
		INT                     m_nNumVertices;     // ��������
		FLOAT                   m_fTerrainWidth;    // ���εĿ��
		FLOAT                   m_fTerrainDepth;    // ���ε����
		FLOAT                   m_fCellSpacing;     // ��Ԫ��ļ��
		FLOAT                   m_fHeightScale;     // �߶�����ϵ��
		std::vector<FLOAT>      m_vHeightInfo;      // �߶���Ϣ

		INT						m_index;
		TERRAINVERTEX			*pVertices;
	//Method
	public:
		//draw method
		cTerrain(IDirect3DDevice9 *pd3dDevice);
		virtual ~cTerrain();
		HRESULT LoadTerrain( wchar_t *pRawFileName, wchar_t *pTextureFile);
		HRESULT InitTerrain(INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale);
		HRESULT DrawTerrain(D3DXMATRIX *pMatWorld);
		HRESULT destory();
		//get height information.
		float getHeight(float x, float z);
};