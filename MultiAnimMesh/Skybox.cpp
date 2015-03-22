#include "dxstdafx.h"
#include "Skybox.h"

cSkybox::cSkybox(IDirect3DDevice9 *pd3dDevice)
{
    m_pd3dDevice     = pd3dDevice;
    m_pTexture       = NULL;
    m_pIndexBuf      = NULL;
    m_pVertexBuf     = NULL;
    m_nNumVertices   = 0;
    m_nNumLatitudes  = 0;
    m_nNumLongitudes = 0;
    m_nVertsPerLongi = 0;
    m_nVertsPerLati  = 0;
    m_fSkyboxRadius  = 0;
}

cSkybox::~cSkybox(void)
{
    SAFE_RELEASE(m_pTexture);
    SAFE_RELEASE(m_pIndexBuf);
    SAFE_RELEASE(m_pVertexBuf);
}

HRESULT cSkybox::LoadSkybox(wchar_t *pTextureFile) 
{
    // �����������
	if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile, &m_pTexture)))
        return FALSE;
    return S_OK;
}

HRESULT cSkybox::InitSkybox(INT nAlpha, INT nBeta, FLOAT fRadius) 
{
    m_fSkyboxRadius  = fRadius;                 // ������İ뾶
    m_nNumLatitudes  = 360 / nAlpha;            // ά���ߵ�����
    m_nNumLongitudes =  90 / nBeta;             // �����ߵ�����
    m_nVertsPerLongi = m_nNumLatitudes + 1;     // ÿ���������ϵĶ�����
    m_nVertsPerLati  = m_nNumLongitudes + 1;    // ÿ��ά�����ϵĶ�����
    m_nNumVertices   = m_nVertsPerLati * m_nVertsPerLongi;

	// ������յ�����
    if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_nNumVertices * sizeof(SKYBOXVERTEX), 
        D3DUSAGE_WRITEONLY, SKYBOXVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuf, 0)))
        return FALSE;

    SKYBOXVERTEX *pVertices = NULL;
    m_pVertexBuf->Lock(0, 0, (void**)&pVertices, 0);

    int nIndex = 0;
    FLOAT fAlpha = 2.0f * D3DX_PI * nAlpha / 360.0f;    // ���Ƚ�ת��Ϊ���ȱ�ʾ
    FLOAT fBeta  = 2.0f * D3DX_PI * nBeta  / 360.0f;    // ά�Ƚ�ת��Ϊ���ȱ�ʾ
    for (int row = 0; row < m_nNumLongitudes+1; row++) 
    {
        for (int col = 0; col < m_nNumLatitudes+1; col++) 
        {
            // ���㶥�������
            pVertices[nIndex]._x = fRadius * cosf(row * fBeta) * cosf(col * fAlpha); 
            pVertices[nIndex]._y = fRadius * sinf(row * fBeta);
            pVertices[nIndex]._z = fRadius * cosf(row * fBeta) * sinf(col * fAlpha);
            // ���㶥�����������
            pVertices[nIndex]._u = col * fAlpha / (2.0f * D3DX_PI);
            pVertices[nIndex]._v = row * fBeta  / (D3DX_PI / 2.0f);

            nIndex++;
        }
    }
    m_pVertexBuf->Unlock();

	// ������յĶ�������
    if (FAILED(m_pd3dDevice->CreateIndexBuffer(m_nNumVertices * 6 *sizeof(WORD), 
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuf, 0)))
        return E_FAIL;

	WORD* pIndices = NULL;
    m_pIndexBuf->Lock(0, 0, (void**)&pIndices, 0);

    nIndex = 0; 
    for (int row = 0; row < m_nNumLongitudes; row++) 
    {
        for (int col = 0; col < m_nNumLatitudes; col++) 
        {
            pIndices[nIndex+0] =   row   * m_nVertsPerLongi + col;
            pIndices[nIndex+1] = (row+1) * m_nVertsPerLongi + col;
            pIndices[nIndex+2] = (row+1) * m_nVertsPerLongi + col + 1;

            pIndices[nIndex+3] =   row   * m_nVertsPerLongi + col;
            pIndices[nIndex+4] = (row+1) * m_nVertsPerLongi + col + 1;
            pIndices[nIndex+5] =   row   * m_nVertsPerLongi + col + 1;
            nIndex += 6;
        }
    }
    m_pIndexBuf->Unlock();

    return S_OK;
}

HRESULT cSkybox::DrawSkybox(D3DXMATRIX *pMatWorld, BOOL bDrawFrame) 
{
    m_pd3dDevice->SetStreamSource(0, m_pVertexBuf, 0, sizeof(SKYBOXVERTEX));
    m_pd3dDevice->SetFVF(SKYBOXVERTEX::FVF);
    m_pd3dDevice->SetIndices(m_pIndexBuf);
    m_pd3dDevice->SetTexture(0, m_pTexture);

    m_pd3dDevice->SetTransform(D3DTS_WORLD, pMatWorld);

    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
        m_nNumVertices, 0, m_nNumVertices * 2);

    m_pd3dDevice->SetTexture(0, 0);
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (bDrawFrame)
	{
        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
            m_nNumVertices, 0, m_nNumVertices * 2);
        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }

    return S_OK;
}

HRESULT cSkybox::distory(){
	SAFE_RELEASE(m_pTexture);
    SAFE_RELEASE(m_pIndexBuf);
    SAFE_RELEASE(m_pVertexBuf);

	return S_OK;
}