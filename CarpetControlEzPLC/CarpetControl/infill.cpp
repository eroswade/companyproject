#include "stdafx.h"
#include "infill.h"

void generateConcentricInfill(Polygons outline, Polygons& result, int offsets[], int offsetsSize)
{
	int step = 0;
	while(1)
	{
		for(unsigned int polygonNr=0; polygonNr<outline.size(); polygonNr++)
			result.push_back(outline[polygonNr]);
		ClipperLib::OffsetPolygons(outline, outline, -offsets[step], ClipperLib::jtSquare, 2, false);
		if (outline.size() < 1)
			break;
		step = (step + 1) % offsetsSize;
	}
}

int compare_int64_t(const void* a, const void* b)
{
	int64_t n = (*(int64_t*)a) - (*(int64_t*)b);
	if (n < 0) return -1;
	if (n > 0) return 1;
	return 0;
}

// extrusionWidth 长
// lineSpacing ?
// infillOverlap 线距
// rotation 旋转角度
//  testout 测试 OffsetPolygons 是否成功
void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation,Polygons &testout)
{
	Polygons outline;
	ClipperLib::OffsetPolygons(in_outline, outline, extrusionWidth * infillOverlap / 100, ClipperLib::jtSquare, 2, false);
	PointMatrix matrix(rotation);
	matrix.apply(outline);
	testout=outline;
	AABB boundary(outline);

	boundary.pmin.X = ((boundary.pmin.X / lineSpacing) - 1) * lineSpacing;
	int lineCount = (boundary.pmax.X - boundary.pmin.X + (lineSpacing - 1)) / lineSpacing;
	vector<vector<int64_t> > cutList;
	for(int n=0; n<lineCount; n++)
		cutList.push_back(vector<int64_t>());

	for(unsigned int polyNr=0; polyNr < outline.size(); polyNr++)
	{
		Point p1 = outline[polyNr][outline[polyNr].size()-1];
		for(unsigned int i=0; i < outline[polyNr].size(); i++)
		{
			Point p0 = outline[polyNr][i];
			int idx0 = (p0.X - boundary.pmin.X) / lineSpacing;
			int idx1 = (p1.X - boundary.pmin.X) / lineSpacing;
			int64_t xMin = p0.X, xMax = p1.X;
			if (p0.X > p1.X) { xMin = p1.X; xMax = p0.X; }
			if (idx0 > idx1) { int tmp = idx0; idx0 = idx1; idx1 = tmp; }
			for(int idx = idx0; idx<=idx1; idx++)
			{
				int x = (idx * lineSpacing) + boundary.pmin.X + lineSpacing / 2;
				if (x < xMin) continue;
				if (x >= xMax) continue;
				int y = p0.Y + (p1.Y - p0.Y) * (x - p0.X) / (p1.X - p0.X);
				cutList[idx].push_back(y);
			}
			p1 = p0;
		}
	}

	int idx = 0;
	for(int64_t x = boundary.pmin.X + lineSpacing / 2; x < boundary.pmax.X; x += lineSpacing)
	{
		qsort(cutList[idx].data(), cutList[idx].size(), sizeof(int64_t), compare_int64_t);
		for(unsigned int i = 0; i + 1 < cutList[idx].size(); i+=2)
		{
			if (cutList[idx][i+1] - cutList[idx][i] < extrusionWidth / 5)
				continue;
			ClipperLib::Polygon p;
			p.push_back(matrix.unapply(Point(x, cutList[idx][i])));
			p.push_back(matrix.unapply(Point(x, cutList[idx][i+1])));
			result.push_back(p);
		}
		idx += 1;
	}
}