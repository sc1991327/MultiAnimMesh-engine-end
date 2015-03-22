/****************************************************************************************

Author: Chao Shen
Date: 2012.3.16

Use This Class Method:
	1��ʵ����һ������cSkinMesh* XXX;
	2��OnCreateDevice()�����д�������
		XXX = new cSkinMesh();
		V_RETURN(XXX->create(pd3dDevice, L"tiny_4anim.x"));
	3��OnDestroyDevice()��������������
		XXX->destroy();
	4��OnFrameRender()�����л��ƶ���
		V(XXX->render(&g_skin1_mat_world, fElapsedTime));

	5���������Ʒ�������OnKeyboardProc()�����жԲ�ͬ�������ò�ͬ��������
		ǰ����Ҫ֪��.X�ļ����ж��ٶ�������������Ϊʲô��
		����g_skin_mesh->m_anim_controller->GetAnimationSetByName("Walk", &g_anim_set);
			g_skin_mesh->m_anim_controller->SetTrackAnimationSet(0, g_anim_set);	

****************************************************************************************/

#pragma once

#include "AllocateHierarchy.h"

class cSkinMesh
{
public:
	// ��Ӻнṹ
	struct BOUNDINGBOX
	{
		D3DXVECTOR3 vMin;   // ��Ӻе���С����
		D3DXVECTOR3 vMax;   // ��Ӻе���󶥵�
	};
	//�⼸���������������п���ʹ�����������񶯻���
	D3DXVECTOR3					m_object_center;	//���������λ��
	float						m_object_radius;	//����������ת�Ƕ�
	bool						m_is_play_anim;		//�Ƿ񲥷Ŷ���Ч��
	ID3DXAnimationController*	m_anim_controller;	//���������ſ��ƶ���ָ�롣

private:
	cAllocateHierarchy*			m_alloc_hierarchy;	//ʵ��AllocateHierarchy()�ӿڵĶ���
	IDirect3DDevice9*			m_device;			//�豸����
	D3DXFRAME*					m_root_frame;		//�����
	LPD3DXMESH					m_output_mesh;
	BOUNDINGBOX					m_meshBox;

private:
	HRESULT setup_bone_matrix_pointers(D3DXFRAME* frame);
	HRESULT setup_bone_matrix_pointers_on_mesh(D3DXMESHCONTAINER* base_mesh_container);	

	HRESULT	load_from_xfile(CONST WCHAR* wfilename);	//����X�ļ���
	void update_frame_matrices(D3DXFRAME* base_frame, CONST D3DXMATRIX* parent_matrix);	//���µõ�1�����ÿ�������ı任����

	HRESULT draw_frame(CONST D3DXFRAME* frame);		//����draw_mesh_container()����������Ƥ����
	HRESULT draw_mesh_container(CONST D3DXMESHCONTAINER* base_mesh_container, CONST D3DXFRAME* base_frame);

	HRESULT getSkinMeshBox();
public:
	//�����ⲿ�������������п��Ա����õĺ�����
	HRESULT create(IDirect3DDevice9* device, CONST WCHAR* wfilename);		//���ڳ�ʼ������Mesh����ĺ�����
	HRESULT render(CONST D3DXMATRIX* mat_world, double app_elapsed_time);	//����BeginScene()�еĻ���Mesh����ĺ�����
	void destroy();
	
	//smooth animation.
	void SmoothChangeAnimation( LPD3DXANIMATIONCONTROLLER pAnimController, LPD3DXANIMATIONSET pAnimSet, FLOAT fCurrTime );

	//get mesh
	LPD3DXMESH	getSkinMesh();
	D3DXVECTOR3 getSkinMeshBoxVMin(CONST D3DXMATRIX mat_world);
	D3DXVECTOR3 getSkinMeshBoxVMax(CONST D3DXMATRIX mat_world);
	D3DXVECTOR3 getSkinMeshBoxCenter( CONST D3DXMATRIX mat_world );

	cSkinMesh();
	virtual ~cSkinMesh();
};


