#include "dxstdafx.h"
#include "StaticMesh.h"

cStaticMesh::cStaticMesh(){
	m_device = NULL;

	m_mesh = NULL;
	//m_pmesh = NULL;
	m_mesh_material = NULL;
	m_mesh_texture = NULL;
	m_num_materials = 0L;

	m_pMesh			= NULL;
	m_pPMesh		= NULL;
	m_pMaterials	= NULL;
	m_pTextures		= NULL;
	m_dwNumMtrls	= 0;
	isLOD			= FALSE;
	isNum			= 0;
}

cStaticMesh::~cStaticMesh(){
	
}

HRESULT cStaticMesh::create( IDirect3DDevice9* device, CONST WCHAR* wfilename ){
	m_device = device;
	HRESULT hr;
	V_RETURN( load_from_file(wfilename) );
	return S_OK;
}

HRESULT cStaticMesh::load_from_file( CONST WCHAR* wfilename ){
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;//存储材质信息
    // Load the mesh from the specified file
    if( FAILED( D3DXLoadMeshFromX( wfilename, D3DXMESH_SYSTEMMEM,
                                   m_device, NULL,
                                   &pD3DXMtrlBuffer, NULL, &m_num_materials,
                                   &m_mesh ) ) )
    {
        // If model is not in current folder, try parent folder
        if( FAILED( D3DXLoadMeshFromX( wfilename, D3DXMESH_SYSTEMMEM,
                                       m_device, NULL,
                                       &pD3DXMtrlBuffer, NULL, &m_num_materials,
                                       &m_mesh ) ) )
        {
            MessageBox( NULL, L"Could not find .x file", L"Meshes.exe", MB_OK );
            return E_FAIL;
        }
    }

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = ( D3DXMATERIAL* )pD3DXMtrlBuffer->GetBufferPointer();
    m_mesh_material = new D3DMATERIAL9[m_num_materials];
    if( m_mesh_material == NULL )
        return E_OUTOFMEMORY;
    m_mesh_texture = new LPDIRECT3DTEXTURE9[m_num_materials];
    if( m_mesh_texture == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < m_num_materials; i++ )
    {
        // Copy the material
        m_mesh_material[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_mesh_material[i].Ambient = m_mesh_material[i].Diffuse;

        m_mesh_texture[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL &&
            lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
        {
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFileA( m_device,
                                                    d3dxMaterials[i].pTextureFilename,
                                                    &m_mesh_texture[i] ) ) )
            {
                // If texture is not in current folder, try parent folder
                const CHAR* strPrefix = "..\\";
                CHAR strTexture[MAX_PATH];
                strcpy_s( strTexture, MAX_PATH, strPrefix );
                strcat_s( strTexture, MAX_PATH, d3dxMaterials[i].pTextureFilename );
                // If texture is not in current folder, try parent folder
                if( FAILED( D3DXCreateTextureFromFileA( m_device,
                                                        strTexture,
                                                        &m_mesh_texture[i] ) ) )
                {
                    MessageBox( NULL, L"Could not find texture map", L"Meshes.exe", MB_OK );
                }
            }
        }
    }
	// Done with the material buffer
    pD3DXMtrlBuffer->Release();

    return S_OK;
}

void cStaticMesh::destory(){
	if( m_mesh_material != NULL )
        delete[] m_mesh_material;

    if( m_mesh_texture )
    {
        for( DWORD i = 0; i < m_num_materials; i++ )
        {
            if( m_mesh_texture[i] )
                m_mesh_texture[i]->Release();
        }
        delete[] m_mesh_texture;
    }
    if( m_mesh != NULL )
        m_mesh->Release();
}

HRESULT cStaticMesh::rander( CONST D3DXMATRIX* mat_world ){
	
	//transform
	m_device ->SetTransform( D3DTS_WORLD, mat_world );

    // a loop
    for( DWORD i = 0; i < m_num_materials; i++ )
    {
        // Set the material and texture for this subset
        m_device->SetMaterial( &m_mesh_material[i] );
        m_device->SetTexture( 0, m_mesh_texture[i] );

        // Draw the mesh subset
        m_mesh->DrawSubset( i );
    }

	return S_OK;
}

LPD3DXMESH cStaticMesh::getStaticMesh(){
	return m_mesh;
}

HRESULT cStaticMesh::getStaticMeshBox(){
	LPD3DXMESH meshbox;
	meshbox = m_mesh;

	D3DXVECTOR3 *pFirstPosition = 0;
    meshbox->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, meshbox->GetNumVertices(), D3DXGetFVFVertexSize(meshbox->GetFVF()), &(m_meshBox.vMin), &(m_meshBox.vMax));
    meshbox->UnlockVertexBuffer();

	return S_OK;
}

D3DXVECTOR3 cStaticMesh::getStaticMeshBoxVMin(CONST D3DXMATRIX mat_world){
	getStaticMeshBox();
	D3DXVECTOR3 min = m_meshBox.vMin;
	D3DXVECTOR3 omin;
	omin.x = mat_world._11*min.x + mat_world._21*min.y + mat_world._31*min.z + mat_world._41*1;
	omin.y = mat_world._12*min.x + mat_world._22*min.y + mat_world._32*min.z + mat_world._42*1;
	omin.z = mat_world._13*min.x + mat_world._23*min.y + mat_world._33*min.z + mat_world._43*1;
	return omin;
}

D3DXVECTOR3 cStaticMesh::getStaticMeshBoxVMax(CONST D3DXMATRIX mat_world){
	getStaticMeshBox();
	D3DXVECTOR3 max = m_meshBox.vMax;
	D3DXVECTOR3 omax;
	omax.x = mat_world._11*max.x + mat_world._21*max.y + mat_world._31*max.z + mat_world._41*1;
	omax.y = mat_world._12*max.x + mat_world._22*max.y + mat_world._32*max.z + mat_world._42*1;
	omax.z = mat_world._13*max.x + mat_world._23*max.y + mat_world._33*max.z + mat_world._43*1;
	return omax;
}

HRESULT cStaticMesh::createLOD( IDirect3DDevice9* mdevice, CONST WCHAR* wfilename ){
	m_device = mdevice;
	// 从X文件中加载网格数据
    LPD3DXBUFFER pAdjBuffer  = NULL;
    LPD3DXBUFFER pMtrlBuffer = NULL;
    D3DXLoadMeshFromX(wfilename, D3DXMESH_MANAGED, m_device, 
        &pAdjBuffer, &pMtrlBuffer, NULL, &m_dwNumMtrls, &m_pMesh);

    // 读取材质和纹理数据
    D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
    m_pMaterials = new D3DMATERIAL9[m_dwNumMtrls];
    m_pTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMtrls];

    for (DWORD i=0; i<m_dwNumMtrls; i++) 
    {
        m_pMaterials[i] = pMtrls[i].MatD3D;
        m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;
        m_pTextures[i]  = NULL;
        D3DXCreateTextureFromFileA(m_device, pMtrls[i].pTextureFilename, &m_pTextures[i]);
    }

    // 优化网格
    m_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, 
        (DWORD*)pAdjBuffer->GetBufferPointer(), (DWORD*)pAdjBuffer->GetBufferPointer(), 0, 0);

    // 创建渐进网格
    D3DXGeneratePMesh(m_pMesh, (DWORD*)pAdjBuffer->GetBufferPointer(), 
        0, 0, 1, D3DXMESHSIMP_FACE, &m_pPMesh);
    m_pPMesh->SetNumFaces(m_pPMesh->GetMaxFaces());

    pAdjBuffer->Release();
    pMtrlBuffer->Release();

    // 设置纹理过滤方式
    m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT cStaticMesh::setFaceNumbersLOD( int num ){
	isNum = num;
	//set faces number
	static int nNumFaces = 0;
    nNumFaces = m_pPMesh->GetNumFaces();
	if ( isNum > 2 && isNum <= nNumFaces && isLOD==FALSE ){ 
		m_pPMesh->SetNumFaces(isNum);
		isLOD = TRUE; 
	}
	else{
		m_pPMesh->SetNumFaces(nNumFaces);
	}

	return S_OK;
}

HRESULT cStaticMesh::randerLOD( CONST D3DXMATRIX* mat_world){
	//transform
	m_device ->SetTransform( D3DTS_WORLD, mat_world );

	for (DWORD i = 0; i < m_dwNumMtrls; i++)
    {
        m_device->SetMaterial(&m_pMaterials[i]);
        m_device->SetTexture(0, m_pTextures[i]);
        m_pPMesh->DrawSubset(i);
	}

	return S_OK;
}

void cStaticMesh::destoryLOD(){
	for (DWORD i = 0; i<m_dwNumMtrls; i++) 
        SAFE_RELEASE(m_pTextures[i]);
    SAFE_DELETE(m_pTextures); 
    SAFE_DELETE(m_pMaterials); 
    SAFE_RELEASE(m_pMesh);
    SAFE_RELEASE(m_pPMesh);
}

LPD3DXMESH cStaticMesh::getStaticMeshLOD(){
	return m_pMesh;
}

HRESULT cStaticMesh::getStaticMeshBoxLOD(){
	LPD3DXMESH meshbox;
	meshbox = m_pMesh;

	D3DXVECTOR3 *pFirstPosition = 0;
    meshbox->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, meshbox->GetNumVertices(), D3DXGetFVFVertexSize(meshbox->GetFVF()), &(m_pMeshBox.vMin), &(m_pMeshBox.vMax));
    meshbox->UnlockVertexBuffer();

	return S_OK;
}

D3DXVECTOR3 cStaticMesh::getStaticMeshBoxVMinLOD(CONST D3DXMATRIX mat_world){
	getStaticMeshBoxLOD();
	D3DXVECTOR3 min = m_pMeshBox.vMin;
	D3DXVECTOR3 omin;
	omin.x = mat_world._11*min.x + mat_world._21*min.y + mat_world._31*min.z + mat_world._41*1;
	omin.y = mat_world._12*min.x + mat_world._22*min.y + mat_world._32*min.z + mat_world._42*1;
	omin.z = mat_world._13*min.x + mat_world._23*min.y + mat_world._33*min.z + mat_world._43*1;
	return omin;
}

D3DXVECTOR3 cStaticMesh::getStaticMeshBoxVMaxLOD(CONST D3DXMATRIX mat_world){
	getStaticMeshBoxLOD();
	D3DXVECTOR3 max = m_pMeshBox.vMax;
	D3DXVECTOR3 omax;
	omax.x = mat_world._11*max.x + mat_world._21*max.y + mat_world._31*max.z + mat_world._41*1;
	omax.y = mat_world._12*max.x + mat_world._22*max.y + mat_world._32*max.z + mat_world._42*1;
	omax.z = mat_world._13*max.x + mat_world._23*max.y + mat_world._33*max.z + mat_world._43*1;
	return omax;
}

D3DXVECTOR3 cStaticMesh::getStaticMeshCenter( CONST D3DXMATRIX mat_world ){
	getStaticMeshBox();
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

D3DXVECTOR3 cStaticMesh::getStaticMeshCenterLOD( CONST D3DXMATRIX mat_world ){
	getStaticMeshBoxLOD();
	D3DXVECTOR3 min = m_pMeshBox.vMin;
	D3DXVECTOR3 max = m_pMeshBox.vMax;
	D3DXVECTOR3 center;
	center.x = (min.x + max.x)/2;
	center.y = (min.y + max.y)/2;
	center.z = (min.z + max.z)/2;

	m_object_center.x = mat_world._11*center.x + mat_world._21*center.y + mat_world._31*center.z + mat_world._41*1;
	m_object_center.y = mat_world._12*center.x + mat_world._22*center.y + mat_world._32*center.z + mat_world._42*1;
	m_object_center.z = mat_world._13*center.x + mat_world._23*center.y + mat_world._33*center.z + mat_world._43*1;
	//m_object_center = D3DXVECTOR3(mat_world._41,mat_world._42,mat_world._43) + m_object_center;
	return m_object_center;
}