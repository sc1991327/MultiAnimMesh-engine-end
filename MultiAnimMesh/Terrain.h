/**********************************************************************

利用高度图（RAM灰度图）生成地形。
The Method of use：
	1：实例化一个对象及其变换矩阵。
		cTerrain* XXX;	D3DXMATRIX aaa;
	2：在OnCreateDevice()方法中初始化。
		XXX = new cTerrain();
		V_RETURN(XXX->LoadTerrain(pd3dDevice, L"aaa.raw", L"aaa.bmp"));
		XXX->InitTerrain(64, 64, 10.0f, 0.3f);
	3：在OnFrameMove()方法中通过对上面定义的矩阵变换来对地形对象做整体变换。
	4：绘制地形在OnFrameRander()方法中
		V( XXX->DrawTerrain(&aaa) );
	5: OnDestoryDevice()中销毁对象。
		XXX->Destory();

	6: getHeight()方法用于使得物体处于地面上。
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

		INT                     m_nCellsPerRow;     // 每行单元格数
		INT                     m_nCellsPerCol;     // 每列单元格数
		INT                     m_nVertsPerRow;     // 每行顶点数
		INT                     m_nVertsPerCol;     // 每列顶点数
		INT                     m_nNumVertices;     // 顶点总数
		FLOAT                   m_fTerrainWidth;    // 地形的宽度
		FLOAT                   m_fTerrainDepth;    // 地形的深度
		FLOAT                   m_fCellSpacing;     // 单元格的间距
		FLOAT                   m_fHeightScale;     // 高度缩放系数
		std::vector<FLOAT>      m_vHeightInfo;      // 高度信息

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