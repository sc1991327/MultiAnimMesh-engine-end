/******************************************************************************

Author: Chao Shen
Date: 2012.3.16

Use The Class Method:
	非LOD使用方法：
	1: 实例化对象cStaticMesh* XXX；
	2：在OnCreateDevice()方法中创建一个对象
		V_RETURN(XXX->create(pd3dDevice, L""));
	3：在OnDestroyDevice()方法中销毁对象。
		XXX ->destory();
	4：在OnFrameRander()方法中绘制对象。
		V(XXX ->rander());

	LOD静态模型使用方法：
	1：实例化对象cStaticMesh* XXX；
	2：在OnCreateDevice()方法中创建一个对象
		V_RETURN(XXX->createLOD(pd3dDevice, L""));
	3：在OnDestroyDevice()方法中销毁对象。
		XXX ->destoryLOD();
	4：在OnFrameRander()方法中绘制对象。
		V(XXX ->randerLOD());
	5：自定义网格面片数量：调用setFaceNumbersLOD()方法。
		注意网格面片数要大于2，且不能超过原来网格面片数，不然无效。

	//碰撞检测中使用外部导入包围盒的使用方法：
		1：创建两个网格对象：XXX，XXXBox
		2：对上述两个网格对象使用一个变换矩阵。
		3：显示时只显示XXX网格对象，碰撞检测时使用XXXBox网格对象。
******************************************************************************/

#pragma once

#include "dxstdafx.h"

class cStaticMesh
{
	//variable
	public:
		// 外接盒结构
		struct BOUNDINGBOX
		{
			D3DXVECTOR3 vMin;   // 外接盒的最小顶点
			D3DXVECTOR3 vMax;   // 外接盒的最大顶点
		};

		D3DXVECTOR3				m_object_center;
		float					m_object_radius;
	private:
		IDirect3DDevice9*		m_device;
		//传统网格
		LPD3DXMESH				m_mesh;
		BOUNDINGBOX				m_meshBox;
		D3DMATERIAL9*			m_mesh_material;
		LPDIRECT3DTEXTURE9*		m_mesh_texture;
		DWORD					m_num_materials;
		/*D3DXVECTOR3				m_meshBoxVMin;
		D3DXVECTOR3				m_meshBoxVMax;*/

		//渐进网格
		LPD3DXMESH				m_pMesh;		// 网格对象
		BOUNDINGBOX				m_pMeshBox;	
		LPD3DXPMESH				m_pPMesh;		// 渐进网格对象
		D3DMATERIAL9*			m_pMaterials;	// 网格的材质信息
		LPDIRECT3DTEXTURE9*		m_pTextures;	// 网格的纹理信息
		DWORD					m_dwNumMtrls;   // 材质的数目
		BOOL					isLOD;			// 保证一次LOD化
		int						isNum;			// 减少的面片数量
		/*D3DXVECTOR3				m_pMeshBoxVMin;
		D3DXVECTOR3				m_pMeshBoxVMax;*/
	//method
	private:
		HRESULT load_from_file( CONST WCHAR* wfilename );

		HRESULT getStaticMeshBox();
		HRESULT getStaticMeshBoxLOD();
	public:
		HRESULT create( IDirect3DDevice9* device, CONST WCHAR* wfilename );
		HRESULT createLOD( IDirect3DDevice9* device, CONST WCHAR* wfilename );
		HRESULT rander( CONST D3DXMATRIX* mat_world );
		HRESULT setFaceNumbersLOD( int num );
		HRESULT randerLOD( CONST D3DXMATRIX* mat_world);	//正值：表示在原网格基础上少多少面。
		void destory();
		void destoryLOD();

		LPD3DXMESH getStaticMesh();
		LPD3DXMESH getStaticMeshLOD();

		//using in collision.
		D3DXVECTOR3 getStaticMeshBoxVMin(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getStaticMeshBoxVMax(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getStaticMeshBoxVMinLOD(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getStaticMeshBoxVMaxLOD(CONST D3DXMATRIX mat_world);

		//用于场景管理，通过静态物体位置来管理场景。
		//HRESULT setStaticMeshCenter( D3DXVECTOR3 center );
		D3DXVECTOR3 getStaticMeshCenter( CONST D3DXMATRIX mat_world );
		D3DXVECTOR3 getStaticMeshCenterLOD( CONST D3DXMATRIX mat_world );

		cStaticMesh();
		virtual ~cStaticMesh();
};
