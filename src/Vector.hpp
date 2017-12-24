#pragma once
#include <math.h>

constexpr double degToRad = M_PI / 180;

template<class T>
class Vector {
public:
	Vector<T>() : x(0), y(0) {};
	Vector<T>(float angle) : x(cos(angle * degToRad)), y(sin(angle * degToRad)) {};
	Vector<T>(T _x, T _y) : x(_x), y(_y) {};
	Vector<T>(const Vector<T>& v) : x(v.x), y(v.y) {};

	T x, y;

	Vector<T>& operator=(const Vector<T>& v)
	{
		this->x = v.x;
		this->y = v.y;
		return *this;
	}

	Vector<T> operator + (const Vector<T>& v) const
	{
		return Vector<T>(x + v.x, y + v.y);
	}

	Vector<T> operator - (const Vector<T>& v) const
	{
		return Vector<T>(x - v.x, y - v.y);
	}

	T operator * (const Vector<T>& v) const
	{
		return x * v.x + y * v.y;
	}

	Vector<T> operator * (T s) const
	{
		return Vector<T>(x * s, y * s);
	}

	Vector<T> operator / (T s) const
	{
		return Vector<T>(x / s, y / s);
	}

	float length() const
	{
		return sqrt(x*x + y*y);
	}
};

using VectorF = Vector<float>;
using VectorI = Vector<int>;
