#include "dxstdafx.h"
#include "SkinMesh.h"

#pragma warning(disable : 4127)

#define release_com(p)    do { if(p) { (p)->Release(); (p) = NULL; } } while(0)

cSkinMesh::cSkinMesh()
{
	m_is_play_anim	  = true;
	m_device		  = NULL;
	m_anim_controller = NULL;
	m_root_frame	  = NULL;
	m_output_mesh	  = NULL;
	m_alloc_hierarchy = new cAllocateHierarchy();
}

cSkinMesh::~cSkinMesh()
{
	D3DXFrameDestroy(m_root_frame, m_alloc_hierarchy);
	release_com(m_anim_controller);

	delete m_alloc_hierarchy;
}

HRESULT cSkinMesh::create(IDirect3DDevice9* device, CONST WCHAR* wfilename)
{
	m_device = device;

	HRESULT hr;
	V_RETURN(load_from_xfile(wfilename));

	return S_OK;
}

void cSkinMesh::destroy()
{
	delete this;
}

HRESULT cSkinMesh::setup_bone_matrix_pointers(D3DXFRAME* frame)
{	
	HRESULT hr;

	if(frame == NULL)
		return S_OK;

	if(frame->pMeshContainer != NULL)
	{
		hr = setup_bone_matrix_pointers_on_mesh(frame->pMeshContainer);

		if(FAILED(hr))
			return hr;
	}

	if(frame->pFrameSibling != NULL)
	{
		hr = setup_bone_matrix_pointers(frame->pFrameSibling);

		if(FAILED(hr))
			return hr;
	}

	if(frame->pFrameFirstChild != NULL)
	{
		hr = setup_bone_matrix_pointers(frame->pFrameFirstChild);

		if(FAILED(hr))
			return hr;
	}
	
	return S_OK;	
}

HRESULT cSkinMesh::setup_bone_matrix_pointers_on_mesh(D3DXMESHCONTAINER* base_mesh_container)
{
	if(base_mesh_container == NULL)
		return S_OK;

	D3DXMESHCONTAINER_DERIVED* mesh_container = (D3DXMESHCONTAINER_DERIVED*) base_mesh_container;

	if(mesh_container->pSkinInfo != NULL)
	{
		UINT num_bones = mesh_container->pSkinInfo->GetNumBones();
		mesh_container->ppBoneMatrices = new D3DXMATRIX*[num_bones];

		if(mesh_container->ppBoneMatrices == NULL)
			return E_OUTOFMEMORY;

		for(UINT i = 0; i < num_bones; i++)
		{
			LPCSTR bone_name = mesh_container->pSkinInfo->GetBoneName(i);
			D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*) D3DXFrameFind(m_root_frame, bone_name);

			if(frame == NULL)
				return E_FAIL;

			mesh_container->ppBoneMatrices[i] = &frame->CombinedTransformMatrix;
		}
	}

	return S_OK;
}

HRESULT cSkinMesh::load_from_xfile(CONST WCHAR* wfilename)
{
	HRESULT hr;

	WCHAR wpath[MAX_PATH];
	DXUTFindDXSDKMediaFileCch(wpath, sizeof(wpath) / sizeof(WCHAR), wfilename);

	V_RETURN(D3DXLoadMeshHierarchyFromXW(wpath, D3DXMESH_MANAGED, m_device, m_alloc_hierarchy, NULL, &m_root_frame,
										 &m_anim_controller));

	V_RETURN(setup_bone_matrix_pointers(m_root_frame));

	V_RETURN(D3DXFrameCalculateBoundingSphere(m_root_frame, &m_object_center, &m_object_radius));

	return S_OK;
}

void cSkinMesh::update_frame_matrices(D3DXFRAME* base_frame, CONST D3DXMATRIX* parent_matrix)
{
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*) base_frame;

	if(parent_matrix != NULL)
		D3DXMatrixMultiply(&frame->CombinedTransformMatrix, &frame->TransformationMatrix, parent_matrix);
	else
		frame->CombinedTransformMatrix = frame->TransformationMatrix;

	if(frame->pFrameSibling != NULL)
		update_frame_matrices(frame->pFrameSibling, parent_matrix);

	if(frame->pFrameFirstChild != NULL)
		update_frame_matrices(frame->pFrameFirstChild, &frame->CombinedTransformMatrix);
}

HRESULT cSkinMesh::draw_mesh_container(CONST D3DXMESHCONTAINER* base_mesh_container, CONST D3DXFRAME* base_frame)
{
	HRESULT hr;

	D3DXMESHCONTAINER_DERIVED* mesh_container = (D3DXMESHCONTAINER_DERIVED*) base_mesh_container;
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*) base_frame;

	if(mesh_container->pSkinInfo != NULL)
	{
		if(mesh_container->UseSoftwareVP)
		{
			V_RETURN(m_device->SetSoftwareVertexProcessing(TRUE));
		}
		
		if(mesh_container->MaxBonesInflPerVertex)
			m_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);

		if(mesh_container->MaxBonesInflPerVertex == 1)
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
		else if(mesh_container->MaxBonesInflPerVertex == 2)
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS);
		else if(mesh_container->MaxBonesInflPerVertex == 3)
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_2WEIGHTS);
		else
			m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS);

		D3DXBONECOMBINATION* bone_comb = (D3DXBONECOMBINATION*) mesh_container->pBoneCombBuffer->GetBufferPointer();

		for(UINT attr_index = 0; attr_index < mesh_container->NumAttrGroups; attr_index++)
		{
			// setup vertex index blending world matrix for every mesh group
			for(UINT palette_index = 0; palette_index < mesh_container->NumMatrixPalettes; palette_index++)
			{
				UINT matrix_index = bone_comb[attr_index].BoneId[palette_index];

				if(matrix_index != UINT_MAX)
				{
					D3DXMATRIX mat_palette;

					D3DXMatrixMultiply(&mat_palette,
									   mesh_container->ppBoneOffsetMatrices[matrix_index],
									   mesh_container->ppBoneMatrices[matrix_index]);

					m_device->SetTransform(D3DTS_WORLDMATRIX(palette_index), &mat_palette);
				}
			}

			DWORD attr_id = bone_comb[attr_index].AttribId;

			m_device->SetMaterial(&mesh_container->pMaterials[attr_id].MatD3D);
			m_device->SetTexture(0, mesh_container->ppTextures[attr_id]);

			mesh_container->MeshData.pMesh->DrawSubset(attr_index);
		}

		// restore render state

		m_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

		if(mesh_container->UseSoftwareVP)
		{
			V_RETURN(m_device->SetSoftwareVertexProcessing(FALSE));
		}
	}
	else
	{
		m_device->SetTransform(D3DTS_WORLD, &frame->CombinedTransformMatrix);

		for(UINT i = 0; i < mesh_container->NumMaterials; i++)
		{
			m_device->SetMaterial(&mesh_container->pMaterials[i].MatD3D);
			m_device->SetTexture(0, mesh_container->ppTextures[i]);

			mesh_container->MeshData.pMesh->DrawSubset(i);
		}
	}

	return S_OK;
}

HRESULT cSkinMesh::draw_frame(CONST D3DXFRAME* frame)
{
	HRESULT hr;
	D3DXMESHCONTAINER* mesh_container = frame->pMeshContainer;

	while(mesh_container != NULL)
	{
		m_output_mesh = mesh_container->MeshData.pMesh;	//get output mesh.
		V_RETURN(draw_mesh_container(mesh_container, frame));
		mesh_container = mesh_container->pNextMeshContainer;
	}

	if(frame->pFrameSibling != NULL)
		draw_frame(frame->pFrameSibling);

	if(frame->pFrameFirstChild != NULL)
		draw_frame(frame->pFrameFirstChild);

	return S_OK;
}

HRESULT cSkinMesh::render(CONST D3DXMATRIX* mat_world, double app_elapsed_time)
{
	HRESULT hr;

	if(0.0f == app_elapsed_time)
		return S_OK;

	if(m_is_play_anim && m_anim_controller != NULL)
		m_anim_controller->AdvanceTime(app_elapsed_time, NULL);

	update_frame_matrices(m_root_frame, mat_world);
	
	V_RETURN(draw_frame(m_root_frame));

	return S_OK;
}

void cSkinMesh::SmoothChangeAnimation( LPD3DXANIMATIONCONTROLLER pAnimController, LPD3DXANIMATIONSET pAnimSet, FLOAT fCurrTime ){
	
	
	static DWORD dwOldTrack = 0;    // 原Track号
    static DWORD dwNewTrack = 1;    // 新Track号

    dwNewTrack  = dwOldTrack == 0 ? 1 : 0;  // 与原Trace号的相对
    pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );  // 将此动画集设置加入到Track中

    pAnimController->UnkeyAllTrackEvents( dwOldTrack );     // 清除原轨道上的事件
    pAnimController->UnkeyAllTrackEvents( dwNewTrack );     // 清除新轨道上的事件

    pAnimController->KeyTrackEnable( dwOldTrack, FALSE, fCurrTime + 0.25f );    // 关闭原轨道
    pAnimController->KeyTrackSpeed(  dwOldTrack, 0.0f,  fCurrTime, 0.25f, D3DXTRANSITION_LINEAR );  // 改变原轨道的速率
    pAnimController->KeyTrackWeight( dwOldTrack, 0.0f,  fCurrTime, 0.25f, D3DXTRANSITION_LINEAR );  // 改变原轨道的权重

    pAnimController->SetTrackEnable( dwNewTrack, TRUE );    // 启用新轨道
    pAnimController->KeyTrackSpeed(  dwNewTrack, 1.0f,  fCurrTime, 0.25f, D3DXTRANSITION_LINEAR );  // 改变新轨道的速率
    pAnimController->KeyTrackWeight( dwNewTrack, 1.0f,  fCurrTime, 0.25f, D3DXTRANSITION_LINEAR );  // 改变新轨道的权重

    dwOldTrack = dwNewTrack;        // 将新轨道设置为当前轨道，为下次变换做准备
}

LPD3DXMESH cSkinMesh::getSkinMesh(){
	return m_output_mesh;
}

HRESULT cSkinMesh::getSkinMeshBox(){
	LPD3DXMESH meshbox;
	meshbox = m_output_mesh;

	D3DXVECTOR3 *pFirstPosition = 0;
    meshbox->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, meshbox->GetNumVertices(), D3DXGetFVFVertexSize(meshbox->GetFVF()), &(m_meshBox.vMin), &(m_meshBox.vMax));
    meshbox->UnlockVertexBuffer();

	return S_OK;
}

D3DXVECTOR3 cSkinMesh::getSkinMeshBoxVMin(CONST D3DXMATRIX mat_world){
	getSkinMeshBox();
	D3DXVECTOR3 min = m_meshBox.vMin;
	D3DXVECTOR3 omin;
	omin.x = mat_world._11*min.x + mat_world._21*min.y + mat_world._31*min.z + mat_world._41*1;
	omin.y = mat_world._12*min.x + mat_world._22*min.y + mat_world._32*min.z + mat_world._42*1;
	omin.z = mat_world._13*min.x + mat_world._23*min.y + mat_world._33*min.z + mat_world._43*1;
	return omin;
}

D3DXVECTOR3 cSkinMesh::getSkinMeshBoxVMax(CONST D3DXMATRIX mat_world){
	getSkinMeshBox();
	D3DXVECTOR3 max = m_meshBox.vMax;
	D3DXVECTOR3 omax;
	omax.x = mat_world._11*max.x + mat_world._21*max.y + mat_world._31*max.z + mat_world._41*1;
	omax.y = mat_world._12*max.x + mat_world._22*max.y + mat_world._32*max.z + mat_world._42*1;
	omax.z = mat_world._13*max.x + mat_world._23*max.y + mat_world._33*max.z + mat_world._43*1;
	return omax;
}

D3DXVECTOR3 cSkinMesh::getSkinMeshBoxCenter(CONST D3DXMATRIX mat_world){
	getSkinMeshBox();
	D3DXVECTOR3 min = m_meshBox.vMin;
	D3DXVECTOR3 max = m_meshBox.vMax;
	D3DXVECTOR3 center;
	center.x = (min.x + max.x)/2;
	center.y = (min.y + max.y)/2;
	center.z = (min.z + max.z)/2;
	//matrix calclulate
	m_object_center.x = mat_world._11*center.x + mat_world._21*center.y + mat_world._31*center.z + mat_world._41*1;
	m_object_center.y = mat_world._12*center.x + mat_world._22*center.y + mat_world._32*center.z + mat_world._42*1;
	m_object_center.z = mat_world._13*center.x + mat_world._23*center.y + mat_world._33*center.z + mat_world._43*1;
	return m_object_center;
}