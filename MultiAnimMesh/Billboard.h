/********************************************************************************

使用billboard()
	1：实例化一个对象及其变换矩阵。
		cBillboard* XXX;	D3DXMATRIX aaa;
	2：在OnCreateDevice()方法中初始化。
		后面的数字用来设定billboard长方形大小。
		XXX = new cBillboard();
		V_RETURN(XXX->InitBillboard(L"Sources/Billboard/tree.dds"));
	4：绘制地形在OnFrameRander()方法中
		g_bill_mat_world = view_matrix;得到View Matrix矩阵。
		V( XXX->DrawBillboard(g_bill_mat_world) );
	5: OnDestoryDevice()中销毁对象。
		XXX->Destory();

********************************************************************************/

#pragma once

#include "dxstdafx.h"

class cBillboard{
	//variable
	private:
		LPDIRECT3DDEVICE9			m_pd3dDevice;		//inner device object
		LPDIRECT3DVERTEXBUFFER9     m_pTreeVB;			// 树木顶点缓冲区
		LPDIRECT3DTEXTURE9          m_pTreeTex;			// 树木纹理
		D3DXMATRIX					m_matView;			//View Matrix.

		struct  CUSTOMVERTEX							//custom struct.Use to store vertex arcthiecture.
		{
			FLOAT _x, _y, _z;   // 顶点位置
			FLOAT _u, _v ;      // 顶点纹理坐标
			CUSTOMVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v)
				: _x(x), _y(y), _z(z), _u(u), _v(v) {}
		};
		#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_TEX1)
	public:


	//method
	private:

	public:
		cBillboard( IDirect3DDevice9 *pd3dDevice );
		~cBillboard();

		HRESULT InitBillboard( wchar_t *pFileName, float downX, float downY, float downZ ,float weight, float height );
		HRESULT DrawBillboard( D3DXMATRIX m_matView );
		HRESULT destory();
};