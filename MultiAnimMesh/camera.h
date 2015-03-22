/********************************************************************************

ʹ��camera()
	1��ʵ����һ��������view����
		cCamera* XXX;	D3DXMATRIX aaa;
	2.ָ��camera���ͣ����仯����
		eCameraType;   time_delta;
	3����OnCreateDevice()�����г�ʼ����
		XXX = new cCamera(LAND_OBJECT);
	4�����Ƶ�����OnFrameRander()�����е���setup�������������Լ��̿��������
		V( XXX->setup_camera(pd3dDevice,time_delta,camera_type,g_camera_one,view_matrix));
	5���� MsgProc���������е���mousemove�������������������������

********************************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>
#include <dxstdafx.h>

enum eCameraType { LAND_OBJECT, AIR_CRAFT };

class cCamera
{
private:
	eCameraType		m_camera_type;
	D3DXVECTOR3		m_right;
	D3DXVECTOR3		m_up;
	D3DXVECTOR3		m_look;	

public:
	D3DXVECTOR3		m_pos;

public:
	cCamera();
	cCamera(eCameraType camera_type);
	~cCamera() { };

	void strafe(float units);	// left/right
	void fly(float units);		// up/down
	void walk(float units);		// forward/backward

	void pitch(float angle);	// rotate on right vector
	void yaw(float angle);		// rotate on up vector
	void roll(float angle);		// rotate on look vector

	void get_view_matrix(D3DXMATRIX* v);
	void set_camera_type(eCameraType camera_type);
	D3DXMATRIX setup_camera(IDirect3DDevice9* pd3dDevice,float time_delta,eCameraType camera_type,cCamera* g_camera,D3DXMATRIX view_matrix);
	void mousemove(cCamera* g_camera,float bianhualiang,int x,int y);
	
	void get_right(D3DXVECTOR3* right);
	void get_up(D3DXVECTOR3* up);
	void get_look(D3DXVECTOR3* look);
};

#endif
