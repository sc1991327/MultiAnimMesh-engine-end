#include "dxstdafx.h"
#include "Collision.h"

#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

const DWORD CUSTOM_VERTEX_FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

cCollision::cCollision(){
	m_pd3dDevice = NULL;
	m_pMeshOne = NULL;
	m_pMeshTwo = NULL;
}

cCollision::~cCollision(){
	SAFE_RELEASE(m_pMeshOne);
    SAFE_RELEASE(m_pMeshTwo);
	/*SAFE_RELEASE(m_pBoxOne);
    SAFE_RELEASE(m_pBoxTwo);*/
}

BOOL cCollision::CollisionTestAABB( D3DXVECTOR3 vMin1, D3DXVECTOR3 vMax1, D3DXVECTOR3 vMin2, D3DXVECTOR3 vMax2 ){
	if (vMax1.x < vMin2.x || vMin1.x > vMax2.x) return FALSE;   // x方向
    if (vMax1.y < vMin2.y || vMin1.y > vMax2.y) return FALSE;   // y方向
    if (vMax1.z < vMin2.z || vMin1.z > vMax2.z) return FALSE;   // z方向
    return TRUE;    // 产生碰撞
}

FLOAT cCollision::Distance( D3DXVECTOR3 &p1, D3DXVECTOR3 &p2 ){
	float dist;
	dist = ((p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) + (p2.z-p1.z)*(p2.z-p1.z));
	return (FLOAT)sqrt(dist);
}

FLOAT cCollision::Area( FLOAT a, FLOAT b, FLOAT c ){
	FLOAT s = (a+b+c)/2;
	return (float)sqrt(s*(s-a)*(s-b)*(s-c));
}

FLOAT cCollision::vector_dot(D3DXVECTOR3 v1, D3DXVECTOR3 v2){
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

void cCollision::vector_cross(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 res){
	res.x = v1.y*v2.z - v1.z*v2.y;
	res.y = v1.z*v2.x - v1.x*v2.z;
	res.z = v1.x*v2.y - v1.y*v2.x;
}

void cCollision::vector_minus(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 res){
	res.x = v1.x - v2.x;
	res.y = v1.y - v2.y;
	res.z = v1.z - v2.z;
}

//线段与三角形相交。
BOOL cCollision::CollisionTestTrangles( D3DXVECTOR3 vp11, D3DXVECTOR3 vp12, D3DXVECTOR3 vp21, D3DXVECTOR3 vp22, D3DXVECTOR3 vp23 ){
	
	D3DXVECTOR3 TriangleV;		//三角形所在平面的法向量
	D3DXVECTOR3 fv11,fv12;		//三角形的边方向向量
	D3DXVECTOR3 CrossPoint;		//直线与平面的交点
	float TriD;					//平面方程常数项
	//get lineV
	D3DXVECTOR3 lineV = vp11 - vp12;
	//get traingleV
	fv11.x = vp21.x - vp22.x;
	fv11.y = vp21.y - vp22.y;
	fv11.z = vp21.z - vp22.z;
	fv12.x = vp21.x - vp23.x;
	fv12.y = vp21.y - vp23.y;
	fv12.z = vp21.z - vp23.z;
	TriangleV.x = fv11.y*fv12.z - fv11.z*fv12.z;
	TriangleV.y = -(fv11.x*fv12.z - fv11.z*fv12.x);
	TriangleV.z = fv11.x*fv12.y - fv11.y*fv12.x;
	TriD = -(TriangleV.x*vp21.x + TriangleV.y*vp21.y + TriangleV.z*vp21.z);
	//求解直线与平面的交点坐标
	//首先将直线方程转换为参数方程形式，然后代入平面方程，求得参数t，将t代入直线的参数方程即可求出交点坐标
	float tempU, tempD;  //临时变量
	tempU = TriangleV.x*vp11.x + TriangleV.y*vp11.y  + TriangleV.z*vp11.z + TriD;
	tempD = TriangleV.x*lineV.x + TriangleV.y*lineV.y + TriangleV.z*lineV.z;
	if(tempD == 0.0){
		//直线与平面平行。
		return FALSE;
	}
	//计算参数t
	float t = -tempU/tempD;
	//计算交点坐标
	CrossPoint.x = lineV.x*t + vp11.x;
	CrossPoint.y = lineV.y*t + vp11.y;
	CrossPoint.z = lineV.z*t + vp11.z;
	//判断交点是否在三角形内部
	//计算三角形三条边的长度
	float d12 = Distance(vp21, vp22);
	float d13 = Distance(vp21, vp23);
	float d23 = Distance(vp22, vp23);
	//计算交点到三个顶点的长度
	float c1 = Distance(CrossPoint, vp21);
	float c2 = Distance(CrossPoint, vp22);
	float c3 = Distance(CrossPoint, vp23);
	//求三角形及子三角形的面积
	float areaD = Area(d12, d13, d23);  //三角形面积
	float area1 = Area(c1, c2, d12);    //子三角形1
	float area2 = Area(c1, c3, d13);    //子三角形2
	float area3 = Area(c2, c3, d23);    //子三角形3
	//根据面积判断点是否在三角形内部
	if(fabs(area1+area2+area3-areaD) > 0.001){
		//交点不在三角形内部即可确定线段与三角形不相交。
		return FALSE;
	}
	//看目标线段与三角形是否相交。
	D3DXVECTOR3 v1,v2;
	v1.x = vp11.x - CrossPoint.x;
	v1.y = vp11.y - CrossPoint.y;
	v1.z = vp11.z - CrossPoint.z;
	v2.x = vp12.x - CrossPoint.x;
	v2.y = vp12.y - CrossPoint.y;
	v2.z = vp12.z - CrossPoint.z;
	float s = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	if ( s > 0 ){ return FALSE; }
	else { return TRUE; }
}

BOOL cCollision::CollisionTestLineAndTrainle(D3DXVECTOR3 vp11, D3DXVECTOR3 vp12, D3DXVECTOR3 vp21, D3DXVECTOR3 vp22, D3DXVECTOR3 vp23){
	CONST FLOAT epsilon = 0.000001f;
    D3DXVECTOR3 e1, e2, p, s, q;
    FLOAT t, u, v, tmp;
    D3DXVECTOR3 direction; 
    vector_minus(vp12, vp11, direction);
    vector_minus(vp22, vp21, e1);
    vector_minus(vp23, vp22, e2);
    vector_cross(direction, e2, p);
    tmp = vector_dot(p, e1);
    if (tmp > -epsilon && tmp < epsilon)
        return false;
    tmp = 1.0f / tmp;
    vector_minus(vp11, vp21, s);
    u = tmp * vector_dot(s, p);
    if (u < 0.0 || u > 1.0)
        return false;
    vector_cross(s, e1, q);
    v = tmp * vector_dot(direction, q);
    if (v < 0.0 || v > 1.0)
        return false;
    if (u + v > 1.0)
        return false;
    t = tmp * vector_dot(e2, q);
    if (t < 0.0 || t > 1.0)
        return false;
    return true;
}

BOOL cCollision::CollisionTestMesh(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo ){
	m_pMeshOne = pMeshOne;
	m_pMeshTwo = pMeshTwo;
	//精确检测，基于三角形面片的检测方法。
	//通过获得索引，两个物体的所有三角面片求交。
	//get and lock
	m_pMeshOne->LockIndexBuffer(0,(LPVOID*)&m_pibOne);
	m_pMeshTwo->LockIndexBuffer(0,(LPVOID*)&m_pibTwo);
	m_pMeshOne->LockVertexBuffer(0,(LPVOID*)&m_pvbOne);
	m_pMeshTwo->LockVertexBuffer(0,(LPVOID*)&m_pvbTwo);
	//initial
	WORD* m_pIndexOne = NULL;
	WORD* m_pIndexTwo = NULL;
	D3DXVECTOR3* m_pVerticesOne = NULL;
	D3DXVECTOR3* m_pVerticesTwo = NULL;
	//get useful data.
	DWORD m_numFacesOne = m_pMeshOne->GetNumFaces();
	DWORD m_numFacesTwo = m_pMeshTwo->GetNumFaces();
	for ( int i=0; i<m_numFacesOne; i++ ){
		WORD index0 = m_pibOne[3*i+0];
		WORD index1 = m_pibOne[3*i+1];
		WORD index2 = m_pibOne[3*i+2];
		D3DXVECTOR3 vp10 = D3DXVECTOR3(pMatrixOne._41,pMatrixOne._42,pMatrixOne._43) + m_pvbOne[index0];
		D3DXVECTOR3 vp11 = D3DXVECTOR3(pMatrixOne._41,pMatrixOne._42,pMatrixOne._43) + m_pvbOne[index1];
		D3DXVECTOR3 vp12 = D3DXVECTOR3(pMatrixOne._41,pMatrixOne._42,pMatrixOne._43) + m_pvbOne[index2];
		for ( int j=0; j<m_numFacesTwo; j++ ){
			WORD ind0 = m_pibTwo[3*j+0];
			WORD ind1 = m_pibTwo[3*j+1];
			WORD ind2 = m_pibTwo[3*j+2];
			D3DXVECTOR3 vp20 = D3DXVECTOR3(pMatrixTwo._41,pMatrixTwo._42,pMatrixTwo._43) +  m_pvbTwo[ind0];
			D3DXVECTOR3 vp21 = D3DXVECTOR3(pMatrixTwo._41,pMatrixTwo._42,pMatrixTwo._43) +  m_pvbTwo[ind1];
			D3DXVECTOR3 vp22 = D3DXVECTOR3(pMatrixTwo._41,pMatrixTwo._42,pMatrixTwo._43) +  m_pvbTwo[ind2];
			//test
			if ( CollisionTestLineAndTrainle(vp10, vp11, vp20, vp21, vp22)==TRUE ){ 
			//if ( CollisionTestTrangles(vp10, vp11, vp20, vp21, vp22)==TRUE ){ 
				m_pMeshOne->UnlockVertexBuffer();
				m_pMeshOne->UnlockIndexBuffer();
				m_pMeshTwo->UnlockVertexBuffer();
				m_pMeshTwo->UnlockIndexBuffer();
				return TRUE; }
			if ( CollisionTestLineAndTrainle(vp10, vp12, vp20, vp21, vp22)==TRUE ){ 
			//if ( CollisionTestTrangles(vp10, vp12, vp20, vp21, vp22)==TRUE ){ 
				m_pMeshOne->UnlockVertexBuffer();
				m_pMeshOne->UnlockIndexBuffer();
				m_pMeshTwo->UnlockVertexBuffer();
				m_pMeshTwo->UnlockIndexBuffer();
				return TRUE; }
			if ( CollisionTestLineAndTrainle(vp11, vp12, vp20, vp21, vp22)==TRUE ){ 
			//if ( CollisionTestTrangles(vp11, vp12, vp20, vp21, vp22)==TRUE ){ 
				m_pMeshOne->UnlockVertexBuffer();
				m_pMeshOne->UnlockIndexBuffer();
				m_pMeshTwo->UnlockVertexBuffer();
				m_pMeshTwo->UnlockIndexBuffer();
				return TRUE; }
		}
	}
	m_pMeshOne->UnlockVertexBuffer();
	m_pMeshOne->UnlockIndexBuffer();
	m_pMeshTwo->UnlockVertexBuffer();
	m_pMeshTwo->UnlockIndexBuffer();

	return FALSE;
}



//Public Metod
BOOL cCollision::CollisionTestA(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo ){
	// 创建外接盒
	m_pMeshOne = pMeshOne;
	m_pMeshTwo = pMeshTwo;

    D3DXVECTOR3 *pFirstPosition = 0;
    m_pMeshOne->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, m_pMeshOne->GetNumVertices(), D3DXGetFVFVertexSize(m_pMeshOne->GetFVF()), &(m_pBoxOne.vMin), &(m_pBoxOne.vMax));
    m_pMeshOne->UnlockVertexBuffer();

    m_pMeshTwo->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, m_pMeshTwo->GetNumVertices(), D3DXGetFVFVertexSize(m_pMeshTwo->GetFVF()), &(m_pBoxTwo.vMin), &(m_pBoxTwo.vMax));
    m_pMeshTwo->UnlockVertexBuffer();

	// 碰撞检测
	D3DXVECTOR3 vMin1 = D3DXVECTOR3(pMatrixOne._41,pMatrixOne._42,pMatrixOne._43) + m_pBoxOne.vMin;
    D3DXVECTOR3 vMax1 = D3DXVECTOR3(pMatrixOne._41,pMatrixOne._42,pMatrixOne._43) + m_pBoxOne.vMax;
	D3DXVECTOR3 vMin2 = D3DXVECTOR3(pMatrixTwo._41,pMatrixTwo._42,pMatrixTwo._43) + m_pBoxTwo.vMin;
    D3DXVECTOR3 vMax2 = D3DXVECTOR3(pMatrixTwo._41,pMatrixTwo._42,pMatrixTwo._43) + m_pBoxTwo.vMax;
	if ( CollisionTestAABB( vMin1, vMax1, vMin2, vMax2 )==TRUE ){
		if ( CollisionTestMesh(pMeshOne, pMeshTwo, pMatrixOne, pMatrixTwo )==TRUE ){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL cCollision::CollisionTestC(LPD3DXMESH pMeshOne, LPD3DXMESH pMeshTwo, D3DXMATRIX pMatrixOne, D3DXMATRIX pMatrixTwo ){
	// 创建外接盒
	m_pMeshOne = pMeshOne;
	m_pMeshTwo = pMeshTwo;

    D3DXVECTOR3 *pFirstPosition = 0;
    m_pMeshOne->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, m_pMeshOne->GetNumVertices(), D3DXGetFVFVertexSize(m_pMeshOne->GetFVF()), &(m_pBoxOne.vMin), &(m_pBoxOne.vMax));
    m_pMeshOne->UnlockVertexBuffer();

    m_pMeshTwo->LockVertexBuffer(0, (void**)&pFirstPosition);
    D3DXComputeBoundingBox(pFirstPosition, m_pMeshTwo->GetNumVertices(), D3DXGetFVFVertexSize(m_pMeshTwo->GetFVF()), &(m_pBoxTwo.vMin), &(m_pBoxTwo.vMax));
    m_pMeshTwo->UnlockVertexBuffer();

	D3DXVECTOR3 min1 = m_pBoxOne.vMin;
	D3DXVECTOR3 max1 = m_pBoxOne.vMax;
	D3DXVECTOR3 min2 = m_pBoxTwo.vMin;
	D3DXVECTOR3 max2 = m_pBoxTwo.vMax;

	// 碰撞检测
	D3DXVECTOR3 vMin1;
	vMin1.x = pMatrixOne._11*min1.x + pMatrixOne._21*min1.y + pMatrixOne._31*min1.z + pMatrixOne._41*1;
	vMin1.y = pMatrixOne._12*min1.x + pMatrixOne._22*min1.y + pMatrixOne._32*min1.z + pMatrixOne._42*1;
	vMin1.z = pMatrixOne._13*min1.x + pMatrixOne._23*min1.y + pMatrixOne._33*min1.z + pMatrixOne._43*1;

	D3DXVECTOR3 vMax1;
	vMax1.x = pMatrixOne._11*max1.x + pMatrixOne._21*max1.y + pMatrixOne._31*max1.z + pMatrixOne._41*1;
	vMax1.y = pMatrixOne._12*max1.x + pMatrixOne._22*max1.y + pMatrixOne._32*max1.z + pMatrixOne._42*1;
	vMax1.z = pMatrixOne._13*max1.x + pMatrixOne._23*max1.y + pMatrixOne._33*max1.z + pMatrixOne._43*1;

	D3DXVECTOR3 vMin2;
	vMin2.x = pMatrixTwo._11*min2.x + pMatrixTwo._21*min2.y + pMatrixTwo._31*min2.z + pMatrixTwo._41*1;
	vMin2.y = pMatrixTwo._12*min2.x + pMatrixTwo._22*min2.y + pMatrixTwo._32*min2.z + pMatrixTwo._42*1;
	vMin2.z = pMatrixTwo._13*min2.x + pMatrixTwo._23*min2.y + pMatrixTwo._33*min2.z + pMatrixTwo._43*1;
    
	D3DXVECTOR3 vMax2;
	vMax2.x = pMatrixTwo._11*max2.x + pMatrixTwo._21*max2.y + pMatrixTwo._31*max2.z + pMatrixTwo._41*1;
	vMax2.y = pMatrixTwo._12*max2.x + pMatrixTwo._22*max2.y + pMatrixTwo._32*max2.z + pMatrixTwo._42*1;
	vMax2.z = pMatrixTwo._13*max2.x + pMatrixTwo._23*max2.y + pMatrixTwo._33*max2.z + pMatrixTwo._43*1;

	if ( CollisionTestAABB( vMin1, vMax1, vMin2, vMax2 )==TRUE ){
		return TRUE;
	}
	return FALSE;
}

BOOL cCollision::CollisionTestC( D3DXVECTOR3 v1Min, D3DXVECTOR3 v1Max, D3DXVECTOR3 v2Min, D3DXVECTOR3 v2Max ){
	// 碰撞检测
	// 传入的点是经过矩阵变换后的点，此处就不用再使用矩阵进行变换了。
	D3DXVECTOR3 vMin1 = v1Min;
    D3DXVECTOR3 vMax1 = v1Max;
	D3DXVECTOR3 vMin2 = v2Min;
    D3DXVECTOR3 vMax2 = v2Max;
	if ( CollisionTestAABB( vMin1, vMax1, vMin2, vMax2 )==TRUE ){
		return TRUE;
	}
	return FALSE;
}

void cCollision::destory(){

}