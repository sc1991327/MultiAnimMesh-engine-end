#include "dxstdafx.h"
#include "GradientMesh.h"

const DWORD CUSTOM_VERTEX_FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

#define release_com(p)    do { if(p) { (p)->Release(); (p) = NULL; } } while(0)

cGradientMesh::cGradientMesh(){
	m_device = NULL;
}

cGradientMesh::~cGradientMesh(){

}

HRESULT cGradientMesh::create(IDirect3DDevice9* device, CONST WCHAR* wsourcefilename, CONST WCHAR* wtargetfilename ){
	m_device = device;
	HRESULT hr;
	V_RETURN(load_from_file(wsourcefilename, wtargetfilename));
	return S_OK;
}

HRESULT cGradientMesh::load_from_file(CONST WCHAR* wsourcefilename, CONST WCHAR* wtargetfilename){
	HRESULT	hr;
	LPD3DXBUFFER pD3DXMtrlBuffer_s;
	LPD3DXBUFFER pD3DXMtrlBuffer_t;

	V_RETURN(D3DXLoadMeshFromXW(wsourcefilename, D3DXMESH_MANAGED, m_device, NULL, &pD3DXMtrlBuffer_s, NULL, &m_num_materials_s, 
								&m_source_mesh));

	V_RETURN(D3DXLoadMeshFromXW(wtargetfilename, D3DXMESH_MANAGED, m_device, NULL, &pD3DXMtrlBuffer_t, NULL, &m_num_materials_t, 
								&m_target_mesh));

	//get mesh.
	// We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials_s = ( D3DXMATERIAL* )pD3DXMtrlBuffer_s->GetBufferPointer();
    m_mesh_material_s = new D3DMATERIAL9[m_num_materials_s];
    if( m_mesh_material_s == NULL )
        return E_OUTOFMEMORY;
    m_mesh_texture_s = new LPDIRECT3DTEXTURE9[m_num_materials_s];
    if( m_mesh_texture_s == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < m_num_materials_s; i++ )
    {
        // Copy the material
        m_mesh_material_s[i] = d3dxMaterials_s[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        //m_mesh_material_s[i].Ambient = m_mesh_material_s[i].Diffuse;

        m_mesh_texture_s[i] = NULL;
        if( d3dxMaterials_s[i].pTextureFilename != NULL &&
            lstrlenA( d3dxMaterials_s[i].pTextureFilename ) > 0 )
        {
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFileA( m_device,
                                                    d3dxMaterials_s[i].pTextureFilename,
                                                    &m_mesh_texture_s[i] ) ) )
            {
                // If texture is not in current folder, try parent folder
                const CHAR* strPrefix = "..\\";
                CHAR strTexture[MAX_PATH];
                strcpy_s( strTexture, MAX_PATH, strPrefix );
                strcat_s( strTexture, MAX_PATH, d3dxMaterials_s[i].pTextureFilename );
                // If texture is not in current folder, try parent folder
                if( FAILED( D3DXCreateTextureFromFileA( m_device,
                                                        strTexture,
                                                        &m_mesh_texture_s[i] ) ) )
                {
                    MessageBox( NULL, L"Could not find texture map", L"Meshes.exe", MB_OK );
                }
            }
        }
    }
	// We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials_t = ( D3DXMATERIAL* )pD3DXMtrlBuffer_t->GetBufferPointer();
    m_mesh_material_t = new D3DMATERIAL9[m_num_materials_t];
    if( m_mesh_material_t == NULL )
        return E_OUTOFMEMORY;
    m_mesh_texture_t = new LPDIRECT3DTEXTURE9[m_num_materials_t];
    if( m_mesh_texture_t == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < m_num_materials_t; i++ )
    {
        // Copy the material
        m_mesh_material_t[i] = d3dxMaterials_t[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_mesh_material_t[i].Ambient = m_mesh_material_t[i].Diffuse;

        m_mesh_texture_t[i] = NULL;
        if( d3dxMaterials_t[i].pTextureFilename != NULL &&
            lstrlenA( d3dxMaterials_t[i].pTextureFilename ) > 0 )
        {
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFileA( m_device,
                                                    d3dxMaterials_t[i].pTextureFilename,
                                                    &m_mesh_texture_t[i] ) ) )
            {
                // If texture is not in current folder, try parent folder
                const CHAR* strPrefix = "..\\";
                CHAR strTexture[MAX_PATH];
                strcpy_s( strTexture, MAX_PATH, strPrefix );
                strcat_s( strTexture, MAX_PATH, d3dxMaterials_t[i].pTextureFilename );
                // If texture is not in current folder, try parent folder
                if( FAILED( D3DXCreateTextureFromFileA( m_device,
                                                        strTexture,
                                                        &m_mesh_texture_t[i] ) ) )
                {
                    MessageBox( NULL, L"Could not find texture map", L"Meshes.exe", MB_OK );
                }
            }
        }
    }

	ID3DXMesh* cloned_mesh;
	// clone source mesh with specified vertex format
	m_source_mesh->CloneMeshFVF(m_source_mesh->GetOptions(), CUSTOM_VERTEX_FVF, m_device, &cloned_mesh);
	release_com(m_source_mesh);
	m_source_mesh = cloned_mesh;

	// clone target mesh with specified vertex format
	m_target_mesh->CloneMeshFVF(m_target_mesh->GetOptions(), CUSTOM_VERTEX_FVF, m_device, &cloned_mesh);
	release_com(m_target_mesh);
	m_target_mesh = cloned_mesh;

	// clone result mesh with specified vertex format from target mesh
	m_target_mesh->CloneMeshFVF(m_target_mesh->GetOptions(), CUSTOM_VERTEX_FVF, m_device, &m_result_mesh);

	m_source_mesh->GetVertexBuffer(&m_source_vb);
	m_target_mesh->GetVertexBuffer(&m_target_vb);
	m_result_mesh->GetVertexBuffer(&m_result_vb);

	return S_OK;
}

void cGradientMesh::destory(){
	release_com(m_source_mesh);
	release_com(m_target_mesh);	
	release_com(m_result_mesh);	

	release_com(m_source_vb);
	release_com(m_target_vb);
	release_com(m_result_vb);

	delete this;
}

HRESULT cGradientMesh::gradient(){
	sVertex* source_vertices;
	sVertex* target_vertices;
	sVertex* result_vertices;

	m_source_vb->Lock(0, 0, (void**) &source_vertices, 0);
	m_target_vb->Lock(0, 0, (void**) &target_vertices, 0);
	m_result_vb->Lock(0, 0, (void**) &result_vertices, 0);

	float time_factor = (float)(timeGetTime() % 2000) / 1000.0f;
	float scalar	  = (time_factor <= 1.0f) ? time_factor : (2.0f - time_factor);

	for(DWORD i = 0; i < m_result_mesh->GetNumVertices(); i++)
	{
		result_vertices[i].x = source_vertices[i].x * (1.0f - scalar) + target_vertices[i].x * scalar;
		result_vertices[i].y = source_vertices[i].y * (1.0f - scalar) + target_vertices[i].y * scalar;
		result_vertices[i].z = source_vertices[i].z * (1.0f - scalar) + target_vertices[i].z * scalar;

		result_vertices[i].nx = source_vertices[i].nx * (1.0f - scalar) + target_vertices[i].nx * scalar;
		result_vertices[i].ny = source_vertices[i].ny * (1.0f - scalar) + target_vertices[i].ny * scalar;
		result_vertices[i].nz = source_vertices[i].nz * (1.0f - scalar) + target_vertices[i].nz * scalar;
	}

	m_source_vb->Unlock();
	m_target_vb->Unlock();
	m_result_vb->Unlock();

	return S_OK;
}

HRESULT cGradientMesh::render(CONST D3DXMATRIX* mat_world, int object_state, double app_elapsed_time){
	HRESULT hr;

	if(0.0f == app_elapsed_time)
		return S_OK;
	
	//Transform befor drawing
	m_device ->SetTransform( D3DTS_WORLD, mat_world );

	//此处设置了三种显示方式：
	//分别显示原始网格，目标网格，过度动画。
	if ( object_state == 0 ){
		for(DWORD i = 0; i < m_num_materials_s; i++){
			m_device->SetMaterial( &m_mesh_material_s[i] );
			m_device->SetTexture( 0, m_mesh_texture_s[i] );
			m_source_mesh->DrawSubset(i);
		}
	}
	if ( object_state == 1 ){
		for(DWORD i = 0; i < m_num_materials_s; i++){
			//m_device->SetMaterial( &m_mesh_material_s[i] );
			//m_device->SetTexture( 0, m_mesh_texture_s[i] );
			m_result_mesh->DrawSubset(i);
		}
	}
	if ( object_state == 2 ){
		for(DWORD i = 0; i < m_num_materials_s; i++){
			m_device->SetMaterial( &m_mesh_material_t[i] );
			m_device->SetTexture( 0, m_mesh_texture_t[i] );
			m_target_mesh->DrawSubset(i);
		}
	}
	return S_OK;
}

LPD3DXMESH cGradientMesh::getSourceMesh(){
	return m_source_mesh;
}

LPD3DXMESH cGradientMesh::getTargetMesh(){
	return m_target_mesh;
}

LPD3DXMESH cGradientMesh::getResultMesh(){
	return m_result_mesh;
}

HRESULT cGradientMesh::getSourceMeshBox(){
	LPD3DXMESH meshbox;
	meshbox = m_source_mesh;

	D3DXVECTOR3 *pFirstPosition = 0;
    meshbox->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, meshbox->GetNumVertices(), D3DXGetFVFVertexSize(meshbox->GetFVF()), &(m_meshBox.vMin), &(m_meshBox.vMax));
    meshbox->UnlockVertexBuffer();

	return S_OK;
}

HRESULT cGradientMesh::getTargetMeshBox(){
	LPD3DXMESH meshbox;
	meshbox = m_target_mesh;

	D3DXVECTOR3 *pFirstPosition = 0;
    meshbox->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, meshbox->GetNumVertices(), D3DXGetFVFVertexSize(meshbox->GetFVF()), &(m_meshBox.vMin), &(m_meshBox.vMax));
    meshbox->UnlockVertexBuffer();

	return S_OK;
}

HRESULT cGradientMesh::getResultMeshBox(){
	LPD3DXMESH meshbox;
	meshbox = m_result_mesh;

	D3DXVECTOR3 *pFirstPosition = 0;
    meshbox->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, meshbox->GetNumVertices(), D3DXGetFVFVertexSize(meshbox->GetFVF()), &(m_meshBox.vMin), &(m_meshBox.vMax));
    meshbox->UnlockVertexBuffer();

	return S_OK;
}

D3DXVECTOR3 cGradientMesh::getSourceMeshBoxVMin(CONST D3DXMATRIX mat_world){
	getSourceMeshBox();
	D3DXVECTOR3 min = m_meshBox.vMin;
	D3DXVECTOR3 omin;
	omin.x = mat_world._11*min.x + mat_world._21*min.y + mat_world._31*min.z + mat_world._41*1;
	omin.y = mat_world._12*min.x + mat_world._22*min.y + mat_world._32*min.z + mat_world._42*1;
	omin.z = mat_world._13*min.x + mat_world._23*min.y + mat_world._33*min.z + mat_world._43*1;
	return omin;
}

D3DXVECTOR3 cGradientMesh::getSourceMeshBoxVMax(CONST D3DXMATRIX mat_world){
	getSourceMeshBox();
	D3DXVECTOR3 max = m_meshBox.vMax;
	D3DXVECTOR3 omax;
	omax.x = mat_world._11*max.x + mat_world._21*max.y + mat_world._31*max.z + mat_world._41*1;
	omax.y = mat_world._12*max.x + mat_world._22*max.y + mat_world._32*max.z + mat_world._42*1;
	omax.z = mat_world._13*max.x + mat_world._23*max.y + mat_world._33*max.z + mat_world._43*1;
	return omax;
}

D3DXVECTOR3 cGradientMesh::getTargetMeshBoxVMin(CONST D3DXMATRIX mat_world){
	getTargetMeshBox();
	D3DXVECTOR3 min = m_meshBox.vMin;
	D3DXVECTOR3 omin;
	omin.x = mat_world._11*min.x + mat_world._21*min.y + mat_world._31*min.z + mat_world._41*1;
	omin.y = mat_world._12*min.x + mat_world._22*min.y + mat_world._32*min.z + mat_world._42*1;
	omin.z = mat_world._13*min.x + mat_world._23*min.y + mat_world._33*min.z + mat_world._43*1;
	return omin;
}

D3DXVECTOR3 cGradientMesh::getTargetMeshBoxVMax(CONST D3DXMATRIX mat_world){
	getTargetMeshBox();
	D3DXVECTOR3 max = m_meshBox.vMax;
	D3DXVECTOR3 omax;
	omax.x = mat_world._11*max.x + mat_world._21*max.y + mat_world._31*max.z + mat_world._41*1;
	omax.y = mat_world._12*max.x + mat_world._22*max.y + mat_world._32*max.z + mat_world._42*1;
	omax.z = mat_world._13*max.x + mat_world._23*max.y + mat_world._33*max.z + mat_world._43*1;
	return omax;
}

D3DXVECTOR3 cGradientMesh::getResultMeshBoxVMin(CONST D3DXMATRIX mat_world){
	getResultMeshBox();
	D3DXVECTOR3 min = m_meshBox.vMin;
	D3DXVECTOR3 omin;
	omin.x = mat_world._11*min.x + mat_world._21*min.y + mat_world._31*min.z + mat_world._41*1;
	omin.y = mat_world._12*min.x + mat_world._22*min.y + mat_world._32*min.z + mat_world._42*1;
	omin.z = mat_world._13*min.x + mat_world._23*min.y + mat_world._33*min.z + mat_world._43*1;
	return omin;
}

D3DXVECTOR3 cGradientMesh::getResultMeshBoxVMax(CONST D3DXMATRIX mat_world){
	getResultMeshBox();
	D3DXVECTOR3 max = m_meshBox.vMax;
	D3DXVECTOR3 omax;
	omax.x = mat_world._11*max.x + mat_world._21*max.y + mat_world._31*max.z + mat_world._41*1;
	omax.y = mat_world._12*max.x + mat_world._22*max.y + mat_world._32*max.z + mat_world._42*1;
	omax.z = mat_world._13*max.x + mat_world._23*max.y + mat_world._33*max.z + mat_world._43*1;
	return omax;
}

D3DXVECTOR3 cGradientMesh::getSourceMeshVCenter(CONST D3DXMATRIX mat_world){
	getSourceMeshBox();
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
	//m_object_center = D3DXVECTOR3(mat_world._41,mat_world._42,mat_world._43) + m_object_center;
	return m_object_center;
}

D3DXVECTOR3 cGradientMesh::getTargetMeshVCenter(CONST D3DXMATRIX mat_world){
	getTargetMeshBox();
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
	//m_object_center = D3DXVECTOR3(mat_world._41,mat_world._42,mat_world._43) + m_object_center;
	return m_object_center;
}

D3DXVECTOR3 cGradientMesh::getResultMeshVCenter(CONST D3DXMATRIX mat_world){
	getResultMeshBox();
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
	//m_object_center = D3DXVECTOR3(mat_world._41,mat_world._42,mat_world._43) + m_object_center;
	return m_object_center;
}