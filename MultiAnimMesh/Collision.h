/************************************************************************

碰撞检测类使用方法：
	1：cCollision*	XXX;创建碰撞检测对象。
	2：在初始化方法中初始化实例对象：XXX = new cCollision();
	3：在消去方法中使用：XXX->destory();消去实例对象。
	4：在渲染场景中使用下面的方法来做碰撞检测：
		if ( XXX->CollisionTestA( g_pMeshColl->getStaticMesh() , g_pMeshTeapot, g_coll_mat_world, matTeapot )==TRUE ){
			。。。
		} 
		方法中的四个参数分别为前两个是网格对象，后两个是网格对象的变换矩阵。
		这里提供了CollisionTestA（）方法用于最简单的碰撞检测。
**************************************************************************/

#pragma once

#include "dxstdafx.h"

class cCollision{
	//variable
	private:
		// 外接盒结构
		struct BOUNDINGBOX
		{
			D3DXVECTOR3 vMin;   // 外接盒的最小顶点
			D3DXVECTOR3 vMax;   // 外接盒的最大顶点
		};

		LPDIRECT3DDEVICE9		m_pd3dDevice;	//device object
		LPD3DXMESH				m_pMeshOne;		//mesh object
		LPD3DXMESH				m_pMeshTwo;		//mesh object
		BOUNDINGBOX				m_pBoxOne;		//bounding box object
		BOUNDINGBOX				m_pBoxTwo;		//bounding box object

		IDirect3DVertexBuffer9* m_pvb;			
		D3DXVECTOR3*			m_pvbOne;		
		D3DXVECTOR3*			m_pvbTwo;		
		DWORD*					m_pibOne;		
		DWORD*					m_pibTwo;		
	public:

	//method
	private:
		//两个包围体检测
		BOOL CollisionTestAABB(D3DXVECTOR3 vMin1, D3DXVECTOR3 vMax1, D3DXVECTOR3 vMin2, D3DXVECTOR3 vMax2);
		//两个网格对象精确检测
		BOOL CollisionTestMesh(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo);
		//method1
		BOOL CollisionTestTrangles( D3DXVECTOR3 vp11, D3DXVECTOR3 vp12, D3DXVECTOR3 vp21, D3DXVECTOR3 vp22, D3DXVECTOR3 vp23 );
		FLOAT Distance(D3DXVECTOR3 &p1, D3DXVECTOR3 &p2);
		FLOAT Area(FLOAT a, FLOAT b, FLOAT c);
		//method2
		BOOL CollisionTestLineAndTrainle(D3DXVECTOR3 vp11, D3DXVECTOR3 vp12, D3DXVECTOR3 vp21, D3DXVECTOR3 vp22, D3DXVECTOR3 vp23);
		FLOAT vector_dot( D3DXVECTOR3 v1, D3DXVECTOR3 v2 );
		void vector_minus( D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 res );
		void vector_cross( D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 res );
		//点与AABB包围体检测是否在其内部
		BOOL CollisionTestPointAndAABB( D3DXVECTOR3 vPoint, D3DXVECTOR3 vMin, D3DXVECTOR3 vMax );
	public:
		cCollision();
		~cCollision();

		//A Method 是精确检测方法。
		BOOL CollisionTestA(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo );
		//C Method 双方都是包围体看是否碰撞。
		BOOL CollisionTestC(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo );
		BOOL CollisionTestC(D3DXVECTOR3 v1Min, D3DXVECTOR3 v1Max, D3DXVECTOR3 v2Min, D3DXVECTOR3 v2Max);

		void destory();
};