/****************************************************************************************

Author: Chao Shen
Date: 2012.3.16

Use This Class Method:
	1：实例化一个对象，cSkinMesh* XXX;
	2：OnCreateDevice()方法中创建对象：
		XXX = new cSkinMesh();
		V_RETURN(XXX->create(pd3dDevice, L"tiny_4anim.x"));
	3：OnDestroyDevice()方法中消除对象：
		XXX->destroy();
	4：OnFrameRender()函数中绘制对象：
		V(XXX->render(&g_skin1_mat_world, fElapsedTime));

	5：动画控制方法：在OnKeyboardProc()方法中对不同按键调用不同动画集。
		前提是要知道.X文件中有多少动画集且其命名为什么。
		例：g_skin_mesh->m_anim_controller->GetAnimationSetByName("Walk", &g_anim_set);
			g_skin_mesh->m_anim_controller->SetTrackAnimationSet(0, g_anim_set);	

****************************************************************************************/

#pragma once

#include "AllocateHierarchy.h"

class cSkinMesh
{
public:
	// 外接盒结构
	struct BOUNDINGBOX
	{
		D3DXVECTOR3 vMin;   // 外接盒的最小顶点
		D3DXVECTOR3 vMax;   // 外接盒的最大顶点
	};
	//这几个参数在主程序中可以使用来控制网格动画。
	D3DXVECTOR3					m_object_center;	//物体的中心位置
	float						m_object_radius;	//物体整体旋转角度
	bool						m_is_play_anim;		//是否播放动画效果
	ID3DXAnimationController*	m_anim_controller;	//动画集播放控制对象指针。

private:
	cAllocateHierarchy*			m_alloc_hierarchy;	//实现AllocateHierarchy()接口的对象。
	IDirect3DDevice9*			m_device;			//设备对象
	D3DXFRAME*					m_root_frame;		//根框架
	LPD3DXMESH					m_output_mesh;
	BOUNDINGBOX					m_meshBox;

private:
	HRESULT setup_bone_matrix_pointers(D3DXFRAME* frame);
	HRESULT setup_bone_matrix_pointers_on_mesh(D3DXMESHCONTAINER* base_mesh_container);	

	HRESULT	load_from_xfile(CONST WCHAR* wfilename);	//加载X文件。
	void update_frame_matrices(D3DXFRAME* base_frame, CONST D3DXMATRIX* parent_matrix);	//更新得到1框架中每个骨骼的变换矩阵

	HRESULT draw_frame(CONST D3DXFRAME* frame);		//调用draw_mesh_container()函数绘制蒙皮网格。
	HRESULT draw_mesh_container(CONST D3DXMESHCONTAINER* base_mesh_container, CONST D3DXFRAME* base_frame);

	HRESULT getSkinMeshBox();
public:
	//用于外部，在主程序框架中可以被调用的函数。
	HRESULT create(IDirect3DDevice9* device, CONST WCHAR* wfilename);		//用于初始化加载Mesh对象的函数。
	HRESULT render(CONST D3DXMATRIX* mat_world, double app_elapsed_time);	//用于BeginScene()中的绘制Mesh对象的函数。
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


