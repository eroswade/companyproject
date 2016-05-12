
#ifndef PATHOPTIMIZER_H
#define PATHOPTIMIZER_H

#include "intpoint.h"
#include <stdint.h>

/*! \file pathOptimizer.h
\brief ����ͼ������
*/


/**
	��������֮��ľ��� 
	 @param p0 ��1
	 @param p1 ��2
	 @return float ����
*/
extern float distanceSquared(const Point& p0, const Point& p1);

/** ·���Ż��� */

/**
����������������ͼ��,�����·���Ż�,
������������������ͼ��֮��ı�����ͼ���ڲ����ı���
*/
class PathOptimizer
{
public:
	Point startPoint;
	std::vector<ClipperLib::Polygon*> polygons;
	std::vector<int> polyStart;
	std::vector<int> polyOrder;

	/**
	 ��ʼ��PathOptimizer. �����ʼλ�õ� 
	 @param startPoint ��ʼ����
	*/
	PathOptimizer(ClipperLib::IntPoint startPoint)
	{
		this->startPoint = startPoint;
	}
	/**
	 ����һ������ͼ��  
	 @param ��������ͼ�ε���
	 @see addPolygons
	*/
	void addPolygon(ClipperLib::Polygon& polygon)
	{
		this->polygons.push_back(&polygon);
	}
	/**
	 ���Ӷ������ͼ��  
	 @param polygons �������ͼ���б� 
	 @see addPolygon
	*/
	void addPolygons(ClipperLib::Polygons& polygons)
	{
		for(unsigned int i=0;i<polygons.size(); i++)
			this->polygons.push_back(&polygons[i]);
	}
	/**
	 �Ż��㷨����,�����㷨
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
