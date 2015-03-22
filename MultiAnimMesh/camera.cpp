#include "dxstdafx.h"
#include "camera.h"

int forex=0,forey=0;

cCamera::cCamera()
{
	m_camera_type = AIR_CRAFT;

	m_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_up	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

cCamera::cCamera(eCameraType camera_type)
{
	m_camera_type = camera_type;

	if(m_camera_type==LAND_OBJECT)
	{
		m_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		m_up	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		m_pos   = D3DXVECTOR3(0.0f, 200.0f, 0.0f);
		m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		m_up	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		m_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
}

void cCamera::get_right(D3DXVECTOR3* right)
{
	*right = m_right;
}

void cCamera::get_up(D3DXVECTOR3* up)
{
	*up = m_up;
} 

void cCamera::get_look(D3DXVECTOR3* look)
{
	*look = m_look;
}

//forward-back
void cCamera::walk(float units)
{
	// move only on xz plane for land object
	if(m_camera_type == LAND_OBJECT)
		m_pos += D3DXVECTOR3(m_look.x, 0.0f, m_look.z) * units;
	else	// AIR_CRAFT
		m_pos += m_look * units;
}

//right-left
void cCamera::strafe(float units)
{
	// move only on xz plane for land object
	if(m_camera_type == LAND_OBJECT)
		m_pos += D3DXVECTOR3(m_right.x, 0.0f, m_right.z) * units;
	else	// AIR_CRAFT
		m_pos += m_right * units;
}

//up-down
void cCamera::fly(float units)
{
	// move only on y_axis for land object
	if(m_camera_type == LAND_OBJECT)
		m_pos.y += units;
	else
		m_pos += m_up * units;
}

//rotate-right-left
void cCamera::pitch(float angle)
{
	D3DXMATRIX transform_matrix;
	D3DXMatrixRotationAxis(&transform_matrix, &m_right, angle);

	// rotate m_up and m_look around m_right vector
	D3DXVec3TransformCoord(&m_up,   &m_up,   &transform_matrix);
	D3DXVec3TransformCoord(&m_look, &m_look, &transform_matrix);
}

//rotate-up-down
void cCamera::yaw(float angle)
{
	D3DXMATRIX transform_matrix;

	// rotate around world y-axis (0, 1, 0) always for land object
	if(m_camera_type == LAND_OBJECT)
		D3DXMatrixRotationY(&transform_matrix, angle);
	else	// rotate around own up vector for aircraft
		D3DXMatrixRotationAxis(&transform_matrix, &m_up, angle);

	// rotate m_right and m_look around m_up or y-axis
	D3DXVec3TransformCoord(&m_right, &m_right, &transform_matrix);
	D3DXVec3TransformCoord(&m_look,  &m_look,  &transform_matrix);
}

//dianbo-rotate
void cCamera::roll(float angle)
{
	// only roll for aircraft type
	if(m_camera_type == AIR_CRAFT)
	{
		D3DXMATRIX transform_matrix;
		D3DXMatrixRotationAxis(&transform_matrix, &m_look, angle);

		// rotate m_up and m_right around m_look vector
		D3DXVec3TransformCoord(&m_up,	 &m_up,		&transform_matrix);	//得到结构变换后的向量
		D3DXVec3TransformCoord(&m_right, &m_right,  &transform_matrix);
	}
}

//cange View Matrix
void cCamera::get_view_matrix(D3DXMATRIX* v)
{
	// keep camera's axis orthogonal to each other
	D3DXVec3Normalize(&m_look, &m_look);	//返回规格化向量

	D3DXVec3Cross(&m_up, &m_look, &m_right);
	D3DXVec3Normalize(&m_up, &m_up);

	D3DXVec3Cross(&m_right, &m_up, &m_look);	//叉乘
	D3DXVec3Normalize(&m_right, &m_right);

	// build the view matrix
	float x = -D3DXVec3Dot(&m_right, &m_pos);	//点乘
	float y = -D3DXVec3Dot(&m_up,    &m_pos);
	float z = -D3DXVec3Dot(&m_look,  &m_pos);

	(*v)(0, 0) = m_right.x;		(*v)(0, 1) = m_up.x;	(*v)(0, 2) = m_look.x;		(*v)(0, 3) = 0.0f;
	(*v)(1, 0) = m_right.y;		(*v)(1, 1) = m_up.y;	(*v)(1, 2) = m_look.y;		(*v)(1, 3) = 0.0f;
	(*v)(2, 0) = m_right.z;		(*v)(2, 1) = m_up.z;	(*v)(2, 2) = m_look.z;		(*v)(2, 3) = 0.0f;
	(*v)(3, 0) = x;				(*v)(3, 1) = y;			(*v)(3, 2) = z;				(*v)(3, 3) = 1.0f;
}

//choose use the first or the tree 
void cCamera::set_camera_type(eCameraType camera_type)
{
	m_camera_type = camera_type;
}

void cCamera::mousemove(cCamera* g_camera,float bianhualiang,int x,int y)
{
	if(y>forey)
	{
		g_camera->pitch(bianhualiang);
		forey=y;
	}
	if(y<forey)
	{
		g_camera->pitch(-bianhualiang);
		forey=y;
	}
	if(x>forex)
	{
		g_camera->yaw(bianhualiang);
		forex=x;
	}
	if(x<forex)
	{
		g_camera->yaw(-bianhualiang);
		forex=x;
	}
}

//handle user operations for camera. 
D3DXMATRIX cCamera::setup_camera(IDirect3DDevice9* pd3dDevice,float time_delta,eCameraType camera_type,cCamera* g_camera,D3DXMATRIX view_matrix)
{
	// update the camera

	float change_value = 4.0f*time_delta;
	if(camera_type==LAND_OBJECT)
	{
		if(GetAsyncKeyState('W') & 0x80000f)
			g_camera->walk(change_value);

		if( GetAsyncKeyState('S') & 0x8000f )
			g_camera->walk(-change_value);

		if( GetAsyncKeyState('A') & 0x8000f )
			g_camera->strafe(-change_value);

		if( GetAsyncKeyState('D') & 0x8000f )
			g_camera->strafe(change_value);

		if( GetAsyncKeyState('R') & 0x8000f )
			g_camera->fly(change_value);

		if( GetAsyncKeyState('F') & 0x8000f )
			g_camera->fly(-change_value);

		change_value=0.01f*time_delta;

		if( GetAsyncKeyState(VK_UP) & 0x8000f )
			g_camera->pitch(change_value);

		if( GetAsyncKeyState(VK_DOWN) & 0x8000f )
			g_camera->pitch(-change_value);

		if( GetAsyncKeyState(VK_LEFT) & 0x8000f )
			g_camera->yaw(-change_value);

		if( GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			g_camera->yaw(change_value);

		// update the view matrix representing the camera's new position/orientation
		g_camera->get_view_matrix(&view_matrix);
		pd3dDevice->SetTransform(D3DTS_VIEW, &view_matrix);
	}
	else
	{
		if(GetAsyncKeyState('W') & 0x80000f)
			g_camera->walk(change_value);

		if( GetAsyncKeyState('S') & 0x8000f )
			g_camera->walk(-change_value);

		if( GetAsyncKeyState('A') & 0x8000f )
			g_camera->strafe(-change_value);

		if( GetAsyncKeyState('D') & 0x8000f )
			g_camera->strafe(change_value);

		if( GetAsyncKeyState('R') & 0x8000f )
			g_camera->fly(change_value);

		if( GetAsyncKeyState('F') & 0x8000f )
			g_camera->fly(-change_value);

		change_value=0.01f*time_delta;

		if( GetAsyncKeyState(VK_UP) & 0x8000f )
			g_camera->pitch(change_value);

		if( GetAsyncKeyState(VK_DOWN) & 0x8000f )
			g_camera->pitch(-change_value);

		if( GetAsyncKeyState(VK_LEFT) & 0x8000f )
			g_camera->yaw(-change_value);

		if( GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			g_camera->yaw(change_value);
		if( GetAsyncKeyState('N') & 0x8000f )
			g_camera->roll(change_value);

		if( GetAsyncKeyState('M') & 0x8000f )
			g_camera->roll(-change_value);

		// update the view matrix representing the camera's new position/orientation
		g_camera->get_view_matrix(&view_matrix);
		pd3dDevice->SetTransform(D3DTS_VIEW, &view_matrix);
	}
	return view_matrix;
}