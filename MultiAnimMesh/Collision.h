/************************************************************************

��ײ�����ʹ�÷�����
	1��cCollision*	XXX;������ײ������
	2���ڳ�ʼ�������г�ʼ��ʵ������XXX = new cCollision();
	3������ȥ������ʹ�ã�XXX->destory();��ȥʵ������
	4������Ⱦ������ʹ������ķ���������ײ��⣺
		if ( XXX->CollisionTestA( g_pMeshColl->getStaticMesh() , g_pMeshTeapot, g_coll_mat_world, matTeapot )==TRUE ){
			������
		} 
		�����е��ĸ������ֱ�Ϊǰ������������󣬺��������������ı任����
		�����ṩ��CollisionTestA��������������򵥵���ײ��⡣
**************************************************************************/

#pragma once

#include "dxstdafx.h"

class cCollision{
	//variable
	private:
		// ��Ӻнṹ
		struct BOUNDINGBOX
		{
			D3DXVECTOR3 vMin;   // ��Ӻе���С����
			D3DXVECTOR3 vMax;   // ��Ӻе���󶥵�
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
		//������Χ����
		BOOL CollisionTestAABB(D3DXVECTOR3 vMin1, D3DXVECTOR3 vMax1, D3DXVECTOR3 vMin2, D3DXVECTOR3 vMax2);
		//�����������ȷ���
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
		//����AABB��Χ�����Ƿ������ڲ�
		BOOL CollisionTestPointAndAABB( D3DXVECTOR3 vPoint, D3DXVECTOR3 vMin, D3DXVECTOR3 vMax );
	public:
		cCollision();
		~cCollision();

		//A Method �Ǿ�ȷ��ⷽ����
		BOOL CollisionTestA(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo );
		//C Method ˫�����ǰ�Χ�忴�Ƿ���ײ��
		BOOL CollisionTestC(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo );
		BOOL CollisionTestC(D3DXVECTOR3 v1Min, D3DXVECTOR3 v1Max, D3DXVECTOR3 v2Min, D3DXVECTOR3 v2Max);

		void destory();
};