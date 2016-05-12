
#ifndef PATHOPTIMIZER_H
#define PATHOPTIMIZER_H

#include "intpoint.h"
#include <stdint.h>

/*! \file pathOptimizer.h
\brief 几何图形排序
*/


/**
	计算点与点之间的距离 
	 @param p0 点1
	 @param p1 点2
	 @return float 距离
*/
extern float distanceSquared(const Point& p0, const Point& p1);

/** 路径优化类 */

/**
用来遍历各个几何图形,做最短路径优化,
可以用来做几个几何图形之间的遍历和图形内部填充的遍历
*/
class PathOptimizer
{
public:
	Point startPoint;
	std::vector<ClipperLib::Polygon*> polygons;
	std::vector<int> polyStart;
	std::vector<int> polyOrder;

	/**
	 初始化PathOptimizer. 输入初始位置点 
	 @param startPoint 初始化点
	*/
	PathOptimizer(ClipperLib::IntPoint startPoint)
	{
		this->startPoint = startPoint;
	}
	/**
	 增加一个几何图形  
	 @param 单个几何图形点阵
	 @see addPolygons
	*/
	void addPolygon(ClipperLib::Polygon& polygon)
	{
		this->polygons.push_back(&polygon);
	}
	/**
	 增加多个几何图形  
	 @param polygons 多个几何图形列表 
	 @see addPolygon
	*/
	void addPolygons(ClipperLib::Polygons& polygons)
	{
		for(unsigned int i=0;i<polygons.size(); i++)
			this->polygons.push_back(&polygons[i]);
	}
	/**
	 优化算法核心,背包算法
	*/
	void optimize()
	{
		std::vector<bool> picked;
		for(unsigned int i=0;i<polygons.size(); i++)
		{
			int best = -1;
			float bestDist = 0x0FFFFFFFFFFFFFFFLL;
			ClipperLib::Polygon* poly = polygons[i];
			for(unsigned int j=0; j<poly->size(); j++)
			{
				float dist = distanceSquared((*poly)[j], startPoint);
				if (dist < bestDist)
				{
					best = j;
					bestDist = dist;
				}
			}
			polyStart.push_back(best);
			picked.push_back(false);
		}

		Point p0 = startPoint;
		for(unsigned int n=0; n<polygons.size(); n++)
		{
			int best = -1;
			float bestDist = 0x0FFFFFFFFFFFFFFFLL;
			for(unsigned int i=0;i<polygons.size(); i++)
			{
				if (picked[i] || (*polygons[i]).size() < 1)
					continue;
				if ((*polygons[i]).size() == 2)
				{
					float dist = distanceSquared((*polygons[i])[0], p0);
					if (dist < bestDist)
					{
						best = i;
						bestDist = dist;
						polyStart[i] = 0;
					}
					dist = distanceSquared((*polygons[i])[1], p0);
					if (dist < bestDist)
					{
						best = i;
						bestDist = dist;
						polyStart[i] = 1;
					}
				}else{
					float dist = distanceSquared((*polygons[i])[polyStart[i]], p0);
					if (dist < bestDist)
					{
						best = i;
						bestDist = dist;
					}
				}
			}
			if (best > -1)
			{
				if (polygons[best]->size() == 2)
				{
					p0 = (*polygons[best])[(polyStart[best] + 1) % 2];
				}else{
					p0 = (*polygons[best])[polyStart[best]];
				}
				picked[best] = true;
				polyOrder.push_back(best);
			}
		}

		p0 = startPoint;
		for(unsigned int n=0; n<polyOrder.size(); n++)
		{
			int nr = polyOrder[n];
			int best = -1;
			float bestDist = 0x0FFFFFFFFFFFFFFFLL;
			for(unsigned int i=0;i<polygons[nr]->size(); i++)
			{
				float dist = distanceSquared((*polygons[nr])[i], p0);
				if (dist < bestDist)
				{
					best = i;
					bestDist = dist;
				}
			}
			polyStart[nr] = best;
			if ((*polygons[nr]).size() <= 2)
			{
				p0 = (*polygons[nr])[(best + 1) % 2];
			}else{
				p0 = (*polygons[nr])[best];
			}
		}
	}
};

#endif//PATHOPTIMIZER_H
