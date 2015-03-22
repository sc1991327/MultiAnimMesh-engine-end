/********************************************************************************

ʹ��billboard()
	1��ʵ����һ��������任����
		cBillboard* XXX;	D3DXMATRIX aaa;
	2����OnCreateDevice()�����г�ʼ����
		��������������趨billboard�����δ�С��
		XXX = new cBillboard();
		V_RETURN(XXX->InitBillboard(L"Sources/Billboard/tree.dds"));
	4�����Ƶ�����OnFrameRander()������
		g_bill_mat_world = view_matrix;�õ�View Matrix����
		V( XXX->DrawBillboard(g_bill_mat_world) );
	5: OnDestoryDevice()�����ٶ���
		XXX->Destory();

********************************************************************************/

#pragma once

#include "dxstdafx.h"

class cBillboard{
	//variable
	private:
		LPDIRECT3DDEVICE9			m_pd3dDevice;		//inner device object
		LPDIRECT3DVERTEXBUFFER9     m_pTreeVB;			// ��ľ���㻺����
		LPDIRECT3DTEXTURE9          m_pTreeTex;			// ��ľ����
		D3DXMATRIX					m_matView;			//View Matrix.

		struct  CUSTOMVERTEX							//custom struct.Use to store vertex arcthiecture.
		{
			FLOAT _x, _y, _z;   // ����λ��
			FLOAT _u, _v ;      // ������������
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