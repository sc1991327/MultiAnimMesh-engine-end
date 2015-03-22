/**************************************************************************

Author: Chao Shen
Date: 2012.3.16

Use This Class Method:
	1：实例化一个对象cGradientMesh*	XXX;
	2：OnCreateDevice()方法中创建对象：
			XXX = new cGradientMesh();
			V_RETURN(g_gradient_mesh->create(pd3dDevice, L"xsource", L"xtarget"));
	3：OnFrameMove()方法中使用顶点变换方法，同时实现对象的整体变换。
			XXX->gradient();
			例：g_gra1_mat_world = mat_rot_x * mat_rot_y * mat_trans;
	4：OnFrameRander()方法中进行绘制。
		此处提供了三种绘制方法分别用于绘制原网格，目标网格，渐变动画。
			V(XXX->render(&g_gra1_mat_world, 2));
	5：OnDestroyDevice()函数中消除对象.
			XXX->destory();

**************************************************************************/

#pragma once

#include "dxstdafx.h"

class cGradientMesh
{
	struct sVertex{
		float x,y,z;
		float nx,ny,nz;
	};
	//Variable
	public:
		// 外接盒结构
		struct BOUNDINGBOX
		{
			D3DXVECTOR3 vMin;   // 外接盒的最小顶点
			D3DXVECTOR3 vMax;   // 外接盒的最大顶点
		};

		D3DXVECTOR3					m_object_center;
		float						m_object_radius;
		//还没有实现。
		int							m_object_state;	//状态控制：1原始网格，2渐变，3目标网格。
	private:
		IDirect3DDevice9*			m_device;
		ID3DXMesh*					m_source_mesh;
		ID3DXMesh*					m_target_mesh;
		ID3DXMesh*					m_result_mesh;
		BOUNDINGBOX					m_meshBox;
		IDirect3DVertexBuffer9*		m_source_vb;
		IDirect3DVertexBuffer9*		m_target_vb;
		IDirect3DVertexBuffer9*		m_result_vb;
		DWORD						m_num_materials_s;
		DWORD						m_num_materials_t;
		D3DMATERIAL9*				m_mesh_material_s;
		D3DMATERIAL9*				m_mesh_material_t;
		LPDIRECT3DTEXTURE9*			m_mesh_texture_s;
		LPDIRECT3DTEXTURE9*			m_mesh_texture_t;

	//Method
	public:
		cGradientMesh();
		virtual ~cGradientMesh();

		void destory();
		HRESULT create(IDirect3DDevice9* device, CONST WCHAR* wsourcefilename, CONST WCHAR* wtargetfilename);
		HRESULT gradient();
		HRESULT render(CONST D3DXMATRIX* mat_world, int object_state, double app_elapsed_time);

		LPD3DXMESH getSourceMesh(); 
		LPD3DXMESH getTargetMesh(); 
		LPD3DXMESH getResultMesh(); 
		
		D3DXVECTOR3 getSourceMeshBoxVMin(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getSourceMeshBoxVMax(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getTargetMeshBoxVMin(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getTargetMeshBoxVMax(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getResultMeshBoxVMin(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getResultMeshBoxVMax(CONST D3DXMATRIX mat_world);

		D3DXVECTOR3 getSourceMeshVCenter(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getTargetMeshVCenter(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getResultMeshVCenter(CONST D3DXMATRIX mat_world);

	private:
		HRESULT getSourceMeshBox();
		HRESULT getTargetMeshBox();
		HRESULT getResultMeshBox();

		HRESULT load_from_file(CONST WCHAR* wsourcefilename, CONST WCHAR* wtargetfilename);
};