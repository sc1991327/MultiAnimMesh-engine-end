/*
   Demo Name:  Frustum Culling
      Author:  Qi Yuwei

   视域剔除类使用方法：
   1.CFrustum创建对象
   2.根据object矩阵、摄像机矩阵和投影矩阵计算视域体
	   D3DXMATRIX mv = object_matrix * view_matrix;
	   xxx.CalculateFrustum((float*)&mv,(float*)&project_matrix);
   3.判断物体是否在视域体中
	   xxx.isBoxVisiable(float x, float y, float z, float height,float width,float len);
	 其中x,y,z为object的中心位置，height，width，len分别为object包围盒的长宽高的一半
*/


#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#define MAX_SIDES       6

#include"Plane.h"


class CFrustum
{
   public:
      void CalculateFrustum(float *mv, float *proj);
      bool isPointVisiable(float x, float y, float z);
      bool isSphereVisiable(float x, float y, float z, float radius);
      bool isBoxVisiable(float x, float y, float z, float height,float width,float len);

   protected:
      void NormalizeFrustum();

   protected:
      CPlane Frustum[MAX_SIDES];
};

#endif