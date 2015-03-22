#include "dxstdafx.h"
#include "Billboard.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

cBillboard::cBillboard( IDirect3DDevice9 *pd3dDevice ){
	m_pd3dDevice	= pd3dDevice;
	m_pTreeVB		= NULL;
	m_pTreeTex		= NULL;

	D3DXMatrixIdentity(&m_matView);
}

cBillboard::~cBillboard()
{
	SAFE_RELEASE(m_pTreeTex);
    SAFE_RELEASE(m_pTreeVB);
    SAFE_RELEASE(m_pd3dDevice);
}

HRESULT cBillboard::InitBillboard( wchar_t *pFileName, float downX, float downY, float downZ, float weight, float height ){
	float ldx = downX - weight/2;
	float ldy = downY;
	float ltx = downX - weight/2;
	float lty = downY + height;
	float rdx = downX + weight/2;
	float rdy = downY;
	float rtx = downX + weight/2;
	float rty = downY + height;
	float z = downZ;

	// 创建树木顶点缓存
    m_pd3dDevice->CreateVertexBuffer( 4 * sizeof(CUSTOMVERTEX), 0, 
        D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pTreeVB, NULL );
    
	CUSTOMVERTEX *pVertices = NULL;		//create a point
    pVertices = NULL;
    m_pTreeVB->Lock(0, 0, (void**)&pVertices, 0);	//create a board to texture.
    pVertices[0] = CUSTOMVERTEX( ldx, ldy, z, 0.0f, 1.0f);
    pVertices[1] = CUSTOMVERTEX( ltx, lty, z, 0.0f, 0.0f); 
    pVertices[2] = CUSTOMVERTEX( rdx, rdy, z, 1.0f, 1.0f); 
    pVertices[3] = CUSTOMVERTEX( rtx, rty, z, 1.0f, 0.0f);
    m_pTreeVB->Unlock();
	//use texture to m_pTreeTex
	D3DXCreateTextureFromFile( m_pd3dDevice, pFileName, &m_pTreeTex );

	return S_OK;
}

HRESULT cBillboard::DrawBillboard( D3DXMATRIX m_matView ){
	// 设置纹理状态
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Alpha混合设置, 设置混合系数
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   true );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	 //根据取景变换矩阵来构造广告牌矩阵
    D3DXMATRIX matBillboard;
    D3DXMatrixIdentity(&matBillboard);
    matBillboard._11 = m_matView._11;
    matBillboard._13 = m_matView._13;
    matBillboard._31 = m_matView._31;
    matBillboard._33 = m_matView._33;
    D3DXMatrixInverse(&matBillboard, NULL, &matBillboard);

    // 根据当前观察方向来构造广告牌矩阵
    /*D3DXMATRIX matBillboard;
    D3DXVECTOR3 vDir = vAt - vEye;
    if( vDir.x > 0.0f )
        D3DXMatrixRotationY( &matBillboard, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
    else
        D3DXMatrixRotationY( &matBillboard, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
*/

    D3DXMATRIX matTree;
    D3DXMatrixIdentity(&matTree);
    matTree = matBillboard * matTree;
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTree );

    // 渲染树木
    m_pd3dDevice->SetTexture( 0, m_pTreeTex );
    m_pd3dDevice->SetStreamSource( 0, m_pTreeVB, 0, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	return S_OK;
}

HRESULT cBillboard::destory(){
	SAFE_RELEASE(m_pTreeTex);
    SAFE_RELEASE(m_pTreeVB);

	return S_OK;
}