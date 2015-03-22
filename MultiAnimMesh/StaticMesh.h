/******************************************************************************

Author: Chao Shen
Date: 2012.3.16

Use The Class Method:
	��LODʹ�÷�����
	1: ʵ��������cStaticMesh* XXX��
	2����OnCreateDevice()�����д���һ������
		V_RETURN(XXX->create(pd3dDevice, L""));
	3����OnDestroyDevice()���������ٶ���
		XXX ->destory();
	4����OnFrameRander()�����л��ƶ���
		V(XXX ->rander());

	LOD��̬ģ��ʹ�÷�����
	1��ʵ��������cStaticMesh* XXX��
	2����OnCreateDevice()�����д���һ������
		V_RETURN(XXX->createLOD(pd3dDevice, L""));
	3����OnDestroyDevice()���������ٶ���
		XXX ->destoryLOD();
	4����OnFrameRander()�����л��ƶ���
		V(XXX ->randerLOD());
	5���Զ���������Ƭ����������setFaceNumbersLOD()������
		ע��������Ƭ��Ҫ����2���Ҳ��ܳ���ԭ��������Ƭ������Ȼ��Ч��

	//��ײ�����ʹ���ⲿ�����Χ�е�ʹ�÷�����
		1�����������������XXX��XXXBox
		2�������������������ʹ��һ���任����
		3����ʾʱֻ��ʾXXX���������ײ���ʱʹ��XXXBox�������
******************************************************************************/

#pragma once

#include "dxstdafx.h"

class cStaticMesh
{
	//variable
	public:
		// ��Ӻнṹ
		struct BOUNDINGBOX
		{
			D3DXVECTOR3 vMin;   // ��Ӻе���С����
			D3DXVECTOR3 vMax;   // ��Ӻе���󶥵�
		};

		D3DXVECTOR3				m_object_center;
		float					m_object_radius;
	private:
		IDirect3DDevice9*		m_device;
		//��ͳ����
		LPD3DXMESH				m_mesh;
		BOUNDINGBOX				m_meshBox;
		D3DMATERIAL9*			m_mesh_material;
		LPDIRECT3DTEXTURE9*		m_mesh_texture;
		DWORD					m_num_materials;
		/*D3DXVECTOR3				m_meshBoxVMin;
		D3DXVECTOR3				m_meshBoxVMax;*/

		//��������
		LPD3DXMESH				m_pMesh;		// �������
		BOUNDINGBOX				m_pMeshBox;	
		LPD3DXPMESH				m_pPMesh;		// �����������
		D3DMATERIAL9*			m_pMaterials;	// ����Ĳ�����Ϣ
		LPDIRECT3DTEXTURE9*		m_pTextures;	// �����������Ϣ
		DWORD					m_dwNumMtrls;   // ���ʵ���Ŀ
		BOOL					isLOD;			// ��֤һ��LOD��
		int						isNum;			// ���ٵ���Ƭ����
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
		HRESULT randerLOD( CONST D3DXMATRIX* mat_world);	//��ֵ����ʾ��ԭ����������ٶ����档
		void destory();
		void destoryLOD();

		LPD3DXMESH getStaticMesh();
		LPD3DXMESH getStaticMeshLOD();

		//using in collision.
		D3DXVECTOR3 getStaticMeshBoxVMin(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getStaticMeshBoxVMax(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getStaticMeshBoxVMinLOD(CONST D3DXMATRIX mat_world);
		D3DXVECTOR3 getStaticMeshBoxVMaxLOD(CONST D3DXMATRIX mat_world);

		//���ڳ�������ͨ����̬����λ������������
		//HRESULT setStaticMeshCenter( D3DXVECTOR3 center );
		D3DXVECTOR3 getStaticMeshCenter( CONST D3DXMATRIX mat_world );
		D3DXVECTOR3 getStaticMeshCenterLOD( CONST D3DXMATRIX mat_world );

		cStaticMesh();
		virtual ~cStaticMesh();
};
