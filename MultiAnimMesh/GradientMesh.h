/**************************************************************************

Author: Chao Shen
Date: 2012.3.16

Use This Class Method:
	1��ʵ����һ������cGradientMesh*	XXX;
	2��OnCreateDevice()�����д�������
			XXX = new cGradientMesh();
			V_RETURN(g_gradient_mesh->create(pd3dDevice, L"xsource", L"xtarget"));
	3��OnFrameMove()������ʹ�ö���任������ͬʱʵ�ֶ��������任��
			XXX->gradient();
			����g_gra1_mat_world = mat_rot_x * mat_rot_y * mat_trans;
	4��OnFrameRander()�����н��л��ơ�
		�˴��ṩ�����ֻ��Ʒ����ֱ����ڻ���ԭ����Ŀ�����񣬽��䶯����
			V(XXX->render(&g_gra1_mat_world, 2));
	5��OnDestroyDevice()��������������.
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
		// ��Ӻнṹ
		struct BOUNDINGBOX
		{
			D3DXVECTOR3 vMin;   // ��Ӻе���С����
			D3DXVECTOR3 vMax;   // ��Ӻе���󶥵�
		};

		D3DXVECTOR3					m_object_center;
		float						m_object_radius;
		//��û��ʵ�֡�
		int							m_object_state;	//״̬���ƣ�1ԭʼ����2���䣬3Ŀ������
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