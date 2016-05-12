
#ifndef POLYGON_OPTIMIZER_H
#define POLYGON_OPTIMIZER_H
#include "intpoint.h"

/*! \file polygonOptimizer.h
\brief 外框优化算法
*/

/**
 @brief 去掉所有外框的重复点，只留特征点．  
 @param polys 外框列表
 @see optimizePolygon 被调用对象
 @sa optimizePolygon2 
 @return void
*/
extern void optimizePolygons(Polygons& polys);

/**
 @brief 去掉一个框的重复点． 

 由于外框查找程序会把整个外框的特征点用点阵的方式表示出来，而不是用失量函数的方式表现．
 所以需要用这个程序处理一些规则图像(矩形),只留一些特征点.
 @param poly 外框点阵坐标，
 @see optimizePolygons 
 @sa optimizePolygon2 
 @todo 需要优化, 
		-# 考虑曲线,
		-# 现在只是用直线找转折的方式.必须考虑用拟合直线找极限点的方式.
 @return void
*/
extern void optimizePolygon(ClipperLib::Polygon& poly);

/**
 @brief 去掉一个框的重复点． 

 由于外框查找程序会把整个外框的特征点用点阵的方式表示出来，而不是用失量函数的方式表现．
 所以需要用这个程序处理一些规则图像(矩形),只留一些特征点.
 @param poly 外框点阵坐标，
 @see optimizePolygons 
 @sa optimizePolygon  不同的地方在于optimizePolygon2做了点量限制. 
 @todo 需要优化, 
		-# 考虑曲线,
		-# 现在只是用直线找转折的方式.必须考虑用拟合直线找极限点的方式.
 @return void
*/
extern void optimizePolygon2(ClipperLib::Polygon& poly);

#endif//POLYGON_OPTIMIZER_H
