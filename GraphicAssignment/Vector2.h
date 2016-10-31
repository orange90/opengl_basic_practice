#ifndef VECTOR2_H
#define VECTOR2_H

struct Vector2
{
	float x;
	float y;

	Vector2()
	{

	}

	Vector2(float xx, float yy)
	{
		x = xx;
		y = yy;
	}

	Vector2 operator+(const Vector2& a) const
	{
		return Vector2(a.x + x, a.y + y);
	}

	/*Vector2 operator+(Vector2 value)
	{
		Vector2 result;
		result.x = value.x + x;
		result.y = value.y+ y;
		y += value.y;
		return result;
	}

	Vector2& operator-(const Vector2& value)
	{
		Vector2 result;
		result.x = x -value.x;
		result.y = y - value.y;
		return result;
	}*/

	
};
/*inline Vector2& operator-(Vector2 const& left, Vector2 const& right)
{
	Vector2 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}*/
#endif