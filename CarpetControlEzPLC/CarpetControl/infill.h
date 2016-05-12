/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef INFILL_H
#define INFILL_H
#include "intpoint.h"

extern int compare_int64_t(const void* a, const void* b);
extern void generateConcentricInfill(Polygons outline, Polygons& result, int offsets[], int offsetsSize);
extern void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation,Polygons &testout);



#endif//INFILL_H
