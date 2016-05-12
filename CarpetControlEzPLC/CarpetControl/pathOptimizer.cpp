#include "stdafx.h"
#include "pathOptimizer.h"

float distanceSquared(const Point& p0, const Point& p1)
{
	float dx = p0.X - p1.X;
	float dy = p0.Y - p1.Y;
	return dx * dx + dy * dy;
}