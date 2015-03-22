/*
   Demo Name:  Frustum Culling
      Author:  Qi Yuwei

   �����޳���ʹ�÷�����
   1.CFrustum��������
   2.����object��������������ͶӰ�������������
	   D3DXMATRIX mv = object_matrix * view_matrix;
	   xxx.CalculateFrustum((float*)&mv,(float*)&project_matrix);
   3.�ж������Ƿ�����������
	   xxx.isBoxVisiable(float x, float y, float z, float height,float width,float len);
	 ����x,y,zΪobject������λ�ã�height��width��len�ֱ�Ϊobject��Χ�еĳ���ߵ�һ��
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