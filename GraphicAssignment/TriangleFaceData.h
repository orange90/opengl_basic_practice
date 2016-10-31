#ifndef TRIANGLEFACEDATA_H
#define TRIANGLEFACEDATA_H

#include "Vector3Point.h";
struct TriangleFaceData
{
	int index;
	Vector3Point p1;
	int p1Index;
	Vector3Point p2;
	int p2Index;
	Vector3Point p3;
	int p3Index;
};

#endif