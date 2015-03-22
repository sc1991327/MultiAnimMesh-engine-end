#include "dxstdafx.h"
#include "resource.h"
#include "SkinMesh.h"
#include "GradientMesh.h"
#include "StaticMesh.h"
#include "Terrain.h"
#include "Skybox.h"
#include "camera.h"
#include "Billboard.h"
#include "Collision.h"
#include"Frustum.h"

#pragma warning(disable : 4127 4995)

#define IDC_TOGGLE_FULLSCREEN	1
#define IDC_TOGGLE_REF			2
#define IDC_CHANGE_DEVICE		3

#define release_com(p)    do { if(p) { (p)->Release(); (p) = NULL; } } while(0)

//这三个对象用于控制文字显示。
ID3DXFont*					g_font;
ID3DXSprite*				g_text_sprite;
bool						g_show_help = true;

//这三个对象用于用户交互。
CDXUTDialogResourceManager	g_dlg_resource_manager;
CD3DSettingsDlg				g_settings_dlg;
CDXUTDialog					g_button_dlg;

//skin mesh objects
cSkinMesh*					g_skin_mesh;		//使用cSkinMesh()类的实例对象。
ID3DXAnimationSet*			g_anim_set;			//cSkinMesh实例的动画集控制对象。
D3DXMATRIX					g_skin1_mat_world;	//cSkinMesh实例的全局变换矩阵。
//gradient mesh objects
cGradientMesh*				g_gradient_mesh;	//使用cGradientMesh()类的实例对象。
D3DXMATRIX					g_gra1_mat_world;	//cGradientMesh实例的全局变换矩阵。
//static mesh objects
cStaticMesh*				g_static_mesh;		//使用cStaticMesh()类的实例对象。
D3DXMATRIX					g_sta1_mat_world;	//cStaticMesh实例的全局变换矩阵。
//static LOD mesh objects
cStaticMesh*				g_static_meshLOD;	//使用cStaticMeshLOD()类的实例对象
D3DXMATRIX					g_sta2_mat_world;	//cStaticMeshLOD实例的全局变换矩阵。
//terrain object
cTerrain*					g_terrain;			//实现地形的cTerrain()类的实例对象。
D3DXMATRIX					g_terr_mat_world;	//cTerrain实例的全局变换矩阵。
//skybox object
cSkybox*					g_skybox;			//使用天空盒的cSkyBox()类的实例对象。
D3DXMATRIX					g_sky_mat_world;	//cSkybox实例的全局变换矩阵。
//billboard object
cBillboard*					g_billboard;		//使用cBillboard()类的实例
D3DXMATRIX					g_bill_mat_world;	//cBillboard实例用到的View矩阵。

//camera
cCamera*					g_camera_one;				//第一人称摄像机cCamera（）类的实例对象
cCamera*					g_camera_three;				//第一人称摄像机cCamera（）类的实例对象
eCameraType					camera_type=LAND_OBJECT;	//摄像机的类型
float						time_delta=1;				//摄像机的变化量
//View Matrix
D3DXMATRIX					view_matrix;

//Collision object
cCollision*					g_collision;
//mesh object's matrix----此处用于碰撞检测测试，控制物体移动。
LPD3DXMESH					g_pMeshTeapot = NULL;
D3DXMATRIX					matTeapot;
cStaticMesh*				g_pMeshColl = NULL;
cStaticMesh*				g_pMeshCollBox = NULL;
D3DXMATRIX					g_coll_mat_world;
FLOAT						change_x = 0.0f;
FLOAT						change_y = 0.0f;
FLOAT						change_z = 0.0f;

// Frustum object used to test if object is in view.
CFrustum g_frustum;
bool isVisible=false;
// Matrices.
D3DXMATRIX mat_proj;


//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // Typically want to skip backbuffer formats that don't support alpha blending

    IDirect3D9* pD3D = DXUTGetD3DObject(); 

    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat, 
					D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	// If video card does not support hardware transform and light, then uses sofaware mode.
	if((pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0)
		pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// !!important, change vertex processing to mixed mode.
	if(pDeviceSettings->BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
		pDeviceSettings->BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;

	static bool is_first_time = true;

	if(is_first_time)
	{
		is_first_time = false;

		// if using reference device, then pop a warning message box.
		if(pDeviceSettings->DeviceType == D3DDEVTYPE_REF)
			DXUTDisplaySwitchingToREFWarning();
	}

    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
/******************************
一般对象的创建和初始化放在这里：
	D3DXCreateFont():创建文字
	g_skin_mesh->create():创建网格对象
******************************/
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, 
								 const D3DSURFACE_DESC* pBackBufferSurfaceDesc, 
								 void* pUserContext )
{
	HRESULT	hr;

	V_RETURN(g_dlg_resource_manager.OnCreateDevice(pd3dDevice));
	V_RETURN(g_settings_dlg.OnCreateDevice(pd3dDevice));

	D3DXCreateFont(pd3dDevice, 18, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
				   DEFAULT_PITCH | FF_DONTCARE, L"Arial", &g_font);
	
	//Create Skin Mesh
	g_skin_mesh = new cSkinMesh();
	V_RETURN(g_skin_mesh->create(pd3dDevice, L"Sources/SkinModels/tiny_4anim.x"));
	//Create Gradient Mesh
	g_gradient_mesh = new cGradientMesh();
	V_RETURN(g_gradient_mesh->create(pd3dDevice, L"Sources/GradientModels/Tween_Source.x", L"Sources/GradientModels/Tween_Target.x"));
	//Create Static Mesh
	g_static_mesh = new cStaticMesh();
	V_RETURN(g_static_mesh->create(pd3dDevice, L"Sources/StaticModels/tiny_4anim.x"));
	//Create LOD Static Mesh
	g_static_meshLOD = new cStaticMesh();
	V_RETURN(g_static_meshLOD->createLOD(pd3dDevice, L"Sources/StaticModelsLOD/bigship.x"));
	//初始化地形对象
	g_terrain = new cTerrain(pd3dDevice);
	V_RETURN(g_terrain->LoadTerrain(L"Sources/Terrain/mountain.raw", L"Sources/Terrain/mountain.bmp"));
	g_terrain->InitTerrain(64, 64, 10.0f, 0.3f);
	//初始化天空盒对象
	g_skybox = new cSkybox(pd3dDevice);
	V_RETURN(g_skybox->LoadSkybox(L"Sources/Skybox/skybox.dds"));
	g_skybox->InitSkybox(10, 10, 300);
	//初始化billboard对象。
	g_billboard = new cBillboard(pd3dDevice);
	V_RETURN(g_billboard->InitBillboard(L"Sources/Billboard/tree.dds",-1, g_terrain->getHeight(-1,0),0,2,3));
	//初始化摄像机
	g_camera_one = new cCamera(LAND_OBJECT);
	g_camera_three=new cCamera(AIR_CRAFT);
	//初始化碰撞检测测试用的物体对象
	g_collision = new cCollision();
	D3DXCreateTeapot(pd3dDevice, &g_pMeshTeapot, NULL);
	g_pMeshColl = new cStaticMesh();
	V_RETURN(g_pMeshColl->create(pd3dDevice, L"Sources/StaticModels/coll_o.x"));
	g_pMeshCollBox = new cStaticMesh();
	V_RETURN(g_pMeshCollBox->create(pd3dDevice, L"Sources/StaticModels/coll_b.x"));

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
/******************************
对View,Projection矩阵的设置，以及灯光材质的设置放在这里。
******************************/
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, 
								void* pUserContext )
{
	HRESULT hr;

	V_RETURN(g_dlg_resource_manager.OnResetDevice());
	V_RETURN(g_settings_dlg.OnResetDevice());
	V_RETURN(g_font->OnResetDevice());
	V_RETURN(D3DXCreateSprite(pd3dDevice, &g_text_sprite));

	// set dialog position and size

	g_button_dlg.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_button_dlg.SetSize(170, 170);

	// set projection matrix
	float aspect = (float)pBackBufferSurfaceDesc->Width / pBackBufferSurfaceDesc->Height;
	D3DXMatrixPerspectiveFovLH(&mat_proj, D3DX_PI/4, aspect, 1.0f, 2000.0f);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_proj);

	// setup material

	D3DMATERIAL9 material;
    ZeroMemory(&material, sizeof(D3DMATERIAL9));

    material.Diffuse.r = 0.5f;
    material.Diffuse.g = 0.5f;
    material.Diffuse.b = 0.8f;
    material.Diffuse.a = 1.0f;

	material.Ambient.r = 0.5f;
    material.Ambient.g = 0.5f;
    material.Ambient.b = 0.8f;
    material.Ambient.a = 1.0f;

    pd3dDevice->SetMaterial(&material);

	// setup light
    
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));

    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
	light.Range		 = 1000.0f;

	D3DXVECTOR3 light_dir(-1.0f, -1.0f, 1.0f);    
    D3DXVec3Normalize((D3DXVECTOR3*) &light.Direction, &light_dir);
    pd3dDevice->SetLight(0, &light);
    pd3dDevice->LightEnable(0, TRUE);
    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00505050);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	g_dlg_resource_manager.OnLostDevice();
	g_settings_dlg.OnLostDevice();
	g_font->OnLostDevice();

	release_com(g_text_sprite);
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
/******************************
对象的销毁放在这里：
	release_com(g_font);
	g_skin_mesh->destroy();
******************************/
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	g_dlg_resource_manager.OnDestroyDevice();
	g_settings_dlg.OnDestroyDevice();	

	release_com(g_font);

	//Distory Skin Mesh
	g_skin_mesh->destroy();
	//Distory Gradient Mesh
	g_gradient_mesh->destory();
	//Distory Static Mesh
	g_static_mesh ->destory();
	//Distory StaticLOD Mesh
	g_static_meshLOD ->destoryLOD();
	//Distory Terrain
	g_terrain->destory();
	//Distory Skybox
	g_skybox->distory();
	//Distory Billboard.
	g_billboard->destory();
	//release collision test object
	g_pMeshTeapot->Release();
	g_pMeshColl->destory();
	g_pMeshCollBox->destory();
	g_collision->destory();

}

//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
/******************************
矩阵变换放在这里
	包括全局变换矩阵，对象变换矩阵等
******************************/
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	//change Skin Mesh's world matrix
	D3DXMATRIX mat_trans, mat_rot_x, mat_rot_y;	
	D3DXMatrixRotationX(&mat_rot_x, -D3DX_PI / 2);
	D3DXMatrixRotationY(&mat_rot_y, timeGetTime() / 1000.0f);
	D3DXMatrixTranslation(&mat_trans, 0.0f, -250.0f, 0.0f);    
	g_skin1_mat_world = mat_rot_x * mat_rot_y * mat_trans;
	//change Gradient Mesh's world matrix
	g_gradient_mesh -> gradient();
	g_gra1_mat_world = mat_rot_x * mat_rot_y * mat_trans;
	//change Static Mesh's World Matrix
	//g_sta1_mat_world = mat_rot_x * mat_rot_y * mat_trans;
	D3DXMatrixTranslation(&g_sta1_mat_world, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&g_sta2_mat_world, 0.0f, 20.0f, 0.0f);
	//change Terrain Map Position
	D3DXMatrixTranslation(&g_terr_mat_world, 0.0f, 0.0f, 0.0f);
	//change Skybox Object Position
	D3DXMatrixTranslation(&g_sky_mat_world, 0.0f, 0.0f, 0.0f);

	//用于碰撞检测测试的物体变换矩阵
	D3DXMatrixTranslation(&matTeapot,  change_x, change_y, change_z);
	D3DXMatrixTranslation(&g_coll_mat_world, 0.0f, 20.0f, 0.0f);
	
}

//--------------------------------------------------------------------------------------
// Render the helper information
//--------------------------------------------------------------------------------------
void RenderText()
{
	CDXUTTextHelper text_helper(g_font, g_text_sprite, 20);
	
	text_helper.Begin();

	// show frame and device states
	text_helper.SetInsertionPos(5, 5);
	text_helper.SetForegroundColor( D3DXCOLOR(1.0f, 0.475f, 0.0f, 1.0f) );
	text_helper.DrawTextLine( DXUTGetFrameStats(true) );
	text_helper.DrawTextLine( DXUTGetDeviceStats() );

	// show helper information
	
	const D3DSURFACE_DESC* surface_desc = DXUTGetBackBufferSurfaceDesc();

	if(g_show_help)
	{
		text_helper.SetInsertionPos(10, surface_desc->Height - 15 * 10);
		text_helper.SetForegroundColor( D3DXCOLOR(1.0f, 0.475f, 0.0f, 1.0f) );
		text_helper.DrawTextLine(L"Controls (F1 to hide):");
		text_helper.DrawTextLine(L"Quit: ESC\n");

		text_helper.SetInsertionPos(10, surface_desc->Height - 15 * 6);		
		text_helper.DrawTextLine(L"1: walk");
		text_helper.DrawTextLine(L"2: jog");
		text_helper.DrawTextLine(L"3: loiter");
		text_helper.DrawTextLine(L"4: wave");
	}
	else
	{
		text_helper.SetInsertionPos(10, surface_desc->Height - 15 * 4);
		text_helper.SetForegroundColor( D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
		text_helper.DrawTextLine(L"Press F1 for help");
	}
	if (isVisible)
	{
		text_helper.SetInsertionPos(10, surface_desc->Height - 15 * 14);
		text_helper.SetForegroundColor( D3DXCOLOR(1.0f, 0.475f, 0.0f, 1.0f) );
		text_helper.DrawTextLine(L"visible");

	}
	else
	{
		text_helper.SetInsertionPos(10, surface_desc->Height - 15 * 14);
		text_helper.SetForegroundColor( D3DXCOLOR(1.0f, 0.475f, 0.0f, 1.0f) );
		text_helper.DrawTextLine(L"not visible");
	}
	text_helper.End();
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
/******************************
这里是真正的场景渲染的位置
	BeginScene(),EndScene()就在这里
	需要绘制的对象的绘制函数在这里被调用。
******************************/
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
    HRESULT hr;

	if(g_settings_dlg.IsActive())
	{
		g_settings_dlg.OnRender(fElapsedTime);
		return;
	}
	
	//handle camera.
	if(camera_type==AIR_CRAFT)
		view_matrix=g_camera_three->setup_camera(pd3dDevice,time_delta,camera_type,g_camera_three,view_matrix);
	else
		view_matrix=g_camera_one->setup_camera(pd3dDevice,time_delta,camera_type,g_camera_one,view_matrix);

	//Use fog
	//在这里可以调节雾化的参数达到好的效果。
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	//pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR);
	//static FLOAT fogStart = 50.0f, fogEnd = 300.0f, fogDensity = 0.01f;
	//pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0x00CCCCCC );
 //   pd3dDevice->SetRenderState( D3DRS_FOGSTART,   *(DWORD*)&fogStart );   // fogStart=50
 //   pd3dDevice->SetRenderState( D3DRS_FOGEND,     *(DWORD*)&fogEnd );     // fogEnd = 300
 //   pd3dDevice->SetRenderState( D3DRS_FOGDENSITY, *(DWORD*)&fogDensity ); // fogDensity = 0.01f

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {	
		//Draw Skin Mesh
		//V(g_skin_mesh->render(&g_skin1_mat_world, fElapsedTime));
		//Draw Gradient Mesh
		//V(g_gradient_mesh->render(&g_gra1_mat_world, 1, fElapsedTime));
		//Draw Static Mesh
		V(g_static_mesh ->rander(&g_sta1_mat_world));
		//Draw Static LOD Mesh
		/*g_static_meshLOD ->setFaceNumbersLOD(800);
		V(g_static_meshLOD ->randerLOD(&g_sta2_mat_world));
		D3DXVECTOR3 v1min = g_static_meshLOD->getStaticMeshBoxVMinLOD(g_sta2_mat_world);
		D3DXVECTOR3 v1max = g_static_meshLOD->getStaticMeshBoxVMaxLOD(g_sta2_mat_world);*/
		//Draw Terrain
		//V( g_terrain->DrawTerrain(&g_terr_mat_world) );
		//Draw Skybox
		//V( g_skybox->DrawSkybox(&g_sky_mat_world) );
		//Draw Billboard
		//g_bill_mat_world = view_matrix;
		//V( g_billboard->DrawBillboard(g_bill_mat_world) );
		//Draw Collision Test Object
		pd3dDevice->SetTransform(D3DTS_WORLD, &matTeapot);
		g_pMeshTeapot->DrawSubset(0);
		V(g_pMeshColl ->rander(&g_coll_mat_world));
		D3DXVECTOR3 v2min = g_pMeshColl->getStaticMeshBoxVMin(g_coll_mat_world);
		D3DXVECTOR3 v2max = g_pMeshColl->getStaticMeshBoxVMax(g_coll_mat_world);

		//Collision
		/*if ( g_collision->CollisionTestA( g_pMeshColl->getStaticMesh() , g_pMeshTeapot, g_coll_mat_world, matTeapot )==TRUE ){
			g_skybox->DrawSkybox(&g_sky_mat_world);
		} */
		/*if ( g_collision->CollisionTestC( g_skin_mesh->getSkinMesh() , g_pMeshTeapot, g_skin1_mat_world, matTeapot )==TRUE ){
			g_skybox->DrawSkybox(&g_sky_mat_world);
		} */
		/*if ( g_collision->CollisionTestC( v1min, v1max, v2min, v2max ) ){
			g_skybox->DrawSkybox(&g_sky_mat_world);
		}*/

		// Update the frustum (only needed if camera moved).
		D3DXMATRIX mv = g_sta1_mat_world * view_matrix;
		g_frustum.CalculateFrustum((float*)&mv,(float*)&mat_proj);

		// Test if object is visible.计算包围盒的长宽高
		D3DXVECTOR3 minvertex=g_static_mesh->getStaticMeshBoxVMin(g_sta1_mat_world);
		D3DXVECTOR3 maxvertex=g_static_mesh->getStaticMeshBoxVMax(g_sta1_mat_world);
		float height=(maxvertex.y-minvertex.y)/2;
		float len=(maxvertex.x-minvertex.x)/2;
		float width=(maxvertex.z-minvertex.z)/2;

		if(g_frustum.isBoxVisiable(g_static_mesh->getStaticMeshCenter(g_sta1_mat_world).x,
			g_static_mesh->getStaticMeshCenter(g_sta1_mat_world).y,
			g_static_mesh->getStaticMeshCenter(g_sta1_mat_world).z,
			height,width,len))
		{
			isVisible=true;
		}
		else
			isVisible=false;

		RenderText();
		//pd3dDevice->SetTransform(D3DTS_WORLD, &matTeapot);
		V(g_button_dlg.OnRender(fElapsedTime));

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	switch(uMsg)
	{
	//handle camera with mouse
	case WM_MOUSEMOVE:
		int x=LOWORD(lParam);
		int y=HIWORD(lParam);
		g_camera_one->mousemove(g_camera_one,0.01f,x,y);
		break;
	}

	*pbNoFurtherProcessing = g_dlg_resource_manager.MsgProc(hWnd, uMsg, wParam, lParam);
	if(*pbNoFurtherProcessing)
		return 0;

	if(g_settings_dlg.IsActive())
	{
		g_settings_dlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	*pbNoFurtherProcessing = g_button_dlg.MsgProc(hWnd, uMsg, wParam, lParam);
	if(*pbNoFurtherProcessing)
		return 0;

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle keybaord event
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboardProc(UINT charater, bool is_key_down, bool is_alt_down, void* user_context)
{
	if(is_key_down)
	{
		switch(charater)
		{
		case VK_F1:
			g_show_help = !g_show_help;
			break;

		case 49:	// press key "1"
			g_skin_mesh->m_anim_controller->GetAnimationSetByName("Walk", &g_anim_set);
			//g_skin_mesh->m_anim_controller->SetTrackAnimationSet(0, g_anim_set);	
			g_skin_mesh->SmoothChangeAnimation(g_skin_mesh->m_anim_controller, g_anim_set, 0);
			break;

		case 50:	// press key "2"
			g_skin_mesh->m_anim_controller->GetAnimationSetByName("Jog", &g_anim_set);
			//g_skin_mesh->m_anim_controller->SetTrackAnimationSet(0, g_anim_set);	
			g_skin_mesh->SmoothChangeAnimation(g_skin_mesh->m_anim_controller, g_anim_set, 0);
			break;	

		case 51:	// press key "3"
			g_skin_mesh->m_anim_controller->GetAnimationSetByName("Loiter", &g_anim_set);
			//g_skin_mesh->m_anim_controller->SetTrackAnimationSet(0, g_anim_set);	
			g_skin_mesh->SmoothChangeAnimation(g_skin_mesh->m_anim_controller, g_anim_set, 0);
			break;

		case 52:	// press key "4"
			g_skin_mesh->m_anim_controller->GetAnimationSetByName("Wave", &g_anim_set);
			//g_skin_mesh->m_anim_controller->SetTrackAnimationSet(0, g_anim_set);	
			g_skin_mesh->SmoothChangeAnimation(g_skin_mesh->m_anim_controller, g_anim_set, 0);
			break;

		case VK_F7:
			if(camera_type==LAND_OBJECT)
				camera_type=AIR_CRAFT;
			else
				camera_type=LAND_OBJECT;
			break;
		//用于碰撞检测测试，控制一个物体移动。
		case 73:	//i
			change_z = change_z + 1;
			break;
		case 75:	//k
			change_z = change_z - 1;
			break;
		case 74:	//j
			change_x = change_x - 1;
			break;
		case 76:	//l
			change_x = change_x + 1;
			break;
		case 85:	//u
			change_y = change_y + 1;
			break;
		case 79:	//o
			change_y = change_y - 1;
			break;
		}
	}
}

//--------------------------------------------------------------------------------------
// Handle events for controls
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT event, int control_id, CDXUTControl* control, void* user_context)
{
	switch(control_id)
	{
	case IDC_TOGGLE_FULLSCREEN:
		DXUTToggleFullScreen();
		break;

	case IDC_TOGGLE_REF:
		DXUTToggleREF();
		break;

	case IDC_CHANGE_DEVICE:
		g_settings_dlg.SetActive(true);
		break;
	}
}

//--------------------------------------------------------------------------------------
// Initialize dialogs
//--------------------------------------------------------------------------------------
void InitDialogs()
{
	g_settings_dlg.Init(&g_dlg_resource_manager);
	g_button_dlg.Init(&g_dlg_resource_manager);

	g_button_dlg.SetCallback(OnGUIEvent);

	int x = 35, y = 10, width = 125, height = 22;

	g_button_dlg.AddButton(IDC_TOGGLE_FULLSCREEN, L"Toggle full screen", x, y,	     width, height);
	g_button_dlg.AddButton(IDC_TOGGLE_REF,		  L"Toggle REF (F3)",	 x, y += 24, width, height);
	g_button_dlg.AddButton(IDC_CHANGE_DEVICE,	  L"Change device (F2)", x, y += 24, width, height, VK_F2);	
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
/******************************
主程序入口：
******************************/
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackKeyboard(OnKeyboardProc);
   
    // TODO: Perform any application-level initialization here
	InitDialogs();

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Multi Animation Mesh" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


