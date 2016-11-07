#ifndef VECTOR3POINT_H
#define VECTOR3POINT_H

struct Vector3Point
{
	int index;
	float x;
	float y;
	float z;


	Vector3Point()
	{

	}

	Vector3Point(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}

	Vector3Point operator+(const Vector3Point& a) const
	{
		return Vector3Point(a.x + x, a.y + y, a.z + z);
	}

	Vector3Point operator*(const float f) const
	{
		return Vector3Point(x*f, y*f, z*f);
	}
};

#endif