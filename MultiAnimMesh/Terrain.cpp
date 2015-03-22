#include "dxstdafx.h"
#include "Terrain.h"

cTerrain::cTerrain( IDirect3DDevice9 *pd3dDevice ){
	//get device object.
	m_pd3dDevice = pd3dDevice;
	m_pTexture      = NULL;
    m_pIndexBuf     = NULL;
    m_pVertexBuf    = NULL;
    m_nCellsPerRow  = 0;
    m_nCellsPerCol  = 0;
    m_nVertsPerRow  = 0;
    m_nVertsPerCol  = 0;
    m_nNumVertices  = 0;
    m_fTerrainWidth = 0.0f;
    m_fTerrainDepth = 0.0f;
    m_fCellSpacing  = 0.0f;
    m_fHeightScale  = 0.0f;

	m_index = 0;
}

cTerrain::~cTerrain(){
	SAFE_RELEASE(m_pTexture);
    SAFE_RELEASE(m_pIndexBuf);
    SAFE_RELEASE(m_pVertexBuf);
}

HRESULT cTerrain::LoadTerrain( wchar_t *pRawFileName, wchar_t *pTextureFile){

    // 读取高度信息
	// read data to inData array.
    std::ifstream inFile;
    inFile.open(pRawFileName, std::ios::binary);

    inFile.seekg(0,std::ios::end);
    std::vector<BYTE> inData(inFile.tellg());

    inFile.seekg(std::ios::beg);
    inFile.read((char*)&inData[0], inData.size());
    inFile.close();
	//inData array's data copy to int array m_vHeightInfo array.
    m_vHeightInfo.resize(inData.size());
    for (unsigned int i=0; i<inData.size(); i++)
        m_vHeightInfo[i] = inData[i];

    // 加载地形纹理
    if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile, &m_pTexture)))
        return FALSE;

    return S_OK;
}

HRESULT cTerrain::InitTerrain(INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale){
    m_nCellsPerRow  = nRows;
    m_nCellsPerCol  = nCols;
    m_fCellSpacing  = fSpace;
    m_fHeightScale  = fScale; 
    m_fTerrainWidth = nRows * fSpace;
    m_fTerrainDepth = nCols * fSpace;
    m_nVertsPerRow  = m_nCellsPerCol + 1;
    m_nVertsPerCol  = m_nCellsPerRow + 1;
    m_nNumVertices  = m_nVertsPerRow * m_nVertsPerCol;

    // 计算实际的几何高度
    for(unsigned int i=0; i<m_vHeightInfo.size(); i++)
        m_vHeightInfo[i] *= m_fHeightScale;

    // 计算地形的灵活顶点
    if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_nNumVertices * sizeof(TERRAINVERTEX), 
        D3DUSAGE_WRITEONLY, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuf, 0)))
        return FALSE;

    pVertices = NULL;
    m_pVertexBuf->Lock(0, 0, (void**)&pVertices, 0);

    FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX =  m_fTerrainWidth / 2.0f;
    FLOAT fStartZ =  m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;
    FLOAT fCoordU = 1.0f / (FLOAT)m_nCellsPerRow;
    FLOAT fCoordV = 1.0f / (FLOAT)m_nCellsPerCol;

    int nIndex = 0, i = 0, j = 0;
    for (float z = fStartZ; z > fEndZ; z -= m_fCellSpacing, i++)
    {
        j = 0;
        for (float x = fStartX; x < fEndX; x += m_fCellSpacing, j++)
        {
            nIndex = i * m_nCellsPerRow + j;
            pVertices[nIndex] = TERRAINVERTEX(x, m_vHeightInfo[nIndex], z, j*fCoordU, i*fCoordV);
            nIndex++;
			m_index++;
        }
    }

    m_pVertexBuf->Unlock();

    // 计算地形的顶点索引
    if (FAILED(m_pd3dDevice->CreateIndexBuffer(m_nNumVertices * 6 *sizeof(WORD), 
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuf, 0)))
        return FALSE;

    WORD* pIndices = NULL;
    m_pIndexBuf->Lock(0, 0, (void**)&pIndices, 0);

    nIndex = 0;
    for(int row = 0; row < m_nCellsPerRow-1; row++)
    {
        for(int col = 0; col < m_nCellsPerCol-1; col++)
        {
            pIndices[nIndex]   =   row   * m_nCellsPerRow + col;
            pIndices[nIndex+1] =   row   * m_nCellsPerRow + col + 1;
            pIndices[nIndex+2] = (row+1) * m_nCellsPerRow + col;

            pIndices[nIndex+3] = (row+1) * m_nCellsPerRow + col;
            pIndices[nIndex+4] =   row   * m_nCellsPerRow + col + 1;
            pIndices[nIndex+5] = (row+1) * m_nCellsPerRow + col + 1;

            nIndex += 6;
        }
    }

    m_pIndexBuf->Unlock();

    return S_OK;
}

HRESULT cTerrain::DrawTerrain(D3DXMATRIX *pMatWorld){
    m_pd3dDevice->SetStreamSource(0, m_pVertexBuf, 0, sizeof(TERRAINVERTEX));
    m_pd3dDevice->SetFVF(TERRAINVERTEX::FVF);
    m_pd3dDevice->SetIndices(m_pIndexBuf);
    m_pd3dDevice->SetTexture(0, m_pTexture);

    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3dDevice->SetTransform(D3DTS_WORLD, pMatWorld);
    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nNumVertices, 0, m_nNumVertices * 2);

    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pd3dDevice->SetTexture(0, 0);

    return S_OK;
}

HRESULT cTerrain::destory(){
	SAFE_RELEASE(m_pTexture);
    SAFE_RELEASE(m_pIndexBuf);
    SAFE_RELEASE(m_pVertexBuf);

	return S_OK;
}

float cTerrain::getHeight( float x, float z ){
	//直接获得方法
	/*FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX =  m_fTerrainWidth / 2.0f;
    FLOAT fStartZ =  m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;
	if ( x>fStartX && x<fEndX && z<fStartZ && z>fEndZ ){
		int n = ((fStartZ-z)/m_fCellSpacing)*m_nVertsPerRow + ((x-fStartX)/m_fCellSpacing);
		return pVertices[n]._y;
	}else{ 
		return 0; 
	}*/

	//遍历实现方法
	for ( int i=0; i<m_index; i++ ){
		if ( (pVertices[i]._x-x)<=m_fCellSpacing/2 && (pVertices[i]._x-x)>-m_fCellSpacing/2 && (pVertices[i]._z-z)<=m_fCellSpacing/2 && (pVertices[i]._z-z)>-m_fCellSpacing/2 ){
			return pVertices[i]._y;
		}
	}
	return 0;
}