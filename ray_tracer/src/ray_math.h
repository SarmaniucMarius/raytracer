#pragma once

#include <math.h>

#define PI 3.14159265359f

inline float
degrees_to_radians(float degrees)
{
	return PI * degrees / 180.0f;
}

// V4 declartions, functions

union v4f {
	struct {
		float x, y, z, w;
	};
	struct {
		float r, g, b, a;
	};
	float e[4];
};

inline v4f
V4f(float a, float b, float c, float d)
{
	v4f result;
	result.x = a;
	result.y = b;
	result.z = c;
	result.w = d;
	return result;
}

inline v4f
operator*(float a, v4f b)
{
	v4f result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	result.w = b.w*a;
	return result;
}

inline v4f
operator*(v4f b, float a)
{
	v4f result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	result.w = b.w*a;
	return result;
}

inline v4f &
operator*=(v4f &b, float a)
{
	b = a * b;
	return b;
}

inline v4f
operator-(v4f a)
{
	v4f result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	result.w = -a.w;
	return result;
}


inline v4f
operator+(v4f a, v4f b)
{
	v4f result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;
	return result;
}

inline v4f &
operator+=(v4f &a, v4f b)
{
	a = a + b;
	return a;
}

inline v4f
operator-(v4f a, v4f b)
{
	v4f result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;
	return result;
}

inline float
length(v4f a)
{
	return (float)sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

inline v4f
normalize(v4f a)
{
	return a * (1.0f / length(a));
}

inline v4f
lerp(v4f a, float t, v4f b)
{
	return (1 - t)*a + t * b;
}

inline float
dot(v4f a, v4f b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

union v4d {
	struct {
		double x, y, z, w;
	};
	struct {
		double r, g, b, a;
	};
	double e[4];
};

inline v4d
V4d(double a, double b, double c, double d)
{
	v4d result;
	result.x = a;
	result.y = b;
	result.z = c;
	result.w = d;
	return result;
}

inline v4d
operator*(double a, v4d b)
{
	v4d result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	result.w = b.w*a;
	return result;
}

inline v4d
operator*(v4d b, double a)
{
	v4d result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	result.w = b.w*a;
	return result;
}

inline v4d &
operator*=(v4d &b, double a)
{
	b = a * b;
	return b;
}

inline v4d
operator-(v4d a)
{
	v4d result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	result.w = -a.w;
	return result;
}


inline v4d
operator+(v4d a, v4d b)
{
	v4d result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;
	return result;
}

inline v4d &
operator+=(v4d &a, v4d b)
{
	a = a + b;
	return a;
}

inline v4d
operator-(v4d a, v4d b)
{
	v4d result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;
	return result;
}

inline double
length(v4d a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

inline v4d
normalize(v4d a)
{
	return a * (1.0 / length(a));
}

inline v4d
lerp(v4d a, double t, v4d b)
{
	return (1.0 - t)*a + t * b;
}

inline double
dot(v4d a, v4d b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

// V3 declartions, functions

union v3f {
	struct {
		float x, y, z;
	};
	struct {
		float r, g, b;
	};
	float e[3];
};

inline v3f
V3f(float a, float b, float c)
{
	v3f result;
	result.x = a;
	result.y = b;
	result.z = c;
	return result;
}

inline v3f
operator*(float a, v3f b)
{
	v3f result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	return result;
}

inline v3f
operator*(v3f b, float a)
{
	v3f result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	return result;
}

inline v3f
operator*(v3f b, v3f a)
{
	v3f result;
	result.x = b.x*a.x;
	result.y = b.y*a.y;
	result.z = b.z*a.z;
	return result;
}

inline v3f
operator/(v3f b, float a)
{
	v3f result;
	result.x = b.x / a;
	result.y = b.y / a;
	result.z = b.z / a;
	return result;
}

inline v3f &
operator*=(v3f &b, float a)
{
	b = a * b;
	return b;
}

inline v3f
operator-(v3f a)
{
	v3f result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}

inline v3f
operator+(v3f a, v3f b)
{
	v3f result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

inline v3f &
operator+=(v3f &a, v3f b)
{
	a = a + b;
	return a;
}

inline v3f
operator-(v3f a, v3f b)
{
	v3f result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

inline float
dot(v3f a, v3f b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline v3f
cross(v3f a, v3f b)
{
	v3f result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return result;
}

inline float
length(v3f a)
{
	return (float)sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

inline float
length_squared(v3f a)
{
	return a.x*a.x + a.y*a.y + a.z*a.z;
}

inline v3f
normalize(v3f a)
{
	return a * (1.0f / length(a));
}

inline v3f
lerp(v3f a, float t, v3f b)
{
	return (1.0f - t)*a + t * b;
}

inline v3f
reflect(v3f v, v3f n)
{
	return v - 2 * n*dot(v, n);
}

inline v3f
random_v3f(float min, float max)
{
	return V3f(random_float(min, max), random_float(min, max), random_float(min, max));
}

v3f random_in_unit_vector()
{
	v3f result = random_v3f(-1.0f, 1.0f);
	while (length_squared(result) > 1)
	{
		result = random_v3f(-1.0f, 1.0f);
	}

	return normalize(result);
}

v3f random_in_unit_disk()
{
	v3f result = V3f(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), .0f);
	while (length_squared(result) >= 1)
	{
		result = V3f(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), .0f);
	}
	return result;
}

union v3d {
	struct {
		double x, y, z;
	};
	struct {
		double r, g, b;
	};
	double e[3];
};

inline v3d
V3d(double a, double b, double c)
{
	v3d result;
	result.x = a;
	result.y = b;
	result.z = c;
	return result;
}

inline v3d
operator*(double a, v3d b)
{
	v3d result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	return result;
}

inline v3d
operator*(v3d b, double a)
{
	v3d result;
	result.x = b.x*a;
	result.y = b.y*a;
	result.z = b.z*a;
	return result;
}

inline v3d
operator/(v3d b, double a)
{
	v3d result;
	result.x = b.x / a;
	result.y = b.y / a;
	result.z = b.z / a;
	return result;
}

inline v3d &
operator*=(v3d &b, double a)
{
	b = a * b;
	return b;
}

inline v3d
operator-(v3d a)
{
	v3d result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}

inline v3d
operator+(v3d a, v3d b)
{
	v3d result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

inline v3d &
operator+=(v3d &a, v3d b)
{
	a = a + b;
	return a;
}

inline v3d
operator-(v3d a, v3d b)
{
	v3d result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

inline double
dot(v3d a, v3d b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline v3d
cross(v3d a, v3d b)
{
	v3d result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return result;
}

inline double
length(v3d a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

inline v3d
normalize(v3d a)
{
	return a * (1.0 / length(a));
}

inline v3d
lerp(v3d a, double t, v3d b)
{
	return (1.0 - t)*a + t * b;
}

// V2 declarations, functions

union v2f {
	struct {
		float x, y;
	};
	float e[2];
};

inline v2f
V2(float a, float b)
{
	v2f result;
	result.x = a;
	result.y = b;
	return result;
}

inline v2f
operator*(float a, v2f b)
{
	v2f result;
	result.x = b.x*a;
	result.y = b.y*a;
	return result;
}

inline v2f
operator*(v2f b, float a)
{
	v2f result;
	result.x = b.x*a;
	result.y = b.y*a;
	return result;
}

inline v2f &
operator*=(v2f &b, float a)
{
	b = a * b;
	return b;
}

inline v2f
operator-(v2f a)
{
	v2f result;
	result.x = -a.x;
	result.y = -a.y;
	return result;
}


inline v2f
operator+(v2f a, v2f b)
{
	v2f result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

inline v2f &
operator+=(v2f &a, v2f b)
{
	a = a + b;
	return a;
}

inline v2f
operator-(v2f a, v2f b)
{
	v2f result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

inline float
dot(v2f a, v2f b)
{
	return a.x*b.x + a.y*b.y;
}

inline v2f
perp(v2f a)
{
	v2f result = { -a.y, a.x };
	return result;
}

inline float
length(v2f a)
{
	return (float)sqrt(a.x*a.x + a.y*a.y);
}

inline v2f
normalize(v2f a)
{
	return a * (1.0f / length(a));
}

union v2d {
	struct {
		double x, y;
	};
	double e[2];
};

inline v2d
V2(double a, double b)
{
	v2d result;
	result.x = a;
	result.y = b;
	return result;
}

inline v2d
operator*(double a, v2d b)
{
	v2d result;
	result.x = b.x*a;
	result.y = b.y*a;
	return result;
}

inline v2d
operator*(v2d b, double a)
{
	v2d result;
	result.x = b.x*a;
	result.y = b.y*a;
	return result;
}

inline v2d &
operator*=(v2d &b, double a)
{
	b = a * b;
	return b;
}

inline v2d
operator-(v2d a)
{
	v2d result;
	result.x = -a.x;
	result.y = -a.y;
	return result;
}


inline v2d
operator+(v2d a, v2d b)
{
	v2d result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

inline v2d &
operator+=(v2d &a, v2d b)
{
	a = a + b;
	return a;
}

inline v2d
operator-(v2d a, v2d b)
{
	v2d result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

inline double
dot(v2d a, v2d b)
{
	return a.x*b.x + a.y*b.y;
}

inline v2d
perp(v2d a)
{
	v2d result = { -a.y, a.x };
	return result;
}

inline double
length(v2d a)
{
	return (double)sqrt(a.x*a.x + a.y*a.y);
}

inline v2d
normalize(v2d a)
{
	return a * (1.0f / length(a));
}

union v2i {
	struct {
		int x, y;
	};
	int e[2];
};

inline v2i
V2i(int a, int b)
{
	v2i result;
	result.x = a;
	result.y = b;
	return result;
}

inline v2i
operator+(v2i a, v2i b)
{
	v2i result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

inline v2i
operator-(v2i a, v2i b)
{
	v2i result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}
