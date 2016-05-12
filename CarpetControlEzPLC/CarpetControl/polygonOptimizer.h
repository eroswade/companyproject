
#ifndef POLYGON_OPTIMIZER_H
#define POLYGON_OPTIMIZER_H
#include "intpoint.h"

/*! \file polygonOptimizer.h
\brief ����Ż��㷨
*/

/**
 @brief ȥ�����������ظ��㣬ֻ�������㣮  
 @param polys ����б�
 @see optimizePolygon �����ö���
 @sa optimizePolygon2 
 @return void
*/
extern void optimizePolygons(Polygons& polys);

/**
 @brief ȥ��һ������ظ��㣮 

 ���������ҳ��������������������õ���ķ�ʽ��ʾ��������������ʧ�������ķ�ʽ���֣�
 ������Ҫ�����������һЩ����ͼ��(����),ֻ��һЩ������.
 @param poly ���������꣬
 @see optimizePolygons 
 @sa optimizePolygon2 
 @todo ��Ҫ�Ż�, 
		-# ��������,
		-# ����ֻ����ֱ����ת�۵ķ�ʽ.���뿼�������ֱ���Ҽ��޵�ķ�ʽ.
 @return void
*/
extern void optimizePolygon(ClipperLib::Polygon& poly);

/**
 @brief ȥ��һ������ظ��㣮 

 ���������ҳ��������������������õ���ķ�ʽ��ʾ��������������ʧ�������ķ�ʽ���֣�
 ������Ҫ�����������һЩ����ͼ��(����),ֻ��һЩ������.
 @param poly ���������꣬
 @see optimizePolygons 
 @sa optimizePolygon  ��ͬ�ĵط�����optimizePolygon2���˵�������. 
 @todo ��Ҫ�Ż�, 
		-# ��������,
		-# ����ֻ����ֱ����ת�۵ķ�ʽ.���뿼�������ֱ���Ҽ��޵�ķ�ʽ.
 @return void
*/
extern void optimizePolygon2(ClipperLib::Polygon& poly);

#endif//POLYGON_OPTIMIZER_H
